#ifndef IAUDIO_API_H
#define IAUDIO_API_H

#include "IMediaStreaming.h"


class IAudioApi : public IMediaApi
{
protected:
	CThreadSafeDeque<ULONG> m_ulAudioTSList;
    
public:

    IAudioApi() {}
    virtual ~IAudioApi() {}

    virtual ULONG LoadFrame(BYTE *_pData, ULONG _ulDatalen, ULONG *_pulCurTimestamp, ULONG _ulSampleRate, ULONG _ulChannels) = 0;
    virtual ULONG PauseSoundBuffer() = 0;
    virtual ULONG PlaySoundBuffer()  = 0;
    virtual ULONG SoundIsPlaying(BOOL *_pbSoundIsPlaying)  = 0;
    virtual ULONG SampleBufferFull(BOOL *_bSampleBufferFull) = 0;
    virtual ULONG BuffersProcessing(BOOL *_pbBuffersProcessing) = 0;
    virtual ULONG UpdateBufferCtx(ULONG *_pulNewTimestamp) = 0;

    ULONG SetAudioTimestamp(ULONG _ulTimestamp);
};

#endif

