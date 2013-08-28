#include "audiotoolbox.h"

#include <iostream>
#include <getopt.h>

#include "avfile.h"
#include "avimage.h"
#include "avhistogram.h"
#include "avspectrogram.h"
#include "avsplitter.h"
#include "avexception.h"

AudioToolbox::AudioToolbox(int argc, char **argv):
    _argc(argc), _argv(argv),
    _verbose(false), _threshold(0),
    _width(0), _height(0),
    _input(0), _histogram(0), _spectrogram(0), _combo(0)
{

}

AudioToolbox::~AudioToolbox() {

}

int AudioToolbox::run() {
    static struct option longopts[] = {
        { "help",           no_argument,        0,  'h' },
        { "verbose",        no_argument,        0,  'v' },
        { "width",          required_argument,  0,  1 },
        { "height",         required_argument,  0,  2 },
        { "input",          required_argument,  0,  3 },
        { "histogram",      required_argument,  0,  4 },
        { "spectrogram",    required_argument,  0,  5 },
        { "combo",          required_argument,  0,  6 },
        { "threshold",      required_argument,  0,  7 },
        { 0,                0,                  0,  0 }
    };

    int c;
    while ((c = getopt_long(_argc, _argv, "vh", longopts, NULL)) != -1) {
        switch (c) {
        case 1:
            _width = atoi(optarg);
            break;
        case 2:
            _height = atoi(optarg);
            break;
        case 3:
            _input = optarg;
            break;
        case 4:
            _histogram = optarg;
            break;
        case 5:
            _spectrogram = optarg;
            break;
        case 6:
            _combo = optarg;
            break;
        case 7:
            _threshold = atof(optarg);
            break;
        case 'v':
            _verbose = true;
            break;
        case 'h':
            _showUsage();
            return 0;
        default:
            _showUsage();
            return 1;
        }
    }

    if (!_input) {
        std::cerr << "Input file is not specified" << std::endl;
        return 1;
    }

    if (!_histogram && !_spectrogram && !_combo) {
        std::cerr << "You need to specify at least one output: spectrogram | histogram | combo" << std::endl;
        return 1;
    }

    if (!_width) {
        std::cerr << "Width is not specified" << std::endl;
        return 1;
    } else if (_width < 2) {
        std::cerr << "Width must be greater than 2" << std::endl;
        return 1;
    }

    if ((_histogram || _combo) && !_height) {
        std::cerr << "Height is not specified" << std::endl;
        return 1;
    } else if ((_histogram || _combo) && _height < 2) {
        std::cerr << "Height must be greater than 2" << std::endl;
        return 1;
    }

    try {
        AVFile file(44100, 1);
        file.open(_input);

        AVImageRGBA histogram_image(_width, _height);
        AVImageRGBA spectrogram_image(_width, 1);
        AVImageRGBA combo_image(_width, _height);

        AVHistogram histogram(&file, &histogram_image, _threshold);
        AVSpectrogram spectrogram(&file, &spectrogram_image, _threshold);

        AVSplitter splitter(2);

        if (_histogram || _combo) splitter.addObject(&histogram);
        if (_spectrogram || _combo) splitter.addObject(&spectrogram);

        file.decoderLoop(&splitter);

        if (_histogram) histogram_image.save(_histogram);
        if (_spectrogram) spectrogram_image.save(_spectrogram);

        if (_combo) {
            combo_image.tileImage(spectrogram_image);
            combo_image.applyMask(histogram_image);
            combo_image.save(_combo);
        }

        if (_verbose) {
            std::cout
                << _argv[0] << std::endl
                << "Duration(sec): " << file.getDurationTime() << std::endl
                << "Duration(samples): " << file.getDurationSamples() << std::endl
                << "Format bitrate: " << file.getBitrate() << std::endl
                << "Codec bitrate: " << file.getCodecBitrate() << std::endl
                << "Codec samplerate: " << file.getCodecSamplerate() << std::endl
                << "Codec channels: " << file.getCodecChannels() << std::endl;
        }
    } catch (AVException &e) {
        std::cerr << "Operation failed because of: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}

void AudioToolbox::_showUsage() {
    std::cerr
        << "Usage:" << std::endl
        << " " << _argv[0] << " [OPTIONS]" << std::endl
        << std::endl
        << "Options:" << std::endl
        << " -h,    --help                  Show this help message" << std::endl
        << " -v,    --verbose               Show debug information" << std::endl
        << "        --width=INT             Set histogram/spectrogram/combo width" << std::endl
        << "        --height=INT            Set histogram/combo height" << std::endl
        << "        --input=FILE            Input audio to process" << std::endl
        << "        --histogram=FILE        Output histogram" << std::endl
        << "        --spectrogram=FILE      Output spectrogram" << std::endl
        << "        --combo=FILE            Output combo image: histogram shaped spectrogram" << std::endl
        << "        --threshold=FLOAT       If specified will switch processing mode to logarithmic" << std::endl
        << std::endl
        << "This software is licensed under GPLv3." << std::endl
        << "Copyright (C) 2008-2013 by aku <alleteam@gmail.com>." << std::endl;
}
