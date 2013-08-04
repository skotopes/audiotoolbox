/*
 *  main.cpp
 *  main
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include <iostream>
#include <unistd.h>

#include "avfile.h"
#include "avimage.h"
#include "avhistogram.h"
#include "avspectrogram.h"
#include "avsplitter.h"
#include "avexception.h"

using namespace std;

int main(int argc, char **argv)
{    
    bool verbose = false;
    int c, width=0, heigh=0;
    char *from_file=0, *to_histogram=0, *to_spectrogram=0;
    
    while ((c = getopt (argc, argv, "f:t:s:w:h:v")) != -1)
        switch (c)
    {
        case 'v':
            verbose = true;
            break;
        case 'f':
            from_file = optarg;
            break;
        case 't':
            to_histogram = optarg;
            break;
        case 's':
            to_spectrogram = optarg;
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'h':
            heigh = atoi(optarg);
            break;
        default:
            cout << "Unknown option: "<< c << endl;
            return 1;
    }
    
    if (!from_file || !to_histogram || !to_spectrogram) {
        cout << "From file, To file required" << endl
        << "-f [filename] -t [filename] -s [filename] -w [width] -h [height]" << endl;
        return 1;
    }

    if (width<2 || heigh <2) {
        cout << "width and/or height is wrong(<2) or absent" << endl
        << "-f [filename] -t [filename] -s [filename] -w [width] -h [height]" << endl;
        return 1;
    }

    try {
        AVFile file(44100, 1);
        file.open(from_file);

        AVImageRGBA histogram_image(width, heigh);
        AVImageRGBA spectrogram_image(width, 1);

        AVHistogram histogram(&file, &histogram_image);
        AVSpectrogram spectrogram(&file, &spectrogram_image);

        AVSplitter splitter(2);
        splitter.addObject(&histogram);
        splitter.addObject(&spectrogram);

        file.decoderLoop(&splitter);

        histogram_image.save(to_histogram);
        spectrogram_image.save(to_spectrogram);

        if (verbose) {
            cout << argv[0] << endl
            << "Duration(sec): " << file.getDurationTime() << endl
            << "Duration(samples): " << file.getDurationSamples() << endl
            << "Format bitrate: " << file.getBitrate() << endl
            << "Codec bitrate: " << file.getCodecBitrate() << endl
            << "Codec samplerate: " << file.getCodecSamplerate() << endl
            << "Codec channels: " << file.getCodecChannels() << endl;
        }

    } catch (AVException &e) {
        cout << e.what();
        return 3;
    }

    return 0;
}
