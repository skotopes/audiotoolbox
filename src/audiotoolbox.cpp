#include "audiotoolbox.h"

#include <iostream>
#include <getopt.h>
#include <string.h>

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
    _input_filename(0), _histogram_filename(0), _spectrogram_filename(0), _combo_filename(0),
    _window_name(0),
    _input(0), _splitter(0),
    _spectrogram_image(0), _histogram_image(0), _combo_image(0),
    _histogram(0), _spectrogram(0)
{

}

AudioToolbox::~AudioToolbox() {
    delete _input;

    delete _spectrogram_image;
    delete _histogram_image;
    delete _combo_image;

    delete _histogram;
    delete _spectrogram;
    delete _splitter;
}

int AudioToolbox::run() {
    option longopts[] = {
        { "help",           no_argument,        0,  'h' },
        { "verbose",        no_argument,        0,  'v' },
        { "width",          required_argument,  0,  1 },
        { "height",         required_argument,  0,  2 },
        { "input",          required_argument,  0,  3 },
        { "histogram",      required_argument,  0,  4 },
        { "spectrogram",    required_argument,  0,  5 },
        { "combo",          required_argument,  0,  6 },
        { "threshold",      required_argument,  0,  7 },
        { "window",         required_argument,  0,  8 },
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
            _input_filename = optarg;
            break;
        case 4:
            _histogram_filename = optarg;
            break;
        case 5:
            _spectrogram_filename = optarg;
            break;
        case 6:
            _combo_filename = optarg;
            break;
        case 7:
            _threshold = atof(optarg);
            break;
        case 8:
            _window_name = optarg;
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

    if (!_input_filename) {
        std::cerr << "Input file is not specified" << std::endl;
        return 1;
    }

    if (!_histogram_filename && !_spectrogram_filename && !_combo_filename) {
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

    if ((_histogram_filename || _combo_filename) && !_height) {
        std::cerr << "Height is not specified" << std::endl;
        return 1;
    } else if ((_histogram_filename || _combo_filename) && _height < 2) {
        std::cerr << "Height must be greater than 2" << std::endl;
        return 1;
    }

    try {
        _log("Opening input file");
        _input = new AVFile(44100, 1);
        _input->open(_input_filename);

        _log("Preparing stream splitter");
        _splitter = new AVSplitter(2);

        if (_histogram_filename || _combo_filename) {
            _log("Adding histogram processing module");
            _histogram_image = new AVImageRGBA(_width, _height);
            _histogram = new AVHistogram(_input, _histogram_image, _threshold);
            _splitter->addObject(_histogram);
        }

        if (_spectrogram_filename || _combo_filename) {
            _log("Adding spectrogram processing module");
            _spectrogram_image = new AVImageRGBA(_width, 1);

            if (_window_name) {
                if (strcmp(_window_name, "hann") == 0) {
                    _spectrogram = new AVSpectrogram(_input, _spectrogram_image, _threshold, AVSpectrogram::Hann);
                } else if (strcmp(_window_name, "hamming") == 0) {
                    _spectrogram = new AVSpectrogram(_input, _spectrogram_image, _threshold, AVSpectrogram::Hamming);
                } else if (strcmp(_window_name, "blackman") == 0) {
                    _spectrogram = new AVSpectrogram(_input, _spectrogram_image, _threshold, AVSpectrogram::Blackman);
                } else if (strcmp(_window_name, "blackmanharris") == 0) {
                    _spectrogram = new AVSpectrogram(_input, _spectrogram_image, _threshold, AVSpectrogram::BlackmanHarris);
                }
            }else {
                _spectrogram = new AVSpectrogram(_input, _spectrogram_image, _threshold);
            }

            _splitter->addObject(_spectrogram);
        }

        _log("Processing data");
        _input->decoderLoop(_splitter);

        if (_histogram_filename) {
            _log("Saving histogram");
            _histogram_image->save(_histogram_filename);
        }

        if (_spectrogram_filename) {
            _log("Saving spectrogram");
            _spectrogram_image->save(_spectrogram_filename);
        }

        if (_combo_filename) {
            _log("Preparing combo file");
            _combo_image = new AVImageRGBA(_width, _height);
            _combo_image->tileImage(_spectrogram_image);
            _combo_image->applyMask(_histogram_image);

            _log("Saving combo file");
            _combo_image->save(_combo_filename);
        }

        _report();
    } catch (AVException &e) {
        std::cerr << "Operation failed because of: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}

void AudioToolbox::_log(const char *message) {
    if (_verbose) std::cout << message << std::endl;
}

void AudioToolbox::_report() {
    if (_verbose) {
        std::cout
            << std::endl
            << "Input file info" << std::endl
            << "===============" << std::endl
            << "Duration(sec):      " << _input->getDurationTime() << std::endl
            << "Duration(samples):  " << _input->getDurationSamples() << std::endl
            << "Format bitrate:     " << _input->getBitrate() << std::endl
            << "Codec bitrate:      " << _input->getCodecBitrate() << std::endl
            << "Codec samplerate:   " << _input->getCodecSamplerate() << std::endl
            << "Codec channels:     " << _input->getCodecChannels() << std::endl;
    }
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
        << "        --window=STRING         Use specific FFT window type. Can be: hann, hamming, blackman, blackmanharris" << std::endl
        << std::endl
        << "This software is licensed under GPLv3." << std::endl
        << "Copyright (C) 2008-2013 by aku <alleteam@gmail.com>." << std::endl;
}
