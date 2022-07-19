//
// Created by wyy on 2022/7/18.
//

#ifndef FFMPEGDEV_V_DECODER_H
#define FFMPEGDEV_V_DECODER_H

#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "../../render/video/video_render.h"
#include "../../../utils/common-define.h"
#include "../base_decoder.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

class VideoDecoder : public BaseDecoder {

public:
    VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer = false);

    ~VideoDecoder();

    void SetRender(VideoRender *render);

//    VideoDecoder(const VideoDecoder &) = delete;
//
//    VideoDecoder &operator=(const VideoDecoder &) = delete;

protected:
    AVMediaType GetMediaType() override {
        return AVMEDIA_TYPE_VIDEO;
    }

    /**
     * 是否需要循环解码
     */
    bool NeedLoopDecode() override;

    /**
     * 准备解码环境
     * 注：在解码线程中回调
     * @param env 解码线程绑定的jni环境
     */
    void Prepare(JNIEnv *env) override;

    /**
     * 渲染
     * 注：在解码线程中回调
     * @param frame 解码RGBA数据
     */
    void Render(AVFrame *frame) override;

    /**
     * 释放回调
     */
    void Release() override;

    const char *const LogSpec() override {
        return "VIDEO";
    };

private:
    const char *TAG = "VideoDecoder";

    //视频数据目标格式
    const AVPixelFormat DST_FORMAT = AV_PIX_FMT_RGBA;

    //存放YUV转换为RGB后的数据
    AVFrame *rgb_frame_ = NULL;

    uint8_t *buf_for_rgb_frame_ = NULL;

    //视频格式转换器
    SwsContext *sws_ctx_ = NULL;

    //视频渲染器
    VideoRender *video_render_ = NULL;

    //显示的目标宽
    int dst_w_;
    //显示的目标高
    int dst_h_;

    /**
     * 初始化渲染器
     */
    void InitRender(JNIEnv *env);

    /**
     * 初始化显示器
     * @param env
     */
    void InitBuffer();

    /**
     * 初始化视频数据转换器
     */
    void InitSws();

};
#endif //FFMPEGDEV_V_DECODER_H
