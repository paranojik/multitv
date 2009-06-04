#ifndef CBUFFER_ID_H
#define CBUFFER_ID_H

#include "CThreadSafeDeque.h"
#include "XJitterCtrl.h"
#include "ISendKernelNotify.h"
#include "CClockSync.h"
#include "media_utilities.h"

class CTransmission;
class CBufferMedia;
class CBufferId : public IJitterBufferEvents, public ISendKernelNotify
{
private:
    ULONG m_Id,
          m_ulRefTS,
          m_ulLastVideoSubSeq,
          m_ulLastAudioSubSeq,
          m_ulLastVideoSeq,
          m_ulLastAudioSeq,
          m_ulPktRcv;

    BOOL  m_bInit,
          m_bFirstPkt,
          m_bRunReadPktThread,
          m_bAudioMute;

    CTransmission *m_pTransmission;

    IThread    *m_pReadPktThread;

    CClockSync  m_ClockSync;
    XJitterCtrl m_JitterCtrl;
    
    _SEMAPHORE  *m_pCodecSemaph;
    
    CThreadSafeDeque<CBufferMedia *> m_pBufferMediaList;

    ULONG ResetMediaCtx(MediaSpec _mediaSpec);
    ULONG CheckBoundaryConditions(BYTE *_pData, ULONG _ulDatalen);

    // IMediaNotifyKernel overrides
    virtual ULONG NotifyKernel(MediaCodeNotify _code);
    
    // IJitterBufferEvents overrides
    virtual BOOL OnReadPacket ();
    virtual void OnAbort      ();

    // dummy overrides
    virtual BOOL OnReadHeader () {return TRUE; }
    virtual void OnBufferFull () {}
    virtual void OnBufferEmpty() {}
    virtual void OnTerminate  () {}

public:

    CBufferId(ULONG _id, _SEMAPHORE *_pCodecSemaph); 
    virtual ~CBufferId();
    
    ULONG OutputBufferFull(MediaSpec _mediaSpec, BOOL *_pBufferFull);
    ULONG PktBufferFull(MediaSpec _mediaSpec, BOOL *_bPktBufFull);
    
    ULONG CreateBufferMedia(MediaSpec _mediaSpec);
    ULONG GetBufferMedia(CBufferMedia **_ppBufferMedia, MediaSpec _mediaSpec);

    ULONG StartPktProcessing(ThreadFunction _pFunc, void *_pCtx);
    ULONG FinishPktProcessing();

    ULONG CheckStartConditions(MediaSpec _mediaSpec, BOOL *_pbIdReadyToTransmit);   
    ULONG GetPacket(BYTE *_pBuf, ULONG *_pulBufLen, MediaSpec *_pMediaSpec);

    BOOL  ClockStarted();
    ULONG SetClockRef();
    ULONG UpdateTime(ULONG *_pulCurTime);

    ULONG GetJitterPktCount(ULONG *_pulJitterPktCount);
    ULONG SetPacket(BYTE *_pData, ULONG _ulDatalen);
    ULONG CheckForBuffering(MediaSpec _mediaSpec, BOOL & _bBufferingNeeded);

    void  SetAudioMute(BOOL _bMuteAudio) {m_bAudioMute = _bMuteAudio; }

    void  SetTransmission(CTransmission *_pTransmission) { m_pTransmission = _pTransmission; }

    ULONG Ok()                  { return m_bInit;               }
    ULONG GetId()               { return m_Id;                  }
    ULONG GetPktRcv()           { return m_ulPktRcv;            }
    BOOL  RunReadPktThread()    { return m_bRunReadPktThread;   }
};

#endif

