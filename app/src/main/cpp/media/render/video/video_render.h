//

#ifndef FFMPEGDEV_VIDEORENDER_H
#define FFMPEGDEV_VIDEORENDER_H

#include <stdint.h>
#include <jni.h>

#include "../../one_frame.h"

class VideoRender {
public:
    virtual void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) = 0;
    virtual void Render(OneFrame *one_frame) = 0;
    virtual void ReleaseRender() = 0;
};


#endif //FFMPEGDEV_VIDEORENDER_H
