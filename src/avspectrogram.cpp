#include "avspectrogram.h"

#include "avfile.h"
#include "avimage.h"
#include "math.h"

#include "iostream"

#define FFT_WINDOW_SIZE 2048

AVSpectrogram::AVSpectrogram(AVFile *file, AVImageRGBA *image):
    _file(file), _image(image),
    _in_r(0), _in_i(0), _out_r(0), _out_i(0),
    _cnt(0),
    _block_size(0), _block_current_pos(0), _block_number(0),
    _low_cnt(0), _mid_cnt(0), _high_cnt(0),
    _low_rms(0), _mid_rms(0), _high_rms(0)
{
    _block_size = _file->getDurationSamples() * _file->getIOChannels() / _image->getWidth();
    _in_r = new float [FFT_WINDOW_SIZE];
    _in_i = new float [FFT_WINDOW_SIZE];
    _out_r = new float [FFT_WINDOW_SIZE];
    _out_i = new float [FFT_WINDOW_SIZE];
}

AVSpectrogram::~AVSpectrogram() {
    delete [] _in_r;
    delete [] _in_i;
    delete [] _out_r;
    delete [] _out_i;

}

size_t AVSpectrogram::push(float *buffer_ptr, size_t buffer_size) {
    size_t to_consume=buffer_size;

    while (to_consume-- > 0) {
        _in_r[_cnt] = *buffer_ptr++;  // yes i understand what i'm doing
        // * got higher priority then ++

        _out_r[_cnt] = _out_i[_cnt] = _in_i[_cnt] = 0;

        if (++_cnt == FFT_WINDOW_SIZE) {
            _processDomain();
            _cnt = 0;
        }

        if (++_block_current_pos == _block_size) {
            _low_rms = _low_rms / _low_cnt / 2;
            _mid_rms = _mid_rms / _mid_cnt;
            _high_rms = _high_rms / _high_cnt * 10;

            // calculate color
            float maximum = _low_rms;
            if (_mid_rms > maximum) {
                maximum = _mid_rms;
            } else if (_high_rms > maximum) {
                maximum = _high_rms;
            }

            _low_rms = _low_rms / maximum * 255;
            _mid_rms = _mid_rms / maximum * 255;
            _high_rms = _high_rms / maximum * 255;

            AVPixelRGBA pixel(_low_rms, _mid_rms, _high_rms, 255);
            _image->drawPoint(_block_number, 0, pixel);

            // nullify variables
            _low_rms = _mid_rms = _high_rms = 0;
            _low_cnt = _mid_cnt = _high_cnt = 0;
            _block_current_pos = 0;
            ++_block_number;
        }
    }

    return buffer_size;
}

void AVSpectrogram::_processDomain() {
    int n = FFT_WINDOW_SIZE;
    bool inverse = 1;

    //Calculate m=log_2(n)
    int m=0, p=1;
    while(p < n) {
        p *= 2;
        m++;
    }

    //Bit reversal
    _out_r[n - 1] = _in_r[n - 1];
    _out_i[n - 1] = _in_i[n - 1];
    int j = 0;
    for(int i = 0; i < n - 1; i++) {
        _out_r[i] = _in_r[j];
        _out_i[i] = _in_i[j];
        int k = n / 2;
        while(k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }

    //Calculate the FFT
    double ca = -1.0;
    double sa = 0.0;
    int l1 = 1, l2 = 1;
    for(int l = 0; l < m; l++) {
        l1 = l2;
        l2 *= 2;
        double u1 = 1.0;
        double u2 = 0.0;
        for(int j = 0; j < l1; j++) {
            for(int i = j; i < n; i += l2) {
                int i1 = i + l1;
                double t1 = u1 * _out_r[i1] - u2 * _out_i[i1];
                double t2 = u1 * _out_i[i1] + u2 * _out_r[i1];
                _out_r[i1] = _out_r[i] - t1;
                _out_i[i1] = _out_i[i] - t2;
                _out_r[i] += t1;
                _out_i[i] += t2;
            }
            double z =  u1 * ca - u2 * sa;
            u2 = u1 * sa + u2 * ca;
            u1 = z;
        }
        sa = sqrt((1.0 - ca) / 2.0);
        if (!inverse) sa = -sa;
        ca = sqrt((1.0 + ca) / 2.0);
    }

    //Divide through n if it isn't the IDFT
    if(!inverse) {
        for(int i = 0; i < n; i++)
        {
            _out_r[i] /= n;
            _out_i[i] /= n;
        }
    }

    // amplitede
    for(int x = 0; x < n; x++) {
        float value = sqrt(_out_r[x] * _out_r[x] + _out_i[x] * _out_i[x]);
        if (x > (n * 1000 / 20000)) {
            // high frequency
            _high_rms += value;
            _high_cnt ++;
        } else if (x > (n * 80 / 20000)) {
            // mid frequency
            _mid_rms += value;
            _mid_cnt ++;
        } else {
            // low frequency
            _low_rms += value;
            _low_cnt ++;
        }
    }
}
