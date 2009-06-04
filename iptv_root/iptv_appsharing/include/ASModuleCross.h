///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __ASMODULE_CROSS__
#define __ASMODULE_CROSS__
///////////////////////////////////////////////////////////////////////////////
#include "ASModuleDll.h"
#include "ASFlags.h"
#include "ASFrame.h"
#include "ASListener.h"
///////////////////////////////////////////////////////////////////////////////
// Forward Declarations
class ISockBuff;
class CxReceiveCross;
///////////////////////////////////////////////////////////////////////////////
class ASMODULE_API_CROSS ASModule
{
private:
	flModes			m_nMode;
	CxReceiveCross* m_pASReceive;
//	CxCaptureCross* m_pASCapture;

public:
	ASModule();
	~ASModule();

	bool Initialize( ISockBuff *pSockBuff,
					 int flMode,
					 long nMediaID,
					 ASListener *pListener = NULL );
	bool End();

	// Receive Methods
	bool GetFrame( ASFrame *pFrame, bool pConvertRGB8 = true );
	bool SaveFrameBitmap( ASFrame *pFrame, char* pFileName );
	bool IsNewFrameAvailable();

	bool SendKeyEvent    ( WPARAM wParam, LPARAM lParam );
	bool SendKey         ( long nVK, bool flDown );
	bool SendPointerEvent( int message, WPARAM wParam, LPARAM lParam );

	// Capture Methods
	// ...
};
///////////////////////////////////////////////////////////////////////////////
#endif /* __ASMODULE_CROSS__ */
///////////////////////////////////////////////////////////////////////////////

