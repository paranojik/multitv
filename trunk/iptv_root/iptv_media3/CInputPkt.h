#ifndef CINPUT_PKT_H
#define CINPUT_PKT_H

#include <deque>
#include "CThreadSafeDeque.h"

class CPkt;
class CInputPkt
{
private:

    std::deque<CPkt *> m_PktQueue;
    _SEMAPHORE m_SemaphPkt;

    ULONG m_ulMaxPktQueueSize;
 
public:

    CInputPkt(ULONG _ulMaxPktQueueSize);
    virtual ~CInputPkt();

    ULONG SetPacket(BYTE *_bufPkt, ULONG _bufSize);
    ULONG GetEncodedPkt(CPkt **_pPkt);
    ULONG EncodedFrameAvailable(BOOL *_pbEncFrameAvailable);
    BOOL  BufferFull();
    ULONG GetPktCount();

};


#endif
