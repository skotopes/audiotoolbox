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

ffavMutex::ffavMutex():
    mtx()
{
    pthread_mutex_init(&mtx, NULL);
}

ffavMutex::~ffavMutex()
{
    pthread_mutex_destroy(&mtx);
}

bool ffavMutex::lockMutex()
{
    if (pthread_mutex_lock(&mtx) != 0)
        return false;

    return true;
}

bool ffavMutex::unlockMutex()
{
    if (pthread_mutex_unlock(&mtx) != 0)
        return false;
    
    return true;
}

ffavCondition::ffavCondition():
    cond(), mtx()
{
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);
}

ffavCondition::~ffavCondition()
{
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mtx);
}

bool ffavCondition::waitCond()
{
    if (pthread_mutex_lock(&mtx) == 0)
    {
        pthread_cond_wait(&cond, &mtx);
        pthread_mutex_unlock(&mtx);
    }
    return true;
}

bool ffavCondition::raiseCond()
{
    if (pthread_mutex_lock(&mtx) == 0)
    {
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);
    }
    return true;
}
