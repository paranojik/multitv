#ifndef CAUDIO_H
#define CAUDIO_H

#include "IMediaStreaming.h"
#include "IAudioApi.h"

class CAudio : public IMediaStreaming
{
private:

    BOOL        m_bMutexOwnerSet,
                m_bRunProcessAudioThread,
                m_bAudioProcessingPaused;

    IAudioApi  *m_pAudioApi;

    _SEMAPHORE  m_AudioSemaph,
                m_ProcessAudioSemaph;
    
    IThread    *m_pProcessAudioThread;

public:

    CAudio(IMediaApi *_pAudioApi, CBufferMedia *_pMediaSource);
    virtual ~CAudio();

    // IMedia implementation
    virtual ULONG PrepareFrame();
    virtual ULONG SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia);

    ULONG InitProcessAudioThread(ThreadFunction _ProcessAudio, void *_pCtx);
    ULONG StopProcessAudioThread();

    ULONG ResumeAudioProcessing();
    ULONG PauseAudioProcessing();

    ULONG PlaySoundBuffer();
    ULONG PauseSoundBuffer();
	ULONG SampleBufferFull(BOOL *_pbSampleBufferFull);
	ULONG SoundIsPlaying(BOOL *_pbSoundIsPlaying);
	ULONG PauseIfBufferEnds();

    ULONG WriteFrame();

    BOOL ProcessAudioPaused()   {return m_bAudioProcessingPaused;   }
    BOOL RunProcessAudio()      {return m_bRunProcessAudioThread;   }
};


#endif

