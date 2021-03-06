//
// Mp4混合器
//

#include "mp4_muxer.h"
#include "../../utils/common-define.h"

Mp4Muxer::Mp4Muxer() {

}

Mp4Muxer::~Mp4Muxer() {
}

void Mp4Muxer::Init(JNIEnv *env, jstring path) {
    const char *u_path = env->GetStringUTFChars(path, NULL);

    int len = strlen(u_path);
    path_ = new char[len];
    strcpy(path_, u_path);

    //新建输出上下文
    avformat_alloc_output_context2(&fmt_ctx_, NULL, NULL, path_);

    // 释放引用
    env->ReleaseStringUTFChars(path, u_path);
}

int Mp4Muxer::AddVideoStream(AVCodecContext *ctx) {
    int stream_index = AddStream(ctx);
    video_configured_ = true;
    Start();
    return stream_index;
}

int Mp4Muxer::AddAudioStream(AVCodecContext *ctx) {
    int stream_index = AddStream(ctx);
    audio_configured_ = true;
    Start();
    return stream_index;
}

int Mp4Muxer::AddStream(AVCodecContext *ctx) {
    AVStream *video_stream = avformat_new_stream(fmt_ctx_, NULL);
    avcodec_parameters_from_context(video_stream->codecpar, ctx);
    video_stream->codecpar->codec_tag = 0;
    return video_stream->index;
}

void Mp4Muxer::Start() {
    if (video_configured_ && audio_configured_) {
        av_dump_format(fmt_ctx_, 0, path_, 1);
        //打开文件输入
        int ret = avio_open(&fmt_ctx_->pb, path_, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE(TAG, "Open av io fail");
            return;
        } else {
            LOGI(TAG, "Open av io: %s", path_);
        }
        //写入头部信息
        ret = avformat_write_header(fmt_ctx_, NULL);
        if (ret < 0) {
            LOGE(TAG, "Write header fail");
            return;
        } else {
            LOGI(TAG, "Write header success");
        }
    }
}

void Mp4Muxer::Write(AVPacket *pkt) {
//    uint64_t time = uint64_t (pkt->pts*av_q2d(GetTimeBase(pkt->stream_index))*1000);
    int ret = av_interleaved_write_frame(fmt_ctx_, pkt);
//    LOGE(TAG, "Write one frame pts: %lld, ret = %s", time , av_err2str(ret))
}

void Mp4Muxer::EndAudioStream() {
    LOGI(TAG, "End audio stream");
    audio_end_ = true;
    Release();
}

void Mp4Muxer::EndVideoStream() {
    LOGI(TAG, "End video stream");
    video_end_ = true;
    Release();
}

void Mp4Muxer::Release() {
    if (video_end_ && audio_end_) {
        if (fmt_ctx_) {
            //写入文件尾部
            av_write_trailer(fmt_ctx_);

            //关闭输出IO
            avio_close(fmt_ctx_->pb);

            //释放资源
            avformat_free_context(fmt_ctx_);

            fmt_ctx_ = NULL;
        }
        delete [] path_;
        LOGI(TAG, "Muxer Release");
        if (mux_finish_cb_) {
            mux_finish_cb_->OnMuxFinished();
        }
    }
}

