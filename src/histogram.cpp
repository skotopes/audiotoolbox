/*
 *  re_histogram.cpp
 *  recoded
 *
 *  Created by Alexandr Kutuzov on 19.01.10.
 *  Copyright 2010 WhiteLabel ltd. All rights reserved.
 *
 */

#include "histogram.h"

histogram::histogram(int width, int height):
    w(width),h(height),img(width,height)
{
}

histogram::~histogram()
{
}


int histogram::buildImage(ffavFile *obj) 
{
    size_t ret;
    int channels, w_pos=0;
    int16_t *buff;
    int64_t s_size, b_size, tmp=0;
    uint64_t pos_s=0, neg_s=0, cnt=0;

    pixelRGBA full_red(255,0,0,255);
    pixelRGBA half_red(255,0,0,127);
    
    channels = obj->getCodecChannels();
    s_size = obj->getDurationSamples();
    b_size = s_size / w;
    
    buff = new int16_t[PULL_BLOCK_SIZE * channels];
    
    while ((ret = obj->pull(buff, PULL_BLOCK_SIZE)) != 0)
    {
        for (size_t i=0; i<ret; i++)
        {
            if (cnt == b_size)
            {
                cnt = 0;

                pos_s = pos_s * h / (b_size*32768);
                neg_s = neg_s * h / (b_size*32768);
                
                img.drawLine(w_pos, h/2-pos_s, w_pos, h/2+neg_s, full_red);
                pos_s = neg_s = 0; 
                w_pos++;
            }
            
            tmp = 0;
            
            for (int c=0; c<channels; c++)
                tmp += buff[i*channels+c];

            tmp /= channels;
            
            if (tmp > 0)
                pos_s += tmp;
            else
                neg_s -= tmp;

            cnt++;
        }
    }
    
    delete [] buff;
    
    return 0;
}

int histogram::saveImage(char *file_name)
{
    img.save(file_name);
    
    return 0;
}

