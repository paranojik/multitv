#include "compat.h"
#include "CFrame.h"
#include "COutputBuffer.h"
#include "global_error.h"
#include "media_utilities.h"
#include "log.h"


COutputBuffer::~COutputBuffer()
{
    CFrame *pFirst;

    while(m_pOutputFrameList.Size())
    {
        pFirst = m_pOutputFrameList.First();

        if (pFirst)
            delete pFirst;

        m_pOutputFrameList.PopFront();
    }
}

ULONG COutputBuffer::DropNextFrame()
{
    ULONG ret = RET_OK;
    
    if (m_pOutputFrameList.Size())
    {
        if (m_pOutputFrameList[0])
            delete m_pOutputFrameList[0];
        
        m_pOutputFrameList.PopFront();
    }
    else
        ret = RET_OUTPUT_BUFFER_EMPTY;

    return ret;
}

ULONG COutputBuffer::GetFrameTS(ULONG _ulIndex, ULONG *_pulTimestamp)
{    
    ULONG ulListSize = m_pOutputFrameList.Size();
	
    if (ulListSize)
    {
        if (_ulIndex < ulListSize)
            CHECK_ERROR(m_pOutputFrameList[_ulIndex]->GetTimestamp(_pulTimestamp), RET_OK)
        else
            return RET_INVALID_ARG;  
    }
    else
        return RET_OUTPUT_BUFFER_EMPTY;

    return RET_OK;
}

ULONG COutputBuffer::GetFrame(CFrame **_ppFrame)
{
    if (!_ppFrame)
        return RET_INVALID_ARG;

    *_ppFrame = NULL;
    
    if (m_pOutputFrameList.Size())
    {   
        *_ppFrame = m_pOutputFrameList.First();
        m_pOutputFrameList.PopFront();
        return RET_OK;
    }

    return RET_OUTPUT_BUFFER_EMPTY;
}

ULONG COutputBuffer::SetFrame(CFrame *_pFrame)
{
    if (!_pFrame)
        return RET_INVALID_ARG;
    
//    if (m_pOutputFrameList.Size() < m_ulMaxFrameQueueSize )
    {
        m_pOutputFrameList.PushBack(_pFrame);
        
        return RET_OK;
    }
    
    return RET_OUTPUT_BUFFER_FULL;    
}
