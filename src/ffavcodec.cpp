/*
 *  ffavcodec.cpp
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include "ffavcodec.h"

ffavDecoder::ffavDecoder():
    aCodecCtx(NULL), aCodec(NULL), aBuff(NULL), myContext(true)
{
}

ffavDecoder::ffavDecoder(const ffavDecoder&):
    aCodecCtx(NULL), aCodec(NULL), aBuff(NULL), myContext(true)
{
}

ffavDecoder::~ffavDecoder()
{
    if (aCodecCtx)
    {
        avcodec_close(aCodecCtx);
    }
    if (myContext)
    {
        av_free(aCodecCtx);
    }
    if (aBuff)
    {
        av_free(aBuff);
    }
}

ffavDecoder& ffavDecoder::operator=(const ffavDecoder&)
{
    throw ffavError("ffavCodec: object copy is not allowed");
}

int ffavDecoder::openCodec(CodecID id)
{
    aCodec = avcodec_find_decoder(id);
    
    if (!aCodec)
        throw ffavError("ffavCodec: codec is not availiable");
    
    if (!aCodecCtx)
        aCodecCtx = avcodec_alloc_context();

    if (avcodec_open(aCodecCtx, aCodec) < 0)
        throw ffavError("ffavCodec: cannot open codec");
    
    aBuff = reinterpret_cast<int16_t*>(av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE));
    
    if (!aBuff)
        throw ffavError("ffavCodec: cannot allocate buffer");
    
    return 0;
}

int ffavDecoder::openCodec(AVCodecContext *ctx)
{
    aCodecCtx = ctx;
    myContext = false;
    return openCodec(aCodecCtx->codec_id);
}

int ffavDecoder::decodePacket(AVPacket *packet, ffavPushPull *obj)
{
    int out_size, len, ret = 0;
    
    // Copy initial packet size and data pointer
    int packet_size = packet->size;
    uint8_t *packet_data = packet->data;
    
    while (packet->size > 0)
    {
        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE; // in bytes
        len = avcodec_decode_audio3(aCodecCtx, aBuff, &out_size, packet);
        
        if(len < 0) 
            break;
        
        if (out_size > 0)
        {
            out_size /= 2;
            obj->push(aBuff, out_size);
            ret += out_size;
        }
        
        packet->size -= len;
        packet->data += len;
    }

    // Restore initial packet size and data pointer,
    // because we going to free it later ;)
    packet->size = packet_size;
    packet->data = packet_data;
    
    return ret;
}

