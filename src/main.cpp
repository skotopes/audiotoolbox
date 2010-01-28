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

#include "ffavfile.h"
#include "histogram.h"

using namespace std;

int main(int argc, char **argv)
{    
    int c, width=0, heigh=0;
    bool verbose = false, single = false;
    char *from_file = NULL, *to_file = NULL;
    
    while ((c = getopt (argc, argv, "f:t:w:h:vs")) != -1)
        switch (c)
    {
        case 'v':
            verbose = true;
            break;
        case 's':
            single = true;
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
            << "From file, To file and Profile required" << endl
            << "-f [filename] -t [filename]" << endl;
            return 1;
    }
    
    if (from_file == NULL||to_file == NULL) {
        cout << "From file, To file required" << endl
        << "-f [filename] -t [filename] -w [width] -h [height] -s" << endl;
        return 1;
    }

    if (width<2 || heigh <2) {
        cout << "width and height is wrong or absent" << endl
        << "-f [filename] -t [filename] -w [width] -h [height] -s" << endl;
        return 1;
    }
    
    ffavFile f;
    
    try
    {
        f.openFile(from_file);
    }
    catch (...)
    {
        return 2;
    }
        
    histogram h(width, heigh);
    h.buildImage(&f, single);
    h.saveImage(to_file);
    
    if (verbose) cout 
        << "Duration(sec): " << f.getDurationSec() << endl
        << "Duration(samples): " << f.getDurationSamples() << endl
        << "File Size: " << f.getFileSize() << endl
        << "Format bitrate: " << f.getBitrate() << endl
        << "Codec bitrate: " << f.getCodecBitrate() << endl
        << "Codec samplerate: " << f.getCodecSamplerate() << endl
        << "Codec channels: " << f.getCodecChannels() << endl;
    
    return 0;
}
