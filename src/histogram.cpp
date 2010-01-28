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

int histogram::buildImage(ffavFile *obj, bool single) 
{
    size_t ret;
    int channels, w_pos=0;
    int16_t *buff, temp;
    int64_t samples_cnt, block_size, count=0;
    uint64_t pos_max=0, neg_max=0, pos_avg=0, neg_avg=0, pos_sqrt=0, neg_sqrt=0;

    pixelRGBA full_red(255,0,0,255);
    pixelRGBA mid_red(255,0,0,190);
    pixelRGBA low_red(255,0,0,127);
    
    channels    = obj->getCodecChannels();
    samples_cnt = obj->getDurationSamples();
    block_size  = samples_cnt * (single ? 1 : channels) / w;
    
    buff = new int16_t[PULL_BLOCK_SIZE * channels];

    while ((ret = obj->pull(buff, PULL_BLOCK_SIZE)) != 0)
    {
        for (size_t i=0; i<ret; i++)
        {
            if (single && i%channels != 0)
                continue;
            
            if (count == block_size)
            {
                pos_max = pos_max * h / (2*32768);
                neg_max = neg_max * h / (2*32768);

                pos_avg = pos_avg * h / (2*32768 * block_size);
                neg_avg = neg_avg * h / (2*32768 * block_size);
                
                pos_sqrt = (uint64_t)sqrt((pos_avg*pos_avg + pos_max*pos_max)/2);
                neg_sqrt = (uint64_t)sqrt((neg_avg*neg_avg + neg_max*neg_max)/2);
                
                img.drawLine(w_pos, h/2-pos_max, w_pos, h/2+neg_max, low_red);
                img.drawLine(w_pos, h/2-pos_sqrt, w_pos, h/2+neg_sqrt, mid_red);
                img.drawLine(w_pos, h/2-pos_avg, w_pos, h/2+neg_avg, full_red);

                count = pos_max = neg_max = pos_avg = neg_avg = 0;
                w_pos++;
            }
            
            temp = buff[i];
            
            if (temp > 0)
            {
                if (temp > pos_max) pos_max = temp;
                pos_avg += temp;
            }
            else
            {
                if ((-temp) > neg_max) neg_max = -temp;
                neg_avg -= temp;
            }
            
            count++;
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

