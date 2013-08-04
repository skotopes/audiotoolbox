/*
 *  image.h
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 26.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#ifndef AVIMAGE_H
#define AVIMAGE_H

#include "avconf.h"

class AVPixelRGBA {
public:
    AVPixelRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
        red(r), green(g), blue(b), alfa(a) {}

    inline uint8_t getR() { return red;}
    inline uint8_t getG() { return green;}
    inline uint8_t getB() { return blue;}
    inline uint8_t getA() { return alfa;}
    
private:
    uint8_t red, green, blue, alfa;
};

class AVImageRGBA {
public:
    AVImageRGBA(int width, int height);
    ~AVImageRGBA();

    AVImageRGBA(const AVImageRGBA&) = delete;
    AVImageRGBA& operator=(const AVImageRGBA&) = delete;
    
    void drawPoint(int x, int y, AVPixelRGBA color);
    void drawLine(int x1, int y1, int x2, int y2, AVPixelRGBA color);
    
    int save(char *f_name);

    int getWidth() { return _width; }
    int getHeight() { return _height; }

private:
    int _width, _height;
    uint8_t *_buffer;
};

#endif
