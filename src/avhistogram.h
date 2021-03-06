#ifndef AVHISTOGRAM_H
#define AVHISTOGRAM_H

#include "avobject.h"

class AVImageRGBA;
class AVFile;

class AVHistogram : public AVObject {
public:
    AVHistogram(AVFile *file, AVImageRGBA *image, float threshold=0);
    virtual ~AVHistogram();

    const char * getName() { return "AVHistogram"; }

    virtual size_t pull(float *buffer_ptr, size_t buffer_size) { return 0; }
    virtual size_t push(float *buffer_ptr, size_t buffer_size);

private:
    AVFile *_file;
    AVImageRGBA *_image;

    float _threshold;
    size_t _block_size, _block_current_pos, _block_number, _pos_cnt, _neg_cnt;
    float _pos_peak, _neg_peak, _pos_rms, _neg_rms;

    void _commitBlockDb();
    void _commitBlockLinear();
};

#endif
