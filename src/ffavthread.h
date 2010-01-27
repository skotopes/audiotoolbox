/*
 *  ffavthread.h
 *  histogramd
 *
 *  Created by Alexandr Kutuzov on 24.01.10.
 *  Copyright 2010 White Label ltd. All rights reserved.
 *
 */

#ifndef FFAVTHREAD_H
#define FFAVTHREAD_H

#include "ffavbase.h"
#include <pthread.h>

class ffavThread {
public:
    ffavThread();
    ffavThread(const ffavThread&);
    virtual ~ffavThread();
    ffavThread& operator=(const ffavThread&);

    bool start();
    bool stop();

protected:
    virtual void run()=0;
    
private:
    static void *aCallback(void *data);
    
    pthread_t pth;
    pthread_attr_t attr;
    volatile bool running;
};

class ffavMutex {
public:
    ffavMutex();
    virtual ~ffavMutex();
    
    bool lockMutex();
    bool unlockMutex();
    
protected:
    pthread_mutex_t mtx;
};

class ffavCondition {
public:
    ffavCondition();
    virtual ~ffavCondition();
    
    bool waitCond();
    bool raiseCond();
    
protected:
    pthread_cond_t cond;
    pthread_mutex_t mtx;
};

#endif
