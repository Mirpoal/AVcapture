
include_directories(AVCaptureAPP_SOURCE_DIR "${PROJECT_SOURCE_DIR}/include")

message(${AVCaptureAPP_SOURCE_DIR}/include)
message(${AVCaptureAPP_SOURCE_DIR}/lib)


add_executable(video
	main.cpp
	)
	
target_link_libraries(video
	avformat
	avdevice
	avcodec
	avutil
	fdk-aac
	openh264
	asound
	xcb
	xcb-composite
	xcb-damage
	xcb-dpms
	xcb-dri2
	xcb-dri3
	xcb-glx
	xcb-xvmc
	xcb-present
	xcb-randr
	xcb-record
	xcb-render
	xcb-res
	xcb-screensaver
	xcb-shape
	xcb-shm
	xcb-sync
	xcb-xevie
	xcb-xf86dri
	xcb-xfixes
	xcb-xinerama
	xcb-xkb
	xcb-xprint
	xcb-xtest
	xcb-xv
	pthread
	)