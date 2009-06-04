#ifndef AUTO_LOCK_H
#define AUTO_LOCK_H

#include "shared/semaphore.h"

class AutoLock
{
private:

    _SEMAPHORE & m_Semaph;

public:

    AutoLock(_SEMAPHORE & _semaph) : m_Semaph(_semaph)
    {
        m_Semaph.Wait();
    }
   
    ~AutoLock()
    {
        m_Semaph.Signal();
    }
};

#endif
