//
// Created by cxp on 2019-08-06.
//

#ifndef FFMPEGDEV_NATIVE_RENDER_H
#define FFMPEGDEV_NATIVE_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "../video_render.h"
#include "../../../../utils/common-define.h"

extern "C" {
#include <libavutil/mem.h>
};

class NativeRender : public VideoRender {

public:
    NativeRender(JNIEnv *env, jobject surface);

    ~NativeRender();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override;

    void Render(OneFrame *one_frame) override;

    void ReleaseRender() override;

private:
    const char *TAG = "NativeRender";

    // Surface引用，必须使用引用，否则无法在线程中操作
    jobject surface_ref_ = NULL;

    // 存放输出到屏幕的缓存数据
    ANativeWindow_Buffer out_buffer_;

    // 本地窗口
    ANativeWindow *native_window_ = NULL;

    //显示的目标宽
    int dst_w_;

    //显示的目标高
    int dst_h_;

};


#endif //FFMPEGDEV_NATIVE_RENDER_H
