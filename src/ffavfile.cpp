/*
 *  ffavfile.cpp
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 21.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include "ffavfile.h"

ffavFile::ffavFile(ffavPushPull *p_obj):
    aFormatCtx(NULL), decoder(), ppObj(NULL), 
    audioStream(-1), myppObj(true), eofed(false)
{
    if (p_obj)
    {
        ppObj = p_obj;
        myppObj = false;
    }
}

ffavFile::ffavFile(const ffavFile&):
    aFormatCtx(NULL), decoder(), ppObj(NULL), 
    audioStream(-1), myppObj(true), eofed(false)
{
    throw ffavError("ffavFile: object copy is not allowed");
}

ffavFile::~ffavFile()
{
    stop();
    if (aFormatCtx)
    {
        av_close_input_file(aFormatCtx);
    }
    if (myppObj)
    {
        delete ppObj;
    }
}

ffavFile& ffavFile::operator=(const ffavFile&)
{
    throw ffavError("ffavFile: object copy is not allowed");
}

int ffavFile::openFile(char *f_name)
{
    if (myppObj)
        ppObj = new ffavFileRingS16(AVCODEC_MAX_AUDIO_FRAME_SIZE / 2 * 60);
        
    if(av_open_input_file(&aFormatCtx, f_name, NULL, 0, NULL) != 0)
        throw ffavError("ffavFile: Unable to open file");
    
    if(av_find_stream_info(aFormatCtx) < 0)
        throw ffavError("ffavFile: Stream init failed");
    
    for (uint32_t i=0; i<aFormatCtx->nb_streams; i++)
    {
        if (aFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO && 
            aFormatCtx->streams[i]->codec->channels > 0)
        {
            audioStream=i;
            break;
        }
    }

    if (audioStream==-1)
        throw ffavError("ffavFile: Audio stream not found");

    decoder.openCodec(aFormatCtx->streams[audioStream]->codec);

    if (!start())
        throw ffavError("ffavFile: Cannot start decoder");
    
    return 0;
}

size_t ffavFile::pull(int16_t *data, size_t len)
{
    return ppObj->pull(data, len*getCodecChannels()); 
}

void ffavFile::run()
{
    AVPacket packet;

    ppObj->resumeBuffer();
    
    while (av_read_frame(aFormatCtx, &packet) == 0)
    {
        if (packet.stream_index == audioStream)
        {
            decoder.decodePacket(&packet, ppObj);
        }
        
        av_free_packet(&packet);
    }
    
    eofed = true;
    ppObj->pauseBuffer();
}
