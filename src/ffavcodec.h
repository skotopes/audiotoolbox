/*
 *  ffavcodec.h
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#ifndef FFAVCODEC_H
#define FFAVCODEC_H

#include "ffavbase.h"

class ffavDecoder: private ffavBase
{
public:
    ffavDecoder();
    ffavDecoder(const ffavDecoder&);
    virtual ~ffavDecoder();
    ffavDecoder& operator=(const ffavDecoder&);
    
    int openCodec(CodecID id);    
    int openCodec(AVCodecContext *ctx);
    int decodePacket(AVPacket *packet, ffavPushPull *obj);

    inline int getBitrate() { return aCodecCtx->bit_rate; };
    inline int getSamplerate() { return aCodecCtx->sample_rate; };
    inline int getChannels() { return aCodecCtx->channels; };
    
private:
    AVCodecContext *aCodecCtx;
    AVCodec *aCodec;
    int16_t *aBuff;
    bool myContext;
};

#endif
