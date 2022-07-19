package com.ebrightmoon.ffmpeg.player;


import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.ebrightmoon.ffmpeg.databinding.ActivityFfmpegPlayerBinding;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 */
public class FFmpegPlayerActivity extends AppCompatActivity {
    private ActivityFfmpegPlayerBinding binding;
    private FFmpegPlayer fFmpegPlayer;
    private int player;
    private String path="/storage/emulated/0/big_buck_bunny.mp4";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityFfmpegPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        init();
    }

    private void init() {
        fFmpegPlayer = new FFmpegPlayer();
        binding.surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                if (player == 0) {
                    player = fFmpegPlayer.createPlayer(path, holder.getSurface());
                    fFmpegPlayer.play(player);
                }
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });
    }
}
