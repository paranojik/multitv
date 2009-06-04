#ifndef COUTPUT_BUFFER_H
#define COUTPUT_BUFFER_H

#include "CThreadSafeDeque.h"

class CFrame;
class COutputBuffer
{
private:

    ULONG m_ulMaxFrameQueueSize;
    CThreadSafeDeque<CFrame *> m_pOutputFrameList;

public:

    COutputBuffer(ULONG _ulMaxFrameQueue) { m_ulMaxFrameQueueSize = _ulMaxFrameQueue; }
    virtual ~COutputBuffer();

    ULONG GetFrame(CFrame **_ppFrame);
    ULONG SetFrame(CFrame *_pFrame);
    ULONG GetFrameTS(ULONG _ulIndex, ULONG *_pulTimestamp);
    ULONG DropNextFrame();


    BOOL BufferFull()       {return (m_ulMaxFrameQueueSize == m_pOutputFrameList.Size() ); }
    ULONG GetMaxQueueSize() {return  m_ulMaxFrameQueueSize;		}
    ULONG GetFramesCount()  {return  m_pOutputFrameList.Size();	}
};

#endif
