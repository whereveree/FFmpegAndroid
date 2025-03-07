#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include "ff_audio_player.h"

#define LOG_TAG "FFmpeg_Native"
#define LOG_I(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static jclass FFmpegCmd = nullptr;

extern "C" JNIEXPORT JNICALL jstring getInfo(JNIEnv *env, jclass clazz) {
    std::string ffmpeg_version = av_version_info();
    LOG_I("ffmpeg version: %s", ffmpeg_version.c_str());
    return env->NewStringUTF(ffmpeg_version.c_str());
}

static const JNINativeMethod gMethods[] = {
        {"getInfo", "()Ljava/lang/String;", (void *) getInfo},
};

extern "C" JNIEXPORT JNICALL jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;

    auto result = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (result != JNI_OK) {
        return JNI_ERR;
    }

    //查找Java类并将其实例化
    jclass clazz = env->FindClass("com/frank/ffmpeg/FFmpegCmd");
    if (!clazz) {
        return JNI_ERR;
    }

    //持久化Java对象
    FFmpegCmd = reinterpret_cast<jclass>(env->NewGlobalRef(clazz));
    if (!FFmpegCmd) {
        return JNI_ERR;
    }

    //注册映射函数,不等于0为异常情况
    auto size = env->RegisterNatives(clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0]));
    if (size != 0) {
        return JNI_ERR;
    }

    auto android_api = android_get_device_api_level();
    LOG_I("android api version: %d", android_api);

    return JNI_VERSION_1_6;
}