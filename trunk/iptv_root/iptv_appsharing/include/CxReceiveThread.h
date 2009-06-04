/* CxReceiveThread.h
** -----------------
** Receive Thread
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Mon Mar 12 16:27:06 BRT 2007
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "CxThreadCross.h"
///////////////////////////////////////////////////////////////////////////////
class ReceiveInterface;
class ISockBuff;
class CxEvent;
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __CX_RECEIVETHREAD__
#define __CX_RECEIVETHREAD__

class CxReceiveThread : public CxThreadCross
{
    private:
        bool              m_flQuiting;

        ReceiveInterface *m_pReceive;
        ISockBuff        *m_pSockBuff;
        BYTE             *m_pBuffer;
        long              m_nBufferSize;

        //CxEvent          *m_pRecvTEvent; // Was only protecting m_flQuiting.
		                                   // Unneeded since bool operations are atomic.

        // Thread Proc
        unsigned int fnThreadProc( void );

    public:
        CxReceiveThread( ISockBuff *pSockBuff, ReceiveInterface *pReceive );
        virtual ~CxReceiveThread( void );

        HANDLE GetEvent( void );

        void End( void );
};
///////////////////////////////////////////////////////////////////////////////
#endif  /* __CX_RECEIVETHREAD__ */

