#ifndef ASF_PKT_SINK_H
#define ASF_PKT_SINK_H

#include <deque>
#include "mediapkt.h"
#include "semaphore.h"

class IASFPktReadSink
{
protected:

    bool m_bEof, 
         m_bHeaderSet;

public:

    IASFPktReadSink() : m_bEof(false), 
                        m_bHeaderSet(false) {}
    
    virtual ~IASFPktReadSink() {}

    virtual unsigned GetPktCount() = 0;
    virtual unsigned ReadHeader(BYTE ** _ppHeader, unsigned & _uHeaderLen) = 0;
    virtual unsigned ReadPacket(MediaPkt **_ppPkt, unsigned & _uPktLen) = 0;

    IASFPktReadSink * GetSinkPktBuffer() { return this; }

    bool HeaderSet()  { return m_bHeaderSet; }
    bool ReachedEOF() { return m_bEof;       }
};

class IASFPktWriteSink : public IASFPktReadSink
{
public:

    IASFPktWriteSink() {}
    virtual ~IASFPktWriteSink() {}
 
    virtual unsigned WriteHeader(BYTE * _pHeader, unsigned _uHeaderLen) = 0;
    virtual unsigned WritePacket(MediaPkt *_pPkt, unsigned _uPktLen)    = 0;

    void SetEOF(bool _bEof) { m_bEof = _bEof; }
};

class ASFPktSink : public IASFPktWriteSink
{
private:

    std::deque<MediaPkt *> m_PktQueueSink;

    BYTE   * m_pHeaderBuf;
    unsigned m_uHeaderLen;

    _SEMAPHORE m_SinkSemaph;

public:

    ASFPktSink() : m_pHeaderBuf(NULL),
                   m_uHeaderLen(0) {}
    
    virtual ~ASFPktSink();

    virtual unsigned ReadHeader(BYTE ** _ppHeader, unsigned & _uHeaderLen);
    virtual unsigned ReadPacket(MediaPkt **_ppPkt, unsigned & _uPktLen);

    virtual unsigned WriteHeader(BYTE * _pHeader, unsigned _uHeaderLen);
    virtual unsigned WritePacket(MediaPkt *_pPkt,  unsigned _uPktLen);

    virtual unsigned GetPktCount() { return m_PktQueueSink.size(); }
};

#endif
