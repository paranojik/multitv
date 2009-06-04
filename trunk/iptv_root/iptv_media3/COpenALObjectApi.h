#ifndef C_OPENAL_OBJECT_API_H
#define C_OPENAL_OBJECT_API_H

#include "IMediaStreaming.h"
#include "alc.h"


class COpenALObjectApi : public IMediaObjectApi
{
private:

    ALCdevice  *m_pAudioDevice;
    ALCcontext *m_pContext; 
    
public:

    COpenALObjectApi();
    virtual ~COpenALObjectApi();

    ULONG GetApiInterface(IMediaApi **_ppMediaApi);
};

#endif
