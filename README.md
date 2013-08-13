About
============

Audiotoolbox is a console utility which allows you to generate histograms and spectrograms for audio files.
It uses ffmpeg libraries and able to eat all modern media formats (including audio from videos).
Also it's quite fast: 6 minutes of mp3 audio can be processed in 3 seconds on 2,7 GHz mobile core i7.

There are 2 processing modules which i've completed:

- avhistogram: generates histogram image with requested width and height. Resulting image contains two layers. First layer is peaks, second layer is RMS. 
- avspectrogram: generates spectrogram image with requested width and height equal to 1. It performs FFT, then splicing frequencies domain into 3 parts: low(red), mid(green), high(blue).

Each processing module support 2 different arithmetic models: 

- linear(default) - actually semi-linear, original samples is passed throw the sqrt. Simplest and the most common from of representation. Spectrograms in this mode will use relative brightness for each computed block. False colors will be used to improve spectrogram readability.
- logarithmic - professional one. All computations will performed strictly by audio standards. Spectrograms in this mode will have absolute brightness. Threshold level which you passing throw the command line will be used 

You can switch to logarithmic mode by passing -l [threshold_level] to application. 
Please keep in mind that level is not in dbSPL. So normally you will want to use -96/-64/etc.

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

	-f [filename] 	- source audio file
	-t [filename]	- histogram output file
	-s [filename]	- spectrogram output file
	-w [integer]	- histogram and spectrogram width
	-h [integer]	- histogram height
	-l [float]		- switches processing into db mode and sets threshold level
	-v 				- verbose output

Examples:

    audiotoolbox -f media_file.mp3 -t histogram.png -w 2000 -h 200
    audiotoolbox -f media_file.mp3 -s spectrogram.png -w 2000
    audiotoolbox -f media_file.mp3 -t histogram.png -s spectrogram.png -w 2000 -h 200
    audiotoolbox -f media_file.mp3 -t histogram.png -s spectrogram.png -w 2000 -h 200 -l -64 -v

Limitations
============

There are several small things that you need to keep in mind, especially if you using this tool in professional area:

- Spectrogram generator uses simplest FFT implementation, zero crossing block aligning or noise filtering is not implemented.
- In some cases last block can be skipped due to incomplete data.

License
============

All code which was written by me is GPLv3. 
Contact me if you want to use this application in commercial products with different license.
