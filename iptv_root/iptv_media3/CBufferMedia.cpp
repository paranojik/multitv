#include "compat.h"
#include "CBufferMedia.h"
#include "CVideoFrame.h"
#include "CAudioFrame.h"
#include "global_error.h"
#include "mediapkt.h"
#include "Global_functions.h"
#include "Media_const.h"
#include "COutputBuffer.h"
#include "CInputPkt.h"
#include "CPkt.h"
#include "media_utilities.h"
#include "Codec.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "MediaNetwork.h"
#include "IKernelNotify.h"
#include "AutoLock.h"

CBufferMedia::CBufferMedia(MediaSpec _mediaSpec, ULONG _ulMaxInputPktQueue, ULONG _ulMaxOutputMediaQueue, _SEMAPHORE *_pCodecSemaph) : 
    m_DecoderSemaph(),
    m_OutputBuffer(_ulMaxOutputMediaQueue),
    m_InputPkt(_ulMaxInputPktQueue),
    m_bInit(FALSE),
    m_pDecoder(NULL),
    m_MediaSpec(_mediaSpec),
    m_pCodecSemaph(_pCodecSemaph)
{

    if (m_pCodecSemaph)
        m_bInit = TRUE;
}


CBufferMedia::~CBufferMedia()
{
    if (m_pDecoder)
        delete m_pDecoder;
}

ULONG CBufferMedia::InitDecoder(BYTE _mediaType)
{
    if (!m_bInit)        
        return RET_INIT_ERROR;
 
    m_pCodecSemaph->Wait(); 

    ULONG ret = RET_OK;

    if (m_pDecoder)
    {    
        delete m_pDecoder;
        m_pDecoder = NULL;
    }

    switch (_mediaType)
    {
        case MTYPE_MEDIA_VIDEO_MPEG4_320x240:
        case MTYPE_MEDIA_VIDEO_MPEG4_176x144:
        case MTYPE_MEDIA_VIDEO_H264_QCIF:
        case MTYPE_MEDIA_VIDEO_H264_QVGA:
            m_pDecoder = new VideoDecoder(_mediaType);
        break;

        case MTYPE_MEDIA_AUDIO_SPEEX_3F:
            m_pDecoder = new SpeexDecoder(_mediaType);
        break;

        case MTYPE_MEDIA_AUDIO_G711_ULAW:
        case MTYPE_MEDIA_AUDIO_G711_ALAW:
            m_pDecoder = new G711Decoder(_mediaType);
        break;	

        case MTYPE_MEDIA_AUDIO_G723_2F:
            m_pDecoder = new G723Decoder(_mediaType);        
        break;		

        case MTYPE_MEDIA_AUDIO_G728_4F:
            m_pDecoder = new G728Decoder(_mediaType);
        break;

        case MTYPE_MEDIA_AUDIO_G729A_6F:
            m_pDecoder = new G729Decoder(_mediaType);
        break;		
						
        case MTYPE_MEDIA_AUDIO_AAC_48:
        case MTYPE_MEDIA_AUDIO_AAC_128:
            m_pDecoder = new AACDecoder(_mediaType); 
        break;		
			
        default:
            ret = RET_INVALID_ARG;
    }

    if (m_pDecoder)
    {
        if (m_pDecoder->Init() != RET_OK)
            ret = RET_ERROR;
    }
    else
    if (ret == RET_OK) 
        ret = RET_LOW_MEMORY;

    m_pCodecSemaph->Signal();

    return ret;
}


ULONG CBufferMedia::SetPacket(BYTE *_buf, ULONG _bufSize, MediaSpec _mediaSpec)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_buf || !_bufSize)
        return RET_INVALID_ARG;

    if (_mediaSpec != m_MediaSpec)        return RET_MEDIA_SPEC_MISMATCH;

    CHECK_ERROR(m_InputPkt.SetPacket(_buf, _bufSize), RET_OK);

    return RET_OK;
};


BOOL CBufferMedia::InputBufferFull()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    return m_InputPkt.BufferFull();
}


ULONG CBufferMedia::GetOutputBuffer(COutputBuffer ** _ppOutputBuffer)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
	
    if (!_ppOutputBuffer)
        return RET_LOW_MEMORY;

    *_ppOutputBuffer = &m_OutputBuffer;

    return RET_OK;
}

ULONG CBufferMedia::GetPktCount(ULONG *_pulPktCount)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
	
    if (!_pulPktCount)
        return RET_INVALID_ARG;

    *_pulPktCount = m_InputPkt.GetPktCount();

    return RET_OK;
}

ULONG CBufferMedia::GetFramesCount(ULONG *_pulFramesCount)
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
	
    if (!_pulFramesCount)
        return RET_INVALID_ARG;

    *_pulFramesCount = m_OutputBuffer.GetFramesCount();
	
    return RET_OK;
}

ULONG CBufferMedia::EncodedFrameAvailable(BOOL *_pbEncFrameAvailable)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    if (!_pbEncFrameAvailable)
        return RET_INVALID_ARG;
	
    *_pbEncFrameAvailable = FALSE;
	
    CHECK_ERROR(m_InputPkt.EncodedFrameAvailable(_pbEncFrameAvailable), RET_OK)

    return RET_OK;
}

ULONG CBufferMedia::ResetCodec()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    m_DecoderSemaph.Wait();
    
    if (m_pDecoder)
    {
        delete m_pDecoder;
        m_pDecoder = NULL;
    }

    m_DecoderSemaph.Signal();

    return RET_OK;
}

ULONG CBufferMedia::GetNextPacket(CPkt **_ppPkt)
{
    if (!_ppPkt)
        return RET_INVALID_ARG;

    *_ppPkt = NULL;

    BOOL bEncFrameAvailable;
    CHECK_ERROR(EncodedFrameAvailable(&bEncFrameAvailable), RET_OK)
	
    if (!bEncFrameAvailable)
        return RET_NO_FRAME_AVAILABLE;

    ULONG ret = RET_ERROR;;

    CPkt *pPkt;
    m_InputPkt.GetEncodedPkt(&pPkt);

    if (pPkt)
    {
        if (pPkt->PacketOk())
        {    
            *_ppPkt = pPkt;
            ret = RET_OK;
        }
        else
        {
            delete pPkt;
        }
    }

    return ret;
}
ULONG CBufferMedia::ReloadDecoder(BYTE _mediaType)
{
    ULONG ret = RET_OK;

    if (m_pDecoder)
    {
         if (_mediaType != m_pDecoder->GetMediaType())					
             ret = InitDecoder(_mediaType);	
    }
    else	
    {    
        ret = InitDecoder(_mediaType);
    }

    return ret;
}

ULONG CBufferVideo::Decode()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    AutoLock autoLock(m_DecoderSemaph);

    ULONG ret = RET_OK;

    CPkt *pPkt;

    CHECK_ERROR(GetNextPacket(&pPkt), RET_OK)
    CHECK_ERROR(ReloadDecoder(pPkt->GetType()), RET_OK)

    VideoCodecParam videoParam;
    memset(&videoParam, 0, sizeof(VideoCodecParam));

    if (pPkt->GetFlags() & PKT_KEYFRAME)
        videoParam.MPKT_keyFrame = true;   

    VideoDecoder *pVideoDecoder = (VideoDecoder *) m_pDecoder;

    ULONG ulDecodedFrameSize = pVideoDecoder->GetFrameSize();

    m_DecodedFrameBuf.SetSize(ulDecodedFrameSize);

    ret = m_pDecoder->Decode(pPkt->GetPayload(),
                             (unsigned int) pPkt->GetPayloadLen(),
                             m_DecodedFrameBuf.GetDataPtr(),
                             (unsigned int &)ulDecodedFrameSize,
                             (void *)&videoParam);

    CVideoFrame *pFrame;
    pFrame = new CVideoFrame(m_DecodedFrameBuf.GetDataPtr(), 
                             m_DecodedFrameBuf.Size(), 
                             pPkt->GetTimestamp(),
                             pVideoDecoder->GetWidth(), 
                             pVideoDecoder->GetHeight(), 
                             RGB24);

    if (pFrame)
    {    
            static int iFrameId = 0;
            pFrame->SetId(++iFrameId);

            ret = m_OutputBuffer.SetFrame(pFrame);
    }

    if (pPkt)
        delete pPkt;

    return ret;
}

ULONG CBufferAudio::Decode()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    AutoLock autoLock(m_DecoderSemaph);

    ULONG ret = RET_OK;

    CPkt *pPkt;

    CHECK_ERROR(GetNextPacket(&pPkt), RET_OK)
    CHECK_ERROR(ReloadDecoder(pPkt->GetType()), RET_OK)

    ULONG ulDecodedFrameSize = MAX_DECODED_FRAME_SIZE;
    m_DecodedFrameBuf.SetSize(ulDecodedFrameSize);

    AudioDecoder *pAudioDecoder = (AudioDecoder *) m_pDecoder;

    ret = pAudioDecoder->Decode(pPkt->GetPayload(),
                                (unsigned int) pPkt->GetPayloadLen(),
                                m_DecodedFrameBuf.GetDataPtr(),
                                (unsigned int &)ulDecodedFrameSize, NULL);

    CAudioFrame *pFrame;
    pFrame = new CAudioFrame(m_DecodedFrameBuf.GetDataPtr(), 
                             ulDecodedFrameSize, 
                             pPkt->GetTimestamp(), 
                             pAudioDecoder->GetChannels(), 
                             pAudioDecoder->GetSampleRate(), 2);

    if (pFrame)
    {    
        static int iFrameId = 0;
        pFrame->SetId(++iFrameId);

        ret = m_OutputBuffer.SetFrame(pFrame);
    }

    if (pPkt)
        delete pPkt;

    return ret;
}



