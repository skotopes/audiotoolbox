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

class ffavFile: public ffavThread
{
public:
    ffavFile(ffavPushPull *p_obj=NULL);
    ffavFile(const ffavFile&);
    virtual ~ffavFile();
    ffavFile& operator=(const ffavFile&);
    
    int openFile(char *f_name);
    size_t pull(int16_t *data, size_t len);
    
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
    bool myppObj;
};

#endif
