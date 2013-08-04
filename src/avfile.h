#ifndef AVFILE_H
#define AVFILE_H

#include "avconf.h"
#include "avcondition.h"
#include "avthread.h"

class AVObject;

struct AVFormatContext;
struct AVCodecContext;
struct SwrContext;

class AVFile
{
public:
    struct Progress {
        float position;
        float duration;
    };

    AVFile(int sample_rate, int channels);
    virtual ~AVFile();

    void open(const char *);
    void close();

    float getDurationTime();
    size_t getDurationSamples();
    float getPosition();
    size_t getBitrate();
    size_t getCodecBitrate();
    int getCodecSamplerate();
    int getCodecChannels();
    int getIOSamplerate();
    int getIOChannels();

    Progress getProgress();
    void seekToPositionPercent(float p);

    bool isEOF() { return eof; }

    void decoderLoop(AVObject *io_object);

private:
    AVFormatContext *formatCtx;
    AVCodecContext *codecCtx;
    SwrContext *swrCtx;
    int audioStream;

    AVObject *_io_object;
    int _io_sample_rate;
    int _io_channels;

    volatile bool do_shutdown;
    volatile bool eof;
    volatile int64_t position;
    volatile int64_t seek_to;

    void allocSWR();
    void fillRing(float * buffer, size_t size);
};

#endif // AVFILE_H
