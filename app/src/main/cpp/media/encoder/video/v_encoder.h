//
// 视频编码器
// Author: Chen Xiaoping
// Create Date: 2019-11-14.
//

#ifndef FFMPEGDEV_V_ENCODER_H
#define FFMPEGDEV_V_ENCODER_H


#include "../base_encoder.h"

class VideoEncoder : public BaseEncoder {


public:
    VideoEncoder(JNIEnv *env, Mp4Muxer *muxer, int width, int height);

protected:

    const char *const LogSpec() override {
        return "视频";
    };

    void InitContext(AVCodecContext *codec_ctx) override;

    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;

    AVFrame *DealFrame(OneFrame *one_frame) override;

    void Release() override;

private:

    const char *TAG = "VideoEncoder";

    SwsContext *sws_ctx_ = NULL;

    AVFrame *yuv_frame_ = NULL;

    int width_ = 0, height_ = 0;

    void InitYUVFrame();

};


#endif //FFMPEGDEV_V_ENCODER_H
