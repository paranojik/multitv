/* CxReceiveCross.cpp
** -------------
** Application Sharing Receive Module for Cross Platform Client
**
**
** Author : Rômulo Fernandes (romulo.fernandes@gmail.com)
*/
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "CxReceiveCross.h"
#include "ASFrame.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Constructor
CxReceiveCross::CxReceiveCross( void )
{
	m_bNewFrame	  = false;
	m_nBufferSize = -1;
}
// Destructor
CxReceiveCross::~CxReceiveCross( void )
{
	// Base already calls End method
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::Initialize( ISockBuff *pSockBuff, long nMediaID, CxAppSharingListener *pListener,
	                             HWND p_hWnd, int p_BitDepth, RECT *p_Bounds, int p_RefreshTime, int p_CaptureMode )
{
	// Initialize Base Class
    if (!CxReceiveBase::Initialize( pSockBuff, nMediaID, pListener ))
	{
		ASEvent( AS_NOTIFY_RECEIVE_ERROR, NULL, 0 );
		return false;
	}

	ASEvent( AS_NOTIFY_RECEIVE_START, NULL, 0 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::End( void )
{
	
	m_FrameMutex.Wait();
	
	if( !CxReceiveBase::End() ) // Already manages m_flEnd
		return false;

	ASEvent( AS_NOTIFY_RECEIVE_STOP, NULL, 0 );

    return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::CreateFrameBuffer( CXBITMAPINFO *pBitmapInfoHeader )
{
	if( !CxReceiveBase::CreateFrameBuffer(pBitmapInfoHeader) )
		return false;

	m_nBufferSize = ((((pBitmapInfoHeader->bmih.biWidth * pBitmapInfoHeader->bmih.biBitCount) + 31) & ~31) >> 3) * pBitmapInfoHeader->bmih.biHeight;// + sizeof( RecordData );

	if (m_nBufferSize <= 0)
		return false;

	m_pBits = new BYTE[m_nBufferSize];

	// Fill the framebuffer with zeros
	memset(m_pBits,0,m_nBufferSize);
	//for (long i = 0; i < m_nBufferSize; i++)
		//m_pBits[i] = 1;

	// ALLOC m_pBits, using pBitmapInfoHeader information (W, H, Bitdepth, ...)
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::DeleteFrameBuffer( void )
{
	if( !m_pBits )
        return true;

    if( !CxReceiveBase::DeleteFrameBuffer() )
        return false;
	
	delete[] m_pBits;
	m_pBits = NULL;
	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::UpdateScreen( RecordData *pData )
{
	// Request access to framebuffer (m_pBits) by locking the mutex
	m_FrameMutex.Wait();

	if (!CxReceiveBase::UpdateScreen( pData ) )
		return false;

	// Unlock framebuffer access
	m_FrameMutex.Signal();

	// Down the new frame avaiable flag
	m_bNewFrame = true;
	
	// Notify kernel that there is a new frame
	ASEvent( AS_NOTIFY_RECEIVE_NEW_FRAME, NULL, 0 );

	// DEBUG BEGIN
	/*
	static unsigned i;
	if ((i > 10) || (i < 0))
	{
		i = 0;
		ASFrame currFrame;
		this->GetFrame( &currFrame );
		//this->SaveFrameBitmap( &currFrame, "Debug.bmp" );
	}
	i++;
	*/
	// DEBUG END

    return true;
}
///////////////////////////////////////////////////////////////////////////////
long CxReceiveCross::GetFrameFormat()
{
	if (m_MemBitmapInfo.bmih.biBitCount == 32)
		return AS_FRAME_FORMAT_DIB_GBRA;

	if (m_MemBitmapInfo.bmih.biBitCount == 24)
		return AS_FRAME_FORMAT_DIB_BGR;

	if (m_MemBitmapInfo.bmih.biBitCount == 16)
	{
		if ((m_MemBitmapInfo.nRedMask   == 63488) &&	// R - 1111100000000000
			(m_MemBitmapInfo.nGreenMask == 31)    &&	// G - 0000000000011111
			(m_MemBitmapInfo.nBlueMask  == 2016) )		// B - 0000011111100000
			return AS_FRAME_FORMAT_DIB_GBR565;
	}

	return AS_FRAME_FORMAT_NULL;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::FlipFrame( ASFrame* pFrame )
{
	if (!pFrame || !pFrame->m_buffer)
		return false;
	
	unsigned lineSize = (pFrame->m_width * 3);
	BYTE* temp = new BYTE[pFrame->m_bufferlength];
	
	// Copy line by line
	for ( int nLine = 0; nLine < pFrame->m_height; nLine++)
	{
		memcpy( &temp[ nLine * lineSize ],
			    &pFrame->m_buffer[ ( pFrame->m_height - 1 - nLine ) * lineSize ],
			    lineSize );
	}

	// Overwrite the frame buffer pointer
	delete[] pFrame->m_buffer;
	pFrame->m_buffer = temp;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::GetFrame( ASFrame *pFrame, bool pConvertRGB )
{
	int index_in, index_out;

	if (!pFrame)
		return false;

	if (pFrame->m_buffer)
		delete[] pFrame->m_buffer;

	if (!pConvertRGB)
	{	
		// Alloc the receptor
		pFrame->m_bufferlength = m_nBufferSize;
		pFrame->m_buffer = new BYTE[pFrame->m_bufferlength];

		// Request framebuffer access
		m_FrameMutex.Wait();

		// Copy image data
		memcpy( pFrame->m_buffer,
				m_pBits,
				m_nBufferSize );

		// Release framebuffer lock
		m_FrameMutex.Signal();

		// Set package frame format
		pFrame->m_format = this->GetFrameFormat();
	}
	else
	{	
		// Convert the frame into RGB8 (24-bit)
		switch(this->GetFrameFormat())
		{
		//////////////////////////////////////////////////////
		case AS_FRAME_FORMAT_DIB_GBRA:
			{
				// Convert 32-bit frame into 24-bit
				// Alloc the receptor
				pFrame->m_bufferlength = m_MemBitmapInfo.bmih.biWidth * m_MemBitmapInfo.bmih.biHeight * 3;
				pFrame->m_buffer = new BYTE[pFrame->m_bufferlength];

				// Copy process
				index_in  = 0;						// Initialize read index
				index_out = 0;						// Initialize write index
				m_FrameMutex.Wait();				// Request framebuffer access
				while ((index_in + 3 < m_nBufferSize) && (index_out + 2 < pFrame->m_bufferlength))
				{	
					if (m_flEnd)
						return false;
					
					// Copy the frame, discarting the Alpha byte
					//								  m_pBits[index_in + 3]; // A -> Discarted
					pFrame->m_buffer[index_out    ] = m_pBits[index_in + 2]; // R
					pFrame->m_buffer[index_out + 1] = m_pBits[index_in + 0]; // G
					pFrame->m_buffer[index_out + 2] = m_pBits[index_in + 1]; // B
					index_in  += 4;
					index_out += 3;
				}
				m_FrameMutex.Signal();				// Release framebuffer lock

				// Set package frame format
				pFrame->m_format = AS_FRAME_FORMAT_RGB;
				break;
			}
		///////////////////////////////////////////////////////
		// Convert 16-bit frame into 24-bit
		case AS_FRAME_FORMAT_DIB_BGR555:
		case AS_FRAME_FORMAT_DIB_BGR565:
			return false;
		case AS_FRAME_FORMAT_DIB_GBR565:
			{
				// Output frame - 8-bit (Byte) access
				int outStride  = m_MemBitmapInfo.bmih.biWidth * 3;
				int outLines   = m_MemBitmapInfo.bmih.biHeight;
				int outColumns = outStride;
				while (outColumns % 4 != 0) outColumns++;	// 32-bit padding for bytes (8 bits)
				int outSize = outColumns * outLines;

				// Input frame - 16-bit (Word) acess
				int inStride   = m_MemBitmapInfo.bmih.biWidth;
				int inLines    = m_MemBitmapInfo.bmih.biHeight;
				int inColumns  = inStride;
				while (inColumns  % 2 != 0)  inColumns++;	// 32-bit padding for words (16 bits)
				int inSize  = inColumns  * inLines;

				// Alloc the output frame
				pFrame->m_bufferlength = outSize;
				pFrame->m_buffer = new BYTE[pFrame->m_bufferlength];
				WORD *inFrame = (WORD*)m_pBits; // Typecast the framebuffer to access 16-bit words

				// Request framebuffer access
				m_FrameMutex.Wait();

				// Convert to 24-bits and flip while copying
				for(int line = 0; line < inLines; line++)
				{
					for(int column = 0; column < inStride; column++)
					{
						// Check for exit flag
						if (m_flEnd)
						{
							delete[] pFrame->m_buffer;
							pFrame->m_buffer = NULL;
							m_FrameMutex.Signal();
							return false;
						}

						index_in  = (inColumns  * (inLines - line - 1)) +  column     ;
						index_out = (outColumns * line                ) + (column * 3);

						pFrame->m_buffer[index_out    ] = (inFrame[index_in] & m_MemBitmapInfo.nRedMask  ) >> 8; // R
						pFrame->m_buffer[index_out + 1] = (inFrame[index_in] & m_MemBitmapInfo.nGreenMask) << 3; // G
						pFrame->m_buffer[index_out + 2] = (inFrame[index_in] & m_MemBitmapInfo.nBlueMask ) >> 3; // B
					}
				}
				
				// Release framebuffer lock
				m_FrameMutex.Signal();
				// Set package frame format
				pFrame->m_format = AS_FRAME_FORMAT_RGB;
				break;
			}
		///////////////////////////////////////////////////////
		} // End of case
	} // End of if

	// Fill some information about the frame
	pFrame->m_width  = m_MemBitmapInfo.bmih.biWidth;
	pFrame->m_height = m_MemBitmapInfo.bmih.biHeight;

	// Down the new frame avaiable flag
	m_bNewFrame = false;

//	pFrame->SaveFrameBitmap();

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::GetBitmap( ASFrame *pFrame )
{
	BITMAPFILEHEADER fileHeader;

	if (!pFrame)
		return false;

	if (pFrame->m_buffer)
		delete[] pFrame->m_buffer;

	// Fill header data
	fileHeader.bfType		= 0x4D42;
	fileHeader.bfSize		= ((((m_MemBitmapInfo.bmih.biWidth * m_MemBitmapInfo.bmih.biBitCount) + 31) & ~31) >> 3) * m_MemBitmapInfo.bmih.biHeight;
	fileHeader.bfReserved1	= 0;
	fileHeader.bfReserved2	= 0;
	fileHeader.bfOffBits	= sizeof( CXBITMAPINFO ) + sizeof( BITMAPFILEHEADER );

	// Alloc the receptor
	pFrame->m_bufferlength = sizeof(CXBITMAPINFO) + sizeof(BITMAPFILEHEADER) + m_nBufferSize;
	pFrame->m_buffer = new BYTE[pFrame->m_bufferlength];

	// Copy file header
	memcpy( pFrame->m_buffer,
			&fileHeader,
			sizeof(BITMAPFILEHEADER) );

	// Copy bitmap info header
	memcpy( &pFrame->m_buffer[sizeof(BITMAPFILEHEADER)],
			&m_MemBitmapInfo,
			sizeof(CXBITMAPINFO) );

	// Request framebuffer access
	m_FrameMutex.Wait();

	// Copy image data
	memcpy( &pFrame->m_buffer[sizeof(CXBITMAPINFO) + sizeof(BITMAPFILEHEADER)],
			m_pBits,
			m_nBufferSize );

	// Release framebuffer lock
	m_FrameMutex.Signal();

	// Fill some information about the frame
	pFrame->m_format = AS_FRAME_FORMAT_BMP;
	pFrame->m_width	 = m_MemBitmapInfo.bmih.biWidth;
	pFrame->m_height = m_MemBitmapInfo.bmih.biHeight;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::SaveFrameBitmap( char* pFileName )
{
	return SaveBitmapFile( pFileName, m_pBits, NULL );
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::IsNewFrameAvailable()
{
	return m_bNewFrame;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::SendKeyEvent( WPARAM wParam, LPARAM lParam )
{
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::SendKey( long nVK, bool flDown )
{
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CxReceiveCross::SendPointerEvent( int message, WPARAM wParam, LPARAM lParam )
{
	return true;
}
///////////////////////////////////////////////////////////////////////////////

