package com.ebrightmoon.ffmpeg;

import android.util.Log;

/**
 * Time: 2022/7/5
 * Author:wyy
 * Description:
 */
public class FFmpegClient {
    private static final String TAG = FFmpegClient.class.getSimpleName();

    static {
        System.loadLibrary("native-lib");
    }

    public FFmpegClient() {

    }
    public FFmpegClient(NavRtspCallBack navRtspCallBack) {
        if (init(navRtspCallBack) == -1) {
            Log.i(TAG, "客户端初始化失败");
        } else {
            Log.i(TAG, "客户端初始化成功");
        }
    }

    public native String ffmpegInfo();

    /**
     * 初始化
     *
     * @param callback
     * @return
     */
    private native int init(NavRtspCallBack callback);

    /**
     * 异步播放RTSL流
     *
     * @param url RTSP URL
     * @return 0：正常，-1：异常
     */
    public native int play(String url);

    /**
     * 暂停标记位，如果监控卡顿，可以通过暂停等待缓存完成后再进行播放
     * 如果获取流失败也可暂停
     */
    public native void stop();

    /**
     * 释放资源
     */
    public native void dispose();

}
