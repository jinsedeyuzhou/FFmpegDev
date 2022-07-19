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


#include <string>
#include <unistd.h>
#include "media/player/def_player/player.h"
#include "media/player/gl_player/gl_player.h"
#include "media/muxer/ff_repack.h"
#include "media/synthesizer/synthesizer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>
#include <libavcodec/avcodec.h>
#include <exception>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_ffmpegInfo(JNIEnv *env, jobject  /* this */) {

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
extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_jni_set_java_vm(vm, reserved);
    LOGE("JNI_OnLoad", "--------", "");
    return JNI_VERSION_1_4;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_pause(JNIEnv *env, jobject thiz, jint player) {
    Player *p = (Player *) player;
    p->pause();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_createPlayer(JNIEnv *env, jobject thiz,
                                                             jstring path, jobject surface) {
    Player *player = new Player(env, path, surface);
    return (jint) player;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_ebrightmoon_ffmpeg_player_FFmpegPlayer_play(JNIEnv *env, jobject thiz, jint player) {
    Player *p = (Player *) player;
    p->play();
}