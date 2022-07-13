package com.ebrightmoon.ffmpeg.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.TextureView;


import com.ebrightmoon.ffmpeg.FFmpegClient;
import com.ebrightmoon.ffmpeg.NavRtspCallBack;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class PlayerView extends TextureView implements TextureView.SurfaceTextureListener, NavRtspCallBack {

    private static final String TAG = PlayerView.class.getSimpleName();
    private String url = null;
    private Bitmap bmp;
    private Rect mDstRect;
    private FFmpegClient client;
    private Thread thread;
    private boolean isStop = false;
    private StatusBack back;

    private ExecutorService livePool = new ThreadPoolExecutor(10, 20, 200L, TimeUnit.MILLISECONDS, new LinkedBlockingQueue<Runnable>());


    public PlayerView(Context context) {
        super(context);
        init();
    }

    public PlayerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public PlayerView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public PlayerView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    public StatusBack getBack() {
        return back;
    }

    public void setBack(StatusBack back) {
        this.back = back;
    }

    private void init() {
        //设置背景透明，记住这里是[是否不透明]
        setOpaque(false);
        //设置监听
        setSurfaceTextureListener(this);
        mDstRect = new Rect();
        Log.d(TAG, "init");
    }

    private void initRtsp() {
        livePool.execute(new Runnable() {
            @Override
            public void run() {
                client = new FFmpegClient(PlayerView.this);
                client.play(url);
                Log.d(TAG, "initRtsp");
            }
        });
    }

    public void play(String url) {
        livePool.execute(new Runnable() {
            @Override
            public void run() {
                if (client == null)
                    client = new FFmpegClient(PlayerView.this);
                client.play(url);
                Log.d(TAG, "initRtsp");
            }
        });
    }

    public void setUrl(String url) {
        this.url = url;
    }

    private void drawBitmap(Bitmap bitmap) {
        //锁定画布
        Canvas canvas = lockCanvas();
        if (canvas != null) {
            //清空画布
            canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            //将bitmap画到画布上
            canvas.drawBitmap(bitmap, null, mDstRect, null);
            //解锁画布同时提交
            unlockCanvasAndPost(canvas);
        }
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
        Log.d(TAG, "onSurfaceTextureAvailable");
        if (url == null) {
            Log.d(TAG, "init: rtsp endpoint is null");
            return;
        }
        isStop = false;
        mDstRect.set(0, 0, getWidth(), getWidth());
        initRtsp();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {
        Log.d(TAG, "onSurfaceTextureSizeChanged:");
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
        Log.d(TAG, "onSurfaceTextureDestroyed");
        isStop = true;
        livePool.shutdown();
        client.stop();
        SystemClock.sleep(200);
        client.dispose();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

    }

    @Override
    public void onFrame(byte[] frame, int nChannel, int width, int height) {
        if (isStop) {
            return;
        }
        int area = width * height;
        int[] pixels = new int[area];
        for (int i = 0; i < area; i++) {
            int r = frame[3 * i];
            int g = frame[3 * i + 1];
            int b = frame[3 * i + 2];
            if (r < 0) {
                r += 255;
            }
            if (g < 0) {
                g += 255;
            }
            if (b < 0) {
                b += 255;
            }
            pixels[i] = Color.rgb(r, g, b);
        }
        if (bmp == null) {
            bmp = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
        } else {
            bmp.setPixels(pixels, 0, width, 0, 0, width, height);
            drawBitmap(bmp);
        }
    }

    /**
     * 返回状态
     *
     * @param code 状态码
     * @param log  状态日志
     */
    @Override
    public void onStatusBack(int code, String log) {
        back.onStatus(code, log);
    }

    public interface StatusBack {
        /**
         * 返回状态
         *
         * @param code 状态码
         * @param log  状态日志
         */
        void onStatus(int code, String log);
    }

}
