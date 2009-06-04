///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __ASMODULE__
#define __ASMODULE__
///////////////////////////////////////////////////////////////////////////////
#include "ASModuleDll.h"
#include "ASFlags.h"
#include "ASListener.h"
///////////////////////////////////////////////////////////////////////////////
#define AS_RECV_STRETCH                0x00000001
#define AS_RECV_STRETCH_PROPORTIONAL   0x00000002
#define AS_RECV_SCROLLBAR              0x00000100

#define ID_RECEIVE_CHILDWIN            (1021)
///////////////////////////////////////////////////////////////////////////////
class ISockBuff;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool Initialize( ISockBuff *pSockBuff, int flMode, long nMediaID,
								  CxAppSharingListener *pListener = NULL,
								  HWND p_hWnd = NULL, int p_BitDepth = 16,
								  RECT *p_Bounds = NULL, int p_RefreshTime = 1000,
								  int p_CaptureMode = AS_GDI );

ASMODULE_API_WIN bool End( void );
ASMODULE_API_WIN bool SetParent( HWND hWndParent );
ASMODULE_API_WIN bool SetWindowPos( int x, int y, int cx, int cy );
ASMODULE_API_WIN bool UpdateWindowPos( void );
ASMODULE_API_WIN bool SetReceiveStyle( long nStyle );
ASMODULE_API_WIN long GetReceiveStyle( void );
ASMODULE_API_WIN long SendKeyFrame( void );
ASMODULE_API_WIN long SetCaptureTimeWindow( unsigned long nmsTime );

// Mouse and Keyboard
  // Receive
ASMODULE_API_WIN long EnableKM(  bool flEnable );
ASMODULE_API_WIN long EnablePen( bool flEnable );
  //Capture
ASMODULE_API_WIN long ProcessKM( char *psKM, unsigned long nSize );
ASMODULE_API_WIN long ResetKM( void );
ASMODULE_API_WIN long Send_CTRL_ALT_DEL( void );
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif /* __ASMODULE__ */
