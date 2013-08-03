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

const AVPixelRGBA hi_red = AVPixelRGBA (255,0,0,255);
const AVPixelRGBA mi_red = AVPixelRGBA (255,0,0,190);
const AVPixelRGBA lo_red = AVPixelRGBA (255,0,0,127);

AVHistogram::AVHistogram(AVFile *file, AVImageRGBA *image):
    _file(file), _image(image),
    _channels(0), _audio_length(0),
    _block_size(0), _block_current_pos(0), _block_number(0), _pos_cnt(0), _neg_cnt(0),
    _pos_peak(0), _neg_peak(0), _pos_rms(0), _neg_rms(0)
{
    _channels = _file->getCodecChannels();
    _audio_length = _file->getDurationSamples() * _file->getCodecChannels();
    _block_size = _audio_length / _image->getWidth();
    _height = _image->getHeight();
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

            // Block RMS value
            // stage 1: prepare RMS value for block
            _pos_rms = _pos_cnt > 0 ? sqrtf(sqrtf(_pos_rms/_pos_cnt)) : 0;
            _neg_rms = _neg_cnt > 0 ? sqrtf(sqrtf(_neg_rms/_neg_cnt)) : 0;
            // stage 2: calculate line points
            _pos_rms = _pos_rms * _height;
            _neg_rms = _neg_rms * _height;

            // Block peak value
            // calculate line points
            _pos_peak = _pos_peak * _height;
            _neg_peak = _neg_peak * _height;

            // Draw peaks
            _image->drawLine(
                        _block_number, _height/2 - _pos_peak/2,
                        _block_number, _height/2 + _neg_peak/2 - 1,
                        lo_red
            );
            // Draw RMS
            _image->drawLine(
                        _block_number, _height/2 - _pos_rms/2,
                        _block_number, _height/2 + _neg_rms/2 - 1,
                        hi_red
            );

            _pos_peak = _neg_peak = 0;
            _pos_rms = _neg_rms = 0;
            _pos_cnt = _neg_cnt = 0;
            ++_block_number;
            _block_current_pos = 0;
        }

    }
    
    return buffer_size;
}
