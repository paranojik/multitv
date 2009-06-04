#include "compat.h"
#include "CBufferId.h"
#include "Global_functions.h"
#include "log.h"
#include "Media_const.h"
#include "CClockSync.h"
#include "CBufferMedia.h"
#include "COutputBuffer.h"
#include "mediapkt.h"
#include "media_utilities.h"
#include "CTransmission.h"

CBufferId::CBufferId(ULONG _id, _SEMAPHORE *_pCodecSemaph) : 
    m_ClockSync(),
    m_JitterCtrl()
{
    m_Id = _id;

    m_bInit             = FALSE;	
    m_bFirstPkt	        = TRUE;
    m_bAudioMute        = FALSE;
    m_pReadPktThread    = NULL;     	
    m_bRunReadPktThread = FALSE;

    m_ulRefTS           = 0;
    m_ulPktRcv          = 0;
    m_ulLastVideoSeq    = 0;
    m_ulLastAudioSeq    = 0;
    m_ulLastVideoSubSeq = 0;
    m_ulLastAudioSubSeq = 0;

    m_pTransmission = NULL;

    m_pCodecSemaph  = _pCodecSemaph;

    if (m_pCodecSemaph)  
        m_bInit = TRUE;
}



CBufferId::~CBufferId()
{    
    if (m_pReadPktThread)
        delete m_pReadPktThread;

    CBufferMedia *pFirst;
	
    while(m_pBufferMediaList.Size())
    {
        pFirst = m_pBufferMediaList.First();
		
        if (pFirst)
            delete pFirst;

        m_pBufferMediaList.PopFront();
    }
}


ULONG CBufferId::StartPktProcessing(ThreadFunction _pFunc, void *_pCtx)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ret = RET_OK;

    if (m_JitterCtrl.Initialize(MAX_INPUT_PKT_QUEUE, MAX_PKT_SIZE, MAX_PKT_SIZE, this))
    {
        if (m_JitterCtrl.Start())
        {          
            // create a thread that write packets to jitter buffer
            if (m_JitterCtrl.Resume())
            {             
                // create a thread that read packets from jitter buffer
                m_bRunReadPktThread = TRUE;
                m_pReadPktThread = CreateIThread(_pFunc, _pCtx, TRUE);

                if (!m_pReadPktThread)
                {
                    ret = RET_ERROR;
                    m_bRunReadPktThread = FALSE;
                }
            }
            else
                ret = RET_ERROR;
        }
        else
            ret = RET_ERROR;
    }
    else
        ret = RET_ERROR;
    
    return ret;
}

ULONG CBufferId::FinishPktProcessing()
{
    if (!m_bInit)
        return RET_INIT_ERROR;
    
    m_bRunReadPktThread = FALSE;
    m_JitterCtrl.Stop();

    m_pReadPktThread->WaitForMe(INFINITE_TIME);

    return RET_OK;
}


ULONG CBufferId::OutputBufferFull(MediaSpec _mediaSpec, BOOL *_pBufferFull)
{
    if (!m_bInit)    
        return RET_INIT_ERROR;

    if (!_pBufferFull)
        return RET_INVALID_ARG;

    *_pBufferFull = FALSE;

    ULONG ulListSize,
		  ret;
	
    ret = RET_INVALID_MEDIA_SPEC;	
    ulListSize = m_pBufferMediaList.Size();
    
    for(ULONG i=0; i < ulListSize; i++)
    {	
        if (!m_pBufferMediaList[i])
            return RET_ERROR;

        if (m_pBufferMediaList[i]->GetMediaSpec() == _mediaSpec)
        {
            ret = RET_OK;

            COutputBuffer *pOutputBuffer;
            CHECK_ERROR(m_pBufferMediaList[i]->GetOutputBuffer(&pOutputBuffer), RET_OK)
	        
            *_pBufferFull = pOutputBuffer->BufferFull();
            break;
        }	
    }

    return ret;   
}

ULONG CBufferId::PktBufferFull(MediaSpec _mediaSpec, BOOL *_bPktBufFull)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_ARG;

    if (!_bPktBufFull)
        return RET_INVALID_ARG;

    CBufferMedia *pBufferMedia;
    CHECK_ERROR(GetBufferMedia(&pBufferMedia, _mediaSpec), RET_OK)

    *_bPktBufFull = pBufferMedia->InputBufferFull();

    return RET_OK;
}


ULONG CBufferId::CreateBufferMedia(MediaSpec _mediaSpec)
{
    if (!m_bInit)
        return RET_INIT_ERROR;
    
    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_ARG;

    
    ULONG ulMaxOutputMediaQueue,
          ulMaxInputPktQueue;
    
    ulMaxInputPktQueue = MAX_INPUT_PKT_QUEUE;

    if (_mediaSpec == VIDEO)
        ulMaxOutputMediaQueue = MAX_OUTPUT_VIDEO_QUEUE;
    else
        ulMaxOutputMediaQueue = MAX_OUTPUT_AUDIO_QUEUE;

    ULONG ulListSize,
          ret;

    ret = RET_OK;
    ulListSize = m_pBufferMediaList.Size();
	
    for(ULONG i=0; i < ulListSize; i++)
    {
        if (m_pBufferMediaList[i]->GetMediaSpec() == _mediaSpec)
        {    
            ret = RET_BUFFER_MEDIA_ALREADY_CREATED;            
            break;
        }
    }
    
    if (ret == RET_OK)
    {
        CBufferMedia * pBufferMedia;

        if (_mediaSpec == VIDEO)
        {      
            pBufferMedia = new CBufferVideo(ulMaxInputPktQueue, ulMaxOutputMediaQueue, m_pCodecSemaph);        }
        else
        {
            pBufferMedia = new CBufferAudio(ulMaxInputPktQueue, ulMaxOutputMediaQueue, m_pCodecSemaph);
        }
  
        if (pBufferMedia)   
            m_pBufferMediaList.PushBack(pBufferMedia);
        else
            ret = RET_LOW_MEMORY;           
    }
    
    return ret;
}

ULONG CBufferId::GetBufferMedia(CBufferMedia **_ppBufferMedia, MediaSpec _mediaSpec)
{
    if (!m_bInit)    
        return RET_INIT_ERROR;

    if (!_ppBufferMedia)
        return RET_INVALID_ARG;

    *_ppBufferMedia = NULL;

    ULONG ulListSize, 
		  ret;
	
    ret = RET_INVALID_MEDIA_SPEC;
    ulListSize = m_pBufferMediaList.Size();

    for(ULONG i=0; i < ulListSize; i++)
    {
        if (m_pBufferMediaList[i]->GetMediaSpec() == _mediaSpec) 
        {    
	    ret = RET_OK;
	    *_ppBufferMedia = m_pBufferMediaList[i];
            
            break;
        }
    }

    return ret;    
}

ULONG CBufferId::CheckStartConditions(MediaSpec _mediaSpec, BOOL *_pbIdReadyToTransmit)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pbIdReadyToTransmit)
        return RET_INVALID_ARG;

    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_ARG;

    ULONG ulMinOutputMediaStartConf,
          ulListSize;

    if (_mediaSpec == VIDEO)
        ulMinOutputMediaStartConf = MIN_OUTPUT_VIDEO_START_CONF;
    else
        ulMinOutputMediaStartConf = MIN_OUTPUT_AUDIO_START_CONF;
	
    *_pbIdReadyToTransmit = FALSE;

    ulListSize = m_pBufferMediaList.Size();
    MediaSpec mediaSpec;

    for(ULONG i=0; i < ulListSize; i++)
    {
        mediaSpec = m_pBufferMediaList[i]->GetMediaSpec();

        if (mediaSpec == _mediaSpec)
        {
            COutputBuffer *pOutputBuffer;
            CHECK_ERROR(m_pBufferMediaList[i]->GetOutputBuffer(&pOutputBuffer), RET_OK)
		
            if (pOutputBuffer->GetFramesCount() >= ulMinOutputMediaStartConf)
                *_pbIdReadyToTransmit = TRUE;

            break;
        }
    }
    
    return RET_OK;
}

ULONG CBufferId::GetJitterPktCount(ULONG *_pulJitterPktCount)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pulJitterPktCount)
        return RET_INVALID_ARG;

    *_pulJitterPktCount = m_JitterCtrl.GetnPacketsLeft();

    return RET_OK;
}

ULONG CBufferId::GetPacket(BYTE *_pBuf, ULONG *_pulBufLen, MediaSpec *_pMediaSpec)
{  	
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pBuf || !_pulBufLen || !_pMediaSpec)
        return RET_INVALID_ARG;

    ULONG ret = RET_OK;

    if (m_JitterCtrl.GetPacket((LPSTR)_pBuf, *_pulBufLen, _pulBufLen, TRUE, FALSE))
    {
        MediaPktExt *pMediaPktExt = (MediaPktExt *)_pBuf;
        ::GetMediaSpec(pMediaPktExt->type, _pMediaSpec);
    
        if (m_bFirstPkt)
        {
            m_ulRefTS = pMediaPktExt->dwMsTimestamp;
            m_bFirstPkt = FALSE;
        }

        if (pMediaPktExt->dwMsTimestamp > m_ulRefTS)
            pMediaPktExt->dwMsTimestamp -= m_ulRefTS;
        else
            pMediaPktExt->dwMsTimestamp = 0;
    }
    else
        ret = RET_ERROR;

    return ret;
}

ULONG CBufferId::ResetMediaCtx(MediaSpec _mediaSpec)
{
    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_ARG;
   
    m_JitterCtrl.Flush(TRUE);
            
    CBufferMedia *pBufferMedia;
    GetBufferMedia(&pBufferMedia, _mediaSpec);
            
    pBufferMedia->ResetCodec();

    return RET_OK;
}

ULONG CBufferId::CheckBoundaryConditions(BYTE *_pData, ULONG _ulDatalen)
{
#if 0

    MediaSpec mediaSpec;
    MediaPktExt *pMediaPktExt = (MediaPktExt *)_pData;
    GetMediaSpec(pMediaPktExt->type, &mediaSpec);

    static ULONG ulLastTime;
    static BOOL bFirstTime = TRUE;
    ULONG ulCurTime;

    if (bFirstTime)
    {
        bFirstTime = FALSE;
        GetTime(&ulLastTime);
    }

    GetTime(&ulCurTime);
    if (ulCurTime - ulLastTime > 2000)
    {    
        ResetMediaCtx(mediaSpec);
        GetTime(&ulLastTime);
    }
    return RET_OK;
    
#endif

    ULONG ulSubSeq,
          ulSeq,
          ulMaxSubSeqMediaDelay,
          ulLastMediaSubSeq,
          ulLastMediaSeq;  
    
    MediaSpec mediaSpec;
    MediaPktExt *pMediaPktExt = (MediaPktExt *)_pData;
    
    ulSeq    = pMediaPktExt->seq;
    ulSubSeq = pMediaPktExt->ulSubSeq;

    GetMediaSpec(pMediaPktExt->type, &mediaSpec);

    if ((mediaSpec == AUDIO) && m_bAudioMute)
        return RET_FILTER_AUDIO_PKT;

    if (((mediaSpec == VIDEO) && (m_ulLastVideoSubSeq)) || ((mediaSpec == AUDIO) && (m_ulLastAudioSubSeq)))
    {
        if (mediaSpec == VIDEO)
        {    
            ulMaxSubSeqMediaDelay = MAX_VIDEO_SUBSEQ_DELAY;
            
            ulLastMediaSubSeq     = m_ulLastVideoSubSeq;
            ulLastMediaSeq        = m_ulLastVideoSeq;
        }
        else
        {    
            ulMaxSubSeqMediaDelay = MAX_AUDIO_SUBSEQ_DELAY;
            
            ulLastMediaSubSeq     = m_ulLastAudioSubSeq;
            ulLastMediaSeq        = m_ulLastAudioSeq;
        }
        
        if (ulSubSeq > ulLastMediaSubSeq)
        {
            if ((ulSubSeq - ulLastMediaSubSeq) > ulMaxSubSeqMediaDelay)
                CHECK_ERROR(ResetMediaCtx(mediaSpec), RET_OK)
        }
    }

    if (mediaSpec == VIDEO)
    {   
        m_ulLastVideoSeq    = ulSeq;
        m_ulLastVideoSubSeq = ulSubSeq;
    }
    else
    {    
        m_ulLastAudioSeq    = ulSeq;
        m_ulLastAudioSubSeq = ulSubSeq; 
    }

    return RET_OK;
}

BOOL CBufferId::OnReadPacket()
{
    NotifyKernel(MN_READ_PACKET);

    return TRUE;
}

void CBufferId::OnAbort()
{
    if (m_pTransmission)
    {
        if (m_pTransmission->DemuxerRunning())
            m_pTransmission->StopDemuxThread(); 
    }
}

BOOL CBufferId::ClockStarted()
{
    if (!m_bInit)
        return FALSE;
    
    return m_ClockSync.ClockStarted();
}

ULONG CBufferId::SetClockRef()
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    CHECK_ERROR(m_ClockSync.SetClockRef(), RET_OK)
    
    return RET_OK;
}

ULONG CBufferId::UpdateTime(ULONG *_pulCurTime)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    CHECK_ERROR(m_ClockSync.UpdateTime(_pulCurTime), RET_OK)
    
    return RET_OK;
}

ULONG CBufferId::SetPacket(BYTE *_pData, ULONG _ulDatalen)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pData || !_ulDatalen)
        return RET_INVALID_ARG;

    ULONG ret;

    if ((ret = CheckBoundaryConditions(_pData, _ulDatalen)) == RET_OK)
    {
        ULONG ulSeq = ((MediaPkt *) _pData)->seq;
        m_JitterCtrl.Insert(ulSeq, (LPCSTR)_pData, _ulDatalen);
        m_ulPktRcv++;
    }

    return ret;
}

ULONG CBufferId::CheckForBuffering(MediaSpec _mediaSpec, BOOL & _bBufferingNeeded)
{   
    if (!m_bInit)
        return RET_INIT_ERROR;

    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_ARG;

    _bBufferingNeeded = FALSE;

    CBufferMedia *pBufferMedia;
    GetBufferMedia(&pBufferMedia, _mediaSpec); 
    
    ULONG ulCount;    
    pBufferMedia->GetPktCount(&ulCount);
            
    if (!ulCount)
    {    
        pBufferMedia->GetFramesCount(&ulCount);
                
        if (!ulCount)
            _bBufferingNeeded = TRUE;
    }

    return RET_OK;
}

ULONG CBufferId::NotifyKernel(MediaCodeNotify _code)
{
    if (!m_bInit)
        return RET_INIT_ERROR;
    
    MediaParam mediaParam;

    mediaParam.code = _code;
    mediaParam.id   = m_Id;

    ULONG ret = RET_OK;

    switch(_code)
    {
        case MN_READ_PACKET:
            m_pNotify->OnMediaNotify(&mediaParam);
        break;

        default:
            return RET_INVALID_ARG;
    }

     return ret;
}
