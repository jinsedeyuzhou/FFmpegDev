#include "utils/common-define.h"
#include <unistd.h>
#include <string>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <err.h>
#include "native-lib.h"
#include <locale>
#include <codecvt>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>
#include <libavcodec/avcodec.h>
#include <exception>

}
enum RTSP_Status {
    INFO_LOG = 0,
    SUCCESS = 1,
    OPEN_INPUT_ERR = -1,
    FIND_STREAM_INFO_ERR = -2,
    FIND_VIDEO_STREAM_ERR = -3,
    FIND_DECODER_ERR = -4,
    OPEN_DECODER_ERR = -5

};

void callback(JNIEnv *env, uint8_t *buf, int channel, int width, int height);

void status_back(JNIEnv *env, int code, const char *log);

extern "C"
JNIEXPORT jstring JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_ffmpegInfos(JNIEnv *env, jobject  /* this */) {

    char info[40000] = {0};
    AVCodec *prev = NULL;
    void *i = 0;
    while ((prev = (AVCodec *) av_codec_iterate(&i))) {
        if (prev->decode != NULL) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (prev->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%s]\n", info, prev->name);
    }
    return env->NewStringUTF(info);
}

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}


jobject gCallback;
jmethodID gVideoCallback, gStatusCallBack;

volatile bool isStop = false;

extern "C" jint
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_init(
        JNIEnv *env, jobject, jobject callback) {
    isStop = false;
    gCallback = env->NewGlobalRef(callback);
    jclass clz = env->GetObjectClass(gCallback);
    if (clz == nullptr) {
        return JNI_ERR;
    } else {
        gVideoCallback = env->GetMethodID(clz, "onFrame", "([BIII)V");
        gStatusCallBack = env->GetMethodID(clz, "onStatusBack", "(ILjava/lang/String;)V");
        env->DeleteLocalRef(clz);
        return JNI_OK;
    }

}

static AVPacket *pAvPacket;
static AVCodecContext *pCodecCtx;
struct SwsContext *pSwsCtx;
static AVFormatContext *pFormatCtx;
static AVCodec *pCodec = nullptr;
static AVDictionary *pAvDic = nullptr;
static const char *pRtspUrl;


extern "C" jint Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_plays(
        JNIEnv *env, jobject, jstring URL) {
    try {
        /*
        * ??????RTSP URL??????
        * */
        pRtspUrl = env->GetStringUTFChars(URL, JNI_FALSE);
        /*
         * ?????????
         */
        av_register_all();
        pFormatCtx = avformat_alloc_context();
        avformat_network_init();
        /*
         * ??????packet??????
         */
        pAvPacket = (AVPacket *) (av_malloc(sizeof(AVPacket)));
        status_back(env, INFO_LOG, "packet???????????????????????????");
        /*
        * ??????????????????
        */
        av_dict_set(&pAvDic, "buffer_size", "1024000", 0);
        av_dict_set(&pAvDic, "max_delay", "500000", 0);
        av_dict_set(&pAvDic, "stimeout", "20000000", 0);
        av_dict_set(&pAvDic, "rtsp_transport", "tcp", 0);

        /*
         * ???????????????????????????
         */
        int err_code = avformat_open_input(&pFormatCtx, pRtspUrl, NULL, NULL);
        if (err_code != 0) {
            LOGD("%d",err_code);
            status_back(env, OPEN_INPUT_ERR, "?????????????????????,???????????????");
            char errors[1024];
            av_strerror(err_code, errors, 1024);
            LOGD("????????????????????? %s, ?????????: %s",
                 pRtspUrl,
                 errors);
            return JNI_ERR;
        }
        status_back(env, INFO_LOG, "?????????????????????");

        /*
         * ??????????????????
         */
        env->ReleaseStringUTFChars(URL, pRtspUrl);
        av_dict_free(&pAvDic);

        /*
         * ?????????????????????
         */
        if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
            status_back(env, FIND_STREAM_INFO_ERR, "????????????????????????");
            return JNI_ERR;
        }
        status_back(env, INFO_LOG, "???????????????????????????");

        /*
         * ????????????????????????
         */
        int video_stream_index = -1;
        for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                //????????????ifmt_ctx->streams[video_stream_index]???
                video_stream_index = i;
                break;
            }
        }
        if (video_stream_index == -1) {
            status_back(env, FIND_STREAM_INFO_ERR, "?????????????????????");
            return JNI_ERR;
        }
        status_back(env, INFO_LOG, "??????????????????");

        /*
         * ??????????????????
         */
        AVFormatContext *oc = avformat_alloc_context();
        AVStream *stream = nullptr;
        pCodecCtx = avcodec_alloc_context3(nullptr);

        pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;
        /*
         * ?????????????????????
         */
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (!pCodec) {
            status_back(env, FIND_DECODER_ERR, "?????????????????????");
            return JNI_ERR;
        }
        status_back(env, INFO_LOG, "?????????????????????");
        /*
         * ???????????????
         */
        if (int err_code = avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
            status_back(env, OPEN_DECODER_ERR, "?????????????????????");
            char error_s[1024];
            av_strerror(err_code, error_s, 1024);
            LOGD("Cannot open codec error code: %s,%d",
                 error_s,
                 err_code);
            return JNI_ERR;
        }
        status_back(env, INFO_LOG, "?????????????????????");
        /*
         * ????????????????????????
         */
        pSwsCtx = sws_getContext(
                //???????????????????????????????????????
                pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                //??????????????????????????????????????????
                pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
                //???????????? AV_PIX_FMT_YUV420P AV_PIX_FMT_RGB24
                SWS_BICUBIC, nullptr, nullptr, nullptr
        );
        /*
         * ????????????????????????????????????
         */
        size_t size = avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width,
                                         pCodecCtx->height);
        uint8_t *picture_buf = (uint8_t *) av_malloc(size);
        AVFrame *pic = av_frame_alloc();
        AVFrame *picrgb = av_frame_alloc();

        size_t size2 = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
                                          pCodecCtx->height);
        uint8_t *picture_buf2 = (uint8_t *) av_malloc(size2);
        avpicture_fill((AVPicture *) pic, picture_buf, AV_PIX_FMT_YUV420P, pCodecCtx->width,
                       pCodecCtx->height);
        avpicture_fill((AVPicture *) picrgb, picture_buf2, AV_PIX_FMT_RGB24, pCodecCtx->width,
                       pCodecCtx->height);

        /*
         * ????????????????????????AVPacket?????????
         */
        while (!isStop && av_read_frame(pFormatCtx, pAvPacket) >= 0) {
            if (pAvPacket->stream_index == video_stream_index) {
                if (stream == nullptr) {
                    stream = avformat_new_stream(oc,
                                                 pFormatCtx->streams[video_stream_index]->codec->codec);
                    avcodec_copy_context(stream->codec,
                                         pFormatCtx->streams[video_stream_index]->codec);
                    stream->sample_aspect_ratio = pFormatCtx->streams[video_stream_index]->codec->sample_aspect_ratio;

                }
                int check = 0;
                pAvPacket->stream_index = stream->id;
                avcodec_decode_video2(pCodecCtx, pic, &check, pAvPacket);
                sws_scale(pSwsCtx, (const uint8_t *const *) pic->data, pic->linesize, 0,
                          pCodecCtx->height, picrgb->data, picrgb->linesize);
                if (gCallback != nullptr) {
                    status_back(env, SUCCESS, "");
                    callback(env, picture_buf2, 3, pCodecCtx->width, pCodecCtx->height);
                }
            }
            av_init_packet(pAvPacket);
            av_packet_unref(pAvPacket);
        }
        /*
         * ????????????
         */
        av_packet_unref(pAvPacket);
        av_free(pic);
        av_free(picrgb);
        av_free(picture_buf);
        av_free(picture_buf2);
        av_read_pause(pFormatCtx);
        avio_close(oc->pb);
        avformat_free_context(oc);
        avformat_close_input(&pFormatCtx);
    } catch (std::exception &e) {
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }

    return isStop ? JNI_OK : JNI_ERR;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_stop(JNIEnv *env, jobject thiz) {
    isStop = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_dispose(JNIEnv *env, jobject thiz) {
    try {
        env->DeleteGlobalRef(gCallback);
        gCallback = nullptr;
    }
    catch (std::exception &e) {
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
}

void callback(JNIEnv *env, uint8_t *buf, int nChannel, int width, int height) {
    try {
        int len = nChannel * width * height;
        jbyteArray gByteArray = env->NewByteArray(len);
        env->SetByteArrayRegion(gByteArray, 0, len, (jbyte *) buf);
        env->CallVoidMethod(gCallback, gVideoCallback, gByteArray, nChannel, width, height);
        env->DeleteLocalRef(gByteArray);
    } catch (std::exception &e) {
        jclass jx = env->FindClass("java/lang/Exception");
        env->ThrowNew(jx, e.what());
    }
}

void status_back(JNIEnv *env, int code, const char *log) {
    try {
        jstring logStr = env->NewStringUTF(log);
        env->CallVoidMethod(gCallback, gStatusCallBack, code, logStr);
        env->DeleteLocalRef(logStr);
    } catch (std::exception &e) {
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_write(JNIEnv *env, jobject thiz,
                                                      jbyteArray rtp2h264,jint length,jint type,
                                                      jlong time_stamp) {
    try {

        jbyte * bytes=env->GetByteArrayElements(rtp2h264, nullptr);
        uint8_t *buf= (uint8_t *) bytes;
        int ret=-1;
        pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
        if (!pCodec) {
            fprintf(stderr, "Codec not found\n");
            exit(1);
        }

        pCodecCtx = avcodec_alloc_context3(pCodec);
        if (!pCodecCtx) {
            fprintf(stderr, "Codec not found\n");
            exit(1);
        }

        AVCodecParserContext *parser;
        parser = av_parser_init(AV_CODEC_ID_H264);
        if (!parser) {
            status_back(env,1,"parser not found\n");
        }
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            fprintf(stderr, "Could not open codec\n");
            exit(1);
        }
        AVFrame  *avFrame= av_frame_alloc();
        if (!avFrame) {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }
        AVPacket *pkt;

        pkt = av_packet_alloc();
        if (!pkt)
            exit(1);

        while (length>0)
        {
//            ret = av_parser_parse2(parser,pCodecCtx,pkt->data,pkt->size,buf,length,0,0,0);

        }



    } catch (std::exception &e) {
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
    }

    return isStop ? JNI_OK : JNI_ERR;

}