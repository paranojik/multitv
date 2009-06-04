#include "compat.h"
#include "CTransmission.h"
#include "global_error.h"
#include "Global_functions.h"
#include "CBufferId.h"
#include "CVideo.h"
#include "CAudio.h"
#include "media_utilities.h"
#include "log.h"

CTransmission::CTransmission(ULONG _id, IMediaKernelNotify *_pNotify, IMediaApi *_pAudioApi, _SEMAPHORE *_pCodecSemaph) : 
    m_BufferId(_id, _pCodecSemaph),
    m_ulError(RET_INVALID_ARG),
    m_bInit(FALSE),
    m_bMediaSet(FALSE),
    m_bDemuxNeeded(FALSE),
    m_bBuffering(FALSE),
    m_bRunBufferThread(FALSE),
    m_bRunDemuxThread(FALSE),
    m_bWaitingRebufferNotify(FALSE),
    m_pBufferThread(NULL),
    m_pDemuxThread(NULL),
    m_pDemuxer(NULL),
    m_pAudioApi(_pAudioApi),
    m_ulLastAudioAlive(0),
    m_ulLastVideoAlive(0),
    m_ulBufferingPercent(0)
{
    if (m_BufferId.Ok() && _pNotify && m_pAudioApi)
    {
        m_ulError = RET_OK;

        GetTime(&m_ulLastBuffering);

        SetKernelNotify(_pNotify);
        m_BufferId.SetKernelNotify(_pNotify);

        m_BufferId.CreateBufferMedia(VIDEO);
        m_BufferId.CreateBufferMedia(AUDIO);

        m_bInit = TRUE;
    }
    else
    {   
        NotifyKernel(MN_ERROR);
    }
}

CTransmission::~CTransmission()
{
    if (m_pVideo)
	delete m_pVideo;

    if (m_pAudio)
	delete m_pAudio;

    if (m_pDemuxer)
        delete m_pDemuxer;

    if (m_pDemuxThread)
        delete m_pDemuxThread;

    if (m_pBufferThread)
        delete m_pBufferThread;
}

ULONG CTransmission::SetDemuxStreamHeader(BYTE *_pHeader, ULONG _uHeaderLen) 
{
    m_bDemuxNeeded = TRUE;

    ULONG ret;

    if (!m_pDemuxer)
    {
        m_pDemuxer = new ASFDemuxer();
    }

    if (m_pDemuxer)
    {
        ret = m_pDemuxer->SetHeader(_pHeader, _uHeaderLen);
    }
    else
    {
        ret = RET_LOW_MEMORY;
    }
    
    return ret;
}

ULONG CTransmission::InitMedia()
{
    ULONG ret = RET_OK;

    if (m_pAudioApi)
    {    
        AllocMedia(AUDIO, m_pAudioApi);
    }

    if (m_bDemuxNeeded)
    {
        AllocMedia(VIDEO, NULL, SIMPOSIUM);          
    }
    else
    {
        AllocMedia(VIDEO);
    }

    if (m_pVideo && m_pAudio)       
    {     
        m_bMediaSet = TRUE;
    }
    else
    {
        ret = RET_ERROR;
    }

    return ret;
}

ULONG CTransmission::SetPacket(BYTE *_pData, ULONG _ulDatalen)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!m_bMediaSet)
    {
        CHECK_ERROR(InitMedia(), RET_OK)
    }
    
    ULONG ret;

    if (m_bDemuxNeeded)
    {
        // antes inicializar a thread de leitura e setar o semaphoro
        if (m_pDemuxer && m_pDemuxer->HeaderSet())
        {
            if (!m_bRunDemuxThread)
            {
                m_BufferId.SetTransmission(this);
                ret = StartDemuxThread();
            }

            if (m_bRunDemuxThread)
            {
                ret = m_pDemuxer->SetPacket(_pData, _ulDatalen, sizeof(MediaPkt));
            }
        }
        else
        {
            ret = RET_ERROR;
        }
    }
    else
    {
        ret = m_BufferId.SetPacket(_pData, _ulDatalen);
    }

    return ret;
}

ULONG CTransmission::StartDemuxThread()
{
    ULONG ret = RET_OK;

    if ( m_pDemuxer && m_pDemuxer->HeaderSet() )
    {
        m_pDemuxer->SetMutexOwner();
        m_pDemuxThread = CreateIThread(_DemuxStream, this, TRUE);

        if (m_pDemuxThread)
            m_bRunDemuxThread = TRUE;
        else
            ret = RET_ERROR;
    }
    else
    {
        ret = RET_ERROR;
    }

    return ret;
}

ULONG CTransmission::StopDemuxThread()
{
    if (m_pDemuxer && m_bRunDemuxThread)
    {
        m_bRunDemuxThread = FALSE;

        m_pDemuxer->ReleaseMutexOwner();
        m_pDemuxThread->WaitForMe(INFINITE_TIME);
    }

    return RET_OK;
}

int CTransmission::_ReadPkt(IThread *_pthread, void * _pCtx)
{	    
    CTransmission * pCtx = (CTransmission *) _pCtx;

#ifdef _DEBUG	
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_ReadPkt - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    MediaSpec mediaSpec;
    BYTE  buf[MAX_PKT_SIZE];
    ULONG ulBufLen;

    pCtx->StartBufferThread();
	
    do
    {    
        if (!pCtx->m_bBuffering)
        {
            if ( pCtx->NeedBuffering() )           
                pCtx->NotifyRebuffer();  
        }

        ulBufLen = MAX_PKT_SIZE;
        
        if (pCtx->m_BufferId.GetPacket(buf, &ulBufLen, &mediaSpec) == RET_OK)
        {
            IMediaStreaming *pMedia;

            if (mediaSpec == VIDEO)     
                pMedia = pCtx->m_pVideo;
            else     
                pMedia = pCtx->m_pAudio;

            while ( pMedia->InputBufferFull() )
            {   
                Wait(100);
            }

            pMedia->SetPacket(buf, ulBufLen, mediaSpec);

            if ( pMedia->MediaPlaying() )
            {
                if (pMedia->WaitingData() && pMedia->EncodedFrameReady())
                    pMedia->StopWaitingData();
            }
            else
            {
                if (mediaSpec == VIDEO)
                    pMedia->Play(_PlayVideo, pCtx);
                else
                    pMedia->Play(_PlayAudio, pCtx);
            }
        }
        else
        {   
            Wait(10);
        }

    }while( pCtx->m_BufferId.RunReadPktThread() );

    if (pCtx->m_pVideo->MediaPlaying())
        pCtx->m_pVideo->Stop();
    
    if (pCtx->m_pAudio->MediaPlaying())
        pCtx->m_pAudio->Stop();

    if (pCtx->m_bRunBufferThread)
        pCtx->StopBufferThread();

#ifdef _DEBUG 
    sprintf(szBuf, "Leaving CTransmission::_ReadPkt - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif
       
    return RET_OK;
}

int CTransmission::_DemuxStream(IThread *_pThread, void *_pCtx)
{
    CTransmission *pCtx = (CTransmission *)_pCtx;

#ifdef _DEBUG	
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_DemuxStream - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    if (pCtx->m_pDemuxer->Init() == RET_OK)
    {
        BYTE *pData;
        ULONG uDatalen;
        MediaSpec mediaSpec;

        bool bFirstAudioFrame = true,
             bFirstVideoFrame = true;

        do
        {
            if (pCtx->m_pDemuxer->GetNextFrame(pCtx->m_BufferId.GetId(), &pData, uDatalen, mediaSpec) == RET_OK)
            {
                if (pData && uDatalen)
                {
                    bool bInitVideoDecoder = bFirstVideoFrame && (mediaSpec == VIDEO);
                    bool bInitAudioDecoder = bFirstAudioFrame && (mediaSpec == AUDIO);

                    if (bInitVideoDecoder || bInitAudioDecoder)
                    {
                        Decoder *pDecoder = pCtx->m_pDemuxer->GetDecoder(mediaSpec);

                        if (pDecoder)
                        {
                            IMediaStreaming * pMedia;

                            if (bInitVideoDecoder)
                            {    
                                pMedia = pCtx->m_pVideo;
                                bFirstVideoFrame = false;
                            }
                            else
                            {
                                pMedia = pCtx->m_pAudio;
                                bFirstAudioFrame = false;
                            }

                           if (pMedia)                            
                               pMedia->SetDecoder(pDecoder);
                           else
                               pCtx->m_bRunDemuxThread = FALSE;
                        }
                        else
                        {
                            pCtx->m_bRunDemuxThread = FALSE;
                        }
                    }
                   
                    pCtx->m_BufferId.SetPacket(pData, uDatalen);
                }
            }
            else
            {
                pCtx->m_bRunDemuxThread = FALSE;
            }
     
        }while (pCtx->m_bRunDemuxThread);
    }

#ifdef _DEBUG 
    sprintf(szBuf, "LeavingCTransmission::_DemuxStream - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}

int CTransmission::_BufferPkt(IThread *_pThread, void *_pCtx)
{
    CTransmission *pCtx = (CTransmission *)_pCtx;

#ifdef _DEBUG
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_BufferPkt - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    do
    {
        pCtx->WaitRebufferNotify();

        if (pCtx->m_pVideo->MediaPlaying() || pCtx->m_pAudio->MediaPlaying() )
        {
            pCtx->m_InitRebufferSemaph.Wait();
            
            pCtx->m_bBuffering = TRUE;

            while (pCtx->m_ulBufferingPercent != BUFFERING_COMPLETE_PERCENT)
            { 
                pCtx->NotifyKernel(MN_BUFFERING_START);
                pCtx->m_ulBufferingPercent += BUFFERING_SLICE_PERCENT;  

                Wait(BUFFERING_SLICE_TIME);
                
                if (!pCtx->m_bRunBufferThread)
                    break;
            }

            GetTime(&pCtx->m_ulLastBuffering);
            
            pCtx->NotifyKernel(MN_BUFFERING_STOP);
            pCtx->m_bBuffering = FALSE;

            pCtx->m_InitRebufferSemaph.Signal();
        }

    }while (pCtx->m_bRunBufferThread );

#ifdef _DEBUG
    sprintf(szBuf, "Leaving CTransmission::_BufferPkt - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}

int CTransmission::_PlayVideo(IThread *_pthread, void* _pCtx)
{    
    CTransmission * pCtx = (CTransmission *) _pCtx;

#ifdef _DEBUG 
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_StartVideo - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    pCtx->m_pVideo->InitWriteThread(pCtx->_WriteVideo, pCtx);

    do
    {			
        if (pCtx->m_pVideo->EncodedFrameReady())
        {
            pCtx->m_pVideo->PrepareFrame();			
            pCtx->NotifyKernel(MN_VIDEO_ALIVE);

            if (pCtx->m_pVideo->WritePaused() && pCtx->m_pVideo->DecodedFrameReady() )
                pCtx->m_pVideo->ResumeWrite();
        }
        else
            pCtx->m_pVideo->WaitData();

    } while( pCtx->m_pVideo->MediaPlaying() );

    pCtx->m_pVideo->StopWriteThread();

#ifdef _DEBUG 
    sprintf(szBuf, "Leaving CTransmission::_StartVideo - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}

int CTransmission::_WriteVideo(IThread *_pThread, void *_pCtx)
{
    CTransmission *pCtx = (CTransmission *) _pCtx;

#ifdef _DEBUG 
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_WriteVideo - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    ULONG ulCurTime;

    do
    {
        if ( !pCtx->m_bBuffering )
        {
            if (pCtx->m_pVideo->DecodedFrameReady())
            {
                if ( !pCtx->m_BufferId.ClockStarted() )                       
                    pCtx->m_BufferId.SetClockRef();
            
                pCtx->m_BufferId.UpdateTime(&ulCurTime);
                pCtx->m_pVideo->SyncStreaming(ulCurTime, pCtx->m_pAudio);
                pCtx->m_pVideo->WriteFrame();
            }
            else
                pCtx->m_pVideo->PauseWrite();
        }
        else
        {
            pCtx->m_InitRebufferSemaph.Wait();
            pCtx->m_InitRebufferSemaph.Signal();
        }

    }while ( pCtx->m_pVideo->RunWriteVideoThread() );

#ifdef _DEBUG 
    sprintf(szBuf, "Leaving CTransmission::_WriteVideo - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}


int CTransmission::_PlayAudio(IThread *_pthread, void* _pCtx)
{
    CTransmission * pCtx = (CTransmission *) _pCtx;	
	
#ifdef _DEBUG 
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_StartAudio - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    pCtx->m_pAudio->InitProcessAudioThread(_ProcessAudio, _pCtx);

    do
    {	  			
        if ( pCtx->m_pAudio->EncodedFrameReady() )
        {    				
            pCtx->m_pAudio->PrepareFrame();
            pCtx->NotifyKernel(MN_AUDIO_ALIVE);
	    
            if (pCtx->m_pAudio->ProcessAudioPaused() && pCtx->m_pAudio->DecodedFrameReady() )
                pCtx->m_pAudio->ResumeAudioProcessing();
        }
        else
            pCtx->m_pAudio->WaitData();            

    } while( pCtx->m_pAudio->MediaPlaying() );
	    
    pCtx->m_pAudio->StopProcessAudioThread();

#ifdef _DEBUG 
    sprintf(szBuf, "Leaving CTransmission::_StartAudio - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}


int CTransmission::_ProcessAudio(IThread *_pThread, void *_pCtx)
{
    CTransmission * pCtx = (CTransmission *) _pCtx;	
	
#ifdef _DEBUG 
    char szBuf[200];
    ULONG id = pCtx->m_BufferId.GetId();
    sprintf(szBuf, "Entering CTransmission::_ProcessAudio - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    BOOL bSoundIsPlaying,
         bSampleBufferFull;

    do
    {
        if ( !pCtx->m_bBuffering )
        {
            if (pCtx->m_pAudio->DecodedFrameReady() )
            {
                if ( !pCtx->m_BufferId.ClockStarted() )                       
                    pCtx->m_BufferId.SetClockRef();

                pCtx->m_pAudio->SampleBufferFull(&bSampleBufferFull);
            
                if (bSampleBufferFull)
                    Wait(20);
                else
                    pCtx->m_pAudio->WriteFrame();
                    
                pCtx->m_pAudio->SoundIsPlaying(&bSoundIsPlaying);
            
                if (!bSoundIsPlaying)
                    pCtx->m_pAudio->PlaySoundBuffer();
            }
            else
                pCtx->m_pAudio->PauseIfBufferEnds();
        }
        else
        {
            pCtx->m_InitRebufferSemaph.Wait();
            pCtx->m_InitRebufferSemaph.Signal();
        }
    }
    while (pCtx->m_pAudio->RunProcessAudio());

#ifdef _DEBUG 
    sprintf(szBuf, "Leaving CTransmission::_ProcessAudio - ID: %lu", id );
    g_FileLog.SetLog(szBuf);
#endif

    return RET_OK;
}


ULONG CTransmission::StartBufferThread()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_BufferSemaph.Wait();
    m_bRunBufferThread = TRUE;

    m_pBufferThread = CreateIThread(_BufferPkt, this, TRUE);

    if (m_pBufferThread)
        return RET_OK;

    return RET_ERROR;
}

ULONG CTransmission::StopBufferThread()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!m_pBufferThread)
        return RET_ERROR;

    m_BufferSemaph.Signal();
    m_bRunBufferThread = FALSE;

    m_pBufferThread->WaitForMe(INFINITE_TIME);  

    return RET_ERROR;
}

ULONG CTransmission::WaitRebufferNotify()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_bWaitingRebufferNotify = TRUE;

    m_BufferSemaph.Wait();
    m_BufferSemaph.Signal();

    m_bWaitingRebufferNotify = FALSE;

    return RET_OK;
}

ULONG CTransmission::NotifyRebuffer()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_BufferSemaph.Signal();
    m_BufferSemaph.Wait();

    return RET_OK;
}

ULONG CTransmission::AllocMedia(MediaSpec _mediaSpec, IMediaApi *_pMediaApi, TransmissionType _type)
{  
    if ((_mediaSpec != VIDEO) && (_mediaSpec != AUDIO))
        return RET_INVALID_MEDIA_SPEC;

    if ((_type != AV_CONF) && (_type != SIMPOSIUM))
        return RET_INVALID_ARG;

    CBufferMedia *pBufferMedia;
    CHECK_ERROR(m_BufferId.GetBufferMedia(&pBufferMedia, _mediaSpec), RET_OK)
    
    IMediaStreaming *_pMedia;

    if (_mediaSpec == VIDEO)
    {
        if (_type == AV_CONF)
        {
            _pMedia = m_pVideo = new CVideo(pBufferMedia);
        }
        else
        {
            _pMedia = m_pVideo = new CVideoSimposium(pBufferMedia);
        }
    }
    else
    {    
         _pMedia = m_pAudio = new CAudio(_pMediaApi, pBufferMedia);
    }

    if (!_pMedia)
        return RET_LOW_MEMORY;

    return RET_OK;
}

ULONG CTransmission::GetId(ULONG *_pulId)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    if (!_pulId)
        return RET_INVALID_ARG;

    *_pulId = m_BufferId.GetId();

    return RET_OK;
}

ULONG CTransmission::CreateConferenceWnd()
{
    if (!m_bInit)
        return RET_INIT_ERROR;
           
    return m_BufferId.StartPktProcessing(_ReadPkt, this);
}

ULONG CTransmission::DestroyConferenceWnd()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_BufferId.FinishPktProcessing();
        
    return RET_OK;
}

ULONG CTransmission::GetVideoReg(VideoFrame_reg ** _ppVideoFrameReg)
{
    if (!m_bInit)
    {    
        return RET_INIT_ERROR; 
    }

    if (!_ppVideoFrameReg)
        return RET_INVALID_ARG;

    *_ppVideoFrameReg = m_pVideo->GetFrameReg();

    return RET_OK;
}

ULONG CTransmission::SetAudioMute(BOOL _bMuteAudio)
{
    if (!m_bInit)
        return RET_INIT_ERROR; 

    m_BufferId.SetAudioMute(_bMuteAudio);

    return RET_OK;
}


BOOL CTransmission::NeedBuffering()
{
    BOOL bVideoPlaying,
         bAudioPlaying,
         bNeedBuffering;

    bNeedBuffering = FALSE;

    ULONG ulCurTime;
    GetTime(&ulCurTime);

    if (ulCurTime - m_ulLastBuffering > MIN_BUFFERING_INTERVAL_MS)
    {
        ULONG ulCount;
        m_BufferId.GetJitterPktCount(&ulCount);

        if (!ulCount)
        {
            bVideoPlaying = m_pVideo->MediaPlaying();
            bAudioPlaying = m_pAudio->MediaPlaying();

            if ( bVideoPlaying || bAudioPlaying )
            {
                bNeedBuffering = TRUE;
            
                if (bVideoPlaying)
                    m_BufferId.CheckForBuffering(VIDEO, bNeedBuffering);
            
                if (bAudioPlaying && bNeedBuffering)
                    m_BufferId.CheckForBuffering(AUDIO, bNeedBuffering);
            }
        }
    }

    return bNeedBuffering;
}

ULONG CTransmission::NotifyKernel(MediaCodeNotify _code)
{
    if (!m_bInit || !m_pNotify)
        return RET_INIT_ERROR;
		
    MediaParam mediaParam;
	
    mediaParam.code = _code;
    mediaParam.id   = m_BufferId.GetId();

    ULONG ret = RET_OK;

    switch(_code)
    {
        case MN_WRITE_PACKET:
            break;
       
        case MN_BUFFERING_START:
        case MN_BUFFERING_STOP:       
        {
            MediaParamExtBuf mediaParamExtBuf;
            memcpy(&mediaParamExtBuf, &mediaParam, sizeof(MediaParam));
            
            if (_code == MN_BUFFERING_STOP)
            {
                mediaParamExtBuf.ulPercent = BUFFERING_COMPLETE_PERCENT;
                m_ulBufferingPercent = 0;
            }
            else
                mediaParamExtBuf.ulPercent = m_ulBufferingPercent;

            m_pNotify->OnMediaNotify(&mediaParamExtBuf);
        }
        break;
        
        case MN_ERROR:
        {
            MediaParamExtError mediaParamExtError;       
            memcpy(&mediaParamExtError, &mediaParam, sizeof(MediaParam));
            
            mediaParamExtError.ulErrorCode = m_ulError;
            m_pNotify->OnMediaNotify(&mediaParamExtError);
        }
        break;

        case MN_VIDEO_ALIVE:
        case MN_AUDIO_ALIVE:
        {
            ULONG *pulLastMediaAlive,
                   ulMediaAliveInterval,
                   ulCurTime;

            BOOL   bNotify = FALSE;

            GetTime(&ulCurTime);

            if (_code == MN_VIDEO_ALIVE)
            {
                pulLastMediaAlive    = &m_ulLastVideoAlive;
                ulMediaAliveInterval = MN_VIDEOALIVE_INTERVAL;
            }
            else
            {     
                pulLastMediaAlive    = &m_ulLastAudioAlive;
                ulMediaAliveInterval = MN_AUDIOALIVE_INTERVAL;
            }

            if ( *pulLastMediaAlive )
            {    
                if  ( (ulCurTime - (*pulLastMediaAlive) ) > ulMediaAliveInterval )
                    bNotify = TRUE;     
            }
            else
                bNotify = TRUE;

            if (bNotify)
            {    
                *pulLastMediaAlive = ulCurTime;
                m_pNotify->OnMediaNotify(&mediaParam);
            }
        }
        break;

        default:
            return RET_INVALID_ARG;
    }
    
    return ret;
}


void CTransmission::GetMediaDbgInfo(MediaDbgInfo & _mediaDbgInfo)
{
    memset(&_mediaDbgInfo, 0, sizeof(MediaDbgInfo));
	
    CBufferMedia *pBufferVideo,
                 *pBufferAudio;

    pBufferVideo = NULL;
    pBufferAudio = NULL;

    ULONG ulCurTime;

    if (m_BufferId.UpdateTime(&ulCurTime) == RET_OK)
        _mediaDbgInfo.ulCurTS = ulCurTime;
        
    _mediaDbgInfo.ulMediaId  = m_BufferId.GetId();
    _mediaDbgInfo.ulPktRcv   = m_BufferId.GetPktRcv();
    _mediaDbgInfo.bBuffering = m_bBuffering;
    
    _mediaDbgInfo.ulBufferingPercent = m_ulBufferingPercent;

    ULONG ulJitterPkts;

    if (m_BufferId.GetJitterPktCount(&ulJitterPkts) == RET_OK)
        _mediaDbgInfo.ulJitterPkts = ulJitterPkts;

    m_BufferId.GetBufferMedia(&pBufferVideo, VIDEO);

    if (pBufferVideo)
    {
        ULONG ulVideoPkts, 
              ulVideoFrames;

        if (pBufferVideo->GetPktCount(&ulVideoPkts) == RET_OK)
            _mediaDbgInfo.ulVideoPkts = ulVideoPkts;
			
        if (pBufferVideo->GetFramesCount(&ulVideoFrames) == RET_OK)
            _mediaDbgInfo.ulBufVideoFrames = ulVideoFrames;	 
    }

    m_BufferId.GetBufferMedia(&pBufferAudio, AUDIO);

    if (pBufferAudio)
    {
        ULONG ulAudioPkts, 
              ulAudioFrames;

        if (pBufferAudio->GetPktCount(&ulAudioPkts) == RET_OK)
            _mediaDbgInfo.ulAudioPkts = ulAudioPkts;
		
        if (pBufferAudio->GetFramesCount(&ulAudioFrames) == RET_OK)
            _mediaDbgInfo.ulBufAudioFrames = ulAudioFrames;	 
    }

    if (m_pVideo)
    {	
        _mediaDbgInfo.ulProcVideoFrames = m_pVideo->GetFramesProcessed();
        _mediaDbgInfo.ulVideoTS	= m_pVideo->GetMediaTimestamp();

        _mediaDbgInfo.bVideoStarted = m_pVideo->MediaPlaying();
        _mediaDbgInfo.bVideoPaused  = m_pVideo->WaitingData();
    }	

    if (m_pAudio)
    {	
        _mediaDbgInfo.ulProcAudioFrames = m_pAudio->GetFramesProcessed();
        _mediaDbgInfo.ulAudioTS = m_pAudio->GetMediaTimestamp();

        _mediaDbgInfo.bAudioStarted = m_pAudio->MediaPlaying();
         _mediaDbgInfo.bAudioPaused  = m_pVideo->WaitingData();
    }	
}

