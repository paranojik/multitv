#include "compat.h"
#include "CVideoFrame.h"
#include "global_error.h"


ULONG CVideoFrame::GetFrameDimensions(ULONG *_pWidth, ULONG *_pHeight)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    if (!_pWidth || !_pHeight)
	    return RET_INVALID_ARG;

    *_pWidth  = m_ulWidth;
    *_pHeight = m_ulHeight;

    return RET_OK;
}


ULONG CVideoFrame::GetPixelFormat(PixFmt *_pPixelFormat)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    if (!_pPixelFormat)
	    return RET_INVALID_ARG;

    *_pPixelFormat = m_PixelFormat;

    return RET_OK;
}
ULONG CVideoFrame::SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp, ULONG  _ulWidth, ULONG _ulHeight, PixFmt _pixFmt)
{
    ULONG ret;

    if ((ret = CFrame::SetData(_pData, _ulDatalen, _ulTimestamp)) == RET_OK)
    {
        m_ulWidth     = _ulWidth;
        m_ulHeight    = _ulHeight;
        m_PixelFormat = _pixFmt;
    }
    else
    {
        m_ulWidth     = 0;
        m_ulHeight    = 0;
        m_PixelFormat = NO_PIX_FMT;
    }

    return ret;
}
