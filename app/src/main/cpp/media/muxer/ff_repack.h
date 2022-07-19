//
// Created by cxp on 2020-08-01.
//

#ifndef LEARNINGVIDEO_FF_REPACK_H
#define LEARNINGVIDEO_FF_REPACK_H


#include <jni.h>

extern "C" {
#include <libavformat/avformat.h>
};

class FFRepack {


public:
    FFRepack(JNIEnv *env, jstring in_path, jstring out_path);

    void Start();

    void Write(AVPacket pkt);

    void Release();

private:
    const char *TAG = "FFRepack";

    AVFormatContext *in_format_cxt_ = NULL;

    AVFormatContext *out_format_cxt_ = NULL;

    int OpenSrcFile(const char *srcPath);

    int InitMuxerParams(const char *destPath);
};


#endif //LEARNINGVIDEO_FF_REPACK_H
