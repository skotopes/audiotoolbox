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

const AVColorRGBA hi_red(255,0,0,255);
const AVColorRGBA lo_red(255,0,0,127);

AVHistogram::AVHistogram(AVFile *file, AVImageRGBA *image):
    _file(file), _image(image),
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
        float v = *buffer_ptr++;    // yes i understand what i'm doing
                                    // * got higher priority then ++

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
            // compute block and apply it the image
            int height = _image->getHeight();
            // Block RMS value
            // stage 1: prepare RMS value for block + make it look like log
            _pos_rms = _pos_cnt > 0 ? sqrtf(sqrtf(_pos_rms/_pos_cnt)) : 0;
            _neg_rms = _neg_cnt > 0 ? sqrtf(sqrtf(_neg_rms/_neg_cnt)) : 0;
            // stage 2: calculate line points
            _pos_rms = _pos_rms * height;
            _neg_rms = _neg_rms * height;

            // Block peak value
            // calculate line points
            _pos_peak = sqrtf(_pos_peak) * height;
            _neg_peak = sqrtf(_neg_peak) * height;

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

            _pos_peak = _neg_peak = 0;
            _pos_rms = _neg_rms = 0;
            _pos_cnt = _neg_cnt = 0;
            _block_current_pos = 0;
            _block_number++;
        }
    }
    
    return buffer_size;
}
