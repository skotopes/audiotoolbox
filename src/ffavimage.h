/*
 *  image.h
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 26.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#ifndef FFAVIMAGE_H
#define FFAVIMAGE_H

#include <stdint.h>

class pixelRGBA {
public:
    pixelRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
        red(r), green(g), blue(b), alfa(a) {};

    inline uint8_t getR() { return red;};
    inline uint8_t getG() { return green;};
    inline uint8_t getB() { return blue;};
    inline uint8_t getA() { return alfa;};
    
private:
    uint8_t red, green, blue, alfa;
};

class imageRGBA {
public:
    imageRGBA(int width, int height);
    ~imageRGBA();
    
    void drawPoint(int x, int y, pixelRGBA color);
    void drawLine(int x1, int y1, int x2, int y2, pixelRGBA color);
    
    int save(char *f_name);
    
private:
    int w, h;
    uint8_t *iBuff;
};

#endif
