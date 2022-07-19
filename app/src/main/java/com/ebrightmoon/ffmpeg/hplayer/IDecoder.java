package com.ebrightmoon.ffmpeg.hplayer;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 *
 */
interface IDecoder extends Runnable {
    /**
     * 暂停解码
     */
    void pause();
    /**
     * 继续解码
     */
    void contine();
    /**
     * 跳转到指定位置
     * 并返回实际帧的时间
     *
     * @param pos: 毫秒
     * @return 实际时间戳，单位：毫秒
     */
    long seekTo(long pos);

    /**
     * 跳转到指定位置,并播放
     * 并返回实际帧的时间
     *
     * @param pos: 毫秒
     * @return 实际时间戳，单位：毫秒
     */
    long seekAndPlay(long pos);

    /**
     * 停止解码
     */
    void stop();
    /**
     * 是否正在解码
     */
    boolean isDecoding();
    /**
     * 是否正在快进
     */
    boolean isSeeking();
    /**
     * 是否停止解码
     */
    boolean isStop();
    /**
     * 设置尺寸监听器
     */

    void setVideoSizeListener(OnVideoSizeChangedListener videoSizeListener);








}
