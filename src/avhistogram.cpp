///*
// *  re_histogram.cpp
// *  recoded
// *
// *  Created by Alexandr Kutuzov on 19.01.10.
// *  Copyright 2010 WhiteLabel ltd. All rights reserved.
// *
// */

#include "avhistogram.h"
#include "avimage.h"
#include "avfile.h"
#include "math.h"

#include <iostream>

const AVColorRGBA hi_red(255,0,0,255);
const AVColorRGBA lo_red(255,0,0,127);

AVHistogram::AVHistogram(AVFile *file, AVImageRGBA *image, float threshold):
    _file(file), _image(image), _threshold(threshold),
    _block_size(0), _block_current_pos(0), _block_number(0), _pos_cnt(0), _neg_cnt(0),
    _pos_peak(0), _neg_peak(0), _pos_rms(0), _neg_rms(0)
{
    _block_size = _file->getDurationSamples() * _file->getIOChannels() / _image->getWidth();
}

AVHistogram::~AVHistogram() {
}

size_t AVHistogram::push(float *buffer_ptr, size_t buffer_size) {
    size_t to_consume=buffer_size;

    while (to_consume-- > 0) {
        float v = *buffer_ptr++;

        // process sample
        if (v > 0) {
            if (v > _pos_peak) _pos_peak = v;
            _pos_rms += v*v;
            _pos_cnt++;
        } else {
            if ((-v) > _neg_peak) _neg_peak = -v;
            _neg_rms += v*v;
            _neg_cnt++;
        }

        if (++_block_current_pos == _block_size) {
            if (_threshold != 0.0f) {
                _commitBlockDb();
            } else {
                _commitBlockLinear();
            }

            _pos_peak = _neg_peak = 0;
            _pos_rms = _neg_rms = 0;
            _pos_cnt = _neg_cnt = 0;
            _block_current_pos = 0;
            _block_number++;
        }
    }
    
    return buffer_size;
}

void AVHistogram::_commitBlockDb() {
    int height = _image->getHeight();

    // Peak value
    _pos_peak = 10 * log10f(_pos_peak);
    _neg_peak = 10 * log10f(_neg_peak);

    // RMS value
    _pos_rms = 10 * log10f(sqrtf(_pos_rms / _pos_cnt));
    _neg_rms = 10 * log10f(sqrtf(_neg_rms / _neg_cnt));

    // Apply threshold
    if (_neg_peak > _threshold) {
        _pos_peak = (_neg_peak - _threshold) / - _threshold * height;
    } else {
        _pos_peak = 0;
    }
    if (_neg_peak > _threshold) {
        _neg_peak = (_neg_peak - _threshold) / - _threshold * height;
    } else {
        _neg_peak = 0;
    }
    if (_pos_rms > _threshold) {
        _pos_rms = (_pos_rms - _threshold) / - _threshold * height;
    } else {
        _pos_rms = 0;
    }
    if (_neg_rms > _threshold) {
        _neg_rms = (_neg_rms - _threshold) / - _threshold * height;
    } else {
        _neg_rms = 0;
    }

    // Draw peaks
    _image->drawLine(
                _block_number, height/2 - _pos_peak/2,
                _block_number, height/2 + _neg_peak/2 - 1,
                lo_red
    );
    // Draw RMS
    _image->drawLine(
                _block_number, height/2 - _pos_rms/2,
                _block_number, height/2 + _neg_rms/2 - 1,
                hi_red
    );
}

void AVHistogram::_commitBlockLinear() {
    int height = _image->getHeight();

    // Block peak value
    _pos_peak = sqrtf(_pos_peak) * height;
    _neg_peak = sqrtf(_neg_peak) * height;

    // Block RMS value
    _pos_rms = sqrtf(sqrtf(_pos_rms/_pos_cnt)) * height;
    _neg_rms = sqrtf(sqrtf(_neg_rms/_neg_cnt)) * height;

    // Draw peaks
    _image->drawLine(
                _block_number, height/2 - _pos_peak/2,
                _block_number, height/2 + _neg_peak/2 - 1,
                lo_red
    );
    // Draw RMS
    _image->drawLine(
                _block_number, height/2 - _pos_rms/2,
                _block_number, height/2 + _neg_rms/2 - 1,
                hi_red
    );
}

