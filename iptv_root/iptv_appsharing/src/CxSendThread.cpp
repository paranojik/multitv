/* CxSendThread.cpp
** ----------------
** Capture Thread
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Thu Feb  7 21:20:06 BRST 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "CxSendThread.h"
#include "CxAppSharingWin.h"
#include <stdio.h>
#include <time.h>
///////////////////////////////////////////////////////////////////////////////
CxSendThread::CxSendThread( SendInterface *pSend, unsigned short int *pChunkList )
{
    m_flQuiting  = false;
    m_pSend      = pSend;
    m_pChunkList = pChunkList;
}

CxSendThread::~CxSendThread( void )
{
    //OutputDebugString( "delete CxSendThread(..)\n" );
}

void CxSendThread::End( void )
{
    m_flQuiting = true;
}
///////////////////////////////////////////////////////////////////////////////
unsigned int CxSendThread::fnThreadProc( void )
{
    DWORD dwTickCount, dwTickCountInit;
    long nSleep;
    unsigned long nWindowTime;
    int i, j, nFirst=-1, nCount, nTotalCount;
    unsigned short int bitmask;
    time_t t_now, t_last_header, t_last_keyframe;
//    char szLog[256];

    if( !m_pEvent )
        m_flQuiting = true;

    // initialize variables
    t_last_header   = time( NULL ) - AS_BITMAPHEADER_TIMEOUT;
    t_last_keyframe = time( NULL );
    dwTickCount = GetTickCount( );

    while( !m_flQuiting )
    {
		CVideoFrame frame;
		((CxCapture*)m_pSend)->m_ScreenSource->GrabScreen(frame, ((CxCapture*)m_pSend)->m_pChunkList);
		frame.GetFrameData((BYTE**)&((CxCapture*)m_pSend)->m_pBits, NULL);

        // send bitmapheader and reset codec
        t_now = time( NULL );
        if( t_now >= AS_BITMAPHEADER_TIMEOUT + t_last_header )
        {
            t_last_header = t_now;
            m_pSend->SendBitmapHeader( );
            m_pSend->ResetCodec( );
        }

        // keyframe
        t_now = time( NULL );
        if( t_now >= AS_KEYFRAME_TIMEOUT + t_last_keyframe )
        {
            t_last_keyframe = t_now;
            m_pSend->SendKeyFrame( );
        }

        // chunks
        nTotalCount = 0;
        dwTickCountInit = GetTickCount( );
        for( j = 0; j < NCHUNK_Y; j++ )
        {
            //copy the bitmask
            bitmask = *(m_pChunkList + j);
            if( bitmask )
            {
                // reset for new marks
                // possible place to loss some chunks (may be we need to use a run conditional controler)
                m_pChunkList[j] = 0;
                for( i = 0; i <= NCHUNK_X ; i++ )
                {
                    if( nFirst < 0 )
                    {
                        if( (bitmask >> (NCHUNK_X-1-i)) & 0x0001 )
                        {
                            nFirst = i;
                            nCount = 1;
                        }
                    }
                    else
                    {
                        if( (bitmask >> (NCHUNK_X-1-i)) & 0x0001 )
                        {
                            nCount++;
                        }
                        else
                        {

//                            sprintf( szLog, "SendData(bitmask: %u) - nLine: %d nChunk: %d nCount: %d\n", bitmask, j, nFirst, nCount );
//                            OutputDebugString( szLog );
                            if( m_pSend->SendData( j, nFirst, nCount ) )
                                nTotalCount += nCount;
                            nFirst = -1;
                            nCount = 0;
                        }
                    }
                }
            }
        }
        nWindowTime = m_pSend->GetTimeWindow( );
        dwTickCount = GetTickCount( );

        nSleep = 0;
        if( nTotalCount > 6 )
        {
            nSleep = nWindowTime - (dwTickCount - dwTickCountInit);
        }
        else // Enhance mouse movement (refreshs fast if just a little has changed)
        {
            nSleep = nWindowTime/4 - (dwTickCount - dwTickCountInit);
        }

        if( nSleep > 0 )
            Sleep( nSleep );
    }

    //OutputDebugString( "leaving: CxSendThread::fnThreadProc\n" );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
