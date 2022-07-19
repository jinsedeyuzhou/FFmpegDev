//
//

#ifndef FFMPEGDEV_I_MUXER_CB_H
#define FFMPEGDEV_I_MUXER_CB_H

class IMuxerCb {
public:
    virtual void OnMuxFinished() = 0;
};

#endif //FFMPEGDEV_I_MUXER_CB_H
