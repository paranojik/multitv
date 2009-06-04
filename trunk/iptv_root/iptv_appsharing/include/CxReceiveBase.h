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
#pragma once
#ifndef CXRECEIVE_BASE__
#define CXRECEIVE_BASE__
///////////////////////////////////////////////////////////////////////////////
#include "CxAppSharing.h"
///////////////////////////////////////////////////////////////////////////////
// Forward Declarations
class CxReceiveThread;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ReceiveInterface
{
    public:
        virtual bool CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader ) = 0;
        virtual bool DeleteFrameBuffer( void ) = 0;
        virtual bool UpdateScreen( RecordData *pData ) = 0;
        virtual bool ResetCodec( void ) = 0;
};
///////////////////////////////////////////////////////////////////////////////
class CxReceiveBase : public CxAppSharing, ReceiveInterface
{
	protected:
        long     m_nChunkResidueX, m_nChunkResidueY, m_nStretchChunkResidueX, m_nStretchChunkResidueY;
        SIZE     m_MySize, m_SizeCapture;
        BYTE    *m_pBits;	// Memory DIB pointer
        bool     m_flEnd;
		bool	 m_flKM, m_flPen;

		// Receive thread
        CxReceiveThread *m_pReceiveThread;

        // Keyboard and mouse events
        virtual bool SendKeyEvent( WPARAM wParam, LPARAM lParam ) = 0;
        virtual bool SendKey( long nVK, bool flDown ) = 0;
        virtual bool SendPointerEvent( int message, WPARAM wParam, LPARAM lParam ) = 0;
	
		CxReceiveBase( void );
        ~CxReceiveBase( void );
		bool Initialize( ISockBuff *pSockBuff, long nMediaID, CxAppSharingListener *pListener);

    public:
        virtual bool End( void );
		bool ResetCodec( void );

        virtual bool CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader );
        virtual bool DeleteFrameBuffer( void );
		virtual bool UpdateScreen( RecordData *pData );

        virtual bool EnableKM( bool flEnable );
        virtual bool EnablePen( bool flEnable );
};
///////////////////////////////////////////////////////////////////////////////
#endif /* CXRECEIVE_BASE__ */
///////////////////////////////////////////////////////////////////////////////

