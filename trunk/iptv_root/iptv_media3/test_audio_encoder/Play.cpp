#include "compat.h"
#include "alc.h"
#include "al.h"
#include "global_error.h"
#include "AudioEncoder.h"
#include "AudioCapture.h"
#include <deque>

extern AudioEncoder *g_pAudioEncoder;
extern unsigned      g_playFmt;




unsigned Play(int _source, std::deque<int> & _freeBuffersList, BYTE *_pBuf, ULONG _uBufSize)
{
    ALint iBuffersProcessed;
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
        
    if (alGetError() != AL_NO_ERROR)
        return RET_ERROR;

    if (iBuffersProcessed)
    {
        ALuint *pUnqueuedBuffers = new ALuint[iBuffersProcessed];
        alSourceUnqueueBuffers(_source, iBuffersProcessed, pUnqueuedBuffers);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;

        for (ULONG i=0; i<(ULONG)iBuffersProcessed; i++)
			_freeBuffersList.push_back(pUnqueuedBuffers[i]);
		        
	    delete pUnqueuedBuffers;
    }

    if (_freeBuffersList.size())
    {
        ALuint bid = _freeBuffersList[0];
                
        switch(g_pAudioEncoder->GetChannels())
        {
            case 1:
                g_playFmt = AL_FORMAT_MONO16;
            break;

            case 2:
                g_playFmt = AL_FORMAT_STEREO16;
            break;
                
            default:
                return RET_ERROR;
        }

        alBufferData(bid, g_playFmt, _pBuf, _uBufSize, g_pAudioEncoder->GetSampleRate());

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;

        alSourceQueueBuffers(_source, 1, &bid);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;                

        _freeBuffersList.pop_front();
    }
            
    ALint iSourceState;

    alGetSourcei(_source, AL_SOURCE_STATE, &iSourceState);
            
    if (alGetError() != AL_NO_ERROR)
        return RET_ERROR;

    if (iSourceState != AL_PLAYING) 
    {   
        alSourcePlay(_source);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;
    }
   
    return RET_OK;
}