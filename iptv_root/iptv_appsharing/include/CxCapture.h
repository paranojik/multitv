/* CxCapture.h
** -----------
** Application Sharing Capture Module
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Sat Mar  8 17:36:05 BRT 2008
*/
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef CXCAPTURE__
#define CXCAPTURE__
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
//#include <tchar.h>
#include "CxAppSharing.h"
#include "resource.h"
#include "iptv_video_capture/include/VideoCapture.h"
using namespace tv::ip;
///////////////////////////////////////////////////////////////////////////////
class CaptureInterface
{
    public:
        virtual bool UpdateData( unsigned short int *p ) = 0;
};

class SendInterface
{
    public:
        virtual bool SendData( long nLine, long nChunk, long nCount ) = 0;
        virtual bool SendBitmapHeader( void ) = 0;
        virtual bool SendKeyFrame( void ) = 0;
        virtual bool ResetCodec( void ) = 0;
        virtual unsigned long GetTimeWindow( void ) = 0;
};
///////////////////////////////////////////////////////////////////////////////
// CxCaptureThread;
class CxCaptureThread;
//CxSendThread
class CxSendThread;
// CxEvent
class CxEvent;
///////////////////////////////////////////////////////////////////////////////
class CxCapture : public CxAppSharing, CaptureInterface, SendInterface
{
    private:
        bool  m_flInitCapture; // am I capturing?

        SIZE  m_SizeCapture;

        // GDI data
        RecordData *m_prdBitmapHeader, *m_prdBits;

        long m_nScreenSizeBits;
        long m_nChunkResidueX, m_nChunkResidueY;
        // used to compute chunks in a capture area defined by a RECT
        unsigned short int m_bmRect, m_nRectYStart, m_nRectYEnd;
        long m_nScreenChunkWidth, m_nScreenChunkHeight;

        // time window
        unsigned long m_nmsTimeWindow;

        // Mirror Map attributes (capture)
        //int   m_MapSize;       // sizeof( m_pMapMem ) returned by Mirror driver
        PVOID m_pMapMem;

        // Send thread
        CxSendThread *m_pSendThread;

        // Wallpaper
        bool RemoveWallpaper( void );
        bool RestoreWallpaper( void );

        // Dynamic Resizing
        bool m_bDynamicResizingState;
        bool RemoveDynamicResizing( void );
        bool RestoreDynamicResizing( void );

        // Rect area
        bool GetRectParams( RECT *pRect );

        // Keyframe
        long m_nKFCycle, m_nKFNeed, m_nKFSending;
//        CxEvent *m_pKFEvent;

        // local framebuffer
        PVOID m_pLocalBits;
        //HBITMAP m_hBMPLocalFB;
        bool CreateLocalFramebuffer( void );
        bool DestroyLocalFramebuffer( void );
        // returns true if the area is equal
        bool CompareArea( long nLine, long nChunk, long nCount );

        // Keyboard/mouse event control
        CxEvent *m_pKBEvent;

        // Process KeyEvent
        bool ProcessKeyEvent( long wVk, long down );

        // Process MouseEvent
        bool ProcessMouseEvent( int message, WPARAM wParam, LPARAM lParam );

    public:
		// Screen capture handler - 2DO: Adjust interface
		ScreenSource*  m_ScreenSource;
		void *m_pBits;
		unsigned short int m_pChunkList[NCHUNK_Y];

        CxCapture( void );
        //CxCapture( const CxCapture &c );
        virtual ~CxCapture( void );

		bool Initialize( ISockBuff *pSockBuff, long nMediaID,
					     CxAppSharingListener *pListener, HWND p_hWnd = NULL,
						 int p_BitDepth = 16, RECT *p_Bounds = NULL,
						 int p_RefreshTime = 1000, int p_CaptureMode = AS_GDI );

		bool End( void );

        // Capture functions
        bool UpdateData( unsigned short int *p );
        bool SendBitmapHeader( void );
        bool SendData( long nLine, long nChunk, long nCount );
        bool ResetCodec( void );
        // auxiliary UpdateData(..) for rect'ed capture area
        bool UpdateDataRect( unsigned short int *p );

        // Capture functions
        // Process KeyEvent
        bool ProcessKMEvent( char *psKM, unsigned long nSize );
        bool ResetKM( void );

        // keyframe
        bool WantKeyFrame( void );
        bool SendKeyFrame( void );

        // capture time window
        void SetTimeWindow( unsigned long nmsTime )    { m_nmsTimeWindow = nmsTime; }
        unsigned long GetTimeWindow( void );
};
#endif /* CXCAPTURE__ */
///////////////////////////////////////////////////////////////////////////////
