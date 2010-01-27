/*
 *  ffavthread.cpp
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 24.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#include "ffavthread.h"
#include <iostream>

ffavThread::ffavThread():
    pth(),attr(),running(false)
{
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

ffavThread::ffavThread(const ffavThread&):
    pth(),attr(),running(false)
{
    throw ffavError("ffavThread: object copy is not allowed");
}

ffavThread::~ffavThread()
{
    if (running)
    {
        std::cerr << "ffavThread: destructor called before joining" << std::endl;
    }
}

ffavThread& ffavThread::operator=(const ffavThread&)
{
    throw ffavError("ffavThread: object copy is not allowed");
}

bool ffavThread::start()
{
    int ec = pthread_create(&pth, NULL, aCallback, this);
    if (ec)
        return false;
    
    return true;
}

bool ffavThread::stop()
{
    if (!running)
        return true;
        
    int ec = pthread_join(pth, NULL);
    if (ec)
        return false;
    
    return true;
}

void *ffavThread::aCallback(void *data)
{
    ffavThread *me = static_cast<ffavThread *>(data);

    me->running = true;
    me->run();
    me->running = false;
    
    pthread_exit(NULL);
}
