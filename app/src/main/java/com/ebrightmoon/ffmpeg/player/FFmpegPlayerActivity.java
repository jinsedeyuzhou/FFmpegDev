package com.ebrightmoon.ffmpeg.player;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.ebrightmoon.ffmpeg.databinding.ActivityFfmpegPlayerBinding;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 */
public class FFmpegPlayerActivity extends AppCompatActivity {
    private ActivityFfmpegPlayerBinding binding;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityFfmpegPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
    }
}
