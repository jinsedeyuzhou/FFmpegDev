//
// Mp4混合器
//

#ifndef FFMPEGDEV_MP4_MUXER_H
#define FFMPEGDEV_MP4_MUXER_H

#include <jni.h>
#include "i_muxer_cb.h"

extern "C" {
#include <libavformat/avformat.h>
};

typedef void (*mux_finish_cb)();

class Mp4Muxer {


public:
    Mp4Muxer();

    ~Mp4Muxer();

    void SetMuxFinishCallback(IMuxerCb *cb) {
        this->mux_finish_cb_ = cb;
    }

    AVRational GetTimeBase(int stream_index) {
        return fmt_ctx_->streams[stream_index]->time_base;
    }

    void Init(JNIEnv *env, jstring path);

    int AddVideoStream(AVCodecContext *ctx);

    int AddAudioStream(AVCodecContext *ctx);

    void Start();

    void Write(AVPacket *pkt);

    void EndVideoStream();

    void EndAudioStream();

    void Release();

private:

    const char *TAG = "Mp4Muxer";

    char *path_;

    AVFormatContext *fmt_ctx_ = NULL;

    bool audio_configured_ = false;

    bool audio_end_ = false;

    bool video_configured_ = false;

    bool video_end_ = false;

    IMuxerCb *mux_finish_cb_ = NULL;

    int AddStream(AVCodecContext *ctx);
};


#endif //FFMPEGDEV_MP4_MUXER_H
