#include "compat.h"
#include "MediaNetwork.h"
#include "global_error.h"
#include "media_utilities.h"
#include "media_codecs.h"
#include "AudioEncoder.h"
#include "VideoEncoder.h"
#include "CFrame.h"
#include "mediapkt.h"
#include "ithread.h"
#include "Media_const.h"

unsigned MediaNetwork::CheckParameters(std::string _transmission, 
                                       std::string _network, 
                                       std::string _bitrate)
{
    unsigned ret = RET_OK;

    if ( (_transmission != "conf") && 
         (_transmission != "ptp") )
    {
        ret = RET_INVALID_ARG;
    }
    
    if ( (_network != "satellite") && 
         (_network != "internet") )
    {
        ret = RET_INVALID_ARG;
    }

    if ( (_bitrate != "33k") && 
         (_bitrate != "64k") && 
         (_bitrate != "128k") && 
         (_bitrate != "256k") && 
         (_bitrate != "384k") &&
         (_bitrate != "512k") && 
         (_bitrate != "1024k") )
    {
        ret = RET_INVALID_ARG;
    }

    return ret;
}

unsigned AudioNetworkEncoder::Init(std::string _transmission, 
                                   std::string _network, 
                                   std::string _bitrate,
                                   std::string _codec)
{

    if (m_pCodec)
    {
        delete m_pCodec;
        m_pCodec = NULL;
    }

    CHECK_ERROR(CheckParameters(_transmission, _network, _bitrate), RET_OK)

    if (_codec == SPEEX_LO)
        m_pCodec = new SpeexLOEncoder(_transmission, _network, _bitrate);
    else
    if (_codec == SPEEX_HI)
        m_pCodec = new SpeexHIEncoder(_transmission, _network, _bitrate);
    else
    if (_codec == G711A)
        m_pCodec = new G711ALawEncoder(_transmission, _network, _bitrate); 
    else
    if (_codec == G711U)
        m_pCodec = new G711ULawEncoder(_transmission, _network, _bitrate); 
    else
    if (_codec == G723_53)
        m_pCodec = new G723_53_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == G723_63)
        m_pCodec = new G723_63_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == G728_96)
        m_pCodec = new G728_96_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == G728_128)
        m_pCodec = new G728_128_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == G728_160)
        m_pCodec = new G728_160_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == G729A)
        m_pCodec = new G729Encoder(_transmission, _network, _bitrate);
    else
    if  (_codec == AAC_48)
        m_pCodec = new AAC_48_Encoder(_transmission, _network, _bitrate);
    else
    if (_codec == AAC_128)
        m_pCodec = new AAC_128_Encoder(_transmission, _network, _bitrate);

    unsigned ret = RET_OK;

    if (m_pCodec)
    {
        m_pCodec->Init();
    
        if (m_pCodec->CodecOk() )
            m_Init = true;
    }
    else
        ret = RET_ERROR;

    return ret;
}

unsigned AudioNetworkEncoder::EncodePkt(CFrame & _frame, 
                                        unsigned _uMediaId, 
                                        unsigned _uSeq, 
                                        BYTE *_pBuf, 
                                        unsigned & _uBufLen, 
                                        unsigned & _uHeaderLen)
{
    if (!_pBuf || !_uBufLen)
        return RET_INVALID_ARG;

    AudioEncoder *pAudioEncoder = (AudioEncoder *)m_pCodec;

    BYTE *pFrameData;
    unsigned uFrameLen,
             ret;

    if ((ret = _frame.GetFrameData(&pFrameData, (ULONG *)&uFrameLen)) == RET_OK)
    {
        _uHeaderLen = sizeof(MediaPktExt);

        if (_uBufLen > MIN_AUDIO_BUF_TO_ENCODE_SIZE + _uHeaderLen)
        {
            BYTE *pEncoderOut;
            unsigned uEncoderOutLen;
        
            pEncoderOut    = _pBuf + _uHeaderLen;
            uEncoderOutLen = _uBufLen - _uHeaderLen;

            AudioCodecParam audioParam;
            memset(&audioParam, 0, sizeof(audioParam));

            if ((ret = pAudioEncoder->Encode(pFrameData, uFrameLen, pEncoderOut, uEncoderOutLen, &audioParam)) == RET_OK)
            {                
                MediaPktExt audioHeader;
                memset(&audioHeader, 0, sizeof(audioHeader));

                _uBufLen = _uHeaderLen + uEncoderOutLen;

                ULONG uTimestamp;
                _frame.GetTimestamp(&uTimestamp);

                audioHeader.id      = _uMediaId;
                audioHeader.seq     = _uSeq;
                audioHeader.datalen = _uBufLen - sizeof(MediaPkt);
                audioHeader.type    = audioParam.MPKT_type;
                audioHeader.flags   = PKT_AUDIO | PKT_REALTIME; 
                
                audioHeader.ulSubSeq      = m_ulSubSeq;
                audioHeader.dwMsTimestamp = uTimestamp;

                memcpy(_pBuf, &audioHeader, sizeof(audioHeader));
            
                m_ulSubSeq++;
            }
        }
    }

    return ret;
}

unsigned VideoNetworkEncoder::Init(std::string _transmission, 
                                   std::string _network, 
                                   std::string _bitrate,
                                   std::string _codec)
{
    CHECK_ERROR(CheckParameters(_transmission, _network, _bitrate), RET_OK)

    if (_codec == MPEG4)
        m_pCodec = new Mpeg4Encoder(_transmission, _network, _bitrate);
    else
    if  (_codec == H264)
        m_pCodec = new H264Encoder(_transmission, _network, _bitrate);  
   
    m_pCodec->Init();
    
    if (m_pCodec->CodecOk() )
        m_Init = true;

    return RET_OK;
}


unsigned VideoNetworkEncoder::EncodePkt(CFrame & _frame, 
                                        unsigned _uMediaId, 
                                        unsigned _uSeq, 
                                        BYTE *_pBuf, 
                                        unsigned & _uBufLen, 
                                        unsigned & _uHeaderLen)
{
    if (!_pBuf || !_uBufLen)
        return RET_INVALID_ARG;

    VideoEncoder *pVideoEncoder = (VideoEncoder *)m_pCodec;

    BYTE *pFrameData;
    unsigned uFrameLen,
             ret;

    if ((ret = _frame.GetFrameData(&pFrameData, (ULONG *)&uFrameLen)) == RET_OK)
    {
        _uHeaderLen = sizeof(MediaPktExt);

        if (_uBufLen > MIN_VIDEO_BUF_TO_ENCODE_SIZE + _uHeaderLen)
        { 
            BYTE *pEncoderOut;
            unsigned uEncoderOutLen;
        
            pEncoderOut    = _pBuf    + _uHeaderLen;
            uEncoderOutLen = _uBufLen - _uHeaderLen;

            VideoCodecParam videoParam;
            memset(&videoParam, 0, sizeof(videoParam));

            if ((ret = pVideoEncoder->Encode(pFrameData, uFrameLen, pEncoderOut, uEncoderOutLen, &videoParam)) == RET_OK)
            {                
                MediaPktExt videoHeader;
                memset(&videoHeader, 0, sizeof(videoHeader));

                _uBufLen = _uHeaderLen + uEncoderOutLen;

                ULONG uTimestamp;
                _frame.GetTimestamp(&uTimestamp);

                videoHeader.id      = _uMediaId;
                videoHeader.seq     = _uSeq;
                videoHeader.datalen = _uBufLen - sizeof(MediaPkt);
                videoHeader.type    = videoParam.MPKT_type;
                
                if (videoParam.MPKT_keyFrame)
                    videoHeader.flags |= PKT_KEYFRAME;

                videoHeader.flags  |= PKT_VIDEO; 
                
                videoHeader.ulSubSeq      = m_ulSubSeq;
                videoHeader.dwMsTimestamp = uTimestamp;

                memcpy(_pBuf, &videoHeader, sizeof(videoHeader));

                m_ulSubSeq++;
            }    
        }
    }

    return RET_OK;
}


unsigned MediaEncoder::SetFrameSrc(IFrameSrc *_pFrameSrc)
{
    m_pFrameSrc = _pFrameSrc;
    return RET_OK;
}

unsigned MediaEncoder::SetEncoder(std::string _transmission, 
                                  std::string _network, 
                                  std::string _bitrate, 
                                  std::string _codec, 
                                  MediaSpec _mediaSpec)
{
    switch (_mediaSpec)
    {
        case VIDEO:
            m_VideoEncoder.Init(_transmission, _network, _bitrate, _codec);
        break;
    
        case AUDIO:
            m_AudioEncoder.Init(_transmission, _network, _bitrate, _codec);    
        break;

        default:
            return RET_ERROR;
    }

    return RET_OK;
}

void MediaEncoder::EncodeAndTransmit(bool & _mediaRunning, 
                                     MediaParamRcvFrame & rcvFrame, 
                                     MediaParamSndPkt & sndPkt, 
                                     MediaNetwork & _mediaNetwork)
{


}


int MediaEncoder::VideoEncoderThread(IThread *_pThread, void *_pParam)
{
    MediaEncoder * pCtx = (MediaEncoder *) _pParam;

    MediaParamRcvFrame rcvFrame;
    memset(&rcvFrame, 0, sizeof(rcvFrame));

    rcvFrame.id   = pCtx->m_uMediaId;
    rcvFrame.code = MN_READ_VIDEO_FRAME;

    MediaParamSndPkt sndPkt;
    memset(&sndPkt, 0, sizeof(sndPkt));

    sndPkt.id   = pCtx->m_uMediaId;
    sndPkt.code = MN_WRITE_PACKET;

    BYTE encodedData[MAX_ENCODED_VIDEO_SIZE];

    unsigned uHeaderLen, 
             uEncodedDataLen,
             uSubSeq;

    uSubSeq = 0;

    VideoEncoder * pVideoEncoder = (VideoEncoder *) pCtx->m_VideoEncoder.GetCodec(); 

    while (pCtx->m_flVideoRunning)
    {    
        pCtx->m_pFrameSrc->OnMediaNotify(&rcvFrame);
        
        if (rcvFrame.pFrame)
        {
            uEncodedDataLen = MAX_ENCODED_VIDEO_SIZE;

            if (pCtx->m_VideoEncoder.EncodePkt(*(rcvFrame.pFrame), pCtx->m_uMediaId, uSubSeq, encodedData, uEncodedDataLen, uHeaderLen) == RET_OK)
            {
                sndPkt.buf     = encodedData;
                sndPkt.uBufLen = uEncodedDataLen;

                pCtx->m_pFrameSrc->OnMediaNotify(&sndPkt);
                
                uSubSeq++;
                Wait( unsigned (1000.0 /  pVideoEncoder->GetFramerate()));
            }
        }
		else
		    Wait(10);
    }

    return RET_OK;
}

int MediaEncoder::AudioEncoderThread(IThread *_pThread, void *_pParam)
{
    MediaEncoder * pCtx = (MediaEncoder *) _pParam;

    while (pCtx->m_flAudioRunning)
    {

    }

    return RET_OK;
}

unsigned MediaEncoder::StartEncoding(MediaSpec _mediaSpec)
{
    unsigned ret = RET_OK;

    if (m_pFrameSrc)
    {
        if ( (_mediaSpec == VIDEO) && !m_flVideoRunning)
        {    
            m_flVideoRunning = true;

            if (!(m_pVideoThread = CreateIThread(VideoEncoderThread, NULL, TRUE)))
            {     
                m_flVideoRunning = false;
                ret = RET_ERROR;
            }
        }
        else
        if ((_mediaSpec == AUDIO) && !m_flAudioRunning)
        {    
            m_flAudioRunning = true;

            if (!(m_pAudioThread = CreateIThread(AudioEncoderThread, NULL, TRUE)))
            {    
                m_flAudioRunning = false;
                ret = RET_ERROR;
            }
        }
        else
            ret = RET_ERROR;
    }
    else
        ret = RET_INIT_ERROR;

    return ret;
}

