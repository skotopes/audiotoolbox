#include "avsplitter.h"
#include "avexception.h"

AVSplitter::AVSplitter(int size) :
    _size(size), _object_count(0), _pool()
{
    _pool = new AVObject *[_size];
}


AVSplitter::~AVSplitter() {
    if (_pool) {
        delete [] _pool;
    }
}

size_t AVSplitter::push(float *buffer_ptr, size_t buffer_size) {
    for (int i=0; i<_object_count; i++) {
        _pool[i]->push(buffer_ptr, buffer_size);
    }

    return buffer_size;
}

void AVSplitter::addObject(AVObject *object) {
    if (_object_count == _size) {
        throw AVException("Object pool if full.");
    }
    _pool[_object_count++] = object;
}
