#ifndef CBUFFER_MEDIA_H
#define CBUFFER_MEDIA_H

#include "COutputBuffer.h"
#include "CInputPkt.h"
#include "media_utilities.h"
#include "IKernelNotify.h"
#include "BufManager.h"

class Decoder;
class COutputBuffer;
class _SEMAPHORE;

class CBufferMedia
{
protected:

    BOOL          m_bInit;  
    MediaSpec	  m_MediaSpec;

    COutputBuffer m_OutputBuffer;    
    CInputPkt	  m_InputPkt;

    _SEMAPHORE	* m_pCodecSemaph, 
                  m_DecoderSemaph;
	        
    BufManager m_DecodedFrameBuf;
    Decoder  * m_pDecoder;

    ULONG GetNextPacket(CPkt **_ppPkt);
    ULONG ReloadDecoder(BYTE _mediaType);
    ULONG InitDecoder(BYTE _CompressionType);  

public:

    CBufferMedia(MediaSpec _mediaSpec, ULONG _ulMaxInputPktQueue, ULONG _ulMaxOutputMediaQueue, _SEMAPHORE *_pCodecSemaph);
    virtual ~CBufferMedia();

    virtual ULONG Decode() = 0;

    ULONG ResetCodec();     
    ULONG GetOutputBuffer(COutputBuffer ** _ppOutputBuffer);
    ULONG SetPacket(BYTE *_buf, ULONG _bufSize, MediaSpec mediaSpec);
    ULONG GetPktCount(ULONG *_pulPktCount);
    ULONG GetFramesCount(ULONG *_pulFramesCount);
    ULONG EncodedFrameAvailable(BOOL *_pbEncFrameAvailable);

    BOOL InputBufferFull();

    void SetDecoder(Decoder *_pDecoder) {m_pDecoder = _pDecoder; }

    MediaSpec GetMediaSpec() {return m_MediaSpec; } 
};

class CBufferVideo : public CBufferMedia
{

public:

    CBufferVideo(ULONG _ulMaxInputPktQueue, 
                 ULONG _ulMaxOutputMediaQueue, 
                 _SEMAPHORE *_pCodecSemaph) : CBufferMedia(VIDEO, _ulMaxInputPktQueue, _ulMaxOutputMediaQueue, _pCodecSemaph) {}

    virtual ~CBufferVideo() {}

    ULONG Decode();

};

class CBufferAudio : public CBufferMedia
{

public:

    CBufferAudio(ULONG _ulMaxInputPktQueue, 
                 ULONG _ulMaxOutputMediaQueue, 
                 _SEMAPHORE *_pCodecSemaph) : CBufferMedia(AUDIO, _ulMaxInputPktQueue, _ulMaxOutputMediaQueue, _pCodecSemaph) {}

    virtual ~CBufferAudio() {}

    ULONG Decode();
};

#endif
