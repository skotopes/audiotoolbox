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

AVColorRGBA AVImageRGBA::getPoint(int x, int y) {
    if (x >= _width || y >= _height || x < 0 || y < 0) {
        std::cerr << "Invalid get position x:" << x <<" y:"<< y << std::endl;
        return AVColorRGBA(0, 0, 0, 0);
    }

    AVColorRGBA c;
    c.setR(_buffer[4 * _width * y + 4 * x + 0]);
    c.setG(_buffer[4 * _width * y + 4 * x + 1]);
    c.setB(_buffer[4 * _width * y + 4 * x + 2]);
    c.setA(_buffer[4 * _width * y + 4 * x + 3]);

    return c;
}

void AVImageRGBA::drawPoint(int x, int y, AVColorRGBA color)
{
    if (x >= _width || y >= _height || x < 0 || y < 0) {
        std::cerr << "Invalid set position x:" << x <<" y:"<< y << std::endl;
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
    
    if (xo && yo) {
        drawPoint(x1, y1, color);
    } else if (xo && !yo) {
        s = (y2-y1)/abs(y2-y1);

        for (float i=y1; y!=y2; i+=s) {
            y = (int)i;
            drawPoint(x, y, color);
        }
    } else {
        k = (x1-x2)/(y1-y2);
        a = y1 - k * x1;
        
        if (abs(x2-x1) > (abs(y2-y1))) {
            s = (x2-x1)/abs(x2-x1);
        } else {
            s = (x2-x1)/abs(y2-y1);
        }

        for (float i=x1; x!=x2; i+=s) {
            x = (int)i;
            y = (int)((k*i)+a);
            drawPoint(x, y, color);
        }
    }
}

void AVImageRGBA::tileImage(AVImageRGBA *image) {
    for (size_t x=0; x < _width; x++) {
        for (size_t y=0; y < _height; y++) {
            size_t ix, iy;
            ix = x % image->_width;
            iy = y % image->_height;

            drawPoint(x, y, image->getPoint(ix, iy));
        }
    }
}

void AVImageRGBA::applyMask(AVImageRGBA *image) {
    for (size_t x=0; x < _width; x++) {
        for (size_t y=0; y < _height; y++) {
            size_t ix, iy;
            ix = x % image->_width;
            iy = y % image->_height;

            AVColorRGBA s = getPoint(ix, iy);
            AVColorRGBA m = image->getPoint(ix, iy);
            s.setA(m.getA());

            drawPoint(x, y, s);
        }
    }
}


void AVImageRGBA::save(char *f_name)
{
    unsigned error = lodepng::encode(f_name, _buffer, _width, _height);

    if (error) {
        throw AVException(std::string("Unable to save image, encoder error: ") + lodepng_error_text(error));
    }
}
