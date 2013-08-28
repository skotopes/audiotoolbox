About
============

Audiotoolbox is a console utility which allows you to generate histograms and spectrograms for audio files.
It uses ffmpeg libraries and able to eat all modern media formats (including audio streams in video containers).
Also it's quite fast: 6 minutes of mp3 audio can be processed in 3 seconds on 2,7 GHz mobile core i7.

There are 2 processing modules which i've completed:

- avhistogram: generates histogram image with requested width and height. Resulting image contains two layers. First layer is peaks, second layer is RMS. 
- avspectrogram: generates spectrogram image with requested width and height equal to 1. It performs FFT, then splicing frequencies domain into 3 parts: 20 to 120 Hz - low (red), 120 to 1500 Hz mid(green), 1500 to 22050 Hz high(blue).

Each processing module support 2 different arithmetic models: 

- linear(default) - actually semi-linear, original samples is passed throw the sqrt. Simplest and the most common from of representation. Spectrograms in this mode will use relative brightness for each computed block. False colors will be used to improve spectrogram readability.
- logarithmic - professional one. All computations will performed strictly by audio standards. Spectrograms in this mode will have absolute brightness. Threshold level which you passing throw the command line will be used as scale.

You can switch to logarithmic mode by passing -l [threshold_level] to application. 
Please keep in mind that level is not in dbSPL. So normally you will want to use something that lower than 0, for example: -96/-64/etc.

Also you can generate combo image which will contain histogram shaped spectrogram.

Compiling
============

You will need to install this dependencies:

- cmake
- c++ compiler that support c++11 (clang or gcc is working well)
- ffmpeg (avcodec, avformat, avutils) (i'm using 1.2.1, so keep in mind that it may or may not work with newer versions)

Compilation process is quite simple, run following commands:

    cmake .
    make

Usage
============

All options:

    -h,    --help                  Show this help message
    -v,    --verbose               Show debug information
           --width=INT             Set histogram/spectrogram/combo width
           --height=INT            Set histogram/combo height
           --input=FILE            Input audio to process
           --histogram=FILE        Output histogram
           --spectrogram=FILE      Output spectrogram
           --combo=FILE            Output combo image: histogram shaped spectrogram
           --threshold=FLOAT       If specified will switch processing mode to logarithmic

Examples:

    audiotoolbox --input=media_file.mp3 --combo=combo.png --width=2000 --height=200
    audiotoolbox --input=media_file.mp3 --histogram=histogram.png --width=2000 --height=200
    audiotoolbox --input=media_file.mp3 --spectrogram=spectrogram.png --with=2000
    audiotoolbox --input=media_file.mp3 --histogram=histogram.png --spectrogram=spectrogram.png --combo=combo.png --with=2000 --height=200 -threshold=-64 -v

Limitations
============

There are several small things that you need to keep in mind, especially if you using this tool in professional area:

- Spectrogram generator uses simplest FFT implementation, zero crossing block aligning or noise filtering is not implemented.
- In some cases last block can be skipped due to incomplete data.

License
============

All code which was written by me is GPLv3.
Contact me if you want to use this application in commercial products with different license.
