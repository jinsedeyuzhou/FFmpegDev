package com.ebrightmoon.ffmpeg.player;

import android.util.Log;
import android.view.Surface;

import com.ebrightmoon.ffmpeg.NavRtspCallBack;

/**
 * Time: 2022/7/5
 * Author:wyy
 * Description:
 */
public class FFmpegPlayer {
    private static final String TAG = FFmpegPlayer.class.getSimpleName();

    static {
        System.loadLibrary("native-lib");
    }

    public FFmpegPlayer() {

    }

    public native String ffmpegInfo();

    public native int createPlayer(String path, Surface surface);

    public native void play(int player);

    public native void pause(int player);





}
