#include "compat.h"
#include "CAudioFrame.h"
#include "global_error.h"



ULONG CAudioFrame::GetAudioChannels(ULONG *_pChannels)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    if (!_pChannels)
        return RET_INVALID_ARG;

    *_pChannels = m_ulChannels;

    return RET_OK;
}

ULONG CAudioFrame::GetSampleRate(ULONG *_pSampleRate)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    if (!_pSampleRate)
        return RET_INVALID_ARG;    
    
    *_pSampleRate = m_ulSampleRate;

    return RET_OK;
}

ULONG CAudioFrame::GetSampleSize(ULONG *_pSampleSize)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    if (!_pSampleSize)
        return RET_INVALID_ARG;

    *_pSampleSize = m_ulSampleSize;

    return RET_OK;
}

ULONG CAudioFrame::SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp, ULONG  _ulChannels, ULONG _ulSampleRate, ULONG _ulSampleSize)
{
    ULONG ret;

    if ((ret = CFrame::SetData(_pData, _ulDatalen, _ulTimestamp)) == RET_OK)
    {
        m_ulChannels   = _ulChannels;
        m_ulSampleRate = _ulSampleRate;	
        m_ulSampleSize = _ulSampleSize;
    }
    else
    {
        m_ulChannels   = 0;
        m_ulSampleRate = 0;	
        m_ulSampleSize = 0;
    }

    return ret;
}
