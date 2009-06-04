#ifndef CVIDEO_FRAME_H
#define CVIDEO_FRAME_H

#include "CFrame.h"
#include "pixel_formats.h"

class CVideoFrame : public CFrame
{
private:
    
    ULONG m_ulWidth,
          m_ulHeight;

    PixFmt m_PixelFormat;

public:
    
    CVideoFrame(BYTE  *_pData, 
                ULONG  _ulDatalen, 
                ULONG  _ulTimestamp, 
                ULONG  _ulWidth, 
                ULONG  _ulHeight, 
                PixFmt _pixFmt) : CFrame(VIDEO, _pData, _ulDatalen, _ulTimestamp)
    {
        m_ulWidth     = _ulWidth; 
        m_ulHeight    = _ulHeight;	
        m_PixelFormat = _pixFmt;
    }
	
    CVideoFrame() : CFrame(VIDEO)
    {
        m_ulWidth     = 0; 
        m_ulHeight    = 0;	
        m_PixelFormat = NO_PIX_FMT;  
    }

    virtual ~CVideoFrame() {}

    ULONG GetFrameDimensions(ULONG *_pWidth, ULONG *_pHeight);
    ULONG GetPixelFormat(PixFmt *_pPixelFormat);

    void  SetVideoFormat(ULONG  _ulWidth, ULONG _ulHeight, PixFmt _pixFmt)
    {
	    m_ulWidth     = _ulWidth;
        m_ulHeight    = _ulHeight;
        m_PixelFormat = _pixFmt;
    }

    ULONG SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp, ULONG  _ulWidth, ULONG _ulHeight, PixFmt _pixFmt);
};

#endif
