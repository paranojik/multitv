/* CxReceiveThread.cpp
** -------------------
** Capture Thread
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Tue Jul 11 12:01:49 BRT 2006
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_shared/shared/socket/isockbuff.h"
#include "iptv_shared/shared/irm/mediapkt.h"
#include "CxReceiveThread.h"
#include "CxReceiveBase.h"
///////////////////////////////////////////////////////////////////////////////
// Buffer para primeira mensagem que define o tamanho real do buffer
#define DEFAULT_BUFFER_SIZE  (sizeof(CXBITMAPINFO) + sizeof( RecordData ) - sizeof( BYTE ) )
///////////////////////////////////////////////////////////////////////////////
CxReceiveThread::CxReceiveThread( ISockBuff *pSockBuff, ReceiveInterface *pReceive )
{
    m_flQuiting   = false;
    m_pReceive    = pReceive;
    m_pSockBuff   = pSockBuff;
    m_nBufferSize = DEFAULT_BUFFER_SIZE;
    m_pBuffer     = new BYTE[m_nBufferSize];
//    m_pRecvTEvent = new CxEvent( true );
}

CxReceiveThread::~CxReceiveThread( void )
{
    //OutputDebugString( "delete CxReceiveThread(..)" );
    End( );

    if( m_pBuffer )
        delete [] m_pBuffer;
    m_pBuffer   = NULL;

//    if( m_pRecvTEvent )
//        delete m_pRecvTEvent;
//    m_pRecvTEvent = NULL;

    m_pSockBuff = NULL;
    m_pReceive  = NULL;
}

void CxReceiveThread::End( void )
{
//    m_pRecvTEvent->Wait( );
//    CxDump( "CxReceiveThread::End" );
    m_flQuiting = true;
//    m_pRecvTEvent->SetEvent( );
}
///////////////////////////////////////////////////////////////////////////////
unsigned int CxReceiveThread::fnThreadProc( void )
{
    unsigned long nRead;
    CXBITMAPINFO *pbih;
    RecordData   *pData;
    // sequencial number
    unsigned long nLastSeq = 0;
//    char szLog[1024];

    if( !m_pEvent )
        m_flQuiting = true;

    while( !m_flQuiting )
    {
//        sprintf( szLog, "Read - m_flQuiting: %d", m_flQuiting );
//        CxDump( szLog );

        if( m_pSockBuff->Read( (void *) m_pBuffer, m_nBufferSize, &nRead ) == S_OK )
        {
//            m_pRecvTEvent->Wait( );
            if( m_flQuiting )
            {
//                m_pRecvTEvent->SetEvent( );
                continue;
            }
//            m_pRecvTEvent->SetEvent( );

//            sprintf( szLog, "Read passed - m_flQuiting: %d", m_flQuiting );
//            CxDump( szLog );
            pData = (RecordData *) m_pBuffer;

            switch( pData->rd_type )
            {
                case RDT_UPDATE_FRAMESIZE:
                    m_pReceive->DeleteFrameBuffer( );

                case RDT_BITMAPHEADER:
                    pbih = (CXBITMAPINFO *)pData->rd_pData;
                    m_nBufferSize = ((((pbih->bmih.biWidth * pbih->bmih.biBitCount) + 31) & ~31) >> 3) * pbih->bmih.biHeight + sizeof( RecordData );

                    if( m_pReceive->CreateFrameBuffer( (CXBITMAPINFO *)pData->rd_pData ) )
                    {
                        nLastSeq = pData->seq;
                        // new buffer size
                        if( m_pBuffer )
                            delete [] m_pBuffer;

                        m_pBuffer  = new BYTE[m_nBufferSize];
                        pData      = (RecordData *) m_pBuffer;
                        pData->seq = nLastSeq;
                    }
                    else
                    {
                        m_pReceive->ResetCodec( );
                    }
                    break;

                case RDT_UPDATE_SCREEN:
                    // debug
                    if( pData->seq != nLastSeq+1 )
                    {
                        // sprintf( szLog, "expected: %d\nreceived: %d", nLastSeq+1, pData->seq  );
                        // MessageBox( GetDesktopWindow( ), szLog, "[Error] Sequential", MB_OK | MB_ICONERROR );
						// MessageBeep( MB_OK ); // Windows Only !!!!!!!!!!
                    }
                    m_pReceive->UpdateScreen( pData );
                    break;

                //default:
                    //MessageBox( NULL, "default", "DEBUG", MB_OK );
            }
            nLastSeq = pData->seq;
        }
//        else
//        {
//            sprintf( szLog, "Read passed (else) - m_flQuiting: %d", m_flQuiting );
//            CxDump( szLog );
//        }
    }
//    sprintf( szLog, "leaving: CxReceiveThread::fnThreadProc(..) - m_flQuiting: %d", m_flQuiting );
//    CxDump( szLog );

    //OutputDebugString( "leaving: CxReceiveThread::fnThreadProc(..)" );
    return 0;
}
///////////////////////////////////////////////////////////////////////////////

