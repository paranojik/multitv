///////////////////////////////////////////////////////////////////////////////
// Multi-platform Compatibility
#pragma once
#ifndef __ASCOMPAT__
#define __ASCOMPAT__

#ifndef NCT_VERSION
#ifndef _WINDEF_

#define HRESULT		long

#ifndef STDMETHODIMP
//#define STDMETHODIMP HRESULT __stdcall // CHECK THIS !!!!!!
#define STDMETHODIMP	HRESULT
#endif

#define BI_RGB		0L
#define BI_BITFIELDS	3L

#define S_OK		((HRESULT)0x00000000L)

typedef struct _RECTL
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECTL, *PRECTL, *LPRECTL;

typedef struct _POINTL
{
    LONG  x;
    LONG  y;
} POINTL, *PPOINTL;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef unsigned int WPARAM;
typedef long         LPARAM;
typedef void*        HWND;
typedef RECTL        RECT;

#endif // _WINDEF_
#endif // NCT_VERSION
#endif // __ASCOMPAT__
