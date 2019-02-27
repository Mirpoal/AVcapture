cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

# set project info
project(AVcapture VERSION 0.0.1 )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(ARCH STREQUAL "x86_64")
	message("you build in x86_linux")
endif()

add_subdirectory("app")