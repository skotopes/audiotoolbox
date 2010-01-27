/*
 *  re_histogram.h
 *  recoded
 *
 *  Created by Alexandr Kutuzov on 19.01.10.
 *  Copyright 2010 WhiteLabel ltd. All rights reserved.
 *
 */

#ifndef RE_HISTOGRAM_H
#define RE_HISTOGRAM_H

#include "ffavfile.h"
#include "ffavimage.h"

#define PULL_BLOCK_SIZE 4096

class histogram {
public:
    histogram(int width, int height);
    ~histogram();

    int buildImage(ffavFile *obj);
    int saveImage(char *file_name);
    
private:
    int w, h;
    imageRGBA img;
};

#endif
