//
// Created by wyy on 2022/7/18.
//

#ifndef FFMPEGDEV_A_DECODER_H
#define FFMPEGDEV_A_DECODER_H

#include "../base_decoder.h"
#include "../../render/audio/audio_render.h"
#include "../../const.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
};

class AudioDecoder : public BaseDecoder {

public:
    AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer);

    ~AudioDecoder();

    void SetRender(AudioRender *render);

protected:
    void Prepare(JNIEnv *env) override;

    void Render(AVFrame *frame) override;

    void Release() override;

    bool NeedLoopDecode() override {
        return true;
    }

    AVMediaType GetMediaType() override {
        return AVMEDIA_TYPE_AUDIO;
    }

    const char *const LogSpec() override {
        return "AUDIO";
    };

private:

    const char *TAG = "AudioDecoder";

    // 音频转换器
    SwrContext *swr_ = NULL;

    // OpeSL ES音频播放器
    AudioRender *render_ = NULL;

    // 输出缓冲
    uint8_t *out_buffer_[2] = {NULL, NULL};

    // 重采样后，每个通道包含的采样数
    // acc默认为1024，重采样后可能会变化
    int dest_nb_sample_ = 1024;

    // 重采样以后，一帧数据的大小
    size_t dest_data_size_ = 0;

    /**
     * 初始化转换工具
     */
    void InitSwr();

    /**
     * 计算重采样后通道采样数和帧数据大小
     */
    void CalculateSampleArgs();

    /**
     * 初始化输出缓冲
     */
    void InitOutBuffer();

    /**
     * 初始化渲染器
     */
    void InitRender();

    /**
     * 释放缓冲区
     */
    void ReleaseOutBuffer();

    /**
     * 采样格式：16位
     */
    AVSampleFormat GetSampleFmt() {
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_FORMAT;
        } else {
            return AV_SAMPLE_FMT_S16;
        }
    }

    /**
     * 采样率
     */
    int GetSampleRate(int spr) {
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_SAMPLE_RATE;
        } else {
            return spr;
        }
    }

};


#endif //FFMPEGDEV_A_DECODER_H
