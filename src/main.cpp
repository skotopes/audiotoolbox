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
#include <stdlib.h>

#include "avfile.h"
#include "avimage.h"
#include "avhistogram.h"
#include "avspectrogram.h"
#include "avsplitter.h"
#include "avexception.h"

int main(int argc, char **argv)
{    
    bool verbose=false;
    float threshold=0;
    int c, width=0, heigh=0;
    char *from_file=0, *to_histogram=0, *to_spectrogram=0;
    
    while ((c = getopt(argc, argv, "f:t:s:w:h:l:v")) != -1)
        switch (c)
    {
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
        case 'l':
            threshold = atof(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        default:
            std::cerr << "Unknown option: "<< c << std::endl;
            return 1;
    }
    
    if (!from_file || (!to_histogram && !to_spectrogram)) {
        std::cerr
            << "From file and spectrogram or histogram is required" << std::endl
            << "-f [source] -t [histogram] -s [spectrogram] -w [width] -h [height] -l [threshold] -v" << std::endl;
        return 1;
    }

    if (width<2 || (to_histogram && heigh <2)) {
        std::cerr
                << "width and/or height is wrong (<2) or absent." << std::endl;
        return 1;
    }

    if (threshold > 0) {
        std::cerr
                << "logarithmic mode threshold can not be higher then 0." << std::endl;
        return 1;
    }

    try {
        AVFile file(44100, 1);
        file.open(from_file);

        AVImageRGBA histogram_image(width, heigh);
        AVImageRGBA spectrogram_image(width, 1);

        AVHistogram histogram(&file, &histogram_image, threshold);
        AVSpectrogram spectrogram(&file, &spectrogram_image, threshold);

        AVSplitter splitter(2);

        if (to_histogram) splitter.addObject(&histogram);
        if (to_spectrogram) splitter.addObject(&spectrogram);

        file.decoderLoop(&splitter);

        if (to_histogram) histogram_image.save(to_histogram);
        if (to_spectrogram) spectrogram_image.save(to_spectrogram);

        if (verbose) {
            std::cout
                << argv[0] << std::endl
                << "Duration(sec): " << file.getDurationTime() << std::endl
                << "Duration(samples): " << file.getDurationSamples() << std::endl
                << "Format bitrate: " << file.getBitrate() << std::endl
                << "Codec bitrate: " << file.getCodecBitrate() << std::endl
                << "Codec samplerate: " << file.getCodecSamplerate() << std::endl
                << "Codec channels: " << file.getCodecChannels() << std::endl;
        }
    } catch (AVException &e) {
        std::cerr
            << "Operation failed because of: " << e.what() << std::endl;
        return 3;
    }

    return 0;
}
