cmake_minimum_required(VERSION 3.10.2)

project(FFmpegDev)

# 支持gnu++11
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# 1. 定义so库和头文件所在目录，方面后面使用
set(ffmpeg_lib_dir ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
set(ffmpeg_head_dir ${CMAKE_SOURCE_DIR})

set(media_dir ${CMAKE_CURRENT_SOURCE_DIR}/media/)
set(utils_dir ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(opengl_dir ${CMAKE_CURRENT_SOURCE_DIR}/opengl)

set(utils_srcs

        #工具
        ${utils_dir}/timer.c
        )
set(opengl_srcs

        #opengl 文件

        # OpenGL EGL核心
        ${opengl_dir}/egl/egl_core.h
        ${opengl_dir}/egl/egl_core.cpp
        ${opengl_dir}/egl/egl_surface.h
        ${opengl_dir}/egl/egl_surface.cpp


        # OpenGL绘制器
        ${opengl_dir}/drawer/drawer.h
        ${opengl_dir}/drawer/drawer.cpp
        ${opengl_dir}/drawer/video_drawer.h
        ${opengl_dir}/drawer/video_drawer.cpp

        # OpenGL绘制代理器
        ${opengl_dir}/drawer/proxy/drawer_proxy.cpp
        ${opengl_dir}/drawer/proxy/def_drawer_proxy_impl.cpp
        )

set(media_srcs

        # 解码器
        ${media_dir}/one_frame.h
        ${media_dir}/const.h

        ${media_dir}/decoder/i_decoder.h
        ${media_dir}/decoder/decode_state.h
        ${media_dir}/decoder/i_decode_state_cb.cpp
        ${media_dir}/decoder/base_decoder.cpp
        ${media_dir}/decoder/audio/a_decoder.cpp
        ${media_dir}/decoder/video/v_decoder.cpp

        # 编码器
        ${media_dir}/encoder/i_encoder.h
        ${media_dir}/encoder/base_encoder.cpp
        ${media_dir}/encoder/i_encode_state_cb.h
        ${media_dir}/encoder/video/v_encoder.cpp
        ${media_dir}/encoder/audio/a_encoder.cpp

        # 打包器
        ${media_dir}/muxer/ff_repack.cpp
        ${media_dir}/muxer/mp4_muxer.cpp
        ${media_dir}/muxer/i_muxer_cb.h

        # 渲染器
        ${media_dir}/render/video/video_render.h

        ${media_dir}/render/video/native_render/native_render.h
        ${media_dir}/render/video/native_render/native_render.cpp

        ${media_dir}/render/video/opengl_render/opengl_render.h
        ${media_dir}/render/video/opengl_render/opengl_render.cpp
        ${media_dir}/render/video/opengl_render/opengl_pixel_receiver.h

        ${media_dir}/render/audio/audio_render.h
        ${media_dir}/render/audio/opensl_render.h
        ${media_dir}/render/audio/opensl_render.cpp


        #合成器
        ${media_dir}/synthesizer/synthesizer.cpp


        # 播放器
        ${media_dir}/player/def_player/player.cpp
        ${media_dir}/player/gl_player/gl_player.cpp
        )


# 2. 添加头文件目录
include_directories(${ffmpeg_head_dir}/include)

# 3. 添加ffmpeg相关的so库 导入imported
add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so)

add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavdevice.so)

add_library(libpostproc
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libpostproc.so)

add_library(x264
        SHARED
        IMPORTED)
set_target_properties(x264
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libx264.so)

add_library(x264-164
        SHARED
        IMPORTED)
set_target_properties(x264-164
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libx264-164.so)


# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 配置目标so库编译信息
add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp

        ${utils_srcs}
        ${opengl_srcs}
        ${media_srcs}


        )

# 指定编译目标库时，cmake要链接的库
target_link_libraries(

        # 指定目标库，native-lib 是在上面 add_library 中配置的目标库
        native-lib

        # 4. 连接 FFmpeg 相关的库
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat
        avdevice
        x264
        x264-164

        -landroid
        OpenSLES
        -lEGL
        -lGLESv2
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

