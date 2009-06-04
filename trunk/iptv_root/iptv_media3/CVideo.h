#ifndef CVIDEO_H
#define CVIDEO_H

#include "IMediaStreaming.h"

class CTransmission;
class CVideo : public IMediaStreaming
{
protected:

    CVideoFrame * m_pCurFrame;
    CWriteData_reg<CVideoFrame> m_FrameReg;

    IThread *m_pWriteThread;

    std::deque<ULONG> m_DiffTSList;

    ULONG   m_ulLastTime;

    BOOL    m_bParamsSet,
            m_bWritePaused,
            m_bRunWriteVideoThread;

    _SEMAPHORE m_WriteSemaph;

    ULONG GetNextFrame(CVideoFrame **_ppFrame);

    typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

public:

    CVideo(CBufferMedia *_pMediaSource);
    virtual ~CVideo();

    // IMedia implementation
    virtual ULONG PrepareFrame();
    virtual ULONG SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia);

    ULONG WriteFrame();

    ULONG InitWriteThread(ThreadFunction _pThreadFunction, CTransmission *_pCtx);
    ULONG StopWriteThread();

    ULONG PauseWrite();
    ULONG ResumeWrite();

    VideoFrame_reg *GetFrameReg();

    BOOL WritePaused()          {return m_bWritePaused; }
    BOOL RunWriteVideoThread()  {return m_bRunWriteVideoThread; }
};

class CVideoSimposium : public CVideo
{

public:

    CVideoSimposium(CBufferMedia *_pMediaSource) : CVideo(_pMediaSource) {}
    virtual ~CVideoSimposium() {}

    virtual ULONG SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia);

};

#endif

