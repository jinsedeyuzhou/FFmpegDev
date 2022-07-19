//
// 默认画面渲染代理
// Created by cxp on 2019-08-08.
//

#ifndef FFMPEGDEV_DEF_DRAWER_PROXY_IMPL_H
#define FFMPEGDEV_DEF_DRAWER_PROXY_IMPL_H


#include "drawer_proxy.h"
#include <vector>

class DefDrawerProxyImpl: public DrawerProxy {

private:
    std::vector<Drawer *> m_drawers;

public:
    void AddDrawer(Drawer *drawer);
    void Draw() override;
    void Release() override;
};


#endif //FFMPEGDEV_DEF_DRAWER_PROXY_IMPL_H
