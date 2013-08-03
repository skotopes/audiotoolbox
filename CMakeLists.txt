cmake_minimum_required (VERSION 2.6)
include(CheckIncludeFileCXX)
project(audiotoolbox CXX)

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

set(CMAKE_CXX_FLAGS_INIT "-std=c++11")

add_subdirectory(src)
