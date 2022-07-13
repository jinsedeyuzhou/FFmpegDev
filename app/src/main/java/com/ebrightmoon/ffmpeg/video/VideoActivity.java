package com.ebrightmoon.ffmpeg.video;

import android.net.Uri;
import android.os.Bundle;
import android.widget.MediaController;
import android.widget.VideoView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.ebrightmoon.ffmpeg.databinding.ActivityVideoBinding;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 */
public class VideoActivity extends AppCompatActivity {

    private ActivityVideoBinding binding;
    private String path = "/storage/emulated/0/big_buck_bunny.mp4";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityVideoBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        initView();
    }

    private void initView() {
        Uri uri = Uri.parse(path);
        binding.videoView.setMediaController(new MediaController(this));
        binding.videoView.setVideoURI(uri);
        binding.videoView.start();
        binding.videoView.requestFocus();
    }
}
