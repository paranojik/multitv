#ifndef C_OPENAL_API_H
#define C_OPENAL_API_H

#include "IMediaStreaming.h"
#include "IAudioApi.h"
#include "al.h"
#include "alc.h"

#define OPENAL_BUFFERS 5

class COpenALApi : public IAudioApi
{
private:

    ALuint  m_Source,
			m_Buffers[OPENAL_BUFFERS];

    ULONG   m_ulCurBufIndex;

    CThreadSafeDeque<ALuint> m_ulFreeBufferList;

public:
    
	COpenALApi();
    virtual ~COpenALApi();

    virtual ULONG LoadFrame(BYTE *_pData, ULONG _ulDatalen, ULONG *_pulCurTimestamp, ULONG _ulSampleRate, ULONG _ulChannels);
    virtual ULONG PauseSoundBuffer();
    virtual ULONG PlaySoundBuffer();
	virtual ULONG SoundIsPlaying(BOOL * _pbSoundIsPlaying);
	virtual ULONG SampleBufferFull(BOOL *_pbSampleBufferFull);
	virtual ULONG BuffersProcessing(BOOL *_pbBuffersProcessing);
	virtual ULONG UpdateBufferCtx(ULONG *_pulNewTimestamp);
};



#endif
