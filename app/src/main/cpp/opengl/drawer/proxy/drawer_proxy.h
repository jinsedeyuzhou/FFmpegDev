//
// Created by cxp on 2019-08-08.
//

#ifndef FFMPEGDEV_DRAWER_PROXY_H
#define FFMPEGDEV_DRAWER_PROXY_H


#include "../drawer.h"

class DrawerProxy {
public:
    virtual void AddDrawer(Drawer *drawer) = 0;
    virtual void Draw() = 0;
    virtual void Release() = 0;
    virtual ~DrawerProxy() {}
};


#endif //FFMPEGDEV_DRAWER_PROXY_H
