#if 0

#ifdef WIN32

#include "compat.h"
#include "CWinDSoundApi.h"
#include "Global_error.h"
#include "Global_functions.h"


CWinDSoundApi::CWinDSoundApi(LPDIRECTSOUNDBUFFER _pSecBuffer)
{
    m_pSecBuffer = _pSecBuffer;
    
    if (m_pSecBuffer)
        m_bInit = TRUE;
}


    
CWinDSoundApi::~CWinDSoundApi()
{
    if (m_pSecBuffer)
        m_pSecBuffer->Release();
}


ULONG CWinDSoundApi::LoadFrame(BYTE *_pData, ULONG _ulDatalen)
{	
	if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pData || !_ulDatalen)
        return RET_INVALID_ARG;

    PVOID pAudioPtr1,
          pAudioPtr2;
    
    DWORD dwLockPtr1,
          dwLockPtr2,
		  dwOffset;

	dwOffset = (m_ulFramesWritten % AUDIO_BUFFER_FRAMES)*AUDIO_FRAME_SIZE;

	if (!SUCCEEDED(m_pSecBuffer->Lock(dwOffset, _ulDatalen, &pAudioPtr1,  &dwLockPtr1, &pAudioPtr2, &dwLockPtr2, 0)))
		return RET_ERROR;
	
	if (pAudioPtr1)
    {	
        MemCopy(pAudioPtr1, _pData, _ulDatalen);
        m_ulFramesWritten++;
	}
    else
		return RET_ERROR;

    if (!SUCCEEDED(m_pSecBuffer->Unlock(pAudioPtr1, dwLockPtr1, pAudioPtr2, dwLockPtr2)))
        return RET_ERROR;
    
	return RET_OK;
}

ULONG CWinDSoundApi::PlaySoundBuffer()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!SUCCEEDED(m_pSecBuffer->Play(0, 0, DSBPLAY_LOOPING)))
        return RET_ERROR;

    return RET_OK;
}

ULONG CWinDSoundApi::PauseSoundBuffer()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!SUCCEEDED(m_pSecBuffer->Stop()))
        return RET_ERROR;

    return RET_OK;
}

ULONG CWinDSoundApi::GetCurrentBufferPos(ULONG *_pulBufSection)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pulBufSection)
        return RET_INVALID_ARG;

    ULONG ulPlayOffset;

    if (!SUCCEEDED(m_pSecBuffer->GetCurrentPosition(&ulPlayOffset, NULL)))
        return RET_ERROR;

    /*if (_pulPlayOffset)
        *_pulPlayOffset = ulPlayOffset;

    if (_pulMsElapsed)
    {
        WAVEFORMATEX wfx;
        m_pSecBuffer->GetFormat(&wfx, sizeof(wfx), NULL);

        double flMsPerByte = (1.0 / wfx.nAvgBytesPerSec)*1000;
        *_pulMsElapsed = ulPlayOffset * flMsPerByte;
    }*/

	*_pulBufSection = ulPlayOffset / AUDIO_FRAME_SIZE;

    return RET_OK;
}

ULONG CWinDSoundApi::SoundIsPlaying(BOOL *_pbSoundIsPlaying)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    if (!_pbSoundIsPlaying)
        return RET_INVALID_ARG;

	*_pbSoundIsPlaying = FALSE;

	DWORD dwBufferStatus;
	
	if (!SUCCEEDED(m_pSecBuffer->GetStatus(&dwBufferStatus)))
		return RET_ERROR;

	if (dwBufferStatus & DSBSTATUS_PLAYING)
		*_pbSoundIsPlaying = TRUE;
	
	return RET_OK;
}

ULONG CWinDSoundApi::SampleBufferFull(BOOL *_pbSampleBufferFull)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    if (!_pbSampleBufferFull)
        return RET_INVALID_ARG;		  

	*_pbSampleBufferFull = TRUE;
    
    if (m_ulFramesWritten)
    {    
		ULONG ulBufPlaySection;
		CHECK_ERROR(GetCurrentBufferPos(&ulBufPlaySection), RET_OK)

		ULONG ulLastWrite = (m_ulFramesWritten-1) % AUDIO_BUFFER_FRAMES;

        if ( ulBufPlaySection != ulLastWrite)	
	    {
		    if ( ulLastWrite < ulBufPlaySection )
		    {
			    if ( ulBufPlaySection - ulLastWrite >= MIN_BUFFER_DISTANCE )
				    *_pbSampleBufferFull = FALSE;
		    }
		    else // m_ulLastWrite > ulBufPlaySection
		    if (ulLastWrite == (AUDIO_BUFFER_FRAMES-1) )
            {
                if (ulBufPlaySection != 0)
                    *_pbSampleBufferFull = FALSE;
            }
            else
                *_pbSampleBufferFull = FALSE;
        }
		else
			*_pbSampleBufferFull = FALSE;    
	}
    else 
        *_pbSampleBufferFull = FALSE;

	return RET_OK;
}

ULONG CWinDSoundApi::SyncStreaming(ULONG _ulCurTime, ULONG _ulAudioTime)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ulDiffSync;
    ulDiffSync = ::abs(double(_ulCurTime - _ulAudioTime));

    if (m_bSyncStarted)
    { 
        DWORD dwBufferFrequency,
              dwNewBufferFrequency;

        dwNewBufferFrequency = 0;
     
		if (ulDiffSync > 2*AUDIO_FRAME_MS)
		{
			if (ulDiffSync >= m_ulLastDiffSync)
			{               
				if (!SUCCEEDED(m_pSecBuffer->GetFrequency(&dwBufferFrequency)))
					return RET_ERROR;				
				
				if (_ulCurTime > _ulAudioTime)    
					dwNewBufferFrequency = dwBufferFrequency + BUF_FREQ_INC;
				else
					dwNewBufferFrequency = dwBufferFrequency - BUF_FREQ_DEC;
			}
		}
		else
			dwNewBufferFrequency = AUDIO_SAMPLE_RATE;

 
		if ((dwNewBufferFrequency >= MIN_BUF_FREQ) && (dwNewBufferFrequency <= MAX_BUF_FREQ)) 
		{
			if (!SUCCEEDED(m_pSecBuffer->SetFrequency(dwNewBufferFrequency)))
				return RET_ERROR;
		}
    }
    else
        m_bSyncStarted = TRUE;
    
    m_ulLastDiffSync = ulDiffSync;
    
    return RET_OK;
}

ULONG CWinDSoundApi::CheckIfLastBufferSection(BOOL *_pbLastSection)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	if (!_pbLastSection)
		return RET_INVALID_ARG;

	*_pbLastSection = FALSE;

	if (m_ulFramesWritten)
	{
		ULONG ulBufPlaySection;
		CHECK_ERROR(GetCurrentBufferPos(&ulBufPlaySection), RET_OK)

		ULONG ulLastWrite = (m_ulFramesWritten-1) % AUDIO_BUFFER_FRAMES;
        
		if ( ulBufPlaySection == ulLastWrite)
		{	
			*_pbLastSection = TRUE;
			
			if (!SUCCEEDED(m_pSecBuffer->Stop()))
				return RET_ERROR;
		}	
	}

	return RET_OK;
}

#endif
#endif
