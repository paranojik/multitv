#ifndef __LOCKED_INC__
#define __LOCKED_INC__


#include "../shared/semaphore.h"


class LockedInc : private _SEMAPHORE
{
public:

    LockedInc(void);

    LONG inc(LPLONG lpVariable);
    LONG dec(LPLONG lpVariable);

};




#endif

