#include <jni.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "Logger.h"
#include "misc/RemapTools.h"
#include "Utils.h"

enum config_enum {
    Load_From_Data      = 0b00000001,
    Remap               = 0b00000010,
    Call_JNI            = 0b00000100,
};

char config;

int  load_and_remap(JavaVM *vm, void *reserved) {
    // get /data/data/[package]/
    std::string local_filestore = "/data/data/" + std::string(GetPackageName()) + "/";
    std::string config_path = local_filestore + "conf";
    
    // Config reading
    FILE *config_f = fopen(config_path.c_str(), "r+b");
    if (config_f) {
        fread(&config, sizeof(char), 1, config_f);
    } else {
        LOGW("No conf: using default config and saving");
        config = config_enum::Load_From_Data |
                config_enum::Remap |
                config_enum::Call_JNI;
        config_f = fopen(config_path.c_str(), "w+b");
        fwrite(&config, sizeof(char), 1, config_f);
    }
    fclose(config_f);
    
    // Get native libraries directory
    // TODO: make this work when apk does not have extractNativeLibs flag
    std::string native;
    if (!(config & config_enum::Load_From_Data)) {
        native = GetNativeLibraryDirectory();
        if (native.empty()) {
            LOGE("Error getting native library directory");
            return JNI_ERR;
        }
        LOGI("Found native library directory: %s", native.c_str());
    }
    
    std::string path;
    if (config & config_enum::Load_From_Data)
         path = local_filestore + "libMCL.so";
    else path = native + "libMCL.so";
    
    // Open the library containing the actual code
    void *open = dlopen(path.c_str(), RTLD_NOW);
    if (open == nullptr) {
        LOGE("Error opening libMCL.so %s", dlerror());
        return JNI_ERR;
    }

    // Call JNI in library
    if (config & config_enum::Call_JNI) {
        void *jni_load = dlsym(open, "loadJNI");
        if (jni_load == nullptr) {
            LOGE("Failed to find symbol loadJNI()");
            return JNI_ERR;
        }

        auto loadJNI = (jint (*)(JavaVM *vm, void *reserved)) jni_load;
        jint jni = loadJNI(vm, reserved); // Call function in main library
    }
    
    if (config & config_enum::Remap) {
        RemapTools::RemapLibrary("libMCL.so");
    }
    
    return JNI_VERSION_1_6;
}

static void* unityLibraryHandle = nullptr;

extern "C" {

typedef jint (*JNI_OnLoad_t)(JavaVM *vm, void *reserved);

JNIEXPORT jboolean JNICALL
load(JNIEnv *env, jobject activityObject, jstring path) {
    LOGI("load(3 args)");
    const char *libraryPath = env->GetStringUTFChars(path, nullptr);
    LOGI("%s", libraryPath);
    if (!libraryPath) {
        return JNI_FALSE; // Failed to obtain library path
    }

    char fullPath[1000];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", libraryPath, "libunity.so");

    unityLibraryHandle = dlopen(fullPath, RTLD_LAZY | RTLD_LOCAL);
    env->ReleaseStringUTFChars(path, libraryPath);

    if (!unityLibraryHandle) {
        return JNI_FALSE; // Failed to load Unity library
    }

    JNI_OnLoad_t jniOnLoad = reinterpret_cast<JNI_OnLoad_t>(dlsym(unityLibraryHandle, "JNI_OnLoad"));
    if (!jniOnLoad) {
        dlclose(unityLibraryHandle);
        unityLibraryHandle = nullptr;
        return JNI_FALSE; // JNI_OnLoad symbol not found
    }

    JavaVM *vm = nullptr;
    if (env->GetJavaVM(&vm) != JNI_OK) {
        dlclose(unityLibraryHandle);
        unityLibraryHandle = nullptr;
        return JNI_FALSE; // Failed to obtain Java VM
    }

    jint result = jniOnLoad(vm, nullptr);
    if (result < JNI_VERSION_1_6) {
        dlclose(unityLibraryHandle);
        unityLibraryHandle = nullptr;
        return JNI_FALSE; // JNI version mismatch
    }

    return JNI_TRUE; // Successfully loaded Unity library
}

JNIEXPORT jboolean JNICALL
unload(JNIEnv *env, jclass activityObject) {
    LOGI("unload(2 args)");
    if (unityLibraryHandle) {
        dlclose(unityLibraryHandle);
        unityLibraryHandle = nullptr;
    }
    return JNI_TRUE;
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnLoad(2 args)");
    LOGI("package: %s", GetPackageName());
    LOGI("pid: %d", getpid());
    JNIEnv *globalEnv;
    if (vm->GetEnv(reinterpret_cast<void**>(&globalEnv), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // Failed to obtain JNIEnv
    }

    jclass clazz = globalEnv->FindClass("com/unity3d/player/NativeLoader");
    if (!clazz) {
        return JNI_ERR; // Class not found
    }

    static const JNINativeMethod methods[] = {
        {"load",   "(Ljava/lang/String;)Z", reinterpret_cast<void *>(load)},
        {"unload", "()Z",                   reinterpret_cast<void *>(unload)}
    };

    jint ret = globalEnv->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(JNINativeMethod));
    if (ret != JNI_OK) {
        return ret; // Failed to register natives
    }
    
    switch(load_and_remap(vm, reserved)) {
        case JNI_VERSION_1_6:
            LOGI("Loaded&Remaped Successfuly");
            break;
        case JNI_ERR:
            LOGE("Loaded&Remaped Error");
            break;
    }
    
    return JNI_VERSION_1_6; // Successful initialization
}

} // extern "C"
