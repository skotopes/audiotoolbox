#ifndef AVOBJECT_H
#define AVOBJECT_H

#include "avconf.h"

class AVObject
{
public:
    AVObject();
    virtual ~AVObject();

    AVObject(const AVObject&) = delete;
    AVObject& operator=(const AVObject&) = delete;

    virtual const char * getName() = 0;
    virtual const char * getRepr() { return getName(); }

    virtual size_t pull(float *buffer_ptr, size_t buffer_size) = 0;
    virtual size_t push(float *buffer_ptr, size_t buffer_size) = 0;
};

#endif // AVOBJECT_H
