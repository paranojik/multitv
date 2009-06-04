///////////////////////////////////////////////////////////////////////////////
// CxEvent.h
// ---------
// Cross Platform Event Control using sharedlib.
// Keeps basic retrocompatibility with Windows' event handling version
//
// Author : Rômulo Fernandes
//
// Note: This header must only be included by CxThread* because it's meant
//       to manage threads. So, if you need to manage threads include CxThread*
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/semaphore.h"

#ifndef __CX_EVENT_CROSS__
#define __CX_EVENT_CROSS__

///////////////////////////////////////////////////////////////////////////////
class CxEvent
{
    private:
		_SEMAPHORE m_Mutex;
        
    public:
        CxEvent( bool bInitialState = false );

        // Unlock the mutex
		BOOL SetEvent();

		// Lock the mutex
		BOOL Wait();
};
///////////////////////////////////////////////////////////////////////////////
#endif /* __CX_EVENT_CROSS__ */

