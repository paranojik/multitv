/* CxCapture.cpp
** -------------
** Application Sharing Capture Module
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Tue Mar 11 20:36:01 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_shared/shared/irm/mediapkt.h"
#include "iptv_shared/shared/socket/isockbuff.h"
#include "iptv_shared/shared/base64.h"
#include <windows.h>
//#include <winable.h>
#include <stdio.h>
#include <math.h>
#include "CxZLib.h"
//#include "CxEventWin.h"
#include "CxCapture.h"
//#include "CxCaptureThread.h"
#include "CxSendThread.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CxCapture::CxCapture( void ) : CxAppSharing( AS_CAPTURING )
{
    m_hWnd            = NULL;
    m_pSendThread     = NULL;
    m_flInitCapture   = false;
    m_hMemBitmap      = NULL;
    m_hOldMemBitmap   = NULL;
    m_prdBitmapHeader = NULL;
    m_prdBits         = NULL;
    m_pChunkBits      = NULL;
    m_pDebug          = NULL;
    m_pBits           = NULL;
    m_pLocalBits      = NULL;
    m_pMapMem         = NULL;
    //m_hBMPLocalFB     = NULL;
    m_nKFCycle        = 0;
    m_nKFNeed         = 0;
    m_nKFSending      = -1;
//    m_pKFEvent        = new CxEvent( true );
    m_pKBEvent        = new CxEvent( true );
    m_nmsTimeWindow   = AS_SENDDATA_TIME;
}

CxCapture::~CxCapture( void )
{
    End( );

//    if( m_pKFEvent )
//        delete [] m_pKFEvent;

    if( m_pKBEvent )
        delete m_pKBEvent;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::Initialize( ISockBuff *pSockBuff, long nMediaID,
							CxAppSharingListener *pListener, HWND p_hWnd,
							int p_BitDepth, RECT *p_Bounds,
							int p_RefreshTime, int p_CaptureMode)
{
	PixFmt captureFormat, dummyPixFmt;
	unsigned int screenWidth, screenHeight;
    RECT rc;

	m_nmsTimeWindow = p_RefreshTime;

    if( m_flInitCapture )
        return true;

    // Listener
    m_pListener = pListener;

    // Remove Wallpaper
    RemoveWallpaper( );

    // Remove Dynamic resizing
    RemoveDynamicResizing( );

	// Grab screen capture handler
	VIDEOINFOLIST sourceList = VideoEnumerator::GetSourceList(false, true);
	if (p_CaptureMode > sourceList.size()) return false;
	m_ScreenSource = (ScreenSource*) VideoEnumerator::GetSource(sourceList[p_CaptureMode]);
	if (!m_ScreenSource) return false;
	m_ScreenSource->StartCapture();

	// Adjust capture settings
	m_ScreenSource->GetInputFormat(screenWidth, screenHeight, dummyPixFmt);
	m_SizeCapture.cx = screenWidth;
	m_SizeCapture.cy = screenHeight;
	
	// Crop area
	if( p_Bounds )
    {
        // screen limits
        rc.left   = max( 0, p_Bounds->left );
        rc.right  = min( p_Bounds->right, m_SizeCapture.cx );
        rc.top    = max( 0, p_Bounds->top );
        rc.bottom = min( p_Bounds->bottom, m_SizeCapture.cy );
    }
    else
    {
        // full screen
        rc.top    = rc.left = 0;
        rc.right  = m_SizeCapture.cx;
        rc.bottom = m_SizeCapture.cy;
        m_bmRect  = 0;
        m_nRectYStart = 0;
        m_nRectYEnd   = NCHUNK_Y;
    }

	// Set capture color format and fill m_MemBitmapInfo structure
	memset(&m_MemBitmapInfo, 0, sizeof(CXBITMAPINFO));
	m_MemBitmapInfo.bmih.biSize = sizeof(BITMAPINFOHEADER);
	m_MemBitmapInfo.bmih.biWidth  = m_SizeCapture.cx;
	m_MemBitmapInfo.bmih.biHeight = m_SizeCapture.cy;
	m_MemBitmapInfo.bmih.biPlanes = 1;
	
	switch (p_BitDepth)
	{
		/*
	case 8:
		captureFormat = RGB8;
		m_MemBitmapInfo.bmih.biBitCount    = 8;
		m_MemBitmapInfo.bmih.biCompression = BI_RGB;
		for ( int i = 0; i < 256; i++ )
		{
		bmi.bmiColors[i].rgbBlue     = blue; // you supply the colors
		bmi.bmiColors[i].rgbRed      = red;
		bmi.bmiColors[i].rgbGreen    = green;
		bmi.bmiColors[i].rgbReserved = 0;
		} 
		break;
		*/
	case 16:
		captureFormat = RGB565;
		m_MemBitmapInfo.bmih.biBitCount    = 16;
		m_MemBitmapInfo.bmih.biCompression = BI_BITFIELDS;
		m_MemBitmapInfo.nRedMask   = 63488; // 1111100000000000
		m_MemBitmapInfo.nGreenMask = 2016;  // 0000011111100000
		m_MemBitmapInfo.nBlueMask  = 31;    // 0000000000011111
		break;
	case 24:
		captureFormat = BGR24;
		m_MemBitmapInfo.bmih.biBitCount    = 24;
		m_MemBitmapInfo.bmih.biCompression = BI_RGB;
		break;
	default:
		return false;
	}
	
	m_ScreenSource->SetFormat(0, 0, captureFormat, true);

    // Prepare pBits to Record Data
	if( m_prdBits )
        delete [] m_prdBits;

	m_nScreenSizeBits = (((((m_MemBitmapInfo.bmih.biWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3) * m_MemBitmapInfo.bmih.biHeight );

    m_prdBits = (RecordData *)new BYTE[sizeof( RecordData ) - sizeof( BYTE ) + m_nScreenSizeBits];
    m_prdBits->type    = MTYPE_MEDIA_APPSHARING;
    m_prdBits->flags   = PKT_REALTIME;
    m_prdBits->rd_type = RDT_UPDATE_SCREEN;

    // Chunk width and Line Width (static values)
//    m_nChunkWidth   = m_MemBitmapInfo.bmih.biWidth  / NCHUNK_X;
//    m_nChunkWidth  += m_MemBitmapInfo.bmih.biWidth  % NCHUNK_X == 0 ? 0 : 1;
//    m_nChunkHeight  = m_MemBitmapInfo.bmih.biHeight / NCHUNK_Y;
//    m_nChunkHeight += m_MemBitmapInfo.bmih.biHeight % NCHUNK_Y == 0 ? 0 : 1;
    m_nChunkWidth   = (rc.right  - rc.left) / NCHUNK_X;
    m_nChunkWidth  += (rc.right  - rc.left) % NCHUNK_X == 0 ? 0 : 1;
    m_nChunkHeight  = (rc.bottom - rc.top ) / NCHUNK_Y;
    m_nChunkHeight += (rc.bottom - rc.top ) % NCHUNK_Y == 0 ? 0 : 1;
    m_nChunkResidueX  = NCHUNK_X * m_nChunkWidth  - (rc.right  - rc.left );
    m_nChunkResidueY  = NCHUNK_Y * m_nChunkHeight - (rc.bottom - rc.top  );

    if( p_Bounds )
        GetRectParams( p_Bounds );

    m_nWidthBits      = ((((m_MemBitmapInfo.bmih.biWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    m_nChunkWidthBits = ((((m_nChunkWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);

    // to fit one 'LINE' of chunks
	if( m_pChunkBits )
        delete [] m_pChunkBits;

    m_pChunkBits = new BYTE[m_nChunkWidthBits * NCHUNK_X * m_nChunkHeight];

	// Prepare Bitmap Header to Record Data
    if( m_prdBitmapHeader )
        delete [] m_prdBitmapHeader;

    m_prdBitmapHeader = (RecordData *)new BYTE[sizeof(m_MemBitmapInfo) + sizeof( RecordData ) - sizeof( BYTE )];
    memcpy( m_prdBitmapHeader->rd_pData, &m_MemBitmapInfo, sizeof( m_MemBitmapInfo ) );

	if( p_Bounds )
    {
        ((CXBITMAPINFO *)m_prdBitmapHeader->rd_pData)->bmih.biWidth     = (rc.right  - rc.left);
        ((CXBITMAPINFO *)m_prdBitmapHeader->rd_pData)->bmih.biHeight    = (rc.bottom - rc.top);
        ((CXBITMAPINFO *)m_prdBitmapHeader->rd_pData)->bmih.biSizeImage = 
            (((((rc.right  - rc.left) * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3) * (rc.bottom - rc.top);
    }

    m_prdBitmapHeader->type    = MTYPE_MEDIA_APPSHARING;
    m_prdBitmapHeader->flags   = PKT_REALTIME;
    m_prdBitmapHeader->rd_type = RDT_BITMAPHEADER;
    m_prdBitmapHeader->rd_size = sizeof(m_MemBitmapInfo) + sizeof( RecordData ) - sizeof( BYTE );
    m_prdBitmapHeader->datalen = (unsigned long) m_prdBitmapHeader->rd_size - sizeof( LMediaPkt );

    // ZLib
    if( m_pZLib )
        delete m_pZLib;

    m_pZLib = new CxZLib( CXZLIB_ENCODE );
    m_pZLib->Initialize( );

    // create local frame buffer
    CreateLocalFramebuffer( );

    m_flInitCapture = true;
    m_nMediaID      = nMediaID;
    m_nSeq          = 1;
	m_pSockBuff     = pSockBuff;

    memset( m_pChunkList, 0, sizeof( m_pChunkList ) );
	WantKeyFrame( );
	
	// send data thread
	if( m_pSendThread )
    {
        m_pSendThread->End();
        m_pSendThread->WaitForMe();
        delete m_pSendThread;
    }
    m_pSendThread = new CxSendThread( this, m_pChunkList );
    m_pSendThread->CreateThread();
	m_pSendThread->ResumeThread();

	ASEvent( AS_NOTIFY_CAPTURE_START, NULL, 0 );

	return m_flInitCapture;
}

bool CxCapture::End( void )
{
    //OutputDebugString( _T( "CxCapture::End( )" ) );
    if( !m_flInitCapture )
        return true;

	// kill the send thread
    if( m_pSendThread )
    {
        m_pSendThread->End( );
        m_pSendThread->WaitForMe( );
        delete m_pSendThread;
        //OutputDebugString( "delete CxThreadEntry( m_pSendThread ) - EndCapture\n" );
        m_pSendThread = NULL;
    }

    if( m_prdBitmapHeader )
        delete [] m_prdBitmapHeader;
    m_prdBitmapHeader = NULL;

    SelectObject( m_hDCMem, m_hOldMemBitmap );
    if( m_hDCMem )
        DeleteDC( m_hDCMem );
    m_hDCMem = NULL;

    if( m_hMemBitmap )
        DeleteObject( m_hMemBitmap );
    m_hMemBitmap = NULL;

    if( m_prdBits )
        delete [] m_prdBits;
    m_prdBits = NULL;

    if( m_pChunkBits )
        delete [] m_pChunkBits;
    m_pChunkBits = NULL;

    if( m_pZLib )
        delete m_pZLib;
    m_pZLib = NULL;

    DestroyLocalFramebuffer( );

    if( m_pDebug )
    {
        m_pDebug->Destroy( );
        delete m_pDebug;
    }
    m_pDebug = NULL;

    // Restore Wallpaper
    RestoreWallpaper( );

    // Restore DynamicResizing
    RestoreDynamicResizing( );

	// 2DO - Destroy ScreenSource

    m_flInitCapture = false;

    ASEvent( AS_NOTIFY_CAPTURE_STOP, NULL, 0 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::GetRectParams( RECT *pRect )
{
    long nFirstX=-1, iChunk;
    unsigned long nX, i;

    if( !pRect )
        return false;

    m_nScreenChunkHeight  = m_MemBitmapInfo.bmih.biHeight / NCHUNK_Y;
    m_nScreenChunkHeight += m_MemBitmapInfo.bmih.biHeight % NCHUNK_Y == 0 ? 0 : 1;

    m_nScreenChunkWidth   = m_MemBitmapInfo.bmih.biWidth  / NCHUNK_X;
    m_nScreenChunkWidth  += m_MemBitmapInfo.bmih.biWidth  % NCHUNK_X == 0 ? 0 : 1;

    m_nRectYStart = (unsigned short int)( pRect->top    / m_nScreenChunkHeight );
    m_nRectYEnd   = (unsigned short int)( pRect->bottom / m_nScreenChunkHeight ) + 1;
    m_nRectYStart = max( m_nRectYStart, 0 );
    m_nRectYEnd   = min( m_nRectYEnd,   NCHUNK_Y );

    // prepare the mask
    for( i = iChunk  = nX =  0; iChunk < m_SizeCapture.cx; i++ )
    {
        // calculate the first chunk
        if( nFirstX < 0 )
        {
            if( pRect->left >= (LONG)iChunk && pRect->left < (LONG)(iChunk + m_nScreenChunkWidth ) )
            {
                nFirstX = i;
                nX = 1;
                m_bmRect = 1;
            }

        }
        // if we have more than 1 chunk
        else
        {
            if( pRect->right >= (LONG)iChunk )
            {
                nX++;
                m_bmRect = m_bmRect << 1 | 0x1;
            }
            else
                break;
        }
        iChunk += m_nScreenChunkWidth;
    }

    // prepare the bit mask;
    m_bmRect = m_bmRect << (NCHUNK_X - (nX + nFirstX));

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::RestoreWallpaper( void )
{
    return SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, NULL, SPIF_SENDCHANGE ) != 0;
}

bool CxCapture::RemoveWallpaper( void )
{
    return SystemParametersInfo( SPI_SETDESKWALLPAPER, 0, "", SPIF_SENDCHANGE ) != 0;
}

bool CxCapture::RestoreDynamicResizing( void )
{
    return SystemParametersInfo( SPI_SETDRAGFULLWINDOWS, m_bDynamicResizingState, NULL, SPIF_SENDCHANGE ) != 0;
}

bool CxCapture::RemoveDynamicResizing( void )
{
    SystemParametersInfo( SPI_GETDRAGFULLWINDOWS, 0, (bool *)&m_bDynamicResizingState, SPIF_SENDCHANGE );
    return SystemParametersInfo( SPI_SETDRAGFULLWINDOWS, false, NULL, SPIF_SENDCHANGE ) != 0;
}
///////////////////////////////////////////////////////////////////////////////
// CaptureInterface
bool CxCapture::UpdateData( unsigned short int *p )
{
    int j;
    unsigned short int bitmask;

    if( m_bmRect )
        return UpdateDataRect( p );

    for( j = 0; j < NCHUNK_Y; j++ )
    {
        bitmask = *(p + j);
        if( bitmask )
        {
            m_pChunkList[j] |= bitmask;
        }
    }

    return true;
}

bool CxCapture::UpdateDataRect( unsigned short int *p )
{
    int i, j, r, q, s, residue, shift, jStart, jEnd;
    unsigned short int bitmask, newbitmask;

    for( j = m_nRectYStart; j < m_nRectYEnd; j++ )
    {
        bitmask = *(p + j) & m_bmRect;

        if( bitmask )
        {
            newbitmask = 0;
            // new bitmask
            for( i = 0; i <= NCHUNK_X ; i++ )
            {
                if( (bitmask >> (NCHUNK_X-1-i)) & 0x0001 )
                {
                    r = 1; // value
                    s = 1; // counter
                    q = m_nScreenChunkWidth * i / m_nChunkWidth; // initial position

                    if( m_nScreenChunkWidth * i % m_nChunkWidth >= m_nChunkWidth/2 )
                        residue = m_nScreenChunkWidth;
                    else
                        residue = 0;

                    while( s * m_nChunkWidth + i * m_nScreenChunkWidth < (i+1) * m_nScreenChunkWidth + residue )
                    {
                        s++;
                        r = r << 1 | 0x1;
                    }
                    shift = max((NCHUNK_X - q - s), 0 );
                    newbitmask |= r << shift;
                }
            }

            jStart = (m_nScreenChunkHeight  ) * (j -   m_nRectYStart) / m_nChunkHeight;
            jEnd   = (m_nScreenChunkHeight + m_nChunkResidueY ) * (j+1 - m_nRectYStart) / m_nChunkHeight;

            jStart = max( jStart, 0 );
            jEnd   = min( jEnd, NCHUNK_Y-1 );

            while( jStart <= jEnd )
            {
                m_pChunkList[jStart] |= newbitmask;
                jStart++;
            }
        }
    }

    return true;
}
///////////////////////////////////////////////////////////////////////////////
// SendInterface
bool CxCapture::SendBitmapHeader( void )
{
    unsigned long nBytes;

    m_prdBitmapHeader->id   = m_nMediaID;
    m_prdBitmapHeader->seq  = m_nSeq++;

    m_pSockBuff->Write( m_prdBitmapHeader, m_prdBitmapHeader->rd_size, &nBytes );

#ifdef _DEBUG
    CxDump( m_prdBitmapHeader, nBytes );
#endif

    return( nBytes == m_prdBitmapHeader->rd_size );
}

bool CxCapture::ResetCodec( void )
{
    if( !m_flInitCapture )
        return false;

    if( m_pZLib )
        m_pZLib->Reset( );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::SendData( long nLine, long nChunk, long nCount )
{
    unsigned long nBytes=0, nBlockWidth, nTotalOut=0;
    long x, xWidth, nCopyLimit;
    BYTE *p, *pLocalFB;

    // check the chunks out to verify if it changes
    if( CompareArea( nLine, nChunk, nCount ) )
        return false;

    m_prdBits->id            = m_nMediaID;
    m_prdBits->seq           = m_nSeq++;
    m_prdBits->rd_nLine      = nLine;
    m_prdBits->rd_nChunk     = nChunk;
    m_prdBits->rd_ChunkCount = nCount;

    nBlockWidth = m_nChunkWidthBits * nCount;
    x = nChunk * m_nChunkWidth;
    xWidth = ((((x * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    p = (BYTE *)m_pBits + (nLine+1) * m_nChunkHeight * m_nWidthBits;
    p -= m_nWidthBits;
    p -= m_nWidthBits * m_nChunkResidueY;

    pLocalFB  = (BYTE *)m_pLocalBits + (NCHUNK_Y-(nLine+1)) * m_nChunkHeight * m_nChunkWidthBits * NCHUNK_X;
//    pLocalFB += m_nChunkWidthBits * NCHUNK_X;
    pLocalFB += m_nChunkWidthBits * NCHUNK_X * m_nChunkResidueY;

    if( nLine == 0 )
        nCopyLimit = m_nChunkHeight - m_nChunkResidueY;
    else
        nCopyLimit = m_nChunkHeight;

    for( long kk = 0; kk < nCopyLimit; kk++ )
    {
        // the chunks that will be packed and sent
        memcpy( (BYTE *)m_pChunkBits + kk * nBlockWidth,
                (BYTE *)p - kk * m_nWidthBits + xWidth,
                nBlockWidth );

        // copy to local frame buffer to keep it up to date
        memcpy( (BYTE *)pLocalFB + kk * m_nChunkWidthBits * NCHUNK_X + xWidth,
                (BYTE *)p - kk * m_nWidthBits + xWidth,
                nBlockWidth );
    }
    m_pZLib->Encode( (BYTE *)m_pChunkBits, nBlockWidth * m_nChunkHeight, (BYTE *)&(m_prdBits->rd_pData), m_nScreenSizeBits, &nTotalOut );

    m_prdBits->rd_size = sizeof( RecordData ) - sizeof( BYTE ) + nTotalOut;
    m_prdBits->datalen = (unsigned long) m_prdBits->rd_size - sizeof( LMediaPkt );
    m_prdBits->rd_CheckSum = adler32( 0, m_prdBits->rd_pData, nTotalOut );
    // DEBUG
//    char pszFilename[256];
//    RECTL rc;
//    sprintf( pszFilename, "c:\\img%04d.bmp", m_nSeq );
//    rc.left   = 0;
//    rc.top    = 0;
//    rc.right  = nCount * m_nChunkWidth;
//    rc.bottom = m_nChunkHeight;
//    SaveBitmapFile( pszFilename, m_pChunkBits, &rc );
//    rc.right  = m_SizeCapture.cx;
//    rc.bottom = m_SizeCapture.cy;
//    SaveBitmapFile( pszFilename, (BYTE *)m_pMapMem, &rc );
    // DEBUG
    m_pSockBuff->Write( m_prdBits, m_prdBits->rd_size, &nBytes );

#ifdef _DEBUG
    CxDump( m_prdBits, nBytes );
#endif
    return ( nBytes != 0 );
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::WantKeyFrame( void )
{
//    m_pKFEvent->Wait( );
    m_nKFNeed = AS_KEYFRAME_FRAGMENTS + 1;
//    OutputDebugString( "WantKeyFrame(..)\n" );
//    m_pKFEvent->SetEvent( );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::SendKeyFrame( void )
{
    long i, iend;
//    char szLog[1024];


//    sprintf( szLog, "SendKeyFrame( ) - m_nKFNeed: %d - m_nKFCycle: %d\n", m_nKFNeed, m_nKFCycle );
//    OutputDebugString( szLog );

    if( m_nKFNeed <= 0 )
        return false;

    i    = m_nKFCycle * AS_KEYFRAME_FRAGMENTS;
    iend = i + AS_KEYFRAME_FRAGMENTS;

    m_nKFSending = m_nKFCycle;

    for(  ;i < iend; i++ )
    {
        m_pChunkList[i] |= 0xFFFF;
    }
    m_nKFCycle = (m_nKFCycle+1)%AS_KEYFRAME_FRAGMENTS;

//    m_pKFEvent->Wait( );
    m_nKFNeed--;
//    sprintf( szLog, "SendKeyFrame( ) - new m_nKFNeed: %d - m_nKFCycle: %d\n", m_nKFNeed, m_nKFCycle );
//    OutputDebugString( szLog );
//    m_pKFEvent->SetEvent( );

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CxCapture::ProcessKeyEvent( long wVk, long down )
{
    INPUT indata;//[INPUT_SIZE];

    indata.type = INPUT_KEYBOARD;

    indata.ki.time        = 0;
    indata.ki.dwExtraInfo = 0;
    indata.ki.wScan       = 0;
    indata.ki.wVk         = (WORD) wVk;
    indata.ki.dwFlags     = ( down ? 0 : KEYEVENTF_KEYUP );

    SendInput( 1, &indata, sizeof( indata ) );

    return true;
}
bool CxCapture::ProcessMouseEvent( int message, WPARAM wParam, LPARAM lParam )
{
    INPUT indata;//[INPUT_SIZE];

    indata.type = INPUT_MOUSE;

    indata.mi.time        = 0;
    indata.mi.dwExtraInfo = 0;
    indata.mi.mouseData   = 0;
    indata.mi.dwFlags     = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    indata.mi.dx          = LOWORD( lParam ) * 65535 / m_SizeCapture.cx;
    indata.mi.dy          = HIWORD( lParam ) * 65535 / m_SizeCapture.cy;

    if( message != WM_MOUSEMOVE )
    {
        if( message == WM_LBUTTONDOWN )
            indata.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
        else if( message == WM_LBUTTONUP )
            indata.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
        else if( message == WM_RBUTTONDOWN )
            indata.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        else if( message == WM_RBUTTONUP )
            indata.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
    }

    if( LOWORD( lParam ) <= ((CXBITMAPINFO *)m_prdBitmapHeader->rd_pData)->bmih.biWidth &&
        HIWORD( lParam ) <= ((CXBITMAPINFO *)m_prdBitmapHeader->rd_pData)->bmih.biHeight )
        SendInput( 1, &indata, sizeof( indata ) );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::ProcessKMEvent( char *psKM, unsigned long nSize )
{
    static const int nBufSize = 1024;
    static char szBuf[nBufSize];
    int nDecode;
    // keyboard variables
    bool down;
    long nKey;
    // mouse variables
    int message;
    WPARAM wParam;
    LPARAM lParam;


    m_pKBEvent->Wait( );

    // keyboard message
    if( *psKM == 'K' )
    {
        // key down/up
        down = psKM[1] == '0' ? true : false;

        //decode64
        nDecode = nBufSize-1;
        decode64( &psKM[2], nSize-2, szBuf, &nDecode );
        nKey = (long)szBuf[0];

        ProcessKeyEvent( nKey, down );
    }
    // Mouse message
    else if( *psKM == 'M' )
    {
        //decode64
        nDecode = nBufSize-1;
        decode64( &psKM[1], nSize-1, szBuf, &nDecode );

        memcpy( &message, &szBuf[0],  sizeof( message ) );
        memcpy( &wParam,  &szBuf[8],  sizeof( wParam  ) );
        memcpy( &lParam,  &szBuf[16], sizeof( lParam  ) );

        ProcessMouseEvent( message, wParam, lParam );
    }
    else
    {
        m_pKBEvent->SetEvent( );
        return false;
    }

    m_pKBEvent->SetEvent( );
    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::ResetKM( void )
{
    // reset keyboard state to avoid problems when stop capturing and a key's hold
    ProcessKeyEvent( VK_SHIFT,    false );
    ProcessKeyEvent( VK_LSHIFT,   false );
    ProcessKeyEvent( VK_RSHIFT,   false );
    ProcessKeyEvent( VK_CONTROL,  false );
    ProcessKeyEvent( VK_LCONTROL, false );
    ProcessKeyEvent( VK_RCONTROL, false );
    ProcessKeyEvent( VK_MENU,     false );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxCapture::CreateLocalFramebuffer( void )
{
    // assert if the capture hdc doesn't exist
	// 2DO

    // create a local frame buffer with the MemBitmapInfo attributes (the same attributes of m_hDCCapture)
    // this local frame buffer will be use to compare with the driver frame buffer and check up whether
    // it had changed or not
    // if( ( m_hBMPLocalFB = CreateDIBSection( m_hDCCapture, (BITMAPINFO *)&(m_MemBitmapInfo.bmih), DIB_RGB_COLORS, &m_pLocalBits, NULL, 0 ) ) == NULL )
    if( ( m_pLocalBits = new BYTE[m_nChunkWidthBits * NCHUNK_X * m_nChunkHeight * NCHUNK_Y] ) == NULL )
        return false;

    return true;
}

bool CxCapture::DestroyLocalFramebuffer( void )
{
    if( m_pLocalBits )
        delete [] m_pLocalBits;
    m_pLocalBits = NULL;

    return true;
}

bool CxCapture::CompareArea( long nLine, long nChunk, long nCount )
{
    unsigned long nBlockWidth;
    long x, xWidth, nCopyLimit;
    BYTE *p, *pLocalFB;
    long i, iend;

    nBlockWidth = m_nChunkWidthBits * nCount;
    x = nChunk * m_nChunkWidth;
    xWidth = ((((x * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3);
    p = (BYTE *)m_pBits + ((nLine+1) * m_nChunkHeight * m_nWidthBits);
    p -= m_nWidthBits;
    p -= m_nWidthBits * m_nChunkResidueY;

    pLocalFB  = (BYTE *)m_pLocalBits + (NCHUNK_Y-(nLine+1)) * m_nChunkHeight * m_nChunkWidthBits * NCHUNK_X;
    pLocalFB += m_nChunkWidthBits * NCHUNK_X * m_nChunkResidueY;

    if( nLine == 0 )
        nCopyLimit = m_nChunkHeight - m_nChunkResidueY;
    else
        nCopyLimit = m_nChunkHeight;

    for( long kk = 0; kk < nCopyLimit; kk++ )
    {
        if( memcmp( (BYTE *)p - kk * m_nWidthBits + xWidth,
            (BYTE *)pLocalFB + kk * m_nChunkWidthBits * NCHUNK_X + xWidth,
            nBlockWidth ) )
            return false;
    }

    // check out if it's a keyframe package
    if( m_nKFNeed >= 0 && m_nKFSending >= 0 )
    {
        i    = m_nKFSending * AS_KEYFRAME_FRAGMENTS;
        iend = i + AS_KEYFRAME_FRAGMENTS - 1;

        if( i <= nLine  && nLine <= iend )
            return false;
    }

    return true;
}
///////////////////////////////////////////////////////////////////////////////
unsigned long CxCapture::GetTimeWindow( void )
{
    // reset key frame flag
    m_nKFSending = -1;

    return m_nmsTimeWindow;
}
///////////////////////////////////////////////////////////////////////////////
