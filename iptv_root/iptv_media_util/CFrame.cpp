#include "compat.h"
#include "CFrame.h"
#include "global_error.h"

CFrame::CFrame(MediaSpec _mediaSpec, BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp)
{
    SetDefaultValues(_mediaSpec);
	
    if ((m_MediaSpec == VIDEO) || (m_MediaSpec == AUDIO))
        SetData(_pData, _ulDatalen, _ulTimestamp);
}

CFrame::CFrame(CFrame & Frame)
{
    CFrame::CFrame(Frame.m_MediaSpec, Frame.m_pData, Frame.m_ulDatalen, Frame.m_ulTimestamp);
}

CFrame::CFrame(MediaSpec _mediaSpec)
{
    SetDefaultValues(_mediaSpec);
}

CFrame & CFrame::operator=(CFrame & Frame)
{
    if ((this == &Frame) || (m_MediaSpec != Frame.m_MediaSpec))
        return *this;

    m_Id = Frame.m_Id;

    SetData(Frame.m_pData, Frame.m_ulDatalen, Frame.m_ulTimestamp);

    return *this;
}


CFrame::~CFrame()
{
    if (m_pData)
        delete [] m_pData;
}

void CFrame::SetDefaultValues(MediaSpec _mediaSpec)
{
    m_bInit = FALSE;

    if ((_mediaSpec == VIDEO) || (_mediaSpec == AUDIO)) 
        m_MediaSpec = _mediaSpec;
    else
	m_MediaSpec = NONE;

    m_ulDatalen   = 0;
    m_ulBufferLen = 0;
    m_ulTimestamp = 0;
    m_Id = 0;

    m_pData = NULL;
}

ULONG CFrame::GetFrameData(BYTE **_ppData, ULONG *_pDatalen)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_ppData)
        return RET_INVALID_ARG;

    *_ppData = m_pData;

    if (_pDatalen)
        *_pDatalen = m_ulDatalen;

    return RET_OK;
}


ULONG CFrame::GetTimestamp(ULONG *_pulTimestamp)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pulTimestamp)
        return RET_INVALID_ARG;

    *_pulTimestamp = m_ulTimestamp;

    return RET_OK;
}


ULONG CFrame::GetDatalen (ULONG *_pulDatalen)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pulDatalen)
        return RET_INVALID_ARG;

    *_pulDatalen = m_ulDatalen;

    return RET_OK;
}


ULONG CFrame::GetMediaSpec(MediaSpec * _pMediaSpec)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (! _pMediaSpec)
        return RET_INVALID_ARG;

    *_pMediaSpec = m_MediaSpec;

    return RET_OK;
}

ULONG CFrame::SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp)
{
    if (!_pData || !_ulDatalen)
        return RET_INVALID_ARG;

    unsigned ret = RET_OK;

    m_ulDatalen   = _ulDatalen;
    m_ulTimestamp = _ulTimestamp;

    if (m_ulDatalen > m_ulBufferLen)
    {
        if (m_pData)
            delete [] m_pData;

        m_pData = new BYTE[m_ulDatalen];
        m_ulBufferLen = m_ulDatalen;
    }

    if (m_pData)  
    {
        m_bInit = TRUE;
        memcpy(m_pData, _pData, m_ulDatalen);
    } 
    else
    {
        // Andre Cotrim 2008-07-08
        // NOTE: The one-step construction/assignment made here before - "*this = CFrame(m_MediaSpec)" - confused both
        // myself and GCC. This operation is now done in two steps and without immediate assignment. Hope this works.
        CFrame frame(m_MediaSpec);
        *this = frame;
        ret = RET_LOW_MEMORY;
    }

    return ret;
}
