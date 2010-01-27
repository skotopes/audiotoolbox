/*
 *  ffavfile.h
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#ifndef FFAVFILE_H
#define FFAVFILE_H

#include "ffavring.h"
#include "ffavcodec.h"
#include "ffavthread.h"

class ffavFileRingS16: private ffavRing, public ffavPushPull
{
public:
    ffavFileRingS16(size_t s): 
        ffavRing(s*2), paused(false), 
        w_req(0), r_req(0), r_cond(), w_cond() {};

    virtual ~ffavFileRingS16() {};

    inline size_t pull(int16_t *data, size_t len)
    {
        if (len*2 > readSpace() && !paused)
        {
            r_req = len;
            r_cond.waitCond();
        }
        size_t ret = pull_data((uint8_t*)data, len*2);
        if (w_req > 0 && (w_req*2) < writeSpace())
        {
            w_req = 0;
            w_cond.raiseCond();
        }
        return ret;
    };
    inline size_t push(int16_t *data, size_t len)
    {
        if (len*2 > writeSpace())
        {
            w_req = len;
            w_cond.waitCond();
        }
        size_t ret = push_data((uint8_t*)data, len*2);
        if (r_req > 0 && r_req*2 < readSpace())
        {
            r_req = 0;
            r_cond.raiseCond();
        }
        return ret; 
    };

    inline bool resetBuffer()
    {
        reset();
        w_cond.raiseCond();
        r_cond.raiseCond();
        return true; 
    };
    inline bool pauseBuffer()
    {
        paused = true;
        r_cond.raiseCond();
        return true;
    };
    
    inline bool resumeBuffer()
    {
        paused = false;
        return true;
    };

private:
    bool paused;
    volatile size_t w_req, r_req;
    ffavCondition r_cond, w_cond;
};

class ffavFile: public ffavThread
{
public:
    ffavFile(ffavPushPull *p_obj=NULL);
    ffavFile(const ffavFile&);
    virtual ~ffavFile();
    ffavFile& operator=(const ffavFile&);
    
    int openFile(char *f_name);
    size_t pull(int16_t *data, size_t len);
    
    inline double isEOF()
        { return eofed; };
    
    inline double getDurationSec()
        { return aFormatCtx->duration/1000000.0; };

    inline int64_t getDurationSamples() 
        { return aFormatCtx->duration*getCodecSamplerate()/1000000L; };

    inline int64_t getFileSize() 
        { return aFormatCtx->file_size; };

    inline int getBitrate()
        { return aFormatCtx->bit_rate; };

    inline int getCodecBitrate() 
        { return decoder.getBitrate(); };

    inline int getCodecSamplerate() 
        { return decoder.getSamplerate(); };

    inline int getCodecChannels() 
        { return decoder.getChannels(); };
    
protected:
    virtual void run();
private:
    AVFormatContext *aFormatCtx;
    ffavPushPull *ppObj;
    ffavDecoder decoder;
    int audioStream;
    bool myppObj, eofed;
};

#endif
