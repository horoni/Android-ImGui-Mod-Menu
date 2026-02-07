#ifndef DAWN_LOGGER_H
#define DAWN_LOGGER_H

#include <android/log.h>

enum daLogType {
    daDEBUG = 3,
    daERROR = 6,
    daINFO = 4,
    daWARN = 5
};

#define TAG OBFUSCATE("ModMenu")

#define LOGD(...) ((void)__android_log_print(daDEBUG, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(daERROR, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(daINFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(daWARN,  TAG, __VA_ARGS__))

#define TLOGD(tag, ...) ((void)__android_log_print(daDEBUG, tag, __VA_ARGS__))
#define TLOGE(tag, ...) ((void)__android_log_print(daERROR, tag, __VA_ARGS__))
#define TLOGI(tag, ...) ((void)__android_log_print(daINFO,  tag, __VA_ARGS__))
#define TLOGW(tag, ...) ((void)__android_log_print(daWARN,  tag, __VA_ARGS__))

#endif