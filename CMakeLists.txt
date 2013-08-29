cmake_minimum_required (VERSION 2.6)
include(CheckIncludeFileCXX)
project(audiotoolbox CXX)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

check_include_file_cxx("libavformat/avformat.h" HAVE_AVFORMAT)
if (NOT ${HAVE_AVFORMAT})
    message(FATAL_ERROR "Could not find libavformat.")
endif()

check_include_file_cxx("libavcodec/avcodec.h" HAVE_AVCODEC)
if (NOT ${HAVE_AVCODEC})
    message(FATAL_ERROR "Could not find libavcodec.")
endif()

check_include_file_cxx("libavutil/avutil.h" HAVE_AVUTIL)
if (NOT ${HAVE_AVUTIL})
    message(FATAL_ERROR "Could not find libavutil.")
endif()

set(audiotoolbox_src
    src/avobject.cpp
    src/avcondition.cpp
    src/avfile.cpp
    src/avhistogram.cpp
    src/avspectrogram.cpp
    src/avsplitter.cpp
    src/avmutex.cpp
    src/avring.cpp
    src/avthread.cpp
    src/avimage.cpp
    src/lodepng.cpp
    src/audiotoolbox.cpp
    src/main.cpp
)

add_executable(audiotoolbox ${audiotoolbox_src})
target_link_libraries(audiotoolbox avformat avcodec avutil swresample m pthread)

