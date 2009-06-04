/*
**
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "ASCompat.h"
#include "ASFrame.h"
#include "ASFlags.h"
#include "CxAppSharing.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ASMODULE_API_CROSS ASFrame::ASFrame()
{
	m_width			= -1;
	m_height		= -1;
	m_bufferlength	= -1;
	m_buffer		= NULL;
	m_format		= AS_FRAME_FORMAT_NULL;
}
/*
ASMODULE_API_CROSS ASFrame::~ASFrame()
{
	if (m_buffer)
		delete[] m_buffer;
}
*/
///////////////////////////////////////////////////////////////////////////////
bool ASMODULE_API_CROSS ASFrame::SaveFrameBitmap( char* pFileName )
{
	BITMAPFILEHEADER fileHeader;
	CXBITMAPINFO infoHeader;
	FILE *fd;
	unsigned long nSize;

	// Fill all the necessary data for the bitmap header
	switch (m_format)
	{
	case AS_FRAME_FORMAT_DIB_GBR565:
		infoHeader.nRedMask				= 63488; // 1111100000000000
		infoHeader.nGreenMask			= 31;	 // 0000000000011111
		infoHeader.nBlueMask			= 2016;	 // 0000011111100000
		infoHeader.bmih.biBitCount		= 16;
		infoHeader.bmih.biCompression	= BI_BITFIELDS;
		break;
	///////////////////////////////////////////////////
	case AS_FRAME_FORMAT_DIB_BGR:
		infoHeader.nRedMask				= 0;
		infoHeader.nGreenMask			= 0;
		infoHeader.nBlueMask			= 0;
		infoHeader.bmih.biBitCount		= 24;
		infoHeader.bmih.biCompression	= BI_RGB;
		break;
	///////////////////////////////////////////////////
	case AS_FRAME_FORMAT_BMP:
		fd = fopen( pFileName, "wb" );
		if( !fd )
			return false;
		
		fwrite( m_buffer, m_bufferlength, 1, fd );
		return true;
	///////////////////////////////////////////////////
	default:
		return false;
	}

	nSize = ((((m_width * infoHeader.bmih.biBitCount) + 31) & ~31) >> 3) * m_height;

	infoHeader.bmih.biWidth			= m_width;
	infoHeader.bmih.biHeight		= m_height;
	infoHeader.bmih.biPlanes		= 1;
	infoHeader.bmih.biSizeImage		= nSize;
	infoHeader.bmih.biXPelsPerMeter	= 0;
	infoHeader.bmih.biYPelsPerMeter	= 0;
	infoHeader.bmih.biClrUsed		= 0;
	infoHeader.bmih.biClrImportant	= 0;
	infoHeader.bmih.biSize			= 40;
	fileHeader.bfType				= 0x4D42;
	fileHeader.bfSize				= nSize;
	fileHeader.bfReserved1			= 0;
	fileHeader.bfReserved2			= 0;
	fileHeader.bfOffBits			= sizeof( CXBITMAPINFO ) + sizeof( BITMAPFILEHEADER );

	// Write the file
	fd = fopen( pFileName, "wb" );

    if( !fd )
        return false;

    fwrite( &fileHeader, sizeof( fileHeader ), 1, fd );
    fwrite( &infoHeader, sizeof( infoHeader ), 1, fd );
	fwrite( m_buffer, m_bufferlength, 1, fd );
    fclose( fd );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
