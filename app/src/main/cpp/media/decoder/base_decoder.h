//
// Created by wyy on 2022/7/14.
//
#ifndef FFMPEGDEV_BASE_DECODER_H
#define FFMPEGDEV_BASE_DECODER_H

#include "../../utils/common-define.h"
#include "i_decoder.h"
#include "decode_state.h"
#include <string>
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat//avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
};

class BaseDecoder : public IDecoder {

public:

    BaseDecoder(JNIEnv *env, jstring path, bool for_synthesizer);

    virtual ~BaseDecoder();

    /**
     * 视频宽度
     * @return
     */
    int width() {
        return codec_ctx_->width;
    }

    /**
     * 视频高度
     * @return
     */
    int height() {
        return codec_ctx_->height;
    }

    /**
     * 视频长度
     * @return
     */

    long duration() {
        return duration_;
    }


    virtual void GoOn() override;

    virtual void Pause() override;

    virtual void Stop() override;

    virtual bool IsRunning() override;

    virtual long GetDuration() override;

    virtual long GetCurPos() override;

    void SetStateReceiver(IDecodeStateCb *cb) override{
    }

protected:
    IDecodeStateCb * state_cb_=NULL;
    /**
    * 是否为合成器提供解码
    * @return true 为合成器提供解码 false 解码播放
    */
    bool ForSynthesizer() {
        return for_synthesizer_;
    }

    const char * path() {
        return path_;
    }


    /**
     * 解码器上下文
     * @return
     */
    AVCodecContext *codec_cxt() {
        return codec_ctx_;
    }

    /**
     * 视频数据编码格式
     * @return
     */
    AVPixelFormat video_pixel_format() {
        return codec_ctx_->pix_fmt;
    }

    /**
     * 获取解码时间基
     */
    AVRational time_base() {
        return fmt_ctx_->streams[stream_index_]->time_base;
    }

    /**
     * 解码一帧数据
     * @return
     */
    AVFrame* DecodeOneFrame();

    /**
     * 音视频索引
     */
    virtual AVMediaType GetMediaType() = 0;

    /**
     * 是否需要自动循环解码
     */
    virtual bool NeedLoopDecode() = 0;

    /**
     * 子类准备回调方法
     * @note 注：在解码线程中回调
     * @param env 解码线程绑定的JVM环境
     */
    virtual void Prepare(JNIEnv *env) = 0;

    /**
     * 子类渲染回调方法
     * @note 注：在解码线程中回调
     * @param frame 视频：一帧YUV数据；音频：一帧PCM数据
     */
    virtual void Render(AVFrame *frame) = 0;

    /**
     * 子类释放资源回调方法
     */
    virtual void Release() = 0;

    /**
     * Log前缀
     */
    virtual const char *const LogSpec() = 0;

    /**
     * 进入等待
     */
    void Wait(long second = 0, long ms = 0);

    /**
     * 恢复解码
     */
    void SendSignal();

    void CallbackState(DecodeState status);

private:
    const char *TAG = "BaseDecoder";
    //================定义解码相关============
    // 解码信息上下文
    AVFormatContext *fmt_ctx_ = NULL;
    // 解码上下文
    AVCodecContext *codec_ctx_ = NULL;
    // 解码器
    AVCodec *codec_ = NULL;
    // 待解码包
    AVPacket *packet_;
    //最终解码数据
    AVFrame *frame_ = NULL;
    //当前播放时间
    int64_t cur_t_s_ = 0;

    //总时长
    long duration_ = 0;

    // 开始播放时间
    int64_t started_t_ = -1;

    //解码状态
    DecodeState state_ = STOP;

    //数据流索引
    int stream_index_ = -1;

    // -------------定义线程相关--------------
    // 线程依附的JVM 环境
    JavaVM *jvm_for_thread_ = NULL;
    // 原始路径jstring 引用，否则无法在线程中操作
    jobject path_ref_ = NULL;

    // 经过转换过的路径 常量指针， 指针指向的值不变，指针常量：指针不变，值可变
    const char *path_ = NULL;

    pthread_mutex_t  mutex_=PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_ = PTHREAD_COND_INITIALIZER;

    // 为合成器提供解码
    bool for_synthesizer_ = false;
    // ---------------私有方法-----------------------
    /**
     * 初始化 jvm 环境
     * @param env
     * @param path
     */
    void Init(JNIEnv *env, jstring path);

    /**
     * 初始化FFmpeg 相关参数
     * @param env
     */
    void InitFFmpegDecoder(JNIEnv *env);

    /**
     * 分配解码过程中需要的缓存
     */
    void AllocFrameBuffer();

    /**
     * 新建解码线程
     */
    void CreateDecodeThread();

    /**
     * 循环解码
     */
    void LoopDecode();

    /**
     * 获取当前帧时间戳
     */
    /**
   * 获取当前帧时间戳
   */
    void ObtainTimeStamp();

    /**
     * 解码完成
     */

    void DoneDecode(JNIEnv *env);

    /**
     * 静态解码方法，用于解码线程回调
     */
    static void Decode(std::shared_ptr<BaseDecoder> that);

    /**
     * 时间同步
     */
    void SyncRender();
};

#endif //FFMPEGDEV_BASE_DECODER_H
