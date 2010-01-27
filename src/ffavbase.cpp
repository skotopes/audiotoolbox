#include "ffavbase.h"

bool ffavBase::ffmpeginit = false;

ffavBase::ffavBase()
{
    if (!ffmpeginit) {
        avcodec_register_all();
        av_register_all();
    }
}

ffavBase::~ffavBase()
{
}

ffavBase ffavBase::operator=(ffavBase)
{
    throw ffavError("ffavBase: object copy is not allowed");
}

ffavPushPull::ffavPushPull()
{
}

ffavPushPull::~ffavPushPull()
{
}
