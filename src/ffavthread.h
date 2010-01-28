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
    inline bool isLocked() { return lock_state; };
    
protected:
    pthread_mutex_t mtx;
    volatile bool lock_state;
};

class ffavCondition: public ffavMutex {
public:
    ffavCondition();
    virtual ~ffavCondition();
    
    bool waitCond();
    bool raiseCond();
    inline bool isWaiting() { return wait_state; };
    
protected:
    pthread_cond_t cond;
    volatile bool wait_state;
};

#endif
