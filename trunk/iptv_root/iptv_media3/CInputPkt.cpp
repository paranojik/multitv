#include "compat.h"
#include "CInputPkt.h"
#include "global_error.h"
#include "Media_const.h"
#include "Global_functions.h"
#include "CPkt.h"


CInputPkt::CInputPkt(ULONG _ulMaxPktQueueSize) :
    m_SemaphPkt()
{ 
    m_ulMaxPktQueueSize = _ulMaxPktQueueSize;
}


CInputPkt::~CInputPkt()
{    
    CPkt *pFirst; 

    while(m_PktQueue.size())
    {
        pFirst = m_PktQueue[0];
        
        if (pFirst)
    	    delete pFirst;
    
        m_PktQueue.pop_front();
    }
}

ULONG CInputPkt::SetPacket(BYTE *_pBufPkt, ULONG _ulBufSize)
{    
    if (!_pBufPkt || !_ulBufSize)
        return RET_INVALID_ARG;

    m_SemaphPkt.Wait();	

    ULONG ret = RET_OK;

    if (m_PktQueue.size() < m_ulMaxPktQueueSize)
    {
//        printf("CInputPkt.cpp - CInputPkt::SetPacket - Allocation new packet, Len: %lu bytes\n", _ulBufSize);

        CPkt *pPkt = new CPkt(_pBufPkt, _ulBufSize);

        if (pPkt)
            m_PktQueue.push_back(pPkt);
        else
            ret = RET_LOW_MEMORY;
    }
    else
        ret = RET_INPUT_BUFFER_FULL;

    m_SemaphPkt.Signal();

    return ret;
}

BOOL CInputPkt::BufferFull()
{
    m_SemaphPkt.Wait();

    BOOL ret = ( m_ulMaxPktQueueSize == m_PktQueue.size() );

    m_SemaphPkt.Signal();

    return ret;
}

ULONG CInputPkt::GetPktCount()
{ 
    m_SemaphPkt.Wait();

    ULONG ulPktCount = (ULONG)m_PktQueue.size();

    m_SemaphPkt.Signal();

    return ulPktCount;
}

ULONG CInputPkt::GetEncodedPkt(CPkt **_pPkt)
{	
    if (!_pPkt)
        return RET_INVALID_ARG;

    m_SemaphPkt.Wait();

    ULONG ret = RET_OK;

    if (m_PktQueue.size())	
    {
        *_pPkt = m_PktQueue[0];
        m_PktQueue.pop_front();
    }
    else
        ret = RET_NO_FRAME_AVAILABLE;

    m_SemaphPkt.Signal();

    return ret;
}

ULONG CInputPkt::EncodedFrameAvailable(BOOL *_pbEncFrameAvailable)
{
    if (!_pbEncFrameAvailable)
        return RET_INVALID_ARG;

    m_SemaphPkt.Wait();

    *_pbEncFrameAvailable = FALSE;

    ULONG ulQueueLen = (ULONG)m_PktQueue.size();

    if (ulQueueLen)
    {	
        if (ulQueueLen >= m_PktQueue[0]->GetSegments())
            *_pbEncFrameAvailable = TRUE;
    }

    m_SemaphPkt.Signal();

    return RET_OK;
}

