///////////////////////////////////////////////////////////////////////////////
// CxThreadGeneric.h
// ---------
// Cross Platform Thread management using sharedlib.
// Keeps basic retrocompatibility with WinThread version
//
// Author : Rômulo Fernandes (romulo.fernandes@gmail.com)
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/ithread.h"
#include "iptv_shared/shared/semaphore.h"
#include "CxEventCross.h"
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __CX_THREAD_CROSS__
#define __CX_THREAD_CROSS__

int CxThreadEntry(IThread* pOwner,void* pParams);

class CxThreadCross
{
    private:
        // Thread Proc
        virtual unsigned int fnThreadProc( void );
        // EndThread
        void EndThread( unsigned int nRetVal );

    protected:
		IThread*   m_pThread;
		long       m_nThreadID;
		CxEvent*   m_pEvent;

    public:
        // Constructors
        CxThreadCross();
        virtual ~CxThreadCross();

        // Worker thread methods
		int  ResumeThread();
        int  SuspendThread();
        bool CreateThread();
        void WaitForMe();

		friend int CxThreadEntry(IThread* pOwner,void* pParams);
};
#endif /* __CX_THREAD_CROSS__ */
///////////////////////////////////////////////////////////////////////////////

