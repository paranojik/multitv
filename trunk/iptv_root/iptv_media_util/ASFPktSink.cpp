#include "compat.h"
#include "ASFPktSink.h"
#include "global_error.h"
#include "media_utilities.h"

ASFPktSink::~ASFPktSink()
{
    m_SinkSemaph.Wait();

    if (m_pHeaderBuf)
        delete m_pHeaderBuf;

    while (!m_PktQueueSink.empty())
    {
        MediaPkt *pPkt = m_PktQueueSink[0];
        delete pPkt;

        m_PktQueueSink.pop_front();
    }

    m_SinkSemaph.Signal();
}

unsigned ASFPktSink::ReadHeader(BYTE ** _ppHeader, unsigned & _uHeaderLen)
{
    if (!_ppHeader)
        return RET_INVALID_ARG;

    m_SinkSemaph.Wait();

    *_ppHeader  = m_pHeaderBuf;
    _uHeaderLen = m_uHeaderLen;

    m_SinkSemaph.Signal();

    return RET_OK;
}

unsigned ASFPktSink::ReadPacket(MediaPkt **_ppPkt, unsigned & _uPktLen)
{
    _uPktLen = 0;

    if (!_ppPkt)
        return RET_INVALID_ARG;

    unsigned ret = RET_OK;

    m_SinkSemaph.Wait();

    if (!m_PktQueueSink.empty())
    {
        *_ppPkt  = m_PktQueueSink[0];

    //    IRM_PKT& pkt = * (IRM_PKT*) m_PktQueueSink[0];
    //    printf("ASFPktSink.cpp - ASFPktSink::ReadPacket - pkt.payload first byte %x\n", *pkt.payload);

        _uPktLen = (*_ppPkt)->datalen + sizeof(MediaPkt);
 
        m_PktQueueSink.pop_front();
    }
    else
    {
        *_ppPkt = NULL;
        ret = RET_INPUT_BUFFER_EMPTY;
    }

    m_SinkSemaph.Signal();

    return ret;
}

unsigned ASFPktSink::WriteHeader(BYTE * _pHeader, unsigned _uHeaderLen)
{
    if (!_pHeader)
        return RET_INVALID_ARG;

    unsigned ret = RET_OK;

    m_SinkSemaph.Wait();

    if (!m_bHeaderSet)
    {
        m_pHeaderBuf = new BYTE[_uHeaderLen];
        
        if (m_pHeaderBuf)
        {    
            memcpy(m_pHeaderBuf, _pHeader, _uHeaderLen);
            m_uHeaderLen = _uHeaderLen;

            m_bHeaderSet = true;
        }
        else
            ret = RET_LOW_MEMORY;
    }
    else
        ret = RET_ERROR;

    m_SinkSemaph.Signal();
    
    return ret;
}

unsigned ASFPktSink::WritePacket(MediaPkt *_pPkt, unsigned _uPktLen)
{
    if (!_pPkt || !_uPktLen)
        return RET_INVALID_ARG; 
    
    unsigned ret = RET_OK;

    m_SinkSemaph.Wait();

    BYTE *pBuf = new BYTE[_uPktLen];

    if (pBuf)
    {
        memcpy(pBuf, _pPkt, _uPktLen);
        m_PktQueueSink.push_back((MediaPkt *) pBuf);
    }
    else
        ret = RET_LOW_MEMORY;

    m_SinkSemaph.Signal();

    return ret;
}
