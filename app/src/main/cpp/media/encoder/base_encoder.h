//
// 基础编码器
//

#ifndef FFMPEGDEV_BASE_ENCODER_H
#define FFMPEGDEV_BASE_ENCODER_H


#include "i_encoder.h"
#include <thread>
#include <mutex>
#include "../muxer/mp4_muxer.h"
#include "../../utils/common-define.h"
#include "i_encode_state_cb.h"
#include <queue>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/frame.h>
};

class BaseEncoder : public IEncoder {


public:
    BaseEncoder(JNIEnv *env, Mp4Muxer *muxer, AVCodecID codec_id);

    void PushFrame(OneFrame *one_frame) override;

    bool TooMuchData() override {
        return src_frames_.size() > 100;
    }

    void SetStateReceiver(IEncodeStateCb *cb) override {
        this->state_cb_ = cb;
    }

protected:

    Mp4Muxer *muxer_ = NULL;

    virtual void InitContext(AVCodecContext *codec_ctx) = 0;

    virtual int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) = 0;

    virtual AVFrame *DealFrame(OneFrame *one_frame) = 0;

    virtual void Release() = 0;

    /**
     * Log前缀
     */
    virtual const char *const LogSpec() = 0;

private:

    const char *TAG = "BaseEncoder";

    // 编码格式 ID
    AVCodecID codec_id_;

    // 线程依附的JVM环境
    JavaVM *jvm_for_thread_ = NULL;

    // 线程等待锁变量
    pthread_mutex_t mutex_ = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_ = PTHREAD_COND_INITIALIZER;

    // 编码器
    AVCodec *codec_ = NULL;

    // 编码上下文
    AVCodecContext *codec_ctx_ = NULL;

    // 编码数据包
    AVPacket *encoded_pkt_ = NULL;

    // 写入Mp4的输入流索引
    int encode_stream_index_ = 0;

    // 原数据时间基
    AVRational src_time_base_;

    // 缓冲队列
    std::queue<OneFrame *> src_frames_;

    // 操作数据锁
    std::mutex frames_lock_;

    // 状态回调
    IEncodeStateCb *state_cb_ = NULL;

    bool Init();

    /**
     * 循环拉去已经编码的数据，直到没有数据或者编码完毕
     * @return true 编码结束；false 编码未完成
     */
    bool DrainEncode();

    /**
     * 编码一帧数据
     * @return 错误信息
     */
    int EncodeOneFrame();

    /**
     * 新建编码线程
     */
    void CreateEncodeThread();

    /**
     * 解码静态方法，给线程调用
     */
    static void Encode(std::shared_ptr<BaseEncoder> that);

    void OpenEncoder();

    /**
     * 循环编码
     */
    void LoopEncode();

    void DoRelease();

    void Wait(int second = 0) {
        pthread_mutex_lock(&mutex_);
        pthread_cond_wait(&cond_, &mutex_);
        pthread_mutex_unlock(&mutex_);
    }

    void SendSignal() {
        pthread_mutex_lock(&mutex_);
        pthread_cond_signal(&cond_);
        pthread_mutex_unlock(&mutex_);
    }

};


#endif //FFMPEGDEV_BASE_ENCODER_H
