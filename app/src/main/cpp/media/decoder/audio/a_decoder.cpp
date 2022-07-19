//
// Created by wyy on 2022/7/18.
//

#include "a_decoder.h"
#include "../../const.h"

AudioDecoder::AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer) : BaseDecoder(
        env, path, forSynthesizer) {

}

AudioDecoder::~AudioDecoder() {
    if (render_ != NULL) {
        delete render_;
    }
}

void AudioDecoder::SetRender(AudioRender *render) {
    render_ = render;
}

void AudioDecoder::Prepare(JNIEnv *env) {
    InitSwr();
    CalculateSampleArgs();
    InitOutBuffer();
    InitRender();
}

void AudioDecoder::InitSwr() {

    AVCodecContext *codeCtx = codec_cxt();

    //初始化格式转换工具
    swr_ = swr_alloc();

    av_opt_set_int(swr_, "in_channel_layout", codeCtx->channel_layout, 0);
    av_opt_set_int(swr_, "out_channel_layout", ENCODE_AUDIO_DEST_CHANNEL_LAYOUT, 0);

    av_opt_set_int(swr_, "in_sample_rate", codeCtx->sample_rate, 0);
    av_opt_set_int(swr_, "out_sample_rate", GetSampleRate(codeCtx->sample_rate), 0);

    av_opt_set_sample_fmt(swr_, "in_sample_fmt", codeCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr_, "out_sample_fmt", GetSampleFmt(),  0);

    swr_init(swr_);

    LOGI(TAG, "sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
         codeCtx->sample_rate, codeCtx->channels, codeCtx->sample_fmt, codeCtx->frame_size,codeCtx->channel_layout);
}

void AudioDecoder::CalculateSampleArgs() {
    // 重采样后一个通道采样数
    dest_nb_sample_ = (int)av_rescale_rnd(ACC_NB_SAMPLES, GetSampleRate(codec_cxt()->sample_rate),
                                           codec_cxt()->sample_rate, AV_ROUND_UP);

    // 重采样后一帧数据的大小
    dest_data_size_ = (size_t)av_samples_get_buffer_size(
            NULL, ENCODE_AUDIO_DEST_CHANNEL_COUNTS,
            dest_nb_sample_, GetSampleFmt(), 1);
}

void AudioDecoder::InitOutBuffer() {
    if (ForSynthesizer()) {
//        if (m_out_buffer[0] == NULL) {
        out_buffer_[0] = (uint8_t *) malloc(dest_data_size_ / 2);
        out_buffer_[1] = (uint8_t *) malloc(dest_data_size_ / 2);
//        }
    } else {
        out_buffer_[0] = (uint8_t *) malloc(dest_data_size_);
    }
}

void AudioDecoder::InitRender() {
    if (render_ != NULL) {
        render_->InitRender();
    }
};

void AudioDecoder::Render(AVFrame *frame) {

    InitOutBuffer();

    // 转换，返回每个通道的样本数
    int ret = swr_convert(swr_, out_buffer_, dest_data_size_/2,
                          (const uint8_t **) frame->data, frame->nb_samples);

    if (ret > 0) {
        if (ForSynthesizer()) {
            if (state_cb_ != NULL) {
                OneFrame *one_frame = new OneFrame(out_buffer_[0], dest_data_size_, frame->pts,
                                                   time_base(), out_buffer_[1], true);
                if (state_cb_->DecodeOneFrame(this, one_frame)) {
                    Wait(0, 200);
                }
            }
        } else {
            render_->Render(out_buffer_[0], (size_t) dest_data_size_);
        }
    }
}

void AudioDecoder::Release() {
    if (swr_ != NULL) {
        swr_free(&swr_);
    }
    if (render_ != NULL) {
        render_->ReleaseRender();
    }
    ReleaseOutBuffer();
}

void AudioDecoder::ReleaseOutBuffer() {
    if (out_buffer_[0] != NULL) {
        free(out_buffer_[0]);
        out_buffer_[0] = NULL;
    }
}