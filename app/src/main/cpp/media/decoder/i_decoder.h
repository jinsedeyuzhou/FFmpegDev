//
// Created by wyy on 2022/7/14.
//

#ifndef FFMPEGDEV_I_DECODER_H
#define FFMPEGDEV_I_DECODER_H

#include "i_decode_state_cb.h"

class IDecoder{
public:
    virtual void GoOn()=0;
    virtual void Pause()=0;
    virtual void Stop()=0;
    virtual bool IsRunning()=0;
    virtual long GetDuration()=0;
    virtual long GetCurPos()=0;
    virtual void SetStateReceiver(IDecodeStateCb *cb) = 0;
};
#endif //FFMPEGDEV_I_DECODER_H
