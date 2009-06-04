/* CxSendThread.h
** ---------------
** Send Thread Interface
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Thu Feb  7 21:20:06 BRST 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "CxCapture.h"
#include "CxThreadCross.h"
#include <windows.h>
#include <tchar.h>

class CxEvent;
class SendInterface;

#ifndef CX_SENDTHREAD__
#define CX_SENDTHREAD__

class CxSendThread : public CxThreadCross
{
    private:
        bool             m_flQuiting;

        SendInterface   *m_pSend;

        // ChunkList
        unsigned short int *m_pChunkList;

        // Thread Proc
        unsigned int fnThreadProc( void );

    public:
        CxSendThread( SendInterface *pSend, unsigned short int *pChunkList );
        virtual ~CxSendThread( void );

        void End( void );
};
///////////////////////////////////////////////////////////////////////////////
#endif  /* CX_SENDTHREAD__ */