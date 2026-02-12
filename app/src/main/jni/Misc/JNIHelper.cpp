#include "JNIHelper.hpp"

#include <android/log.h>
#include "xdl.h"

#define LOG_TAG "JNI_HELPER"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


typedef jint (*JNI_GetJVMs)(JavaVM**, jsize, jsize*);

void JNIHelper::DetachThreadDestructor(void* value) {
    if (value != nullptr && g_vm != nullptr) {
        g_vm->DetachCurrentThread();
    }
}

void JNIHelper::MakeKey() {
    pthread_key_create(&g_thread_key, DetachThreadDestructor);
    g_is_key_created = true;
}

JavaVM* JNIHelper::FindJavaVM() {
    JavaVM* vm;
    jsize count = 0;
    void* handle = handle = xdl_open("libart.so", XDL_TRY_FORCE_LOAD);

    if (!handle) {
        LOGE("Failed to dlopen libart.so");
        return nullptr;
    }

    auto fn = (JNI_GetJVMs)xdl_sym(handle, "JNI_GetCreatedJavaVMs", nullptr);

    if (!fn) {
        LOGE("Failed to find JNI_GetCreatedJavaVMs symbol");
        return nullptr;
    }

    if (fn(&vm, 1, &count) == JNI_OK && count > 0) {
        LOGI("Found JavaVM via libart.so!");
        return vm;
    }

    LOGE("JNI_GetCreatedJavaVMs returned 0 VMs");
    return nullptr;
}

void JNIHelper::Init(JavaVM* vm) {
    if (g_vm == nullptr) g_vm = vm;
    static pthread_once_t key_once = PTHREAD_ONCE_INIT;
    pthread_once(&key_once, MakeKey);
}

JNIEnv* JNIHelper::GetEnv() {
    JNIEnv* env = nullptr;
    if (g_is_key_created && (env = (JNIEnv*)pthread_getspecific(g_thread_key))) return env;
    if (g_vm == nullptr) {
        g_vm = FindJavaVM();
        if (g_vm == nullptr) {
            return nullptr;
        }
        static pthread_once_t key_once = PTHREAD_ONCE_INIT;
        pthread_once(&key_once, MakeKey);
    }

    int status = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (status == JNI_EDETACHED) {
        if (g_vm->AttachCurrentThread(&env, nullptr) != 0) {
            return nullptr;
        }
        if (g_is_key_created) {
            pthread_setspecific(g_thread_key, (void*)env);
        }
    }
    return env;
}

JavaVM* JNIHelper::GetVM() {
    if (!g_vm) g_vm = FindJavaVM();
    return g_vm;
}

