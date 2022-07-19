//
// Created by wyy on 2022/7/15.
//

#ifndef FFMPEGDEV_ONE_FRAME_H
#define FFMPEGDEV_ONE_FRAME_H

#include <malloc.h>
#include "../utils/common-define.h"

extern "C" {
#include <libavutil/rational.h>
};

class OneFrame {
public:
    uint8_t *data = NULL;
    int line_size;
    int64_t pts;
    AVRational time_base;
    uint8_t *ext_data = NULL;

    // 是否自动回收data和ext_data
    bool auto_recycle = true;

    OneFrame(uint8_t *data, int line_size, int64_t pts, AVRational time_base,
             uint8_t *ext_data = NULL, bool auto_recycle = true) {
        this->data = data;
        this->line_size = line_size;
        this->pts = pts;
        this->time_base = time_base;
        this->ext_data = ext_data;
        this->auto_recycle = auto_recycle;
    }

    OneFrame(const OneFrame &) = delete;

    OneFrame &operator=(const OneFrame &) = delete;

    ~OneFrame() {
        if (auto_recycle) {
            if (data != NULL) {
                free(data);
                data = NULL;
            }
            if (ext_data != NULL) {
                free(ext_data);
                ext_data = NULL;
            }
        }
    }
};

#endif //FFMPEGDEV_ONE_FRAME_H
