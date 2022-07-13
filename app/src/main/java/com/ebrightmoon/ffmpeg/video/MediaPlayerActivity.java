package com.ebrightmoon.ffmpeg.video;

import android.media.MediaCodec;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.LinearLayout;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import com.ebrightmoon.ffmpeg.databinding.ActivityMediaPlayerBinding;
import com.ebrightmoon.ffmpeg.databinding.ActivityVideoBinding;

import java.io.IOException;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 */
public class MediaPlayerActivity extends AppCompatActivity implements MediaPlayer.OnCompletionListener, MediaPlayer.OnErrorListener, MediaPlayer.OnInfoListener,
        MediaPlayer.OnPreparedListener, MediaPlayer.OnSeekCompleteListener, MediaPlayer.OnVideoSizeChangedListener, SurfaceHolder.Callback {

    private ActivityMediaPlayerBinding binding;
    private Display currDisplay;
    private SurfaceHolder holder;
    private MediaPlayer player;
    private int vWidth, vHeight;
    //private boolean readyToPlay = false;
    private String path = "/storage/emulated/0/big_buck_bunny.mp4";
//    private String path = "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMediaPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        initView();
    }

    private void initView() {

        MediaCodec mediaCodec;
        holder = binding.surfaceView.getHolder();
        holder.addCallback(this);
        holder.setFixedSize(320, 220);

        player = new MediaPlayer();
        player.setOnCompletionListener(this);
        player.setOnErrorListener(this);
        player.setOnInfoListener(this);
        player.setOnPreparedListener(this);
        player.setOnSeekCompleteListener(this);
        player.setOnVideoSizeChangedListener(this);
        try {
            player.setDataSource(path);

        } catch (IllegalArgumentException | IllegalStateException | IOException e) {
            e.printStackTrace();
        }
        currDisplay = getWindowManager().getDefaultDisplay();
        binding.start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                player.start();
            }
        });
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    public void onCompletion(MediaPlayer mp) {
        // 当MediaPlayer播放完成后触发
        Log.v("Play Over:::", "onComletion called");

    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        Log.v("Play Error:::", "onError called");
        switch (what) {
            case MediaPlayer.MEDIA_ERROR_SERVER_DIED:
                Log.v("Play Error:::", "MEDIA_ERROR_SERVER_DIED");
                break;
            case MediaPlayer.MEDIA_ERROR_UNKNOWN:
                Log.v("Play Error:::", "MEDIA_ERROR_UNKNOWN");
                break;
            default:
                break;
        }
        return false;
    }

    @Override
    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        // 当一些特定信息出现或者警告时触发
        switch (what) {
            case MediaPlayer.MEDIA_INFO_BAD_INTERLEAVING:
                break;
            case MediaPlayer.MEDIA_INFO_METADATA_UPDATE:
                break;
            case MediaPlayer.MEDIA_INFO_VIDEO_TRACK_LAGGING:
                break;
            case MediaPlayer.MEDIA_INFO_NOT_SEEKABLE:
                break;
        }
        return false;
    }

    @Override
    public void onPrepared(MediaPlayer mp) {
        vWidth = player.getVideoWidth();
        vHeight = player.getVideoHeight();
        if (vWidth > currDisplay.getWidth() || vHeight > currDisplay.getHeight()) {
            //如果video的宽或者高超出了当前屏幕的大小，则要进行缩放
            float wRatio = (float) vWidth / (float) currDisplay.getWidth();
            float hRatio = (float) vHeight / (float) currDisplay.getHeight();

            //选择大的一个进行缩放
            float ratio = Math.max(wRatio, hRatio);

            vWidth = (int) Math.ceil((float) vWidth / ratio);
            vHeight = (int) Math.ceil((float) vHeight / ratio);

            //设置surfaceView的布局参数
            binding.surfaceView.setLayoutParams(new LinearLayout.LayoutParams(vWidth, vHeight));

            //然后开始播放视频
            player.start();
        } else {
            binding.surfaceView.setLayoutParams(new LinearLayout.LayoutParams(vWidth, vHeight));
            player.start();
        }
    }

    @Override
    public void onSeekComplete(MediaPlayer mp) {
        // seek操作完成时触发
        Log.v("Seek Completion", "onSeekComplete called");
        if (player != null) {
            player.release();
        }
    }

    @Override
    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        // 当video大小改变时触发
        //这个方法在设置player的source后至少触发一次
        Log.v("Video Size Change", "onVideoSizeChanged called");

    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        // 当SurfaceView中的Surface被创建的时候被调用
        // 在这里我们指定MediaPlayer在当前的Surface中进行播放
        player.setDisplay(holder);
        //在指定了MediaPlayer播放的容器后，我们就可以使用prepare或者prepareAsync来准备播放了
        player.prepareAsync();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        // 当Surface尺寸等参数改变时触发
        Log.v("Surface Change:::", "surfaceChanged called");
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.v("Surface Destory:::", "surfaceDestroyed called");

    }
}
