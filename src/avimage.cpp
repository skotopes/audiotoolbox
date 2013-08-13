/*
 *  avimage.cpp
 *
 *  Created by Alexandr Kutuzov on 26.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include <vector>
#include <math.h>

#include "avimage.h"
#include "lodepng.h"
#include "avexception.h"

#include <iostream>


AVImageRGBA::AVImageRGBA(int width, int height):
    _width(width), _height(height), _buffer(NULL)
{
    size_t size = _width * _height * 4;
    _buffer = new uint8_t [size];
    for (int i=0; i<size; i++) {
        _buffer[i]=0;
    }
}

AVImageRGBA::~AVImageRGBA()
{
    delete [] _buffer;
}

void AVImageRGBA::drawPoint(int x, int y, AVColorRGBA color)
{
    if (x >= _width || y >= _height || x < 0 || y < 0)
    {
        std::cerr << "Invalid position x:" << x <<" y:"<< y << std::endl;
        return;
    }   

    _buffer[4 * _width * y + 4 * x + 0] = color.getR();
    _buffer[4 * _width * y + 4 * x + 1] = color.getG();
    _buffer[4 * _width * y + 4 * x + 2] = color.getB();
    _buffer[4 * _width * y + 4 * x + 3] = color.getA();
}

void AVImageRGBA::drawLine(int x1, int y1, int x2, int y2, AVColorRGBA color)
{
    int x=x1, y=y1;
    float k=0, a=0, s;
    bool xo=false, yo=false;

    if (x1 == x2) xo = true;
    if (y1 == y2) yo = true;
    
    if (xo && yo)
    {
        drawPoint(x1, y1, color);
    }
    else if (xo && !yo)
    {
        s = (y2-y1)/abs(y2-y1);

        for (float i=y1; y!=y2; i+=s)
        {
            y = (int)i;
            drawPoint(x, y, color);
        }
    }
    else
    {
        k = (x1-x2)/(y1-y2);
        a = y1 - k * x1;
        
        if (abs(x2-x1) > (abs(y2-y1))) 
            s = (x2-x1)/abs(x2-x1);
        else
            s = (x2-x1)/abs(y2-y1);
        
        for (float i=x1; x!=x2; i+=s)
        {
            x = (int)i;
            y = (int)((k*i)+a);
            drawPoint(x, y, color);
        }
    }
}

int AVImageRGBA::save(char *f_name)
{
    std::vector<uint8_t> buffer;
    LodePNG::Encoder iEnc;

    iEnc.getSettings().zlibsettings.windowSize = 2048;
    iEnc.encode(buffer, _buffer, _width, _height);
    
    LodePNG::saveFile(buffer, f_name);
    
    return 0;
}
