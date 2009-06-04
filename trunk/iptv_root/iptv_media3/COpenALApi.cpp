#include "compat.h"
#include "COpenALApi.h"
#include "global_error.h"
#include "media_utilities.h"

COpenALApi::COpenALApi()
{
	alGetError();
    alGenSources(1, &m_Source);

    m_ulCurBufIndex = 0;

    if (alGetError() == AL_NO_ERROR)
    {
        alGenBuffers(OPENAL_BUFFERS, m_Buffers);
        
        if (alGetError() == AL_NO_ERROR)
        {
            for(ULONG i=0; i<OPENAL_BUFFERS; i++)
                m_ulFreeBufferList.PushBack(m_Buffers[i]);
            
            m_bInit = TRUE;
        }
    }
}

COpenALApi::~COpenALApi()
{    
    alSourceStop(m_Source);
    alSourcei(m_Source, AL_BUFFER, AL_NONE);
    alDeleteBuffers(OPENAL_BUFFERS, m_Buffers);
    alDeleteSources(1, &m_Source);
}


ULONG COpenALApi::LoadFrame(BYTE *_pData, 
                            ULONG _ulDatalen, 
                            ULONG *_pulCurTimestamp, 
                            ULONG _ulSampleRate, 
                            ULONG _ulChannels)
{    
    if (!m_bInit)
		return RET_INIT_ERROR;

    if (!_pData || !_ulDatalen || !_ulSampleRate)
        return RET_INVALID_ARG;

    if ( (_ulChannels != 1) && (_ulChannels != 2) )
        return RET_INVALID_ARG;
  
    BOOL bBufferFull;
    CHECK_ERROR(SampleBufferFull(&bBufferFull), RET_OK)

    if (bBufferFull)
        return RET_BUFFER_FULL;

    ALenum audio_format;

    if (_ulChannels == 1)
        audio_format = AL_FORMAT_MONO16;
    else
        audio_format = AL_FORMAT_STEREO16;


	CHECK_ERROR(UpdateBufferCtx(_pulCurTimestamp), RET_OK)

    if (m_ulFreeBufferList.Size())
    {
        ALuint bid = m_ulFreeBufferList.First();
        
        alBufferData(bid, audio_format, _pData, _ulDatalen, _ulSampleRate);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;

        alSourceQueueBuffers(m_Source, 1, &bid);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;

        m_ulFreeBufferList.PopFront();
    }
    else
        return RET_ERROR;

	return RET_OK;
}
    
ULONG COpenALApi::PauseSoundBuffer()
{    
    if (!m_bInit)
		return RET_INIT_ERROR;

    alSourcePause(m_Source);
    
    if (alGetError() != AL_NO_ERROR)
        return RET_ERROR;

	return RET_OK;
}

ULONG COpenALApi::PlaySoundBuffer()
{
    if (!m_bInit)
		return RET_INIT_ERROR;

    alSourcePlay(m_Source);

    if (alGetError() != AL_NO_ERROR)
        return RET_ERROR;

    return RET_OK;
}

ULONG COpenALApi::SoundIsPlaying(BOOL *_pbSoundIsPlaying)
{    
    if (!m_bInit)
		return RET_INIT_ERROR;

    if (!_pbSoundIsPlaying)
        return RET_INVALID_ARG;

    *_pbSoundIsPlaying = FALSE;

    ALint iSourceState;
    alGetSourcei(m_Source, AL_SOURCE_STATE, &iSourceState);

    if (alGetError() != AL_NO_ERROR)
        return RET_ERROR;

    if (iSourceState == AL_PLAYING)
        *_pbSoundIsPlaying = TRUE;
	
    return RET_OK;
}


ULONG COpenALApi::SampleBufferFull(BOOL *_pbSampleBufferFull)
{
    if (!m_bInit)
		return RET_INIT_ERROR;

	if (!_pbSampleBufferFull)
		return RET_INVALID_ARG;

	*_pbSampleBufferFull = FALSE;

    ALint iBuffersQueued;
	alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &iBuffersQueued);

	if (alGetError() != AL_NO_ERROR)
		return RET_ERROR;

    if (iBuffersQueued == OPENAL_BUFFERS)
    {
		ALint iBuffersProcessed;      
		alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

	    if (alGetError() != AL_NO_ERROR)
		    return RET_ERROR;

        if (!iBuffersProcessed)
            *_pbSampleBufferFull = TRUE;
    }

	return RET_OK;
}


ULONG COpenALApi::BuffersProcessing(BOOL *_pbBuffersProcessing)
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	if (!_pbBuffersProcessing)
		return RET_INVALID_ARG;

	*_pbBuffersProcessing = FALSE;

	ALint iBuffersQueued;
	alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &iBuffersQueued);

	if (alGetError() != AL_NO_ERROR)
		return RET_ERROR;

	if (iBuffersQueued) 
		*_pbBuffersProcessing = TRUE;
		
	return RET_OK;
}

ULONG COpenALApi::UpdateBufferCtx(ULONG *_pulNewTimestamp)
{
	if (!m_bInit)
		return RET_INIT_ERROR;

	if (!_pulNewTimestamp)
		return RET_INVALID_ARG;

	*_pulNewTimestamp = 0;

    ALint iBuffersProcessed;
    alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

	if (alGetError() != AL_NO_ERROR)
		return RET_ERROR;

    if (iBuffersProcessed)
    {
        ALuint *pUnqueuedBuffers = new ALuint[iBuffersProcessed];

        if (!pUnqueuedBuffers)
            return RET_LOW_MEMORY;

        alSourceUnqueueBuffers(m_Source, iBuffersProcessed, pUnqueuedBuffers);

        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR; 

        for (ULONG i=0; i<(ULONG)iBuffersProcessed; i++)
		{    
			m_ulFreeBufferList.PushBack(pUnqueuedBuffers[i]);
			m_ulAudioTSList.PopFront();	
		}
	
	    delete pUnqueuedBuffers;
	}

	if (m_ulAudioTSList.Size())
		*_pulNewTimestamp = m_ulAudioTSList.First();	

	return RET_OK;
}
