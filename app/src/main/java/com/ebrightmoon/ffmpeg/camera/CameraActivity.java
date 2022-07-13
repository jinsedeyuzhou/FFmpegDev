package com.ebrightmoon.ffmpeg.camera;

import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.ebrightmoon.ffmpeg.databinding.ActivityCameraBinding;

/**
 * Time: 2022/7/13
 * Author:wyy
 * Description:
 */
public class CameraActivity extends AppCompatActivity {

    private ActivityCameraBinding binding;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityCameraBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
    }
}
