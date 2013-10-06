#ifndef AVSPECTROGRAM_H
#define AVSPECTROGRAM_H

#include "avobject.h"

class AVFile;
class AVImageRGBA;

class AVSpectrogram : public AVObject
{
public:
    enum WindowType {
        Square,
        Hann,
        Hamming,
        Blackman,
        BlackmanHarris
    };

    AVSpectrogram(AVFile *file, AVImageRGBA *image, float threshold=0, WindowType window_type=Square);
    virtual ~AVSpectrogram();

    const char * getName() { return "AVSpectrogram"; }

    virtual size_t pull(float *buffer_ptr, size_t buffer_size) { return 0; }
    virtual size_t push(float *buffer_ptr, size_t buffer_size);

private:
    AVFile *_file;
    AVImageRGBA *_image;

    float _threshold;
    WindowType _window_type;

    float *_in_r, *_in_i, *_out_r, *_out_i;
    size_t _cnt;

    size_t _block_size, _block_current_pos, _block_number;
    size_t _low_cnt, _mid_cnt, _high_cnt;
    float _low_rms, _mid_rms, _high_rms;

    void _processDomain();

    void _windowHann();
    void _windowHamming();
    void _windowBlackman();
    void _windowBlackmanHarris();

    void _fft(bool inverse=false);
    void _postProcess();

    void _commitBlockDb();
    void _commitBlockLinear();
};

#endif // AVSPECTROGRAM_H
