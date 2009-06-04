#ifndef IMEDIA_H
#define IMEDIA_H


#include "CBufferMedia.h"
#include "Media_const.h"
#include "Data_reg.h"
#include "ithread.h"
#include "CThreadSafeDeque.h"

class IMediaApi
{
protected:
    BOOL m_bInit;

public:
    
    IMediaApi() {m_bInit = FALSE; }
    virtual ~IMediaApi() {}
};

class IMediaObjectApi
{
protected:
    BOOL  m_bInit;

public:

    IMediaObjectApi() {m_bInit = FALSE; }
    virtual ~IMediaObjectApi() {}

    virtual ULONG GetApiInterface(IMediaApi **_ppMediaApi) = 0;
};

class IThread;
class Decoder;
class IMediaStreaming
{
protected:

    BOOL           m_bInit,
                   m_bWaitingData,
                   m_bMediaPlaying;

    ULONG          m_ulFramesDropped,
                   m_ulFramesProcessed,
                   m_ulCurTimestamp,
                   m_ulLastLoadMediaTime;

    _SEMAPHORE     m_PlaySemaph,
                   m_TimeElapsedSemaph;

    MediaSpec      m_MediaSpec;

    CBufferMedia  *m_pMediaSource;

    IThread       *m_pThread;

    ULONG SetLoadMediaTime();

public:
    
    IMediaStreaming(CBufferMedia *_pMediaSource);
    virtual ~IMediaStreaming();

    virtual ULONG PrepareFrame()= 0;
    virtual ULONG SyncStreaming(ULONG _ulCurTime, IMediaStreaming *_pOtherMedia) = 0;

    ULONG SetDecoder(Decoder *_pDecoder);
    ULONG SetPacket(BYTE *_buf, ULONG _bufSize, MediaSpec _mediaSpec);

    BOOL  EncodedFrameReady();
    BOOL  DecodedFrameReady();
    BOOL  InputBufferFull();

    ULONG DropNextFrame(); 
    ULONG GetTimeSinceLastLoadedFrame(ULONG *_pulElapsedTime);
	
    ULONG Play(ThreadFunction _pFunc, void *_pCtx);  
    ULONG Stop();
    
    ULONG StopWaitingData();
    ULONG WaitData();

    MediaSpec   GetMediaSpec()	{return  m_MediaSpec;           }

    ULONG GetFramesProcessed()  {return	 m_ulFramesProcessed;	}
    ULONG GetFramesDropped()	{return  m_ulFramesDropped;     }
    ULONG GetMediaTimestamp()	{return  m_ulCurTimestamp;      }

    BOOL  WaitingData()	        {return  m_bWaitingData;        }
    BOOL  MediaPlaying()        {return  m_bMediaPlaying;       }    

    void SetMediaTimestamp(ULONG _ulTimestamp) {m_ulCurTimestamp = _ulTimestamp;}
};

#endif
