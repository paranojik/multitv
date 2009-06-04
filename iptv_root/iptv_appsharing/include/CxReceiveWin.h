/* CxReceive.h
** ------------
** Application Sharing Receive Module
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Wed Mar 12 19:06:39 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <tchar.h>
#include "CxReceiveBase.h"
#include "resource.h"
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CXRECEIVE_WIN__
#define CXRECEIVE_WIN__
///////////////////////////////////////////////////////////////////////////////
// CxReceiveThread
class CxReceiveThread;
// Windows Proc
LRESULT g_WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
///////////////////////////////////////////////////////////////////////////////
class CxReceiveWin : public CxReceiveBase
{
    private:
        long     m_nCurrentScrollX, m_nCurrentScrollY;
        long     m_nStyle;
        long     m_nStretchChunkWidth, m_nStretchChunkHeight, m_nCenterYOffset;

//        CxEvent *m_pRecvEvent; // Was protecting m_flEnd.
								 // Does not need protection, since booleans only do atomic operations.

        HDC      m_hDCStretchMem;
        HBITMAP  m_hBMPMemStretch, m_hOldStretchBitmap;

        // Keyboard events
        bool SendKeyEvent( WPARAM wParam, LPARAM lParam );
        bool SendKey( long nVK, bool flDown );

        // Mouse events
        bool SendPointerEvent( int message, WPARAM wParam, LPARAM lParam );

        // Scroll Functions
        void HandleScroll( unsigned int nMsg, WPARAM wParam = 0 );

        // Keyboard and Mouse
        HCURSOR m_hCurEnable, m_hCursor;

    public:
        CxReceiveWin( void );
        ~CxReceiveWin( void );

		bool Initialize( ISockBuff *pSockBuff, long nMediaID,
		                 CxAppSharingListener *pListener, HWND p_hWnd = NULL,
						 int p_BitDepth = 16, RECT *p_Bounds = NULL,
						 int p_RefreshTime = 1000, int p_CaptureMode = AS_GDI );
        bool End( void );

        bool SetParent( HWND hWndParent );
        bool SetWindowPos( int x, int y, int cx, int cy );
        bool UpdateWindowPos( void );
        bool SetWindowStyle( long nStyle );
        long GetWindowStyle( void )         { return m_nStyle; }

        bool CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader );
        bool DeleteFrameBuffer( void );
        bool UpdateScreen( RecordData *pData );

        bool UpdateStretch( void );
        bool UpdateStretchHDC(  unsigned long nWidth, unsigned long nHeight );

        bool EnableKM( bool flEnable );
        bool EnablePen( bool flEnable );

        friend LRESULT g_WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
};
#endif /* CXRECEIVE_WIN__ */
///////////////////////////////////////////////////////////////////////////////