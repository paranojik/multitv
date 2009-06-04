///////////////////////////////////////////////////////////////////////////////
// ASModule.cpp : Defines the entry point for the DLL application.
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include <windows.h>
#include "ASModuleWin.h"
#include "CxAppSharing.h"
#include "CxCapture.h"
#include "CxReceiveWin.h"
///////////////////////////////////////////////////////////////////////////////
HINSTANCE g_hInstance;
CxAppSharing *pAS = NULL;
///////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch( ul_reason_for_call )
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            UnregisterClass( SZ_AS_CLASSNAME, g_hInstance );
            break;

    }
    g_hInstance = (HINSTANCE)hModule;
    return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool Initialize( ISockBuff *pSockBuff, int flMode, long nMediaID,
								  CxAppSharingListener *pListener, HWND p_hWnd,
								  int p_BitDepth, RECT *p_Bounds, int p_RefreshTime,
								  int p_CaptureMode )
{
    if( pAS )
        return false;

	switch (flMode)
	{
	case AS_CAPTURING:
		pAS = new CxCapture;
		break;
	case AS_RECEIVING:
		pAS = new CxReceiveWin;
		break;
	default:
		pAS = NULL;
	}

	if (!pAS)
		return false;
	
	return pAS->Initialize( pSockBuff, nMediaID, pListener, p_hWnd, p_BitDepth,
							p_Bounds, p_RefreshTime, p_CaptureMode );
}
///////////////////////////////////////////////////////////////////////////////
//Receive
ASMODULE_API_WIN bool Initialize( ISockBuff *pSockBuff, int flMode, long nMediaID, HWND hWnd, CxAppSharingListener *pListener  )
{
    if( pAS )
        return false;

    if( flMode != AS_CAPTURING && flMode != AS_RECEIVING )
        return false;

    if( flMode == AS_CAPTURING )
    {
        pAS = new CxCapture;
    }
    else
        pAS = new CxReceiveWin;

    if( pAS )
        return pAS->Initialize( pSockBuff, nMediaID, pListener, hWnd );//(HWND) 0x000307F6 );//hWnd );

    return false;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool End( void )
{
    if( pAS )
    {
        pAS->End( );
        delete pAS;
    }
    pAS = NULL;
    return true;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool SetParent( HWND hWndParent )
{
    if( !pAS )
        return false;

    if( pAS->GetMode( ) == AS_RECEIVING )
        return ((CxReceiveWin*)pAS)->SetParent( hWndParent );
    
	return true;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool SetWindowPos( int x, int y, int cx, int cy )
{
    if( !pAS )
        return false;

    if( pAS->GetMode( ) == AS_RECEIVING )
        return ((CxReceiveWin*)pAS)->SetWindowPos( x, y, cx, cy );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool UpdateWindowPos( void )
{
    if( !pAS )
        return false;

    if( pAS->GetMode( ) == AS_RECEIVING )
		return ((CxReceiveWin*)pAS)->UpdateWindowPos( );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN bool SetReceiveStyle( long nStyle)
{
    if( !pAS )
        return false;

    if( pAS->GetMode( ) == AS_RECEIVING )
		return ((CxReceiveWin*)pAS)->SetWindowStyle( nStyle );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long GetReceiveStyle( void )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_RECEIVING )
		return ((CxReceiveWin*)pAS)->GetWindowStyle( );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long SendKeyFrame( void )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_CAPTURING )
        return ((CxCapture*)pAS)->WantKeyFrame( );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long SetCaptureTimeWindow( unsigned long nmsTime )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_CAPTURING )
    {
        ((CxCapture*)pAS)->SetTimeWindow( nmsTime );
        return nmsTime;
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long EnableKM( bool flEnable )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_RECEIVING )
        return ((CxReceiveWin*)pAS)->EnableKM( flEnable );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long EnablePen( bool flEnable )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_RECEIVING )
        return ((CxReceiveWin*)pAS)->EnablePen( flEnable );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long ProcessKM( char *psKM, unsigned long nSize )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_CAPTURING )
        return ((CxCapture*)pAS)->ProcessKMEvent( psKM, nSize );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long ResetKM( void )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_CAPTURING )
        return ((CxCapture*)pAS)->ResetKM( );

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_WIN long Send_CTRL_ALT_DEL( void )
{
    if( !pAS )
        return 0;

    if( pAS->GetMode( ) == AS_CAPTURING )
    {
        return true;
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
