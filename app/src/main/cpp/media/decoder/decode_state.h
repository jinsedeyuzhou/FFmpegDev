//
// Created by wyy on 2022/7/14.
// 定义了解码器解码的状态

#ifndef FFMPEGDEV_DECODE_STATE_H
#define FFMPEGDEV_DECODE_STATE_H
enum DecodeState {
    STOP,
    PREPARE,
    START,
    DECODING,
    PAUSE,
    FINISH
};
#endif //FFMPEGDEV_DECODE_STATE_H
