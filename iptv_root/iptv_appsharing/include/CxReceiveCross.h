/* CxReceive.h
** ------------
** Application Sharing Receive Module
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Wed Mar 12 19:06:39 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CXRECEIVE_CROSS__
#define CXRECEIVE_CROSS__
///////////////////////////////////////////////////////////////////////////////
#include "CxReceiveBase.h"
#include "iptv_shared/shared/semaphore.h"
///////////////////////////////////////////////////////////////////////////////
// Forward definitions
class CxReceiveThread;
class ASFrame;
class _SEMAPHORE;
///////////////////////////////////////////////////////////////////////////////
class CxReceiveCross : public CxReceiveBase
{
private:
	_SEMAPHORE	 m_FrameMutex;	// Protects framebuffer (m_pBits) access.
	bool		 m_bNewFrame;	// Advises when a new frame is available, without trying to get it.
	long		 m_nBufferSize;	// Store the current framebuffer size.

	bool SendKeyEvent    ( WPARAM wParam, LPARAM lParam );
	bool SendKey         ( long nVK, bool flDown );
	bool SendPointerEvent( int message, WPARAM wParam, LPARAM lParam );

	bool CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader );
	bool DeleteFrameBuffer( void );	
	bool UpdateScreen( RecordData *pData );

	bool FlipFrame( ASFrame* pFrame );
	long GetFrameFormat();

public:
	CxReceiveCross( void );
	~CxReceiveCross( void );

	bool Initialize( ISockBuff *pSockBuff, long nMediaID, CxAppSharingListener *pListener,
	                 HWND p_hWnd = NULL, int p_BitDepth = 16, RECT *p_Bounds = NULL,
					 int p_RefreshTime = 1000, int p_CaptureMode = AS_GDI );
	bool End( void );

	bool GetFrame ( ASFrame *pFrame, bool pConvertRGB = false );
	bool GetBitmap( ASFrame *pFrame );
	bool SaveFrameBitmap( char* pFileName );
	bool IsNewFrameAvailable();
};
///////////////////////////////////////////////////////////////////////////////
#endif /* CXRECEIVE_CROSS__ */
///////////////////////////////////////////////////////////////////////////////

