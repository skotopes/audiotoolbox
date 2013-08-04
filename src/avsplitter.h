#ifndef AVSPLITTER_H
#define AVSPLITTER_H

#include "avobject.h"

class AVSplitter: public AVObject
{
public:
    AVSplitter(int size);
    virtual ~AVSplitter();

    virtual const char * getName() { return "AVSplitter"; }

    virtual size_t pull(float *buffer_ptr, size_t buffer_size) { return 0; };
    virtual size_t push(float *buffer_ptr, size_t buffer_size);

    void addObject(AVObject *object);

private:
    size_t _size, _object_count;
    AVObject **_pool;
};

#endif // AVSPLITTER_H
