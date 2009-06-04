/* CxReceive.cpp
** -------------
** Application Sharing Receive Module
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Wed Mar 12 19:06:39 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_shared/shared/irm/mediapkt.h"
#include "iptv_shared/shared/socket/isockbuff.h"
#include "iptv_shared/shared/base64.h"
#include "ASCompat.h"
#include "CxReceiveBase.h"
#include "CxReceiveThread.h"
#include "CxZLib.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Constructor
CxReceiveBase::CxReceiveBase( void ) : CxAppSharing( AS_RECEIVING )
{
    m_pReceiveThread  = NULL;
    m_pChunkBits      = NULL;
    m_pBits           = NULL;
    m_SizeCapture.cx  = 1;
    m_SizeCapture.cy  = 1;
    m_flKM            = false;
    m_flPen           = false;
    m_flEnd           = false;
}
// Destructor
CxReceiveBase::~CxReceiveBase( void )
{
    End();
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::Initialize( ISockBuff *pSockBuff, long nMediaID, CxAppSharingListener *pListener )
{
    m_pListener = pListener;
    m_pSockBuff = pSockBuff;
    m_nMediaID  = nMediaID;

    m_pZLib = new CxZLib( CXZLIB_DECODE );

	// Create Receive Thread
    if( m_pReceiveThread )
    {
        m_pReceiveThread->End( );
        m_pReceiveThread->WaitForMe( );
        delete m_pReceiveThread;
        //OutputDebugString( "delete CxThreadEntry(..) - m_pReceiveThread\n" );
    }

	m_pReceiveThread = new CxReceiveThread( m_pSockBuff, this );
	m_pReceiveThread->CreateThread();
	m_pReceiveThread->ResumeThread();

    return( m_pReceiveThread != NULL );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::End( void )
{
    if( m_flEnd )
        return false;

	m_flEnd = true;

    // Terminate the receive thread
    if( m_pReceiveThread )
    {
        m_pReceiveThread->End( );
        m_pReceiveThread->WaitForMe( );
        delete m_pReceiveThread;
        m_pReceiveThread = NULL;
    }

	// Unalloc the decompressor
	if( m_pZLib )
        delete m_pZLib;
	m_pZLib = NULL;

	// Unalloc the Chunk Bits
	if( m_pChunkBits )
		delete [] m_pChunkBits;
    m_pChunkBits = NULL;

    // Notify the termination -> Should be done by derived class
	// ASEvent( AS_NOTIFY_RECEIVE_STOP, NULL, 0 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader )
{
    if( m_pBits )
        return false;

    m_pZLib->Initialize( );

    memcpy( &m_MemBitmapInfo, pBitmapInfoHeader, sizeof( *pBitmapInfoHeader ) );

    m_SizeCapture.cx = m_MemBitmapInfo.bmih.biWidth;
    m_SizeCapture.cy = m_MemBitmapInfo.bmih.biHeight;

    // Chunk width and Line Width (static values)
    m_nChunkWidth   = m_MemBitmapInfo.bmih.biWidth  / NCHUNK_X;
    m_nChunkWidth  += m_MemBitmapInfo.bmih.biWidth  % NCHUNK_X == 0 ? 0 : 1;
    m_nChunkHeight  = m_MemBitmapInfo.bmih.biHeight / NCHUNK_Y;
    m_nChunkHeight += m_MemBitmapInfo.bmih.biHeight % NCHUNK_Y == 0 ? 0 : 1;

    m_nWidthBits      = ((((m_MemBitmapInfo.bmih.biWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    m_nChunkWidthBits = ((((m_nChunkWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);

    m_nChunkResidueX  =  NCHUNK_X * m_nChunkWidth - m_MemBitmapInfo.bmih.biWidth;
    m_nChunkResidueX  = ((((m_nChunkResidueX * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    m_nChunkResidueY  = NCHUNK_Y * m_nChunkHeight - m_MemBitmapInfo.bmih.biHeight;

    if( m_pChunkBits )
        delete [] m_pChunkBits;

    // sizeof chunk's line
    m_pChunkBits = new BYTE[m_nChunkWidthBits * NCHUNK_X * m_nChunkHeight];

    return (m_pChunkBits != NULL);
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::DeleteFrameBuffer( void )
{
    if( !m_pBits )
        return true;

    // ZLib Terminator
    m_pZLib->End();

    if( m_pChunkBits )
        delete [] m_pChunkBits;
	m_pChunkBits = NULL;

	m_pBits = NULL;

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::UpdateScreen( RecordData *pData )
{
	if( !m_pBits )
        return false;

    unsigned long nCheckSum = 0, nBlockWidth, nCopyBlockWidth, nTotalOut;
    long x, xWidth, nRefLine, nCopyLimit;
    BYTE *p;

	// Check for packet integrity
    nCheckSum = adler32( nCheckSum , pData->rd_pData, pData->rd_size - sizeof( RecordData ) + sizeof( BYTE ) );
    if( nCheckSum != pData->rd_CheckSum )
    {
		// Convert this to a kernel event ?
        //MessageBox( NULL, "Checksum error", "Error", MB_OK | MB_ICONERROR );
    }
    else
    {
		// Decompress it
        m_pZLib->Decode( pData->rd_pData, pData->rd_size - sizeof( RecordData ) + sizeof( BYTE ),
                         m_pChunkBits, m_nChunkWidthBits * NCHUNK_X * m_nChunkHeight, &nTotalOut );
    }

    nRefLine    = NCHUNK_Y -1 - pData->rd_nLine;
    nBlockWidth = m_nChunkWidthBits * pData->rd_ChunkCount;
    x = pData->rd_nChunk * m_nChunkWidth;
    xWidth = ((((x * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    p = (BYTE *)m_pBits + nRefLine * m_nChunkHeight * m_nWidthBits;

    // Boundary protection (height)
    if( pData->rd_nLine == 0 )
        nCopyLimit = m_MemBitmapInfo.bmih.biHeight - nRefLine * m_nChunkHeight;
    else
        nCopyLimit = m_nChunkHeight;
    
	// Boundary protection (width)
    if( pData->rd_nChunk + pData->rd_ChunkCount >= NCHUNK_X )
        nCopyBlockWidth = nBlockWidth - m_nChunkResidueX;
    else
        nCopyBlockWidth = nBlockWidth;

    // Update framebuffer
	for( long kk = 0; kk < nCopyLimit; kk++ )
        memcpy( (BYTE *)p + ( kk * m_nWidthBits + xWidth ),
                (BYTE *)m_pChunkBits + kk * nBlockWidth,
                 nCopyBlockWidth );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::ResetCodec( void )
{
    if( !m_pZLib )
        return false;

    m_pZLib->Reset( );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::EnableKM( bool flEnable )
{
    return m_flKM = flEnable;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveBase::EnablePen( bool flEnable )
{
    return m_flPen = flEnable;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
