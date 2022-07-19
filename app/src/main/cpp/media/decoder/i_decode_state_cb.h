//
// Created by wyy on 2022/7/15.
// 编码回调接口定义

#ifndef FFMPEGDEV_I_DECODE_STATE_CB_H
#define FFMPEGDEV_I_DECODE_STATE_CB_H

#include "../one_frame.h"

class IDecoder;

class IDecodeStateCb{

public:
    IDecodeStateCb();

    virtual void DecodePrepare(IDecoder *decoder) = 0;
    virtual void DecodeReady(IDecoder *decoder) = 0;
    virtual void DecodeRunning(IDecoder *decoder) = 0;
    virtual void DecodePause(IDecoder *decoder) = 0;
    virtual bool DecodeOneFrame(IDecoder *decoder, OneFrame *frame) = 0;
    virtual void DecodeFinish(IDecoder *decoder) = 0;
    virtual void DecodeStop(IDecoder *decoder) = 0;
};

#endif //FFMPEGDEV_I_DECODE_STATE_CB_H
