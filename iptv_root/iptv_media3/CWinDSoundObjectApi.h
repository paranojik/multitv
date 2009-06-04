#ifndef C_WIN_DSOUND_OBJECT_API_H
#define C_WIN_DSOUND_OBJECT_API_H

#include "IMedia.h"
#include <dsound.h>

class CWinDSoundObjectApi : public IMediaObjectApi
{
private:
    
    LPDIRECTSOUND8 m_pDSound;

public:

    CWinDSoundObjectApi(IBaseWnd *_pWnd);
    virtual ~CWinDSoundObjectApi();

    ULONG GetApiInterface(IBaseWnd * _pViewerWnd, IBaseWnd * _pTransmWnd, IMediaApi **_ppMediaApi);

};

#endif
