//
// 基础编码器
//

#include <unistd.h>
#include "base_encoder.h"

BaseEncoder::BaseEncoder(JNIEnv *env, Mp4Muxer *muxer, AVCodecID codec_id)
: muxer_(muxer),
codec_id_(codec_id) {
    if (Init()) {
        env->GetJavaVM(&jvm_for_thread_);
        CreateEncodeThread();
    }
}

bool BaseEncoder::Init() {
    codec_ = avcodec_find_encoder(codec_id_);
    if (codec_ == NULL) {
        LOGE(TAG, "Fail to find encoder, code id is %d", codec_id_);
        return false;
    }
    codec_ctx_ = avcodec_alloc_context3(codec_);
    if (codec_ctx_ == NULL) {
        LOGE(TAG, "Fail to alloc encoder context");
        return false;
    }

    encoded_pkt_ = av_packet_alloc();
    av_init_packet(encoded_pkt_);

    return true;
}

void BaseEncoder::CreateEncodeThread() {
    // 使用智能指针，线程结束时，自动删除本类指针
    std::shared_ptr<BaseEncoder> that(this);
    std::thread t(Encode, that);
    t.detach();
}

static int encode_count = 0;
static int encode_in_count = 0;

void BaseEncoder::Encode(std::shared_ptr<BaseEncoder> that) {
    JNIEnv * env;

    //将线程附加到虚拟机，并获取env
    if (that->jvm_for_thread_->AttachCurrentThread(&env, NULL) != JNI_OK) {
        LOGE(that->TAG, that->LogSpec(), "Fail to Init encode thread");
        return;
    }

    that->OpenEncoder();
    that->LoopEncode();
    that->DoRelease();
    //解除线程和jvm关联
    that->jvm_for_thread_->DetachCurrentThread();

}

void BaseEncoder::OpenEncoder() {
    InitContext(codec_ctx_);

    int ret = avcodec_open2(codec_ctx_, codec_, NULL);
    if (ret < 0) {
        LOGE(TAG, LogSpec(), "Fail to open encoder : %d", codec_);
        return;
    }

    encode_stream_index_ = ConfigureMuxerStream(muxer_, codec_ctx_);
}

void BaseEncoder::LoopEncode() {
    if (state_cb_ != NULL) {
        state_cb_->EncodeStart();
    }
    encode_count = 0;
    encode_in_count = 0;
    while (true) {
        if (src_frames_.size() == 0) {
            Wait();
        }
        while (src_frames_.size() > 0) {
            // 1. 获取待解码数据
            frames_lock_.lock();
            OneFrame *one_frame = src_frames_.front();
            src_frames_.pop();
            frames_lock_.unlock();
            encode_count++;

            AVFrame *frame = NULL;
            if (one_frame->line_size != 0) { //如果数据长度为0，说明编码已经结束，压入空frame，使编码器进入结束状态
                src_time_base_ = one_frame->time_base;
                // 2. 子类处理数据
                frame = DealFrame(one_frame);
                delete one_frame;
                if (state_cb_ != NULL) {
                    state_cb_->EncodeSend();
                }
                if (frame == NULL) {
                    continue;
                }
            } else {
                delete one_frame;
            }
            // 3. 将数据发送到编码器
            int ret = avcodec_send_frame(codec_ctx_, frame);
            switch (ret) {
                case AVERROR_EOF:
                    LOGE(TAG, LogSpec(), "Send frame finish [AVERROR_EOF]");
                    break;
                case AVERROR(EAGAIN): //编码编码器已满，先取出已编码数据，再尝试发送数据
                    while (ret == AVERROR(EAGAIN)) {
                        LOGE(TAG, LogSpec(), "Send frame error[EAGAIN]: %s", av_err2str(AVERROR(EAGAIN)));
                        // 4. 将编码好的数据榨干
                        if (DrainEncode()) return; //编码结束
                        // 5. 重新发送数据
                        ret = avcodec_send_frame(codec_ctx_, frame);
                    }
                    break;
                case AVERROR(EINVAL):
                    LOGE(TAG, LogSpec(), "Send frame error[EINVAL]: %s", av_err2str(AVERROR(EINVAL)));
                    break;
                case AVERROR(ENOMEM):
                    LOGE(TAG, LogSpec(), "Send frame error[ENOMEM]: %s", av_err2str(AVERROR(ENOMEM)));
                    break;
                default:
//                    LOGE(TAG, "Send frame to encode, pts: %lld",
//                            (uint64_t )(one_frame.pts*av_q2d(one_frame.time_base)*1000))
                    break;
            }
            if (ret != 0) break;
        }

        if (DrainEncode()) break; //编码结束
    }
}

bool BaseEncoder::DrainEncode() {
    int state = EncodeOneFrame();
    while (state == 0) {
        state = EncodeOneFrame();
    }
    return state == AVERROR_EOF;
}

int BaseEncoder::EncodeOneFrame() {
    int state = avcodec_receive_packet(codec_ctx_, encoded_pkt_);
    switch (state) {
        case AVERROR_EOF: //解码结束
            LOGE(TAG, LogSpec(), "Encode finish");
            break;
        case AVERROR(EAGAIN): //编码还未完成，待会再来
            LOGE(TAG, LogSpec(), "Encode error[EAGAIN]: %s", av_err2str(AVERROR(EAGAIN)));
            break;
        case AVERROR(EINVAL):
            LOGE(TAG, LogSpec(),  "Encode error[EINVAL]: %s", av_err2str(AVERROR(EINVAL)));
            break;
        case AVERROR(ENOMEM):
            LOGE(TAG, LogSpec(), "Encode error[ENOMEM]: %s", av_err2str(AVERROR(ENOMEM)));
            break;
        default:
            //将视频pts/dts转换为容器pts/dts，可以手动转换pts和dts，或直接使用av_packet_rescale_ts
//            m_encoded_pkt->pts = av_rescale_q(m_encoded_pkt->pts, m_src_time_base,
//                                              m_muxer->GetTimeBase(m_encode_stream_index));
//            m_encoded_pkt->dts = av_rescale_q(m_encoded_pkt->dts, m_src_time_base,
//                                              m_muxer->GetTimeBase(m_encode_stream_index));

            av_packet_rescale_ts(encoded_pkt_, src_time_base_,
                                 muxer_->GetTimeBase(encode_stream_index_));
            if (state_cb_ != NULL) {
                state_cb_->EncodeFrame(encoded_pkt_->data);
                long cur_time = (long)(encoded_pkt_->pts*av_q2d(muxer_->GetTimeBase(encode_stream_index_))*1000);
                state_cb_->EncodeProgress(cur_time);
            }
            encoded_pkt_->stream_index = encode_stream_index_;
            muxer_->Write(encoded_pkt_);
            break;
    }
    av_packet_unref(encoded_pkt_);
    return state;
}

void BaseEncoder::PushFrame(OneFrame *one_frame) {
    frames_lock_.lock();

    encode_in_count++;

    src_frames_.push(one_frame);

    frames_lock_.unlock();

    SendSignal();
}

void BaseEncoder::DoRelease() {
    if (encoded_pkt_ != NULL) {
        av_packet_free(&encoded_pkt_);
        encoded_pkt_ = NULL;
    }
    if (codec_ctx_ != NULL) {
        avcodec_close(codec_ctx_);
        avcodec_free_context(&codec_ctx_);
    }
    Release();

    if (state_cb_ != NULL) {
        state_cb_->EncodeFinish();
    }
    LOGE("cccccc", "all encode count : %d,  %d", encode_in_count, encode_count);
}