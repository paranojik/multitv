#include "compat.h"
#include "CFrame.h"
#include "global_error.h"
#include "Global_functions.h"
#include "CAudio.h"
#include "COutputBuffer.h"
#include "media_utilities.h"
#include "CAudioFrame.h"
#include "log.h"

CAudio::CAudio(IMediaApi *_pAudioApi, CBufferMedia *_pMediaSource) : IMediaStreaming(_pMediaSource)
{   
	if (m_bInit)
	{        
        m_bMutexOwnerSet         = FALSE;
        m_bRunProcessAudioThread = FALSE;
        m_bAudioProcessingPaused = FALSE;

        m_pProcessAudioThread = NULL;

		m_MediaSpec = AUDIO;
		m_pAudioApi = (IAudioApi *) _pAudioApi;
	}
}

CAudio::~CAudio()
{
    if (m_pAudioApi)
        delete m_pAudioApi;
}

ULONG CAudio::PrepareFrame()
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

	ULONG ret = m_pMediaSource->Decode();

	if (ret == RET_OUTPUT_BUFFER_FULL)
	{	
		Wait(100);
		ret = RET_OK;
	}
    return ret;
}

ULONG CAudio::WriteFrame()
{
    if (!m_bInit)    
        return RET_INIT_ERROR;

    COutputBuffer *pOutputBuffer;
	CHECK_ERROR(m_pMediaSource->GetOutputBuffer(&pOutputBuffer), RET_OK)

	if (!pOutputBuffer->GetFramesCount())
		return RET_NO_FRAME_AVAILABLE;

	CFrame *pFrame;
	CHECK_ERROR(pOutputBuffer->GetFrame(&pFrame), RET_OK)

    ULONG ret;

	m_AudioSemaph.Wait();

	if (pFrame)
	{
		if (pFrame->FrameOk())
		{
			ULONG ulAudioTimestamp,
                  ulDatalen;
				
			pFrame->GetTimestamp(&ulAudioTimestamp);
			m_pAudioApi->SetAudioTimestamp(ulAudioTimestamp);

            BYTE *pFrameData;

            if ((ret = pFrame->GetFrameData(&pFrameData, &ulDatalen)) == RET_OK)
			{
				ULONG ulNewTimestamp, 
                      ulSampleRate,
                      ulChannels;

                CAudioFrame *pAudioFrame = (CAudioFrame *) pFrame;

                CHECK_ERROR(pAudioFrame->GetSampleRate(&ulSampleRate), RET_OK)
                CHECK_ERROR(pAudioFrame->GetAudioChannels(&ulChannels), RET_OK)
                
				if ((ret = m_pAudioApi->LoadFrame(pFrameData, ulDatalen, &ulNewTimestamp, ulSampleRate, ulChannels)) == RET_OK)
				{
					if (ulNewTimestamp)
						SetMediaTimestamp(ulNewTimestamp);

					SetLoadMediaTime();
					m_ulFramesProcessed++;
				}
			}
		}
		else
            ret = RET_ERROR;

		delete pFrame;
	}
	else
		ret = RET_ERROR;

	m_AudioSemaph.Signal();

    return RET_OK;
}

ULONG CAudio::SoundIsPlaying(BOOL *_pbSoundIsPlaying)
{    
    if (!m_bInit)
	    return RET_INIT_ERROR;

    ULONG ret;

    m_AudioSemaph.Wait();

	ret = m_pAudioApi->SoundIsPlaying(_pbSoundIsPlaying);

    m_AudioSemaph.Signal();

	return ret;
}

ULONG CAudio::SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;    
    
    return RET_OK;
}

ULONG CAudio::InitProcessAudioThread(ThreadFunction _ProcessAudio, void *_pCtx)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (m_pProcessAudioThread)
        return RET_ERROR;

    m_bRunProcessAudioThread = TRUE;
    m_ProcessAudioSemaph.Wait();
    
    m_pProcessAudioThread = CreateIThread(_ProcessAudio, _pCtx, TRUE);

    if (m_pProcessAudioThread)
        return RET_OK;

    return RET_ERROR;
}

ULONG CAudio::StopProcessAudioThread()
{
    if (!m_bInit)    
	    return RET_INIT_ERROR;

    if (!m_pProcessAudioThread)
        return RET_ERROR;

    m_bRunProcessAudioThread = FALSE;
    m_ProcessAudioSemaph.Signal();

    m_pProcessAudioThread->WaitForMe(INFINITE_TIME);

    return RET_OK;
}

ULONG CAudio::PauseAudioProcessing()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_bAudioProcessingPaused = TRUE;

    m_ProcessAudioSemaph.Wait();
    m_ProcessAudioSemaph.Signal();

    m_bAudioProcessingPaused = FALSE;

    return RET_OK;
}

ULONG CAudio::ResumeAudioProcessing()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_ProcessAudioSemaph.Signal();
    m_ProcessAudioSemaph.Wait();

    return RET_OK;
}

ULONG CAudio::PauseSoundBuffer()
{
    if (!m_bInit)
	    return RET_INIT_ERROR;
    
    ULONG ret;

    m_AudioSemaph.Wait();

    ret = m_pAudioApi->PauseSoundBuffer();
    
    m_AudioSemaph.Signal();

    return ret;
}

ULONG CAudio::PlaySoundBuffer()
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    ULONG ret;

    m_AudioSemaph.Wait();

    ret = m_pAudioApi->PlaySoundBuffer();

    m_AudioSemaph.Signal();
    
    return ret;
}

ULONG CAudio::SampleBufferFull(BOOL *_pbSampleBufferFull)
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

    ULONG ret;

    m_AudioSemaph.Wait();

    ret = m_pAudioApi->SampleBufferFull(_pbSampleBufferFull);

    m_AudioSemaph.Signal();

    return ret;
}

ULONG CAudio::PauseIfBufferEnds()
{
    if (!m_bInit)
	    return RET_INIT_ERROR;

	BOOL  bBuffersProcessing;
	ULONG ulNewTimestamp;

	do 
	{
		Wait(10);

		CHECK_ERROR(m_pAudioApi->UpdateBufferCtx(&ulNewTimestamp), RET_OK)

		if (ulNewTimestamp)
			SetMediaTimestamp(ulNewTimestamp);

        if (DecodedFrameReady())
            return RET_OK;

		CHECK_ERROR(m_pAudioApi->BuffersProcessing(&bBuffersProcessing), RET_OK)

	}while (bBuffersProcessing);
    
    PauseAudioProcessing();
    
	return RET_OK;
}
