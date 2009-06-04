#include "compat.h"
#include "COpenALObjectApi.h"
#include "COpenALApi.h"
#include "global_error.h"

COpenALObjectApi::COpenALObjectApi()
{
    alGetError();
    
    m_pAudioDevice = alcOpenDevice(NULL);
    
    if (m_pAudioDevice) 
    {
        m_pContext = alcCreateContext(m_pAudioDevice,NULL);
        
        if (m_pContext)
        {
            if (alcMakeContextCurrent(m_pContext))
                m_bInit = TRUE;
        }
    }
}

COpenALObjectApi::~COpenALObjectApi()
{
	ALCcontext *pContext;
	ALCdevice  *pDevice;

	pContext = alcGetCurrentContext();
	pDevice  = alcGetContextsDevice(pContext);
	
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);
}

ULONG COpenALObjectApi::GetApiInterface(IMediaApi **_ppMediaApi)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_ppMediaApi)
        return RET_INVALID_ARG;
    
    *_ppMediaApi = new COpenALApi();

    if (*_ppMediaApi)
        return RET_OK;

    return RET_LOW_MEMORY;
}
