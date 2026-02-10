#include <ImGuiHook.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <asm-generic/mman.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_android.h>
#include <ImGui/backends/android_native_app_glue.h>

#include <misc/Utils.h>
#include <Dobby/dobby.h>
#include <Obfuscate.h>
#include <Logger.h>

#include <KittyMemory/KittyInclude.hpp>
#include "xdl.h"

#include <Menu.h>

#if defined(INPUT_SDL) || defined(INPUT_UNITY)
static ElfScanner get_elf(const char *name) {
    ElfScanner elf;
    do { sleep(1);
        elf = ElfScanner::createWithPath(name);
        TLOGW("Input", "%s invalid", name);
    } while (!elf.isValid());
    return elf;
}
#endif

#if defined(INPUT_UNIVERSAL)
#define HOOKINPUT(ret, func, ...) \
    ret (*orig##func)(__VA_ARGS__); \
    ret my##func(__VA_ARGS__)

HOOKINPUT(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    if (!ImGui::GetCurrentContext()) return;
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

static int32_t (*origInput2)(void* consumer, void* factory, bool isRaw, long sequenceId,
                              uint32_t* outPolicyFlags, void** outEventPtr);
int32_t myInput2(void* consumer, void* factory, bool isRaw, long sequenceId,
                  uint32_t* outPolicyFlags, void** outEventPtr) {
    int32_t result = origInput2(consumer, factory, isRaw, sequenceId, outPolicyFlags, outEventPtr);
    if (!ImGui::GetCurrentContext()) return result;

    if (outEventPtr && *outEventPtr) {
        AInputEvent* event = (AInputEvent*)*outEventPtr;
        ImGui_ImplAndroid_HandleInputEvent(event);
    }

    return result;
}

static void input_hook() {
    void* hndl = xdl_open("libinput.so", XDL_TRY_FORCE_LOAD);
    if (!hndl) {
        LOGE("INPUT: UNIVERSAL: Failed to open libinput.so");
        return;
    }

    void *sym_input = xdl_sym(hndl, "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE", nullptr);
    if (sym_input) {
        if (DobbyHook(sym_input, (void*)myInput, (void**)&origInput) != 0) {
            LOGE("failed to hook <A14 input");
        } else {
            LOGI("Hooked <A14 input");
            return;
        }
    }

    // >A15 64-bit symbol
    sym_input = xdl_sym(hndl, "_ZN7android13InputConsumer7consumeEPNS_26InputEventFactoryInterfaceEblPjPPNS_10InputEventE", nullptr);
    if (sym_input) {
        if (DobbyHook(sym_input, (void*)myInput2, (void**)&origInput2) != 0) {
            LOGE("failed to hook >A15 input");
        } else {
            LOGI("Hooked >A15 input");
            return;
        }
    }

}
#elif defined(INPUT_UNITY)
#  include <inputs/touch_input_unity.h>
static void input_hook() {
    ElfScanner unityELF = get_elf("libunity.so");
    RegisterNativeFn nativeInjectEvent = KittyScanner::findRegisterNativeFn(unityELF, "nativeInjectEvent");
    if (!nativeInjectEvent.isValid())
        TLOGE("Input", "nativeInjectEvent register invalid");
    if(DobbyHook((void *) nativeInjectEvent.fnPtr, (void *) Unity_input::hook, (void **) &Unity_input::o_inject_event) != 0)
        TLOGE("Input", "nativeInjectEvent hook error!");
}
#elif defined(INPUT_SDL)
#  include <inputs/touch_input_sdl.h>
static void input_hook() {
    get_elf("libSDL2.so");
    if(DobbyHook((void *) DobbySymbolResolver(OBFUSCATE("libSDL2.so"), OBFUSCATE("Java_org_libsdl_app_SDLActivity_onNativeTouch")), (void *) SDL_Input::hook, (void **) &SDL_Input::o_onNativeTouch) != 0)
        TLOGE("Input", "onNativeTouch hook error!");
}
#endif


#if defined(RENDER_VULKAN)
#  include "Vulkan.h"
#elif defined(RENDER_EGL)
EGLBoolean (*o_swapbuffers)(EGLDisplay dpy, EGLSurface surf) = nullptr;
EGLBoolean swapbuffers_hook(EGLDisplay dpy, EGLSurface surf) {
    EGLint w, h;
    eglQuerySurface(dpy, surf, EGL_WIDTH, &w);
    eglQuerySurface(dpy, surf, EGL_HEIGHT, &h);

    Menu::InitializeContext(w, h);
    Menu::DrawFrame(w, h);

    return o_swapbuffers(dpy, surf);
}
#elif defined(RENDER_JAVA_EGL)
extern "C"{
JNIEXPORT void JNICALL
Java_com_horoni_menu_NativeMethods_onDrawFrame(JNIEnv *env, jclass clazz) {
    Menu::DrawFrame();
}
JNIEXPORT void JNICALL
Java_com_horoni_menu_NativeMethods_onSurfaceChanged(JNIEnv *env, jclass clazz, jint width, jint height) {
    Menu::InitializeContext(width, height);
}
JNIEXPORT void JNICALL
Java_com_horoni_menu_NativeMethods_onSurfaceCreated(JNIEnv *env, jclass clazz) {
    return;
}
}
#endif

static void hook_render() {
    #if defined(RENDER_EGL)
    auto swapBuffers = ((uintptr_t) DobbySymbolResolver(OBFUSCATE("libEGL.so"), OBFUSCATE("eglSwapBuffers")));
    KittyMemory::setAddressProtection((void *)swapBuffers, sizeof(swapBuffers), PROT_READ | PROT_WRITE | PROT_EXEC);
    DobbyHook((void *) swapBuffers, (void *) swapbuffers_hook, (void **) &o_swapbuffers);
    #elif defined(RENDER_VULKAN)
    VK::Hook();
    #endif
}
static void hook_input() {
    input_hook();
}

void initMenuHooks() {
    #if defined(RENDER_EGL)
    #define targetLib OBFUSCATE("libEGL.so")

    do { // This loop May be root of some problems
      usleep(100000); // sleep for 100ms
    } while (!isLibraryLoaded(targetLib));
    sleep(1);

    #endif
    hook_render();
    hook_input();

    LOGI("ImGUI Hooks initialized");
}
