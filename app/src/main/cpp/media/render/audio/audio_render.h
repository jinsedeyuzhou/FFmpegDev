//
// 音频渲染器定义
// Created by cxp on 2019-08-15.
//

#ifndef FFMPEGDEV_AUDIO_RENDER_H
#define FFMPEGDEV_AUDIO_RENDER_H


#include <cstdint>

class AudioRender {
public:
    virtual void InitRender() = 0;
    virtual void Render(uint8_t *pcm, int size) = 0;
    virtual void ReleaseRender() = 0;
    virtual ~AudioRender() {}
};


#endif //FFMPEGDEV_AUDIO_RENDER_H
