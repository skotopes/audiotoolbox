#ifndef FFAVBASE_H
#define FFAVBASE_H

#include <stdint.h>
#include <unistd.h>
#include "ffaverror.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

class ffavBase
{
public:
    ffavBase();
    virtual ~ffavBase();
    ffavBase operator=(ffavBase);
private:
    static bool ffmpeginit;
};

class ffavPushPull {
public:
    ffavPushPull();
    virtual ~ffavPushPull();
    virtual size_t push(int16_t *data, size_t len)=0;
    virtual size_t pull(int16_t *data, size_t len)=0;
    virtual bool resetBuffer()=0;
    virtual bool pauseBuffer()=0;
    virtual bool resumeBuffer()=0;
};

#endif
