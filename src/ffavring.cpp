#include "ffavring.h"

ffavRing::ffavRing(size_t s):
        buff(NULL),w_ptr(0), r_ptr(0), size(0), size_mask(0)
{
    size_t power_of_two;

    for (power_of_two = 1; 1 << power_of_two < s; power_of_two++);

    size_mask = size = 1 << power_of_two;
    size_mask -= 1;
    
    buff = reinterpret_cast<uint8_t*>(av_malloc(size));

    if (!buff)
        throw ffavError("ffavRing: cannot allocate memory");
}

ffavRing::ffavRing(const ffavRing&):
        buff(NULL),w_ptr(0), r_ptr(0), size(0), size_mask(0)
{
    throw ffavError("ffavRing: object copy is not allowed");
}

ffavRing::~ffavRing()
{
    av_free(buff);
}

ffavRing& ffavRing::operator=(const ffavRing&)
{
    throw ffavError("ffavRing: object copy is not allowed");
}

size_t ffavRing::pull_data(uint8_t *data, size_t len)
{
    size_t read_space = readSpace();
    size_t to_read;
    size_t n0, n1, n2;

    if (read_space == 0) {
        return 0;
    }

    to_read = len > read_space ? read_space : len;

    n0 = r_ptr + to_read;

    if (n0 > size) {
        n1 = size - r_ptr;
        n2 = n0 & size_mask;
    } else {
        n1 = to_read;
        n2 = 0;
    }

    memcpy (data, buff + r_ptr, n1);
    r_ptr = (r_ptr + n1) & size_mask;

    if (n2) {
        memcpy (data + n1, buff + r_ptr, n2);
        r_ptr = (r_ptr + n2) & size_mask;
    }

    return to_read;
}

size_t ffavRing::push_data(uint8_t *data, size_t len)
{
    size_t write_space = writeSpace();
    size_t to_write;
    size_t n0, n1, n2;

    if (write_space == 0) {
        return 0;
    }

    to_write = len > write_space ? write_space : len;

    n0 = w_ptr + to_write;

    if (n0 > size) {
        n1 = size - w_ptr;
        n2 = n0 & size_mask;
    } else {
        n1 = to_write;
        n2 = 0;
    }

    memcpy (buff + w_ptr, data, n1);
    w_ptr = (w_ptr + n1) & size_mask;

    if (n2) {
        memcpy (buff + w_ptr, data + n1, n2);
        w_ptr = (w_ptr + n2) & size_mask;
    }

    return to_write;
}

size_t ffavRing::readSpace()
{
    size_t w, r;

    w = w_ptr;
    r = r_ptr;

    if (w > r) {
        return w - r;
    } else {
        return (w - r + size) & size_mask;
    }
}

size_t ffavRing::writeSpace()
{
    size_t w, r;

    w = w_ptr;
    r = r_ptr;

    if (w > r) {
        return ((r - w + size) & size_mask) - 1;
    } else if (w < r) {
        return (r - w) - 1;
    } else {
        return size - 1;
    }
}
