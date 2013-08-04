ADD_DEFINITIONS(-D__STDC_CONSTANT_MACROS)

set(audiotoolbox_src
    avobject.cpp
    avcondition.cpp
    avfile.cpp
    avhistogram.cpp
    avspectrogram.cpp
    avsplitter.cpp
    avmutex.cpp
    avring.cpp
    avthread.cpp
    avimage.cpp
    lodepng.cpp
    main.cpp
)

add_executable(audiotoolbox ${audiotoolbox_src})
target_link_libraries(audiotoolbox avformat avcodec avutil swresample m pthread)
