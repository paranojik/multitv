#include "compat.h"
#include "IMediaStreaming.h"
#include "global_error.h"
#include "Global_functions.h"
#include "COutputBuffer.h"
#include "media_utilities.h"

IMediaStreaming::IMediaStreaming(CBufferMedia *_pMediaSource) : 
    m_PlaySemaph(),
    m_TimeElapsedSemaph()
{
    m_bInit             = FALSE;
    m_bWaitingData      = FALSE;
    m_bMediaPlaying     = FALSE;

    m_ulFramesDropped   = 0;
    m_ulFramesProcessed = 0;
    m_ulCurTimestamp    = 0;

    m_pMediaSource      = _pMediaSource;	
    m_pThread	        = NULL;

    GetTime(&m_ulLastLoadMediaTime);

    if (m_pMediaSource)
        m_bInit = TRUE;
}

IMediaStreaming::~IMediaStreaming()
{
    if (m_pThread)
        delete m_pThread;
}


ULONG IMediaStreaming::Play(ThreadFunction _pFunc, void *_pCtx)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;
    
    m_PlaySemaph.Wait();

    m_bMediaPlaying = TRUE;
    m_pThread = CreateIThread(_pFunc, _pCtx, TRUE);
    
    if (m_pThread)
        return RET_OK;

    return RET_ERROR;
}

ULONG IMediaStreaming::Stop()
{
    if (!m_bInit)
	    return RET_INIT_ERROR;
    
    if ( m_bMediaPlaying && m_pThread)    
    {
        m_bMediaPlaying = FALSE;  
        m_PlaySemaph.Signal();
        m_pThread->WaitForMe(INFINITE_TIME);
    }

    return RET_OK;
}

ULONG IMediaStreaming::WaitData()
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;
     
    if (m_bMediaPlaying)
    {
        m_bWaitingData = TRUE;

        m_PlaySemaph.Wait();
        m_PlaySemaph.Signal();

        m_bWaitingData = FALSE;
    }
    else
        return RET_ERROR;

    return RET_OK;
}


ULONG IMediaStreaming::StopWaitingData()
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

    m_PlaySemaph.Signal();
    m_PlaySemaph.Wait();

	return RET_OK;
}

ULONG IMediaStreaming::SetDecoder(Decoder *_pDecoder)
{
    if (!_pDecoder)
        return RET_INVALID_ARG;

    unsigned ret = RET_OK;

    if (m_pMediaSource)
    {
        m_pMediaSource->SetDecoder(_pDecoder);
    }
    else
    {
        ret = RET_ERROR;
    }

    return ret;
}

ULONG IMediaStreaming::SetPacket(BYTE *_buf, ULONG _bufSize, MediaSpec _mediaSpec)
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

    CHECK_ERROR(m_pMediaSource->SetPacket(_buf, _bufSize, _mediaSpec), RET_OK)

    return RET_OK;
}

BOOL IMediaStreaming::InputBufferFull()
{
    return m_pMediaSource->InputBufferFull();
}

BOOL IMediaStreaming::DecodedFrameReady()
{	
	ULONG ulDecodedFrames;
    BOOL  ret;

	m_pMediaSource->GetFramesCount(&ulDecodedFrames);

	if (ulDecodedFrames)
		ret = TRUE;
    else
        ret = FALSE;

	return ret;
}

BOOL IMediaStreaming::EncodedFrameReady()
{
	BOOL bEncodedFrameAvailable;
	m_pMediaSource->EncodedFrameAvailable(&bEncodedFrameAvailable);

    return bEncodedFrameAvailable;
}


ULONG IMediaStreaming::DropNextFrame()
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

	COutputBuffer *pOutputBuffer;
	CHECK_ERROR(m_pMediaSource->GetOutputBuffer(&pOutputBuffer), RET_OK)

    CHECK_ERROR(pOutputBuffer->DropNextFrame(), RET_OK);
    m_ulFramesDropped++;

    return RET_OK;
}

ULONG IMediaStreaming::GetTimeSinceLastLoadedFrame(ULONG *_pulElapsedTime)
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

    if (!_pulElapsedTime)
        return RET_INVALID_ARG;

    *_pulElapsedTime = 0;

    ULONG ret = RET_OK;

    m_TimeElapsedSemaph.Wait();

    ULONG ulCurTime;
    GetTime(&ulCurTime);

    if (ulCurTime >= m_ulLastLoadMediaTime)
        *_pulElapsedTime = ulCurTime - m_ulLastLoadMediaTime;
    else
        ret = RET_ERROR;

    m_TimeElapsedSemaph.Signal();   

    return ret;
}

ULONG IMediaStreaming::SetLoadMediaTime()
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

    m_TimeElapsedSemaph.Wait();
    GetTime(&m_ulLastLoadMediaTime);
    m_TimeElapsedSemaph.Signal();   

    return RET_OK;
}
