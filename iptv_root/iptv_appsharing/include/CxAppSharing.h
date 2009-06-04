/* AsClient.h
** ----------
** Application Sharing Main - CxWinApp Interface
**
**
** Author : Guilherme Cox <cox@ieee.org>
** Updated: Wed Dec  6 17:25:34 BRST 2006
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __CXAPPSHARING__
#define __CXAPPSHARING__
///////////////////////////////////////////////////////////////////////////////
#include "ASFlags.h"
#include "ASCompat.h"
#include "ASListener.h"
#include "iptv_shared/shared/irm/mediapkt.h"

// This macro is used to build a module compatible with NCT version.
#ifdef NCT_VERSION
#include "CxAppSharingWin.h"
#endif /* NCT_VERSION */
///////////////////////////////////////////////////////////////////////////////
#define UNICODE_MAX_PATH           (32767)
// Used to format error messages;
#define AS_BUFFER_SIZE				(1024)
// Timeout (seconds) to send Bitmap Headers
#define AS_BITMAPHEADER_TIMEOUT       (10)
// Timeout (seconds) to send KeyFrame
#define AS_KEYFRAME_TIMEOUT           (15)
// KeyFrame Fragments
#define AS_KEYFRAME_FRAGMENTS          (4)
// Chunks
#define NCHUNK_X                      (16)
#define NCHUNK_Y                      (16)

// frame-rate control
// how much time to send a frame (ms)
#define AS_SENDDATA_TIME            (1000)
//
// Default Color masks (16 and 32 bpp)
// default 5-5-5 16 bbp
#define CLR_MASK_16_RED         (0x00007C00)
#define CLR_MASK_16_GREEN       (0x000003E0)
#define CLR_MASK_16_BLUE        (0x0000001F)
//default 8-8-8 32 bpp
#define CLR_MASK_32_RED         (0x00FF0000)
#define CLR_MASK_32_GREEN       (0x0000FF00)
#define CLR_MASK_32_BLUE        (0x000000FF)

// Process key event
#define INPUT_SIZE              (128)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ISockBuff
class ISockBuff;
// CxZLib
class CxZLib;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
typedef struct _CHANGES_RECORD
{
    ULONG  id;
    ULONG  type;     //screen_to_screen, blit, newcache, oldcache
    RECTL  rect;
    POINTL point;
} CHANGES_RECORD, *PCHANGES_RECORD;

typedef struct _MyBitmapInfo
{
    BITMAPINFOHEADER bmih;
    unsigned long nRedMask;
    unsigned long nGreenMask;
    unsigned long nBlueMask;
} CXBITMAPINFO, *PCXBITMAPINFO;

enum RecordDataTypes
{
    RDT_BITMAPHEADER,
    RDT_UPDATE_FRAMESIZE,           // to be tested when user change video settings during capture
    RDT_UPDATE_SCREEN,
};

typedef int INT;

struct RecordData : public LMediaPkt
{
    unsigned long rd_size;   // my size
    long          rd_type;   // BitmapHeader, update screen, mouse change cursor, ???
    long          rd_nLine;
    long          rd_nChunk;
    long          rd_ChunkCount;
    unsigned long rd_CheckSum;
    BYTE          rd_pData[1];
};
#pragma pack( pop )

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CxAppSharing
{
    private:
        int   m_flMode;        // capturing or receiving

    protected:

// This macro is used to build a module compatible with NCT version
#ifdef NCT_VERSION
		// hWnd
		HWND         m_hWnd;

		// GDI data
		HDC          m_hDCMem;
		HBITMAP      m_hMemBitmap, m_hOldMemBitmap;

		// Debug Window
		char szDebugBuf[AS_BUFFER_SIZE];
		DebugWin *m_pDebug;
#endif

        // network
        ISockBuff   *m_pSockBuff;
        long         m_nMediaID, m_nSeq;

        CXBITMAPINFO m_MemBitmapInfo;

        // ZLib
        CxZLib      *m_pZLib;

        // Chunk data
        BYTE *m_pChunkBits;
        long m_nChunkWidth, m_nChunkHeight, m_nWidthBits, m_nChunkWidthBits;

		// this class can't be instantiated
        CxAppSharing( int flMode );

        // Listener
        CxAppSharingListener *m_pListener;

    public:
		virtual ~CxAppSharing( void );

		virtual bool Initialize( ISockBuff *pSockBuff, long nMediaID,
			                     CxAppSharingListener *pListener, HWND p_hWnd = NULL,
								 int p_BitDepth = 16, RECT *p_Bounds = NULL,
								 int p_RefreshTime = 1000, int p_CaptureMode = AS_GDI ) = 0;

        virtual bool End( void );

        long GetMode( void )            { return m_flMode; };

        // Listener
        bool ASEvent( int nEvent, void *pParams, unsigned int nSize );

		bool SaveBitmapFile( char *pszFilename, BYTE *pBits, RECTL *pRect );
};

///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
bool CxDump( RecordData *prd, unsigned long nSize = 0 );
bool CxDump( char *pszStr );
#endif
#endif  /* __CXAPPSHARING__ */

