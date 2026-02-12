#ifndef HORONI_JNIHELPER_HPP
#define HORONI_JNIHELPER_HPP

#include <jni.h>
#include <pthread.h>

class JNIHelper {
private:
    inline static JavaVM* g_vm = nullptr;
    inline static pthread_key_t g_thread_key;
    inline static bool g_is_key_created = false;

    static void DetachThreadDestructor(void* value);
    static void MakeKey();

    static JavaVM* FindJavaVM();

public:
    static void Init(JavaVM* vm);

    static JNIEnv* GetEnv();
    static JavaVM* GetVM();
};

#endif // HORONI_JNIHELPER_HPP
