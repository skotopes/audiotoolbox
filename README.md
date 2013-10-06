About
============

Audiotoolbox is a console utility which allows you to generate histograms and spectrograms for audio files.
It uses ffmpeg libraries and is able to eat all modern media formats (including audio streams in video containers).
Also it's quite fast: 6 minutes of mp3 audio can be processed in 3 seconds on a 2.7 GHz mobile core i7.

There are 2 processing modules which I've completed:

- avhistogram: generates a histogram image with the requested width and height. The resulting image contains two layers. The first layer is peaks, the second layer is RMS. 
- avspectrogram: generates a spectrogram image with the requested width and height equal to 1. It performs FFT, then slicing frequencies domain into 3 parts: 20 to 120 Hz - low (red), 120 to 1500 Hz mid(green), 1500 to 22050 Hz high(blue).

Each processing module supports 2 different arithmetic models: 

- linear(default) - actually semi-linear, the original sample is passed through the sqrt. This is the simplest and the most common form of representation. Spectrograms in this mode will use relative brightness for each computed block. False colors will be used to improve spectrogram readability.
- logarithmic - professional. All computations will be performed strictly by audio standards. Spectrograms in this mode will have absolute brightness. The threshold level which you passing through the command line will be used as a scale.

You can switch to the logarithmic mode by passing -l [threshold_level] to application. 
Please keep in mind that the level is not in dbSPL. So normally you will want to use something that is lower than 0, for example: -96/-64/etc.

Also you can generate a combo image which will contain a histogram shaped spectrogram.

Compiling
============

You will need to install these dependencies:

- cmake
- c++ compiler that support c++11 (clang or gcc is working well)
- ffmpeg (avcodec, avformat, avutils) (I'm using 1.2.1, so keep in mind that it may or may not work with newer versions)

The compilation process is quite simple, run the following commands:

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
           --threshold=FLOAT       If specified, it will switch processing mode to logarithmic
           --window=STRING         Use specific FFT window type. Can be: hann, hamming, blackman, blackmanharris

Examples:

    audiotoolbox --input=media_file.mp3 --combo=combo.png --width=2000 --height=200
    audiotoolbox --input=media_file.mp3 --histogram=histogram.png --width=2000 --height=200
    audiotoolbox --input=media_file.mp3 --spectrogram=spectrogram.png --with=2000
    audiotoolbox --input=media_file.mp3 --histogram=histogram.png --spectrogram=spectrogram.png --combo=combo.png --with=2000 --height=200 -threshold=-64 --window=blackmanharris -v

License
============

All code which was written by me is GPLv3.
Contact me if you want to use this application in commercial products with a different license.
