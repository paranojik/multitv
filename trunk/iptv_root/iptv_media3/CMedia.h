#ifndef CMEDIA_H
#define CMEDIA_H

#include "COpenALObjectApi.h"

class CTransmission;
class IMediaKernelNotify;
class IMediaObjectApi;
class IThread;
class CVideoFrame;
class _SEMAPHORE;

class CMedia
{
private:

    BOOL m_bConfEnded,
         m_bMuteAll;
    
    std::deque<CTransmission *> m_pTransmissionList;

    _SEMAPHORE	m_CodecSemaph,
                m_MediaSemaph;

    COpenALObjectApi    m_AudioObjectApi;
    
    IMediaKernelNotify *m_pKernelNotify; 

    typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

    ULONG SetAudioMute(BOOL _bMuteAudio, BOOL _bApplyToAll, ULONG _id = ULONG(-1));

public:

    CMedia();
    virtual ~CMedia();

    ULONG AddTransmission(ULONG _id, 
                          BOOL _bHeader=FALSE, 
                          BOOL _bEncoded=FALSE, 
                          BYTE *_pHeader = NULL, 
                          ULONG _uHeaderLen = 0);

    ULONG EndTransmission(ULONG _id);
    ULONG EndAllTransmissions();

    ULONG SetMute(BOOL _bMuteAudio);

    ULONG GetMediaDbgInfo(ULONG _ulId, MediaDbgInfo & _mediaDbgInfo);
    ULONG GetVideoReg(ULONG _id, VideoFrame_reg ** _ppVideoFrameReg);   
    ULONG SetPacket(BYTE *_pData, ULONG _ulDatalen);

    BOOL  GetMuteStatus() {return m_bMuteAll;   }
    ULONG ConfEnded()     {return m_bConfEnded; } 
    
    void  SetKernelNotify(IMediaKernelNotify *_pNotify) {m_pKernelNotify = _pNotify;    }
};


#endif
