#include <jni.h>
#include <unistd.h>
#include <android/log.h>

#define LOG_V(LOG_TAG, ...) __android_log_print(ANDROID_LOG_VERBOSE,    LOG_TAG, __VA_ARGS__)
#define LOG_D(LOG_TAG, ...) __android_log_print(ANDROID_LOG_DEBUG,      LOG_TAG, __VA_ARGS__)
#define LOG_I(LOG_TAG, ...) __android_log_print(ANDROID_LOG_INFO,       LOG_TAG, __VA_ARGS__)
#define LOG_W(LOG_TAG, ...) __android_log_print(ANDROID_LOG_WARN,       LOG_TAG, __VA_ARGS__)
#define LOG_E(LOG_TAG, ...) __android_log_print(ANDROID_LOG_ERROR,      LOG_TAG, __VA_ARGS__)

#define JNI_EXPAND_AND_JOIN(pkg, cls, func) Java_##pkg##_##cls##_##func

typedef int64_t Long;
typedef int32_t Int;
typedef int16_t Short;
typedef int8_t Byte;

// 统一封装 JNI 方法声明与实现
#define JNI_FUN(CLASS_NAME, FUNC_NAME, RETURN_TYPE, ...) \
    extern "C" JNIEXPORT RETURN_TYPE JNICALL JNI_EXPAND_AND_JOIN(com_frank_ffmpeg, CLASS_NAME, FUNC_NAME) \
    (JNIEnv *env, jobject thiz, ##__VA_ARGS__)

#define JNI_FUN_AUDIO(FUNC_NAME, RETURN_TYPE, ...) \
    extern "C" JNIEXPORT RETURN_TYPE JNICALL JNI_EXPAND_AND_JOIN(com_frank_ffmpeg, AudioPlayer, FUNC_NAME) \
    (JNIEnv *env, jobject thiz, ##__VA_ARGS__)