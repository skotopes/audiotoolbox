#ifndef AVSPECTROGRAM_H
#define AVSPECTROGRAM_H

#include "avobject.h"

class AVFile;
class AVImageRGBA;

class AVSpectrogram : public AVObject
{
public:
    AVSpectrogram(AVFile *file, AVImageRGBA *image);
    virtual ~AVSpectrogram();

    const char * getName() { return "AVSpectrogram"; }

    virtual size_t pull(float *buffer_ptr, size_t buffer_size) { return 0; }
    virtual size_t push(float *buffer_ptr, size_t buffer_size);

private:
    AVFile *_file;
    AVImageRGBA *_image;

    float *_in_r, *_in_i, *_out_r, *_out_i;
    size_t _cnt;

    size_t _block_size, _block_current_pos, _block_number;
    size_t _low_cnt, _mid_cnt, _high_cnt;
    float _low_rms, _mid_rms, _high_rms;

    void _processDomain();
};

#endif // AVSPECTROGRAM_H
