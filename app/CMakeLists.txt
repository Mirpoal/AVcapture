cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(AVCaptureAPP VERSION 0.0.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# ffmpeg include and lib
include_directories(
	third/ffmpeg/include
	third/fdk-aac/include
	third/libxcb/include
	third/openh264/include
	third/xcb_proto/include

	)
link_directories(
	third/ffmpeg/lib
	third/fdk-aac/lib
	third/libxcb/lib
	third/openh264/lib
	third/xcb_proto/lib

	)
	
add_subdirectory("src")