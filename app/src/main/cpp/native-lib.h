//
// Created by wyy on 2022/7/7.
//
#ifndef FFMPEGDEV_NATIVE_LIB_H
#define FFMPEGDEV_NATIVE_LIB_H
void callback(JNIEnv *env, uint8_t *buf, int channel, int width, int height);

void status_back(JNIEnv *env, int code, const char* log);

#endif //FFMPEGDEV_NATIVE_LIB_H
