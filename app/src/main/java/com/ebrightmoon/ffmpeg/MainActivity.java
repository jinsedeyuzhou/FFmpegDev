package com.ebrightmoon.ffmpeg;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.TextView;

import com.ebrightmoon.ffmpeg.databinding.ActivityMainBinding;
import com.ebrightmoon.ffmpeg.view.PlayerView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'ffmpeg' library on application startup.

    private ActivityMainBinding binding;
    private PlayerView playerView;
    private StringBuilder stringBuilder;
//    private String url="https://www.w3school.com.cn/example/html5/mov_bbb.mp4";
    private String url="/storage/emulated/0/big_buck_bunny.mp4";
//    private String url="rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        // Example of a call to a native method
        binding.statusTv.setText(stringBuilder);
        stringBuilder = new StringBuilder();
        playerView = new PlayerView(this);
        initListener();
        initdata();
    }

    private void initdata() {
        stringBuilder.append("网络请求成功" + "\n");
        binding.statusTv.setText(stringBuilder);
//        playerView.setUrl("");
//        playerView.setUrl("rtmp://58.200.131.2:1935/livetv/chctv");
//        playerView.setUrl("rtmp://ns8.indexforce.com/home/mystream");
//        playerView.setUrl("rtmp://58.200.131.2:1935/livetv/hunantv");
        playerView.setUrl(url);
//        playerView.setUrl("rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mov");
        binding.rl.addView(playerView);
    }

    private void initListener() {
        playerView.setBack((code, log) -> {
            this.runOnUiThread(() -> {
                stringBuilder.append(log + "\n");
                binding.statusTv.setText(stringBuilder);
                if (code == 1) {
                    binding.statusTv.setVisibility(View.GONE);
                }
            });
        });

        binding.play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                playerView.play(url);
            }
        });
    }

    /**
     * A native method that is implemented by the 'ffmpeg' native library,
     * which is packaged with this application.
     */

}