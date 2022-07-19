//
// Created by wyy on 2022/7/18.
//

#include "v_decoder.h"
#include "../../one_frame.h"


VideoDecoder::VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer)
        : BaseDecoder(env, path, for_synthesizer) {
}

VideoDecoder::~VideoDecoder() {
    delete video_render_;
}

void VideoDecoder::SetRender(VideoRender *render) {
    this->video_render_ = render;
}

void VideoDecoder::Prepare(JNIEnv *env) {
    InitRender(env);
    InitBuffer();
    InitSws();
}

void VideoDecoder::InitRender(JNIEnv *env) {
    if (video_render_ != NULL) {
        int dst_size[2] = {-1, -1};
        video_render_->InitRender(env, width(), height(), dst_size);

        dst_w_= dst_size[0];
        dst_h_ = dst_size[1];
        if (dst_w_ == -1) {
            dst_w_ = width();
        }
        if (dst_h_ == -1) {
            dst_h_ = height();
        }
        LOGI(TAG, "dst %d, %d", dst_w_, dst_h_);
    } else {
        LOGE(TAG, "Init render error, you should call SetRender first!");
    }
}

void VideoDecoder::InitBuffer() {
    rgb_frame_ = av_frame_alloc();
    // 获取缓存大小
    int numBytes = av_image_get_buffer_size(DST_FORMAT, dst_w_, dst_h_, 1);
    // 分配内存
    buf_for_rgb_frame_ = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    // 将内存分配给RgbFrame，并将内存格式化为三个通道后，分别保存其地址
    av_image_fill_arrays(rgb_frame_->data, rgb_frame_->linesize,
                         buf_for_rgb_frame_, DST_FORMAT, dst_w_, dst_h_, 1);
}

void VideoDecoder::InitSws() {
    // 初始化格式转换工具
    sws_ctx_ = sws_getContext(width(), height(), video_pixel_format(),
                               dst_w_, dst_h_, DST_FORMAT,
                               SWS_FAST_BILINEAR, NULL, NULL, NULL);
}

void VideoDecoder::Render(AVFrame *frame) {
    sws_scale(sws_ctx_, frame->data, frame->linesize, 0,
              height(), rgb_frame_->data, rgb_frame_->linesize);
    OneFrame * one_frame = new OneFrame(rgb_frame_->data[0], rgb_frame_->linesize[0], frame->pts, time_base(), NULL, false);
    video_render_->Render(one_frame);

    if (state_cb_ != NULL) {
        if (state_cb_->DecodeOneFrame(this, one_frame)) {
            Wait(0, 200);
        }
    }
}

bool VideoDecoder::NeedLoopDecode() {
    return true;
}

void VideoDecoder::Release() {
    LOGE(TAG, "[VIDEO] release");
    if (rgb_frame_ != NULL) {
        av_frame_free(&rgb_frame_);
        rgb_frame_ = NULL;
    }
    if (buf_for_rgb_frame_ != NULL) {
        free(buf_for_rgb_frame_);
        buf_for_rgb_frame_ = NULL;
    }
    if (sws_ctx_ != NULL) {
        sws_freeContext(sws_ctx_);
        sws_ctx_ = NULL;
    }
    if (video_render_ != NULL) {
        video_render_->ReleaseRender();
        video_render_ = NULL;
    }
}