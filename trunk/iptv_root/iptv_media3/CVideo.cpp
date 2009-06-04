#include "compat.h"
#include "global_error.h"
#include "Global_functions.h"
#include "CVideoFrame.h"
#include "CVideo.h"
#include "COutputBuffer.h"
#include "media_utilities.h"

CVideo::CVideo(CBufferMedia *_pMediaSource) : IMediaStreaming(_pMediaSource)
{
    if (m_bInit)
    {
        m_bRunWriteVideoThread = FALSE;
        m_MediaSpec    = VIDEO;        m_pCurFrame    = NULL;
        m_pWriteThread = NULL;
        m_ulLastTime   = 0;
        m_bParamsSet   = FALSE;
        m_bWritePaused = FALSE;
    }
}

CVideo::~CVideo()
{
    if (m_pWriteThread)
        delete m_pWriteThread;
}

ULONG CVideo::PrepareFrame()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ret = m_pMediaSource->Decode();

    if (ret == RET_OUTPUT_BUFFER_FULL)
    {	
        Wait(50);
        ret = RET_OK;
    }
    
    return ret;
}

ULONG CVideo::InitWriteThread(ThreadFunction _pThreadFunction, CTransmission *_pCtx)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (m_pWriteThread)
        return RET_ERROR;

    m_WriteSemaph.Wait();
    m_bRunWriteVideoThread = TRUE;

    m_pWriteThread = CreateIThread(_pThreadFunction, _pCtx, TRUE);

    if (!m_pWriteThread)
        return RET_ERROR;

    return RET_OK;
}

ULONG CVideo::StopWriteThread()
{
    if (!m_bInit)    
        return RET_INIT_ERROR;

    if (!m_pWriteThread)
        return RET_ERROR;

    m_WriteSemaph.Signal();
    m_bRunWriteVideoThread = FALSE;

    m_pWriteThread->WaitForMe(INFINITE_TIME);

    return RET_OK;
}

ULONG CVideo::WriteFrame()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (m_pCurFrame)
    {
        if (m_pCurFrame->FrameOk())
        {
            m_FrameReg.WriteData(*m_pCurFrame);
            m_ulFramesProcessed++;
        }

        delete m_pCurFrame;
        m_pCurFrame = NULL;
    }
    else
        return RET_ERROR;

    return RET_OK;
}

ULONG CVideo::PauseWrite()
{
    if (!m_bInit)    
        return RET_INIT_ERROR;
     
    m_bWritePaused = TRUE;

    m_WriteSemaph.Wait();
    m_WriteSemaph.Signal();

    m_bWritePaused = FALSE;

    return RET_OK;
}

ULONG CVideo::ResumeWrite()
{
    if (!m_bInit)    
        return RET_INIT_ERROR;

    m_WriteSemaph.Signal();
    m_WriteSemaph.Wait();

    return RET_OK;
}

typedef CReadData_reg<CVideoFrame> VideoFrame_reg;
VideoFrame_reg *CVideo::GetFrameReg()
{
    if (!m_bInit)
        return NULL;    
    
    return &m_FrameReg;
}   

ULONG CVideo::GetNextFrame(CVideoFrame **_ppFrame)
{
    if (!_ppFrame)
        return RET_INVALID_ARG;

    *_ppFrame = NULL;

    COutputBuffer *pOutputBuffer;
    CHECK_ERROR(m_pMediaSource->GetOutputBuffer(&pOutputBuffer), RET_OK)

    CFrame *pFrame;
    CHECK_ERROR(pOutputBuffer->GetFrame(&pFrame), RET_OK)
	
    ULONG ret = RET_OK;

    if (pFrame->FrameOk())
    {
        *_ppFrame = (CVideoFrame *)pFrame; 
    }
    else
    {
        ret = RET_ERROR;
    }

    return ret;
}


ULONG CVideo::SyncStreaming(ULONG _ulClockTime, IMediaStreaming *_pOtherMedia)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pOtherMedia)
        return RET_INVALID_ARG;

    ULONG ulCurTime,
          ulVideoTS,
          ulAudioTS,
          ulDiffTS,
          ulElapsedAudioTime,
          ulSyncTime,
          ulSyncVideoAhead,
          ulSyncVideoBehind,
          ulWaitTime,
          ulMeanDiffTime,
          ulCycleTime;

    BOOL  bUseAvgFps = FALSE;

    CVideoFrame *pFrame;
    CHECK_ERROR(GetNextFrame(&pFrame), RET_OK)

    pFrame->GetTimestamp(&ulVideoTS);
    m_pCurFrame = (CVideoFrame *) pFrame;

    ulAudioTS = _pOtherMedia->GetMediaTimestamp();

    ulMeanDiffTime = 0;
    ulCycleTime    = 0;
    ulDiffTS	   = 0;
    
    _pOtherMedia->GetTimeSinceLastLoadedFrame(&ulElapsedAudioTime);
    
    if (ulElapsedAudioTime < MAX_WAIT_AUDIO_LOAD_MS)
        ulSyncTime = ulAudioTS;
    else
        ulSyncTime = _ulClockTime;

    if (ulVideoTS > ulSyncTime)
    {    
        ulSyncVideoAhead  = ulVideoTS  - ulSyncTime;
        ulSyncVideoBehind = 0;
    }
    else
    {   
        ulSyncVideoAhead  = 0;
        ulSyncVideoBehind = ulSyncTime - ulVideoTS;
    }

    if (ulSyncVideoAhead)
        ulWaitTime = ulSyncVideoAhead;
    else
        ulWaitTime = 0;

    GetTime(&ulCurTime);

    if (m_bParamsSet)
    {      
        if (ulCurTime >= m_ulLastTime)
            ulCycleTime = ulCurTime - m_ulLastTime;
        else
            return RET_ERROR;

        if (ulVideoTS > m_ulCurTimestamp)
        {
            ulDiffTS = ulVideoTS - m_ulCurTimestamp;

            if (ulDiffTS < MAX_TS_DIFF_MS)
                m_DiffTSList.push_back(ulDiffTS);
                 
            while (m_DiffTSList.size() > 10)
                m_DiffTSList.pop_front();

            if (m_DiffTSList.size() == 10)
                bUseAvgFps = TRUE;
        }
        else
            return RET_ERROR;

        if (bUseAvgFps)
        {
            ULONG ulTotalDiffTime,
                  ulListSize;

            ulListSize = (ULONG)m_DiffTSList.size();
            ulTotalDiffTime = 0;

            for (ULONG i=0; i < ulListSize; i++)
                ulTotalDiffTime += m_DiffTSList[i];
   
            ulMeanDiffTime = ulTotalDiffTime / ulListSize;
            
            if (ulSyncVideoAhead)
                ulWaitTime = ulMeanDiffTime + ulSyncVideoAhead/8;
            else
                ulWaitTime = 0;
        }
    }
    else
        m_bParamsSet = TRUE;

    if (ulWaitTime)
    {    
        if (ulWaitTime > MAX_ASYNC_MS)
            ulWaitTime = MAX_ASYNC_MS;

        Wait(ulWaitTime);

    }

    m_ulLastTime     = ulCurTime;
    m_ulCurTimestamp = ulVideoTS;

    return RET_OK;
}


ULONG CVideoSimposium::SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia)
{
    CVideoFrame *pFrame;
    CHECK_ERROR(GetNextFrame(&pFrame), RET_OK)

    ULONG ulVideoTS;

    pFrame->GetTimestamp(&ulVideoTS);
    m_pCurFrame = (CVideoFrame *) pFrame;

    Wait(35);

    return RET_OK;
}







#if 0
    printf("CVideo.cpp - CVideo::SyncStreaming - Waiting: %lu, ulVideoTS: %lu - ulAudioTS: %lu\n", ulWaitTime, ulVideoTS, ulAudioTS);

    printf("CVideo.cpp - CVideo::SyncStreaming - ulDiffTS: %lu - ulSyncVideoAhead: %lu - ulSyncVideoBehind: %lu\
            - ulMeanDiffTime: %lu - ulCycleTime: %lu - ulWaitTime: %lu - ulClockTime: %lu\
            - ulVideoTS: %lu - ulAudioTS: %lu\n", 
            ulDiffTS, ulSyncVideoAhead, ulSyncVideoBehind, ulMeanDiffTime, ulCycleTime, ulWaitTime, _ulClockTime, ulVideoTS, ulAudioTS);

#ifdef _DEBUG
    char szBuf[200];
    sprintf(szBuf, "ulDiffTS: %lu - ulSyncVideoAhead: %lu - ulSyncVideoBehind: %lu - ulMeanDiffTime: %lu - ulCycleTime: %lu - ulWaitTime: %lu - ulClockTime: %lu - ulVideoTS: %lu - ulAudioTS: %lu", ulDiffTS, ulSyncVideoAhead, ulSyncVideoBehind, ulMeanDiffTime, ulCycleTime, ulWaitTime, _ulClockTime, ulVideoTS, ulAudioTS);
    g_FileLog.SetLog(szBuf);
#endif  

#endif

