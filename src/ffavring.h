#ifndef FFAVRING_H
#define FFAVRING_H

#include "ffavbase.h"

/**
  * audio processing ring base class
  * as base we using unsigned short int array
  *
  */

class ffavRing
{
public:
    ffavRing(size_t s);
    ffavRing(const ffavRing&);
    virtual ~ffavRing();
    ffavRing& operator=(const ffavRing&);

    inline void reset()
                { r_ptr = w_ptr = 0;}

protected:
    size_t pull_data(uint8_t *data, size_t len);
    size_t push_data(uint8_t *data, size_t len);
    size_t readSpace();
    size_t writeSpace();

private:
    uint8_t *buff;
    volatile size_t w_ptr;
    volatile size_t r_ptr;
    size_t size, size_mask;
};

/**
  * audio processing ring 16 signed int
  *
  */

class ffavRingS16: private ffavRing
{
public:
    ffavRingS16(size_t s): ffavRing(s*2) {};
    virtual ~ffavRingS16() {};

    inline size_t pull(int16_t *data, size_t len)
                    { return pull_data((uint8_t*)data, len*2); };
    inline size_t push(int16_t *data, size_t len)
                    { return push_data((uint8_t*)data, len*2); };
};

class ffavFileRingS16: private ffavRing, public ffavPushPull
{
public:
    ffavFileRingS16(size_t s): ffavRing(s*2) {};
    virtual ~ffavFileRingS16() {};
    inline size_t pull(int16_t *data, size_t len)
                    { return pull_data((uint8_t*)data, len*2); };
    inline size_t push(int16_t *data, size_t len)
                    { return push_data((uint8_t*)data, len*2); };
};

#endif // APRING_H
