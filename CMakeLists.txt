cmake_minimum_required (VERSION 2.6)
project(recoded CXX)

INCLUDE(CheckIncludeFileCXX)
check_include_file_cxx("libavformat/avformat.h" HAVE_AVFORMAT)

INCLUDE(CheckIncludeFileCXX)
check_include_file_cxx("libavcodec/avcodec.h" HAVE_AVCODEC)

INCLUDE(CheckIncludeFileCXX)
check_include_file_cxx("libavutil/avutil.h" HAVE_AVUTIL)


if (NOT ${HAVE_AVFORMAT})
    MESSAGE(FATAL_ERROR "Could not find libavformat.")
endif()
if (NOT ${HAVE_AVCODEC})
    MESSAGE(FATAL_ERROR "Could not find libavcodec.")
endif()
if (NOT ${HAVE_AVUTIL})
    MESSAGE(FATAL_ERROR "Could not find libavutil.")
endif()


set(CMAKE_INSTALL_PREFIX "/usr")

set( histogramd_src
    3rd/lodepng.cpp
    src/ffavbase.cpp
    src/ffavfile.cpp	
    src/ffavring.cpp	
    src/ffavcodec.cpp	
    src/ffavimage.cpp	
    src/ffavthread.cpp	
    src/histogram.cpp
    src/main.cpp
)

add_executable(histogramd ${histogramd_src})

target_link_libraries(histogramd avformat avcodec avutil pthread)

set_target_properties(histogramd PROPERTIES VERSION 0.0.3 SOVERSION 0)

install(TARGETS histogramd RUNTIME DESTINATION bin)
