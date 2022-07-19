//
//

#include <string.h>
#include "native_render.h"

NativeRender::NativeRender(JNIEnv *env, jobject surface) {
    surface_ref_ = env->NewGlobalRef(surface);
}

NativeRender::~NativeRender() {

}

void NativeRender::InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) {
    // 初始化窗口
    native_window_ = ANativeWindow_fromSurface(env, surface_ref_);

    // 绘制区域的宽高
    int windowWidth = ANativeWindow_getWidth(native_window_);
    int windowHeight = ANativeWindow_getHeight(native_window_);

    // 计算目标视频的宽高
    dst_w_ = windowWidth;
    dst_h_ = dst_w_ * video_height / video_width;
    if (dst_h_ > windowHeight) {
        dst_h_ = windowHeight;
        dst_w_ = windowHeight * video_width / video_height;
    }
    LOGE(TAG, "windowW: %d, windowH: %d, dstVideoW: %d, dstVideoH: %d",
         windowWidth, windowHeight, dst_w_, dst_h_);

    //设置宽高限制缓冲区中的像素数量
    ANativeWindow_setBuffersGeometry(native_window_, windowWidth,
                                     windowHeight, WINDOW_FORMAT_RGBA_8888);

    dst_size[0] = dst_w_;
    dst_size[1] = dst_h_;
}

void NativeRender::Render(OneFrame *one_frame) {
    //锁定窗口
    ANativeWindow_lock(native_window_, &out_buffer_, NULL);
    uint8_t *dst = (uint8_t *) out_buffer_.bits;
    // 获取stride：一行可以保存的内存像素数量*4（即：rgba的位数）
    int dstStride = out_buffer_.stride * 4;
    int srcStride = one_frame->line_size;

    // 由于window的stride和帧的stride不同，因此需要逐行复制
    for (int h = 0; h < dst_h_; h++) {
        memcpy(dst + h * dstStride, one_frame->data + h * srcStride, srcStride);
    }
    //释放窗口
    ANativeWindow_unlockAndPost(native_window_);
}

void NativeRender::ReleaseRender() {
    if (native_window_ != NULL) {
        ANativeWindow_release(native_window_);
    }
    av_free(&out_buffer_);
}