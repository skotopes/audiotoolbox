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
#include "avexception.h"

using namespace std;

int main(int argc, char **argv)
{    
    int c, width=0, heigh=0;
    bool verbose = false;
    char *from_file = NULL, *to_file = NULL;
    
    while ((c = getopt (argc, argv, "f:t:w:h:v")) != -1)
        switch (c)
    {
        case 'v':
            verbose = true;
            break;
        case 'f':
            from_file = optarg;
            break;
        case 't':
            to_file = optarg;
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'h':
            heigh = atoi(optarg);
            break;
            
        default:
            cout << "Unknown option: "<< c << endl
            << "From file, To file required" << endl
            << "-f [filename] -t [filename]" << endl;
            return 1;
    }
    
    if (from_file == NULL||to_file == NULL) {
        cout << "From file, To file required" << endl
        << "-f [filename] -t [filename] -w [width] -h [height]" << endl;
        return 1;
    }

    if (width<2 || heigh <2) {
        cout << "width and/or height is wrong(<2) or absent" << endl
        << "-f [filename] -t [filename] -w [width] -h [height]" << endl;
        return 1;
    }

    try {
        AVFile file;
        file.open(from_file);

        AVImageRGBA image(width, heigh);

        AVHistogram histogram(&file, &image);

        file.decoderLoop(&histogram);
        image.save(to_file);

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
