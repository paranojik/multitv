/* CxAppSharingWin.h
** ----------
** Windows only part of CxAppSharing
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Wed Dec  6 17:25:34 BRST 2006
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <tchar.h>
#include "DebugWin.h"
#include "resource.h"

#pragma once
#ifndef __ASCLIENTWIN__
#define __ASCLIENTWIN__

#define SZ_AS_CLASSNAME			_T("ASCLIENT")
#define SZ_AS_CAPTION			_T("Application Sharing - IPTV")

#define SZ_MIRROR_DRIVERNAME	_T("CxDD Mirror Driver")

// DrvEscape defines
#define MAP_READ     0x20001

#define MAP_EVENT    0x20004
#define UNMAP_EVENT  0x20008

//#define MAP_RECTACK  0x20010
//#define MAP_TEST     0x20020

#define MAPFILE_SIZE   20000

#define MAP_FILENAME  L"CxVideo.dat"

///////////////////////////////////////////////////////////////////////////////
// DebugWin
class DebugWin;
// hInstance
extern HINSTANCE g_hInstance;

#endif //__ASCLIENTWIN__