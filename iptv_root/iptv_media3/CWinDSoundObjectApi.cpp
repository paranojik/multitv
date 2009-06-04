#if 0

#ifdef WIN32

#include "compat.h"
#include "CWinDSoundObjectApi.h"
#include "CWinDSoundApi.h"
#include "Global_error.h"
#include "CWinWnd.h"
#include "Media_const.h"


CWinDSoundObjectApi::CWinDSoundObjectApi(IBaseWnd *_pWnd)
{
    m_bInit = FALSE;

    if (_pWnd)
    {
        HWND hwnd;
        ((CWinWnd *)_pWnd)->GetWindowHandle(&hwnd);
        
        if (SUCCEEDED(DirectSoundCreate8(NULL, &m_pDSound, NULL)) &&
            SUCCEEDED(m_pDSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
        {
            m_bInit = TRUE;
        }
    }
}

CWinDSoundObjectApi::~CWinDSoundObjectApi()
{
    if (m_pDSound)
        m_pDSound->Release();
}

ULONG CWinDSoundObjectApi::GetApiInterface(IBaseWnd * _pViewerWnd, IBaseWnd *_pTransmWnd, IMediaApi **_ppMediaApi)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pViewerWnd || !_pTransmWnd || !_ppMediaApi)
        return RET_INVALID_ARG;

    *_ppMediaApi = NULL;
   
    WAVEFORMATEX wfx;
    ZeroMemory(&wfx, sizeof (wfx));
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.nChannels       = AUDIO_CHANNELS;
    wfx.nSamplesPerSec  = AUDIO_SAMPLE_RATE;
    wfx.wBitsPerSample  = AUDIO_SAMPLE_BIT_DEEPTH;
    wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample/8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
 
    DSBUFFERDESC dsBufferDesc;
    ZeroMemory(&dsBufferDesc, sizeof(dsBufferDesc));

    dsBufferDesc.dwSize         = sizeof(dsBufferDesc);
    dsBufferDesc.dwBufferBytes  = AUDIO_FRAME_SIZE*AUDIO_BUFFER_FRAMES;
    
    dsBufferDesc.dwFlags = (DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY|DSBCAPS_GETCURRENTPOSITION2);
    
    dsBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    dsBufferDesc.lpwfxFormat     = &wfx;
  
    LPDIRECTSOUNDBUFFER pSecBuffer;
    if (!SUCCEEDED(m_pDSound->CreateSoundBuffer(&dsBufferDesc, &pSecBuffer, NULL)))
		return RET_ERROR;

    *_ppMediaApi = new CWinDSoundApi(pSecBuffer);

    if (*_ppMediaApi)
        return RET_OK;

    return RET_LOW_MEMORY;
}

#endif
#endif
