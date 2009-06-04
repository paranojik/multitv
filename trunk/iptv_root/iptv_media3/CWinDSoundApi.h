#ifndef C_WIN_DSOUND_API_H
#define C_WIN_DSOUND_API_H

#include "IMedia.h"
#include <dsound.h>
#include "Media_const.h"

class CWinDSoundApi : public IAudioApi
{
private:

    LPDIRECTSOUNDBUFFER m_pSecBuffer;
    
public:

    CWinDSoundApi(LPDIRECTSOUNDBUFFER _pSecBuffer);
    virtual ~CWinDSoundApi();

    virtual ULONG LoadFrame(BYTE *_pData, ULONG _ulDatalen);
    virtual ULONG PauseSoundBuffer();
    virtual ULONG PlaySoundBuffer();
    virtual ULONG GetCurrentBufferPos(ULONG *_pulBufSection);
	virtual ULONG SyncStreaming(ULONG _ulCurTime, ULONG _ulAudioTime);
	virtual ULONG SoundIsPlaying(BOOL * _pbSoundIsPlaying);
	virtual ULONG SampleBufferFull(BOOL *_pbSampleBufferFull);
	virtual ULONG CheckIfLastBufferSection(BOOL *_pbLastSection);
    
    virtual ULONG GetSampleRate()                    { return RET_OK; }
    virtual ULONG SetSampleRate(ULONG _ulSampleRate) { return RET_OK; }
};

#endif
