#ifndef CMEDIA_THREAD_WND_H
#define CMEDIA_THREAD_WND_H

#include "ISendKernelNotify.h"
#include "CBufferId.h"
#include "CVideoFrame.h"
#include "Data_reg.h"
#include "Demuxer.h"

class CVideo;
class CAudio;
class IMediaApi;
class IThread;

enum TransmissionType {AV_CONF, SIMPOSIUM};

class CTransmission : public ISendKernelNotify
{
private:

    typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

    friend class CBufferId;

    BOOL        m_bInit,
                m_bMediaSet,
                m_bBuffering,
                m_bDemuxNeeded,
                m_bRunBufferThread,
                m_bRunDemuxThread,
                m_bWaitingRebufferNotify;

    ULONG       m_ulLastAudioAlive,
                m_ulLastVideoAlive,
                m_ulBufferingPercent,
                m_ulLastBuffering,
                m_ulError;
    
    _SEMAPHORE  m_BufferSemaph,
                m_InitRebufferSemaph;

    CBufferId   m_BufferId; 
    
    CVideo    * m_pVideo;
    CAudio    * m_pAudio;

    IMediaApi *m_pAudioApi;
	
    IDemuxer  * m_pDemuxer;

    IThread   * m_pBufferThread,
              * m_pDemuxThread;

    ULONG AllocMedia(MediaSpec _mediaSpec, IMediaApi *_pMediaApi = NULL, TransmissionType _type = AV_CONF);
    BOOL NeedBuffering();

    ULONG WaitRebufferNotify();
    ULONG NotifyRebuffer();

    ULONG StartBufferThread();
    ULONG StopBufferThread();

    ULONG StartDemuxThread();
    ULONG StopDemuxThread();
      
    ULONG InitMedia();

    static int _ReadPkt(IThread *_pThread, void* _pCtx);		
    static int _BufferPkt(IThread *_pThread, void *_pCtx);
    static int _PlayVideo(IThread *_pThread, void* _pCtx);
    static int _WriteVideo(IThread *_pThread, void *_pCtx);
    static int _PlayAudio(IThread *_pThread, void* _pCtx);
    static int _ProcessAudio(IThread *_pThread, void *_pCtx);
    static int _DemuxStream(IThread *_pThread, void *_pCtx);

    // ISendKernelNotify overrides
    virtual ULONG NotifyKernel(MediaCodeNotify _code);

public: 
     
    CTransmission(ULONG _id, IMediaKernelNotify *_pNotify, IMediaApi *_pAudioApi, _SEMAPHORE *_pCodecSemaph);
    virtual ~CTransmission();
	
    ULONG CreateConferenceWnd();
    ULONG DestroyConferenceWnd();
    
    ULONG GetId(ULONG *_pulId);
    ULONG GetVideoReg(VideoFrame_reg ** _ppVideoFrameReg);
    ULONG SetPacket(BYTE *_pData, ULONG _ulDatalen);
    ULONG SetAudioMute(BOOL _bMuteAudio);
    ULONG SetDemuxStreamHeader(BYTE *_pHeader, ULONG _uHeaderLen);

    BOOL DemuxerRunning() {return m_bRunDemuxThread; }

    void GetMediaDbgInfo(MediaDbgInfo & _mediaDbgInfo);   
};

#endif
