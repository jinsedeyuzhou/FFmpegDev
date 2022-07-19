//
// Created by wyy on 2022/7/14.
//
#include "base_decoder.h"
#include "../../utils/timer.c"

BaseDecoder::BaseDecoder(JNIEnv *env, jstring path, bool for_synthesizer) : for_synthesizer_(
        for_synthesizer) {
    Init(env, path);
    CreateDecodeThread();
}

void BaseDecoder::Init(JNIEnv *env, jstring path) {
    path_ref_ = env->NewGlobalRef(path);
    path_ = env->GetStringUTFChars(path, NULL);
    env->GetJavaVM(&jvm_for_thread_);
}

BaseDecoder::~BaseDecoder() {
    if (fmt_ctx_ != NULL) delete fmt_ctx_;
    if (codec_ctx_ != NULL) delete codec_ctx_;
    if (frame_ != NULL) delete frame_;
    if (packet_ != NULL) delete packet_;
}

void BaseDecoder::CreateDecodeThread() {
    //使用智能指针，线程结束，自动删除本类指针
    std::shared_ptr<BaseDecoder> that(this);
    std::thread t(Decode, that);
    // 当使用join()函数时，主调线程阻塞，等待被调线程终止，然后主调线程回收被调线程资源，并继续运行；
    //当使用detach()函数时，主调线程继续运行，被调线程驻留后台运行，主调线程无法再取得该被调线程的控制权。当主调线程结束时，由运行时库负责清理与被调线程相关的资源。
    //detach()函数,目标线程就成为了守护线程，驻留后台运行
    t.detach();
}

void BaseDecoder::Decode(std::shared_ptr<BaseDecoder> that) {
    JNIEnv *env;

    //将线程附加到虚拟机，并获取env
    if (that->jvm_for_thread_->AttachCurrentThread(&env, NULL) != JNI_OK) {
        LOGE(that->TAG, that->LogSpec(), "Fail to Init decode thread");
        return;
    }
    that->CallbackState(PREPARE);
    that->InitFFmpegDecoder(env);
    that->AllocFrameBuffer();
    av_usleep(1000);
    that->Prepare(env);
    that->LoopDecode();
    that->DoneDecode(env);

    that->CallbackState(STOP);

    that->jvm_for_thread_->DetachCurrentThread();
}

void BaseDecoder::InitFFmpegDecoder(JNIEnv *env) {
    //1、初始化上下文
    fmt_ctx_ = avformat_alloc_context();

    //2，打开文件
    if (avformat_open_input(&fmt_ctx_, path_, NULL, NULL) != 0) {
        LOGE(TAG, LogSpec(), "Fail to open file [%s]", path_);
        DoneDecode(env);
        return;
    }

    //3，获取音视频流信息
    if (avformat_find_stream_info(fmt_ctx_, NULL) < 0) {
        LOGE(TAG, LogSpec(), "Fail to find stream info");
        DoneDecode(env);
        return;
    }
//4，查找编解码器
    //4.1 获取视频流的索引
    int vIdx = -1;//存放视频流的索引
    for (int i = 0; i < fmt_ctx_->nb_streams; ++i) {
        if (fmt_ctx_->streams[i]->codecpar->codec_type == GetMediaType()) {
            vIdx = i;
            break;
        }
    }
    if (vIdx == -1) {
        LOGE(TAG, LogSpec(), "Fail to find stream index");
        DoneDecode(env);
        return;
    }
    stream_index_ = vIdx;
    //4.2 获取解码器参数
    AVCodecParameters *codecPar = fmt_ctx_->streams[vIdx]->codecpar;
    //4.3 获取解码器
//    m_codec = avcodec_find_decoder_by_name("h264_mediacodec");//硬解码
    codec_ = avcodec_find_decoder(codecPar->codec_id);

    //4.4 获取解码器上下文
    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (avcodec_parameters_to_context(codec_ctx_, codecPar) != 0) {
        LOGE(TAG, LogSpec(), "Fail to obtain av codec context");
        DoneDecode(env);
        return;
    }

    //5，打开解码器
    if (avcodec_open2(codec_ctx_, codec_, NULL) < 0) {
        LOGE(TAG, LogSpec(), "Fail to open av codec");
        DoneDecode(env);
        return;
    }

    duration_ = (long) ((float) fmt_ctx_->duration / AV_TIME_BASE * 1000);

    LOGE(TAG, LogSpec(), "Decoder init success");
}

void BaseDecoder::AllocFrameBuffer() {
    // 初始化待解码和解码数据结构
    // 1）初始化AVPacket，存放解码前的数据
    packet_ = av_packet_alloc();
    // 2）初始化AVFrame，存放解码后的数据
    frame_ = av_frame_alloc();
}

void BaseDecoder::LoopDecode() {
    if (STOP == state_) { // 如果已被外部改变状态，维持外部配置
        state_ = START;
    }

    CallbackState(START);
    LOGE(TAG, LogSpec(), "Start loop decode");

    while (true) {
        if (state_ != DECODING &&
            state_ != START &&
            state_ != STOP) {
            CallbackState(state_);
            Wait();
            CallbackState(state_);
            // 恢复同步起始时间，去除等待流失的时间
            started_t_ = GetCurMsTime() - cur_t_s_;
        }

        if (state_ == STOP) {
            break;
        }

        if (-1 == started_t_) {
            started_t_ = GetCurMsTime();
        }

        if (DecodeOneFrame() != NULL) {
            SyncRender();
            Render(frame_);

            if (state_ == START) {
                state_ = PAUSE;
            }
        } else {
            LOGE(TAG, LogSpec(), "m_state = %d", state_);
            if (ForSynthesizer()) {
                state_ = STOP;
            } else {
                state_ = FINISH;
            }
            CallbackState(FINISH);
        }
    }
}

AVFrame *BaseDecoder::DecodeOneFrame() {
    int ret = av_read_frame(fmt_ctx_, packet_);
    while (ret == 0) {
        if (packet_->stream_index == stream_index_) {
            switch (avcodec_send_packet(codec_ctx_, packet_)) {
                case AVERROR_EOF: {
                    av_packet_unref(packet_);
                    LOGE(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR_EOF));
                    return NULL; //解码结束
                }
                case AVERROR(EAGAIN):
                    LOGE(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(EAGAIN)));
                    break;
                case AVERROR(EINVAL):
                    LOGE(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(EINVAL)));
                    break;
                case AVERROR(ENOMEM):
                    LOGE(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(ENOMEM)));
                    break;
                default:
                    break;
            }
            //TODO 这里需要考虑一个packet有可能包含多个frame的情况
            int result = avcodec_receive_frame(codec_ctx_, frame_);
            if (result == 0) {
                ObtainTimeStamp();
                av_packet_unref(packet_);
                return frame_;
            } else {
                LOGE(TAG, LogSpec(), "Receive frame error result: %s", av_err2str(AVERROR(result)));
            }
        }
        // 释放packet
        av_packet_unref(packet_);
        ret = av_read_frame(fmt_ctx_, packet_);
    }
    av_packet_unref(packet_);
    LOGI(TAG, "ret = %s", av_err2str(AVERROR(ret)));
    return NULL;
}

void BaseDecoder::ObtainTimeStamp() {
    if (frame_->pkt_dts != AV_NOPTS_VALUE) {
        cur_t_s_ = packet_->dts;
    } else if (frame_->pts != AV_NOPTS_VALUE) {
        cur_t_s_ = frame_->pts;
    } else {
        cur_t_s_ = 0;
    }
    cur_t_s_ = (int64_t) ((cur_t_s_ * av_q2d(fmt_ctx_->streams[stream_index_]->time_base)) * 1000);
}
void BaseDecoder::CallbackState(DecodeState status) {
    if (state_cb_ != NULL) {
        switch (status) {
            case PREPARE:
                state_cb_->DecodePrepare(this);
                break;
            case START:
                state_cb_->DecodeReady(this);
                break;
            case DECODING:
                state_cb_->DecodeRunning(this);
                break;
            case PAUSE:
                state_cb_->DecodePause(this);
                break;
            case FINISH:
                state_cb_->DecodeFinish(this);
                break;
            case STOP:
                state_cb_->DecodeStop(this);
                break;
        }
    }
}

void BaseDecoder::SyncRender() {
    if (ForSynthesizer()) {
        //        av_usleep(15000);
        return;
    }
    int64_t ct = GetCurMsTime();
    int64_t pass_time = ct - started_t_;
    if (cur_t_s_ > pass_time) {
        av_usleep((unsigned int) ((cur_t_s_ - pass_time) * 1000));
    }
}

void BaseDecoder::DoneDecode(JNIEnv *env) {
    LOGE(TAG, LogSpec(), "Decode done and decoder release");
    // 释放缓存
    if (packet_ != NULL) {
        av_packet_free(&packet_);
    }
    if (frame_ != NULL) {
        av_frame_free(&frame_);
    }
    // 关闭解码器
    if (codec_ctx_ != NULL) {
        avcodec_close(codec_ctx_);
        avcodec_free_context(&codec_ctx_);
    }
    // 关闭输入流
    if (fmt_ctx_ != NULL) {
        avformat_close_input(&fmt_ctx_);
        avformat_free_context(fmt_ctx_);
    }
    // 释放转换参数
    if (path_ref_ != NULL && path_ != NULL) {
        env->ReleaseStringUTFChars((jstring) path_ref_, path_);
        env->DeleteGlobalRef(path_ref_);
    }

    // 通知子类释放资源
    Release();
}

void BaseDecoder::Wait(long second, long ms) {
//    LOG_INFO(TAG, LogSpec(), "Decoder run into wait, state：%s", GetStateStr())
    pthread_mutex_lock(&mutex_);
    if (second > 0 || ms > 0) {
        timeval now;
        timespec outtime;
        gettimeofday(&now, NULL);
        int64_t destNSec = now.tv_usec * 1000 + ms * 1000000;
        outtime.tv_sec = static_cast<__kernel_time_t>(now.tv_sec + second + destNSec / 1000000000);
        outtime.tv_nsec = static_cast<long>(destNSec % 1000000000);
        pthread_cond_timedwait(&cond_, &mutex_, &outtime);
    } else {
        pthread_cond_wait(&cond_, &mutex_);
    }
    pthread_mutex_unlock(&mutex_);
}

void BaseDecoder::SendSignal() {
//    LOG_INFO(TAG, LogSpec(), "Decoder wake up, state: %s", GetStateStr())
    pthread_mutex_lock(&mutex_);
    pthread_cond_signal(&cond_);
    pthread_mutex_unlock(&mutex_);
}

void BaseDecoder::GoOn() {
    state_ = DECODING;
    SendSignal();
}

void BaseDecoder::Pause() {
    state_ = PAUSE;
}

void BaseDecoder::Stop() {
    state_ = STOP;
    SendSignal();
}

bool BaseDecoder::IsRunning() {
    return DECODING == state_;
}

long BaseDecoder::GetDuration() {
    return duration_;
}

long BaseDecoder::GetCurPos() {
    return (long) cur_t_s_;
}
