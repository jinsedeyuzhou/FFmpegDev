package com.ebrightmoon.ffmpeg;


import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.ebrightmoon.ffmpeg.camera.CameraActivity;
import com.ebrightmoon.ffmpeg.databinding.ActivityMainBinding;
import com.ebrightmoon.ffmpeg.player.FFmpegPlayerActivity;
import com.ebrightmoon.ffmpeg.video.MediaPlayerActivity;
import com.ebrightmoon.ffmpeg.video.VideoActivity;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'ffmpeg' library on application startup.

    private ActivityMainBinding binding;
    private String url = "/storage/emulated/0/big_buck_bunny.mp4";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        // Example of a call to a native method
        initListener();
        initdata();
    }

    private void initdata() {



    }

    private void initListener() {
        binding.camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, CameraActivity.class));
            }
        });

        binding.video.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, VideoActivity.class));
            }
        });

        binding.ffmpeg.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, FFmpegPlayerActivity.class));
            }
        });

        binding.mediaPlayer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, MediaPlayerActivity.class));
            }
        });
    }


}