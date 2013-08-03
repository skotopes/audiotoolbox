/*
 *  image.cpp
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 26.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include <vector>
#include <math.h>

#include "ffavbase.h"
#include "ffavimage.h"
#include "../3rd/lodepng.h"

imageRGBA::imageRGBA(int width, int height):
    w(width), h(height), iBuff(NULL)
{
    iBuff = new uint8_t[w*h*4];
}

imageRGBA::imageRGBA(const imageRGBA&):
    w(0), h(0), iBuff(NULL)
{
    throw ffavError("imageRGBA: object copy is not allowed");
}


imageRGBA::~imageRGBA()
{
    delete [] iBuff;
}

imageRGBA& imageRGBA::operator=(const imageRGBA&)
{
    throw ffavError("imageRGBA: object copy is not allowed");
}

void imageRGBA::drawPoint(int x, int y, pixelRGBA color)
{
    if (x >= w || y >= h || x < 0 || y < 0)
    {
        return;
    }   

    iBuff[4 * w * y + 4 * x + 0] = color.getR();
    iBuff[4 * w * y + 4 * x + 1] = color.getG();
    iBuff[4 * w * y + 4 * x + 2] = color.getB();
    iBuff[4 * w * y + 4 * x + 3] = color.getA();
}

void imageRGBA::drawLine(int x1, int y1, int x2, int y2, pixelRGBA color)
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

int imageRGBA::save(char *f_name)
{
    std::vector<uint8_t> buffer;
    LodePNG::Encoder iEnc;

    iEnc.getSettings().zlibsettings.windowSize = 2048;
    iEnc.encode(buffer, iBuff, w, h);
    
    LodePNG::saveFile(buffer, f_name);
    
    return 0;
}
