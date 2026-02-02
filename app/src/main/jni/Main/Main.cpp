#include <thread>

#include <Obfuscate.h>
#include <Logger.h>
#include <ImGuiHook.h>
#include <BNM/Loading.hpp>

#include <Offsets.h>
#include <Hooks.h>

#define OBFS(...) OBFUSCATE(__VA_ARGS__)

/* If i will sell cheat or release to public access
 * 1. Configure OBFUSCATE_BNM in GlobalSettings.hpp
 * 2. Wrap all strings to OBFUSCATE()
 * 3. Update Obfuscate.h
 * 4. Obfuscate cheat library with `oxorany`
 * 5. Library Remap
 */

// Call anything from JNI_OnLoad here
extern "C" {
    // JNI Support
    JavaVM *jvm = nullptr;
    JNIEnv *env = nullptr;

    __attribute__((visibility ("default")))
    jint JNI_OnLoad(JavaVM *vm, [[maybe_unused]] void *reserved) {
    //jint loadJNI(JavaVM *vm, [[maybe_unused]] void *reserved) {
        jvm = vm;
        vm->AttachCurrentThread(&env, nullptr);
        
        BNM::Loading::TryLoadByJNI(env);
        BNM::Loading::AddOnLoadedEvent(ofst::Init);
        BNM::Loading::AddOnLoadedEvent(Setup_Hooks);
        
        LOGI("loadJNI(): Initialized");

        return JNI_VERSION_1_6;
    }
}

__attribute__((constructor))
void init() {
    std::thread(initMenuHooks).detach();
}
