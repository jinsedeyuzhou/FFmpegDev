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
        // ???MediaPlayer?????????????????????
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
        // ????????????????????????????????????????????????
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
            //??????video??????????????????????????????????????????????????????????????????
            float wRatio = (float) vWidth / (float) currDisplay.getWidth();
            float hRatio = (float) vHeight / (float) currDisplay.getHeight();

            //??????????????????????????????
            float ratio = Math.max(wRatio, hRatio);

            vWidth = (int) Math.ceil((float) vWidth / ratio);
            vHeight = (int) Math.ceil((float) vHeight / ratio);

            //??????surfaceView???????????????
            binding.surfaceView.setLayoutParams(new LinearLayout.LayoutParams(vWidth, vHeight));

            //????????????????????????
            player.start();
        } else {
            binding.surfaceView.setLayoutParams(new LinearLayout.LayoutParams(vWidth, vHeight));
            player.start();
        }
    }

    @Override
    public void onSeekComplete(MediaPlayer mp) {
        // seek?????????????????????
        Log.v("Seek Completion", "onSeekComplete called");
        if (player != null) {
            player.release();
        }
    }

    @Override
    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        // ???video?????????????????????
        //?????????????????????player???source?????????????????????
        Log.v("Video Size Change", "onVideoSizeChanged called");

    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        // ???SurfaceView??????Surface???????????????????????????
        // ?????????????????????MediaPlayer????????????Surface???????????????
        player.setDisplay(holder);
        //????????????MediaPlayer??????????????????????????????????????????prepare??????prepareAsync??????????????????
        player.prepareAsync();
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        // ???Surface??????????????????????????????
        Log.v("Surface Change:::", "surfaceChanged called");
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.v("Surface Destory:::", "surfaceDestroyed called");

    }
}
