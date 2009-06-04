#ifndef CCLOCK_SYNC_H
#define CCLOCK_SYNC_H

#include "semaphore.h"

class CClockSync
{
private:

    BOOL       m_bClockStarted;
    _SEMAPHORE m_ClockSemaph;

protected:    

    ULONG m_Ref,
		  m_ulTime;

public:

    CClockSync();
    virtual ~CClockSync();

    ULONG SetClockRef();
    ULONG UpdateTime(ULONG * _pulCurTime);
    BOOL  ClockStarted() {return m_bClockStarted; }
};

#endif
