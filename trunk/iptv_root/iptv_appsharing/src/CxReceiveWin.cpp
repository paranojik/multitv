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
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_shared/shared/base64.h"
#include <windows.h>
//#include <winable.h>
#include <stdio.h>
#include <math.h>
#include "CxReceiveWin.h"
//#include "CxReceiveThread.h"
//#include "CxZLib.h"
//#include "shared/irm/mediapkt.h"
//#include "shared/socket/isockbuff.h"
#include "Resource.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Constructor
CxReceiveWin::CxReceiveWin( void )
{
    m_hWnd            = NULL;
    m_hMemBitmap      = NULL;
    m_hDCStretchMem   = NULL;
    m_hBMPMemStretch  = NULL;
    m_pDebug          = NULL;
    m_nStyle          = AS_RECV_SCROLLBAR;
    m_nStretchChunkWidth  = 0;
    m_nStretchChunkHeight = 0;
}
// Destructor
CxReceiveWin::~CxReceiveWin( void )
{
	// Base already calls End method
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::Initialize( ISockBuff *pSockBuff, long nMediaID,
							   CxAppSharingListener *pListener, HWND p_hWnd,
							   int p_BitDepth, RECT *p_Bounds,
							   int p_RefreshTime, int p_CaptureMode)
{
    WNDCLASSEX wndcls;
    RECT rc;
    HWND hWnd = p_hWnd;

    if( m_hWnd )
        return true;

    m_hDCMem         = NULL;
    m_hMemBitmap     = NULL;

    if( !GetClassInfoEx( g_hInstance, SZ_AS_CLASSNAME, &wndcls ) )
    {
        wndcls.cbSize        = sizeof( wndcls );
        wndcls.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndcls.lpfnWndProc   = (WNDPROC)g_WindowProc;
        wndcls.cbClsExtra    = 0;
        wndcls.cbWndExtra    = sizeof( CxReceiveWin * );
        wndcls.hInstance     = g_hInstance;
        wndcls.hIcon         = NULL;
        wndcls.hCursor       = NULL;
        wndcls.hbrBackground = CreateSolidBrush( RGB( 0, 0, 0 ) );
        wndcls.lpszMenuName  = NULL;
        wndcls.lpszClassName = SZ_AS_CLASSNAME;
        wndcls.hIconSm       = NULL;

	    if( RegisterClassEx( &wndcls ) == 0 )
        {
			ASEvent( AS_NOTIFY_RECEIVE_ERROR, NULL, 0 );
            return false;
        }
    }

    GetClientRect( hWnd, &rc );

    m_hWnd = CreateWindowEx( 0, SZ_AS_CLASSNAME, SZ_AS_CAPTION, WS_CHILD | WS_TABSTOP, rc.left, rc.top, rc.right, rc.bottom,
			 hWnd, (HMENU)ID_RECEIVE_CHILDWIN, g_hInstance, (LPVOID)this );

    if( m_hWnd )
    {
        m_hCurEnable = LoadCursor( g_hInstance, MAKEINTRESOURCE( IDC_CURSOR1 ) );
        m_hCursor    = LoadCursor( NULL, IDC_ARROW );

		// Initialize Base Class
        if (!CxReceiveBase::Initialize( pSockBuff, nMediaID, pListener ))
			return false;
    }

//    if( !m_pDebug )
//        m_pDebug = new DebugWin( g_hInstance );
//    m_pDebug->Create( );

    ShowWindow( m_hWnd, SW_SHOW );

	ASEvent( m_hWnd ? AS_NOTIFY_RECEIVE_START : AS_NOTIFY_RECEIVE_ERROR, NULL, 0 );

    return( m_hWnd != NULL );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::End( void )
{
	if( !CxReceiveBase::End() ) // Already manages m_flEnd
		return false;

    if( m_pDebug )
    {
        m_pDebug->Destroy( );
        delete m_pDebug;
    }
    m_pDebug = NULL;

    if( m_hDCStretchMem )
    {
        SelectObject( m_hDCStretchMem, m_hOldStretchBitmap );
        DeleteDC( m_hDCStretchMem );
    }
    m_hDCStretchMem = NULL;

    if( m_hBMPMemStretch )
    {
        DeleteObject( m_hBMPMemStretch );
    }
    m_hBMPMemStretch = NULL;

    if( m_hDCMem )
    {
        SelectObject( m_hDCMem, m_hOldMemBitmap );
        DeleteDC( m_hDCMem );
    }
    m_hDCMem = NULL;

    if( m_hMemBitmap )
    {
        DeleteObject( m_hMemBitmap );
    }
    m_hMemBitmap = NULL;

    if( m_hWnd )
        ::DestroyWindow( m_hWnd );
    m_hWnd = NULL;

	// Notify the termination
	ASEvent( AS_NOTIFY_RECEIVE_STOP, NULL, 0 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
void CxReceiveWin::HandleScroll( unsigned int nMsg, WPARAM wParam )
{
    SCROLLINFO si;

    if( nMsg == WM_SIZE || !nMsg )
    {
        LONG_PTR nStyles = GetWindowLongPtr( m_hWnd, GWL_STYLE );

        if( m_nStyle & AS_RECV_SCROLLBAR )
        {
            nStyles = (m_SizeCapture.cx > m_MySize.cx)?(nStyles | WS_HSCROLL):(nStyles & ~WS_HSCROLL);  // add/remove Horizontal Scroll
            nStyles = (m_SizeCapture.cy > m_MySize.cy)?(nStyles | WS_VSCROLL):(nStyles & ~WS_VSCROLL);  // add/remove Vertical Scroll
        }
        else
        {
            nStyles &= ~(WS_HSCROLL|WS_VSCROLL);
        }

        m_nCurrentScrollX = min( m_SizeCapture.cx - m_MySize.cx, m_nCurrentScrollX );
        m_nCurrentScrollX = max( 0, m_nCurrentScrollX );       // m_nCurrentScrollX can't be negative;

        m_nCurrentScrollY = min( m_SizeCapture.cy - m_MySize.cy, m_nCurrentScrollY );
        m_nCurrentScrollY = max( 0, m_nCurrentScrollY );       // m_nCurrentScrollY can't be negative;

        SetWindowLongPtr( m_hWnd, GWL_STYLE, nStyles );
        ::SetWindowPos( m_hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );

        if( nStyles & WS_VSCROLL )
        {
            // Set the vertical scrolling range and page size
            si.cbSize = sizeof( si );
            si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
            si.nMin   = 0;
            si.nMax   = m_SizeCapture.cy;
            si.nPage  = m_MySize.cy;
            si.nPos   = m_nCurrentScrollY;
            ::SetScrollInfo( m_hWnd, SB_VERT, &si, TRUE );
        }

        if( nStyles & WS_HSCROLL )
        {
            // Set the horizontal scrolling range and page size.
            si.cbSize = sizeof( si );
            si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
            si.nMin   = 0;
            si.nMax   = m_SizeCapture.cx;
            si.nPage  = m_MySize.cx;
            si.nPos   = m_nCurrentScrollX;
            ::SetScrollInfo( m_hWnd, SB_HORZ, &si, TRUE );
        }
        UpdateWindow( m_hWnd );
    }

    // Handle Horizontal Scrolling
    if( nMsg == WM_HSCROLL )
    {
        long xDelta;     // xDelta = new_pos - current_pos  
        long xNewPos;    // new position 
        long yDelta = 0;

        switch( LOWORD( wParam ) )
        {
            // User clicked the scroll bar shaft right of the scroll box.
            case SB_PAGERIGHT:
                xNewPos = m_nCurrentScrollX + m_MySize.cx;
                break;

            // User clicked the scroll bar shaft left of the scroll box.
            case SB_PAGELEFT:
                xNewPos = m_nCurrentScrollX - m_MySize.cx;
                break;

            // User clicked the right arrow.
            case SB_LINERIGHT:
                xNewPos = m_nCurrentScrollX + 5;
                break;

            // User clicked the left arrow.
            case SB_LINELEFT:
                xNewPos = m_nCurrentScrollX - 5;
                break;

            // User dragged the scroll box.
            case SB_THUMBPOSITION:
                xNewPos = HIWORD( wParam );
                break;

            default:
                xNewPos = m_nCurrentScrollX;
        }

        xNewPos = min( m_SizeCapture.cx - m_MySize.cx, xNewPos );
        xNewPos = max( 0, xNewPos );                  // xNewPos can't be negative;

        // If the current position does not change, do not scroll.
        if( xNewPos == m_nCurrentScrollX )
            return;

        // Determine the amount scrolled (in pixels).
        xDelta = xNewPos - m_nCurrentScrollX;

        // Reset the current scroll position.
        m_nCurrentScrollX = xNewPos;

        // Scroll the window. (The system repaints most of the
        // client area when ScrollWindowEx is called; however, it is
        // necessary to call UpdateWindow in order to repaint the
        // rectangle of pixels that were invalidated.)
        ::ScrollWindowEx( m_hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
                          (CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, SW_INVALIDATE );
        UpdateWindow( m_hWnd );

        // Reset the scroll bar.
        si.cbSize = sizeof( si );
        si.fMask  = SIF_POS;
        si.nPos   = m_nCurrentScrollX;
        ::SetScrollInfo( m_hWnd, SB_HORZ, &si, TRUE );
    }
    // Handle Vertical Scrolling
    else if( nMsg == WM_VSCROLL )
    {
        long xDelta = 0;
        long yNewPos;        // new position 
        long yDelta;         // yDelta = new_pos - current_pos  

        switch( LOWORD( wParam ) )
        {
            // User clicked the scroll bar shaft below of the scroll box.
            case SB_PAGEDOWN:
                yNewPos = m_nCurrentScrollY + m_MySize.cy;
                break;

            // User clicked the scroll bar shaft above of the scroll box.
            case SB_PAGEUP:
                yNewPos = m_nCurrentScrollY - m_MySize.cy;
                break;

            // User clicked the bottom arrow.
            case SB_LINEDOWN:
                yNewPos = m_nCurrentScrollY + 5;
                break;

            // User clicked the top arrow.
            case SB_LINEUP:
                yNewPos = m_nCurrentScrollY - 5;
                break;

            // User dragged the scroll box.
            case SB_THUMBPOSITION:
                yNewPos = HIWORD( wParam );
                break;

            default:
                yNewPos = m_nCurrentScrollY;
        }

        yNewPos = min( m_SizeCapture.cy - m_MySize.cy, yNewPos );
        yNewPos = max( 0, yNewPos );                  // yNewPos can't be negative;

        // If the current position does not change, do not scroll.
        if( yNewPos == m_nCurrentScrollY )
            return;

        // Determine the amount scrolled (in pixels).
        yDelta = yNewPos - m_nCurrentScrollY;

        // Reset the current scroll position.
        m_nCurrentScrollY = yNewPos;

        // Scroll the window. (The system repaints most of the
        // client area when ScrollWindowEx is called; however, it is
        // necessary to call UpdateWindow in order to repaint the
        // rectangle of pixels that were invalidated.)
        ::ScrollWindowEx( m_hWnd, -xDelta, -yDelta, (CONST RECT *) NULL, 
                          (CONST RECT *) NULL, (HRGN) NULL, (LPRECT) NULL, SW_INVALIDATE );
        UpdateWindow( m_hWnd );

        // Reset the scroll bar.
        si.cbSize = sizeof( si );
        si.fMask  = SIF_POS;
        si.nPos   = m_nCurrentScrollY;
        ::SetScrollInfo( m_hWnd, SB_VERT, &si, TRUE );
    }
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SetParent( HWND hWndParent )
{
    return ( ::SetParent( m_hWnd, hWndParent ) != NULL );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SetWindowPos( int x, int y, int cx, int cy )
{
    bool ret = ::SetWindowPos( m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER ) != 0;
    HandleScroll( WM_SIZE );

    UpdateStretch( );

    InvalidateRect( m_hWnd, NULL, false );
    UpdateWindow( m_hWnd );

    return(  ret );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::UpdateWindowPos( void )
{
    RECT rc;

    if( m_hWnd == NULL )
        return false;

    GetWindowRect( ::GetParent( m_hWnd ), &rc );
    ScreenToClient( m_hWnd, (LPPOINT) &rc );
    ScreenToClient( m_hWnd, (LPPOINT) &rc+1 );

    return SetWindowPos( rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );

}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SetWindowStyle( long nStyle )
{
    if( !m_hWnd )
        return false;

    if( nStyle & AS_RECV_STRETCH )
    {
        nStyle &= ~(AS_RECV_SCROLLBAR|AS_RECV_STRETCH_PROPORTIONAL);
    }

    if( nStyle & AS_RECV_STRETCH_PROPORTIONAL )
    {
        nStyle &= ~(AS_RECV_SCROLLBAR|AS_RECV_STRETCH);
    }

    // debug - cox test
//    nStyle &= ~(AS_RECV_SCROLLBAR|AS_RECV_STRETCH_PROPORTIONAL);
//    nStyle |= AS_RECV_STRETCH;

    m_nStyle = nStyle;

    // Handle Stretch
    UpdateStretch( );

    // ScrollBar control
    HandleScroll( WM_SIZE );

    InvalidateRect( m_hWnd, NULL, m_nStyle & AS_RECV_SCROLLBAR );
    UpdateWindow( m_hWnd );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::UpdateStretch( void )
{
//    HDC hdc;

    if( !m_hWnd )
        return false;

    if( m_nStyle & AS_RECV_STRETCH )
    {
        m_nStretchChunkWidth   = m_MySize.cx / NCHUNK_X;
        m_nStretchChunkWidth  += m_MySize.cx % NCHUNK_X == 0 ? 0 : 1;
        m_nStretchChunkHeight  = m_MySize.cy / NCHUNK_Y;
        m_nStretchChunkHeight += m_MySize.cy % NCHUNK_Y == 0 ? 0 : 1;

        m_nStretchChunkResidueX = NCHUNK_X * m_nStretchChunkWidth  - m_MySize.cx;
        m_nStretchChunkResidueY = NCHUNK_Y * m_nStretchChunkHeight - m_MySize.cy;

//        if( m_nStretchChunkHeight && m_nChunkHeight )
//        {
//            m_nStretchYOffset  = m_nChunkResidueY * m_nStretchChunkHeight / m_nChunkHeight;
//            m_nStretchYOffset += m_nChunkResidueY * m_nStretchChunkHeight % m_nChunkHeight == 0 ? 0 : 1;
//        }
        m_nCenterYOffset = 0;
    }

    if( m_nStyle & AS_RECV_STRETCH_PROPORTIONAL )
    {
        float x, y;
        long nX, nY;

        x = (float)m_MySize.cx/(float)m_SizeCapture.cx;
        y = (float)m_MySize.cy/(float)m_SizeCapture.cy;

        if( x < y )
        {
            nX = m_MySize.cx;
            nY = (long)(m_SizeCapture.cy * x);
        }
        else
        {
            nX = (long)(m_SizeCapture.cx * y);
            nY = m_MySize.cy;
        }
        m_nStretchChunkWidth   = nX / NCHUNK_X;
        m_nStretchChunkWidth  += nX % NCHUNK_X == 0 ? 0 : 1;
        m_nStretchChunkHeight  = nY / NCHUNK_Y;
        m_nStretchChunkHeight += nY % NCHUNK_Y == 0 ? 0 : 1;

        m_nStretchChunkResidueX = NCHUNK_X * m_nStretchChunkWidth  - nX;
        m_nStretchChunkResidueY = NCHUNK_Y * m_nStretchChunkHeight - nY;

        m_nCenterYOffset = max( m_MySize.cy - m_nStretchChunkHeight * NCHUNK_Y, 0 );
        m_nCenterYOffset >>= 1;

//        if( m_nStretchChunkHeight && m_nChunkHeight )
//        {
//            m_nStretchYOffset = m_nChunkResidueY * m_nStretchChunkHeight / m_nChunkHeight;
//            m_nStretchYOffset += m_nChunkResidueY * m_nStretchChunkHeight % m_nChunkHeight == 0 ? 0 : 1;
//        }
    }

    // fill the buffer with black buffer
    BitBlt( m_hDCStretchMem, 0, 0, GetDeviceCaps( m_hDCStretchMem, HORZRES),
            GetDeviceCaps( m_hDCStretchMem, HORZRES), NULL, 0, 0, BLACKNESS );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::UpdateStretchHDC( unsigned long nWidth, unsigned long nHeight )
{
    HDC hdc;

    if( !m_hWnd || !m_hDCMem )
        return false;

    // dealloc old HDCs
    if( m_hDCStretchMem )
    {
        SelectObject( m_hDCStretchMem, m_hOldStretchBitmap );
        DeleteDC( m_hDCStretchMem );
    }
    m_hDCStretchMem = NULL;

    if( m_hBMPMemStretch )
        DeleteObject( m_hBMPMemStretch );
    m_hBMPMemStretch = NULL;

    // alloc new HDCs
    hdc = GetDC( m_hWnd );
    // Stretch hDC/hBitMap
    m_hDCStretchMem  = CreateCompatibleDC( m_hDCMem );
    m_hBMPMemStretch = CreateCompatibleBitmap( m_hDCMem,
                                               nWidth, 
                                               nHeight );
    m_hOldStretchBitmap = (HBITMAP)SelectObject( m_hDCStretchMem, m_hBMPMemStretch );

    ReleaseDC( m_hWnd, hdc );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader )
{
    HDC hdc;
    //CXBITMAPINFO bmi;
	
    if( !m_hWnd )
        return false;

	if( !CxReceiveBase::CreateFrameBuffer(pBitmapInfoHeader) )
		return false;

    hdc          = GetDC( m_hWnd );
    m_hDCMem     = CreateCompatibleDC( hdc );
    m_hMemBitmap = CreateDIBSection( m_hDCMem, (BITMAPINFO *)&m_MemBitmapInfo.bmih, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0 );
    m_hOldMemBitmap = (HBITMAP)SelectObject( m_hDCMem, m_hMemBitmap );

    // Stretch hDC/hBitMap
    m_hDCStretchMem  = CreateCompatibleDC( m_hDCMem );
    m_hBMPMemStretch = CreateCompatibleBitmap( m_hDCMem,
                                               GetDeviceCaps( hdc, HORZRES), 
                                               GetDeviceCaps( hdc, VERTRES ) );
    m_hOldStretchBitmap = (HBITMAP)SelectObject( m_hDCStretchMem, m_hBMPMemStretch );

    ReleaseDC( m_hWnd, hdc );
    GdiFlush( );

    if( ( m_nStyle & AS_RECV_STRETCH ) || ( m_nStyle & AS_RECV_STRETCH_PROPORTIONAL) ) 
        UpdateStretch( );

    HandleScroll( WM_SIZE );
    InvalidateRect( m_hWnd, NULL, TRUE );

    return (m_hMemBitmap != NULL);
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::DeleteFrameBuffer( void )
{
	if( !m_pBits )
        return true;

    if( !CxReceiveBase::DeleteFrameBuffer() )
        return false;

    // stretch bitmap/hdc
    SelectObject( m_hDCStretchMem, m_hOldStretchBitmap );
    if( m_hDCStretchMem )
        DeleteDC( m_hDCStretchMem );
    m_hDCStretchMem = NULL;

    if( m_hBMPMemStretch )
        DeleteObject( m_hBMPMemStretch );
    m_hBMPMemStretch = NULL;

    // hdc mem/bitmap
    SelectObject( m_hDCMem, m_hOldMemBitmap );
    if( m_hDCMem )
        DeleteDC( m_hDCMem );
    m_hDCMem = NULL;

    if( m_hMemBitmap )
        DeleteObject( m_hMemBitmap );
    m_hMemBitmap = NULL;

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::UpdateScreen( RecordData *pData )
{
    if( !m_hWnd )
        return false;

	if (!CxReceiveBase::UpdateScreen( pData ) )
		return false;

	RECT rc;

    // DEBUG
//    char pszFilename[256];
//    static int i=2;
//    RECTL rcx;
//    sprintf( pszFilename, "c:\\recv%04d.bmp", pData->seq );
//    rcx.left   = 0;
//    rcx.top    = 0;
//    rcx.right  = pData->rd_ChunkCount * m_nChunkWidth;
//    rcx.bottom = m_nChunkHeight;
//    SaveBitmapFile( pszFilename, m_pChunkBits, &rcx );
    // DEBUG

    if( m_nStyle & AS_RECV_STRETCH )
    {
        rc.left   = m_nStretchChunkWidth  * pData->rd_nChunk;
        rc.top    = m_nStretchChunkHeight * pData->rd_nLine - m_nStretchChunkHeight;
        rc.right  = rc.left + pData->rd_ChunkCount * m_nStretchChunkWidth;
        rc.bottom = rc.top  + 2*m_nStretchChunkHeight;
    }
    else if( m_nStyle & AS_RECV_STRETCH_PROPORTIONAL )
    {
        rc.left   = m_nStretchChunkWidth  * pData->rd_nChunk;
        rc.top    = m_nStretchChunkHeight * pData->rd_nLine + m_nCenterYOffset - m_nStretchChunkHeight;
        rc.right  = rc.left + pData->rd_ChunkCount * m_nStretchChunkWidth;
        rc.bottom = rc.top  + m_nStretchChunkHeight + m_nStretchChunkHeight;
    }
    else
    {
        rc.left   = m_nChunkWidth  * pData->rd_nChunk - m_nCurrentScrollX;
        rc.top    = m_nChunkHeight * pData->rd_nLine  - m_nCurrentScrollY - m_nChunkResidueY;
        rc.right  = rc.left + pData->rd_ChunkCount * m_nChunkWidth;
        rc.bottom = rc.top  + m_nChunkHeight;
    }

//    char szLog[1024];
//    sprintf( szLog, "rc.top: %d - rc.bottom: %d - rc.left: %d - rc.right: %d - residue: %d", rc.top, rc.bottom, rc.left, rc.right, m_nChunkResidueY );
//    OutputDebugString( szLog );

//    CxDump( "CxReceive::UpdateScreen - wait" );
//    m_pRecvEvent->Wait( );
    if( m_hWnd && IsWindow( m_hWnd ) && !m_flEnd )
    {
//        CxDump( "InvalidateRect" );
        InvalidateRect( m_hWnd, (RECT *)&rc, FALSE );
//        CxDump( "UpdateWindow" );
//        CxDump( szLog );
        //UpdateWindow( m_hWnd );
    }
//    CxDump( "CxReceive::UpdateScreen - signal" );
//    m_pRecvEvent->SetEvent( );
//    CxDump( "CxReceive::UpdateScreen - end" );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::EnableKM( bool flEnable )
{
    if( flEnable )
        SetCursor( m_hCurEnable );
    else
        SetCursor( m_hCursor );

	return ( CxReceiveBase::EnableKM( flEnable ) );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::EnablePen( bool flEnable )
{
    return ( CxReceiveBase::EnablePen( flEnable ) );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SendKeyEvent( WPARAM wParam, LPARAM lParam )
{
    static const int nBufSize = 1024;
    static char szBuf[nBufSize];
    int nEncode;
    bool down = (lParam & 0x80000000) == 0;

    // verifica se tem direito de enviar (tem a vez?)
    if( !m_flKM )
        return false;

    szBuf[0] = 'K';  // keyboard message
    szBuf[1] = down ? '0' : '1';  // 0 for key down - 1 for key up

    // magic 3 number => 'K'  - key up/down and '\0'
    nEncode = nBufSize - 3;

    // base 64
    encode64( (const char *)&wParam, sizeof( wParam ), &szBuf[2], &nEncode );

    // send event to kernel
    ASEvent( AS_NOTIFY_KMMSG, (void *)szBuf, nEncode+2 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SendKey( long nVK, bool flDown )
{
    long nState;

    nState = flDown ? 0 : 0x80000000 ;

    return SendKeyEvent( (WPARAM) nVK, nState );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveWin::SendPointerEvent( int message, WPARAM wParam, LPARAM lParam )
{
    static const int nBufSize = 1024;
    static char szBuf[nBufSize], szParams[24];
    int nEncode;

    if( m_flPen )
    {
        int nNotify = 0;
        static char szNotifyBuf[2*sizeof(long)];

        // Notify interface about mouse move/LButton's click
        if( message == WM_MOUSEMOVE )
            nNotify = AS_NOTIFY_MOUSEMOVE;
        else if( message == WM_LBUTTONDOWN )
            nNotify = AS_NOTIFY_LBUTTONDOWN;
        else if( message == WM_LBUTTONUP )
            nNotify = AS_NOTIFY_LBUTTONUP;

        if( nNotify )
        {
            memcpy( szNotifyBuf, &wParam, sizeof( long ) );
            memcpy( szNotifyBuf + sizeof(long), &lParam, sizeof( long ) );
            ASEvent( nNotify, szNotifyBuf, sizeof( szNotifyBuf ) );
        }
        return true;
    }

    // verifica se tem direito de enviar (tem a vez?)
    if( !m_flKM )
        return false;

    szBuf[0] = 'M';  // mouse message

    // magic 2 number => 'M' and '\0'
    nEncode = nBufSize - 2;

    memset( szParams, 0, sizeof( szParams ) );
    memcpy( &szParams[0],  &message, sizeof( message ) );
    memcpy( &szParams[8],  &wParam,  sizeof( wParam  ) );
    memcpy( &szParams[16], &lParam,  sizeof( lParam  ) );

    // base 64
    encode64( (const char *)&szParams, sizeof( szParams ), &szBuf[1], &nEncode );

    // send event to kernel
    ASEvent( AS_NOTIFY_KMMSG, (void *)szBuf, nEncode+1 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Global scope functions
// g_WindowProc - friend of CxReceive class
LRESULT g_WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    long nRet = 0;
//	int wmId, wmEvent;
    int nx, ny;
	HDC hDC;
	PAINTSTRUCT ps;
    CREATESTRUCT *pCS;
    PRECT pRect;
    CxReceiveWin *pThis = (CxReceiveWin *)GetWindowLong( hWnd, GWL_USERDATA );

	switch( message )
	{
		case WM_CREATE:
            pCS = (CREATESTRUCT *)lParam;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG)(pCS->lpCreateParams) );
            pThis = (CxReceiveWin *)pCS->lpCreateParams;
            pThis->m_nCurrentScrollX = pThis->m_nCurrentScrollY = 0;
			nRet = 0;
			break;

        case WM_DESTROY:
//            PostQuitMessage( 0 );
            pThis->m_hWnd = NULL;
            nRet = 0;
            break;

        case WM_SIZE:
            pThis->m_MySize.cx = LOWORD( lParam );
            pThis->m_MySize.cy = HIWORD( lParam );

            // Handling the scrolling.
            pThis->HandleScroll( message );

            // Stretch window?
            // Chunk width and Line Width
            pThis->UpdateStretch( );
            nRet = 0;
            break;

        case WM_HSCROLL:
        case WM_VSCROLL:
            pThis->HandleScroll( message, wParam );
            break;

        case WM_DISPLAYCHANGE:
            pThis->UpdateStretchHDC( LOWORD( lParam ), HIWORD( lParam ) );
            break;

        case WM_PAINT:
            hDC = BeginPaint( hWnd, &ps );
            pRect = &ps.rcPaint;

            if( pThis->m_hDCStretchMem && (pThis->m_nStyle & AS_RECV_STRETCH || pThis->m_nStyle & AS_RECV_STRETCH_PROPORTIONAL ) )
            {
                StretchBlt( pThis->m_hDCStretchMem, 0, pThis->m_nCenterYOffset, pThis->m_nStretchChunkWidth * NCHUNK_X - pThis->m_nStretchChunkResidueX, pThis->m_nStretchChunkHeight * NCHUNK_Y - pThis->m_nStretchChunkResidueY,
                            pThis->m_hDCMem, 0, 0, pThis->m_nChunkWidth * NCHUNK_X - pThis->m_nChunkResidueX, pThis->m_nChunkHeight * NCHUNK_Y  - pThis->m_nChunkResidueY, SRCCOPY );

                BitBlt( hDC, pRect->left, pRect->top, (pRect->right - pRect->left), (pRect->bottom - pRect->top),
                        pThis->m_hDCStretchMem, pRect->left, pRect->top, SRCCOPY );
            }
            else
            {
                BitBlt( hDC, pRect->left, pRect->top, (pRect->right - pRect->left), (pRect->bottom - pRect->top),
                        pThis->m_hDCMem, pRect->left + pThis->m_nCurrentScrollX, pRect->top + pThis->m_nCurrentScrollY, SRCCOPY );
            }

            EndPaint( hWnd, &ps );
            nRet = 0;
            break;

//		case WM_COMMAND:
//			wmId    = LOWORD( wParam );
//			wmEvent = HIWORD( wParam );
			// Parse the menu selections:
//			switch( wmId )
//			{
//                case 1:
//				default:
//					nRet = 1;
//			}
//		break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            nRet = !pThis->SendKeyEvent( wParam, lParam );
            break;

        case WM_CHAR:
            nRet = 0;
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
            nx = LOWORD( lParam );
            ny = HIWORD( lParam );
            lParam = MAKELPARAM( nx + pThis->m_nCurrentScrollX, ny + pThis->m_nCurrentScrollY );
            pThis->SendPointerEvent( message, wParam, lParam );
            nRet = 0;
            break;

        case WM_MOUSEACTIVATE:
            SetFocus( pThis->m_hWnd );
            nRet = MA_ACTIVATE;
            break;

        case WM_KILLFOCUS:
            pThis->SendKey( VK_SHIFT,    false );
            pThis->SendKey( VK_LSHIFT,   false );
            pThis->SendKey( VK_RSHIFT,   false );
            pThis->SendKey( VK_CONTROL,  false );
            pThis->SendKey( VK_LCONTROL, false );
            pThis->SendKey( VK_RCONTROL, false );
            pThis->SendKey( VK_MENU,     false );
            nRet = 0;
            break;

        case WM_SETCURSOR:
            if( pThis->m_flKM )
                SetCursor( pThis->m_hCurEnable );
            else
                SetCursor( pThis->m_hCursor );
            nRet = true;
            break;

        default:
            nRet = (long) DefWindowProc( hWnd, message, wParam, lParam );
	}
    return nRet;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////