//
// Created by wyy on 2022/7/6.
//
#ifndef FFMPEGDEV_COMMON_DEFINE_H
#define FFMPEGDEV_COMMON_DEFINE_H

#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "FFmpegLog"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define UTF_8 "UTF-8"

#endif //FFMPEGDEV_COMMON_DEFINE_H
