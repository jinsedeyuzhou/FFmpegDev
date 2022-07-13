package com.ebrightmoon.ffmpeg;

public interface NavRtspCallBack {

    /**
     * 从native层接收的原始帧数据
     *
     * @param frame    缓存
     * @param nChannel 通道
     * @param width    宽度
     * @param height   长度
     */
    void onFrame(byte[] frame, int nChannel, int width, int height);

    /**
     * 返回状态
     *
     * @param code 状态码
     * @param log  状态日志
     */
    void onStatusBack(int code, String log);

}
