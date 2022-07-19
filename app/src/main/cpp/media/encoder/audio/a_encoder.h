//
// 音频编码器
//

#ifndef FFMPEGDEV_AUDIO_ENCODER_H
#define FFMPEGDEV_AUDIO_ENCODER_H


#include "../base_encoder.h"

extern "C" {
#include <libswresample/swresample.h>
};

class AudioEncoder : public BaseEncoder {


public:
    AudioEncoder(JNIEnv *env, Mp4Muxer *muxer);

protected:
    void InitContext(AVCodecContext *codec_ctx) override;

    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;

    AVFrame *DealFrame(OneFrame *one_frame) override;

    void Release() override;

    const char *const LogSpec() override {
        return "音频";
    };

private:
    AVFrame *frame_ = NULL;

    void InitFrame();

};


#endif //FFMPEGDEV_AUDIO_ENCODER_H
