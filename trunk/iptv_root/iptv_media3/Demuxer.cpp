#include "Demuxer.h"
#include "DemuxedMediaType.h"
#include "global_error.h"
#include "media_utilities.h"
#include "mediapkt.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"

IDemuxer::IDemuxer() : m_uCurSeq(0),
                       m_bInit(false),
                       m_bHeaderSet(false) 
{ 
    memset( &m_urlProtocol, 0, sizeof (URLProtocol) );
    memset( &m_urlCtx,      0, sizeof (URLContext)  );
}

ULONG IDemuxer::SetHeader(BYTE *_pHeader, ULONG _uHeaderLen)
{
    ULONG cbWrite;

    CHECK_ERROR(m_StreamBuf.WriteData(_pHeader, _uHeaderLen, cbWrite), RET_OK)
    CHECK_ERROR(m_StreamHeader.SetData(_pHeader, _uHeaderLen), RET_OK)

    m_bHeaderSet = TRUE;

    return RET_OK;
}

ULONG IDemuxer::GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen)
{
    if (!_ppHeader)
        return RET_INVALID_ARG;

    if (!m_bHeaderSet)
        return RET_ERROR;

    *_ppHeader  = m_StreamHeader.GetDataPtr();
    _uHeaderLen = m_StreamHeader.Size();  
 
    return RET_OK;
}

ULONG IDemuxer::GetNextFrame(ULONG _id, BYTE **_ppData, ULONG & _uDatalen, MediaSpec & _mediaSpec)
{
    _mediaSpec = NONE;

    if (!m_bInit)
    {
        return RET_INIT_ERROR;
    }

    if (!_ppData)
    {        
        return RET_INVALID_ARG;
    }

    *_ppData = NULL;

    AVPacket pkt;
    if ( av_read_frame(m_pFmtCtx, &pkt) != 0)
    {
        return RET_ERROR;
    }

    _uDatalen = pkt.size + sizeof(MediaPktExt);

    m_DemuxedFrame.SetSize(_uDatalen);

    if (m_DemuxedFrame.Size() != _uDatalen)
    {    
        _uDatalen = 0;
        return RET_ERROR;
    }

    BYTE *pData = m_DemuxedFrame.GetDataPtr();

    MediaPktExt *pMediaPktExt = (MediaPktExt *) pData;
    memset(pMediaPktExt, 0, sizeof(MediaPktExt));

    BYTE *pPayload = &pData[sizeof(MediaPktExt)]; 
    memcpy(pPayload, pkt.data, pkt.size);

    pMediaPktExt->id      = _id;
    pMediaPktExt->seq     = m_uCurSeq++;
    pMediaPktExt->datalen = sizeof(MediaPktExt) - sizeof(MediaPkt) + pkt.size; 

    pMediaPktExt->dwMsTimestamp = pkt.pts;

    AVCodecContext *pCodecCtx = m_pFmtCtx->streams[pkt.stream_index]->codec;

    if (pCodecCtx->codec_type == CODEC_TYPE_AUDIO)
    {
        pMediaPktExt->flags = PKT_AUDIO;
        pMediaPktExt->type  = ASF_DEMUXED_AUDIO;
        _mediaSpec = AUDIO;
    }
    else
    if (pCodecCtx->codec_type == CODEC_TYPE_VIDEO)
    {
        pMediaPktExt->flags = PKT_VIDEO | PKT_KEYFRAME;
        pMediaPktExt->type = ASF_DEMUXED_VIDEO;
        _mediaSpec = VIDEO;
    }
    else
    {
        return RET_ERROR;
    } 

    bool bIndexFound = false;
 
    for (unsigned index=0; index<m_DemuxedPktInfoList.size(); index++)
    {
        if (m_DemuxedPktInfoList[index].m_iStreamIndex == pkt.stream_index)
        {
            bIndexFound = true;
            pMediaPktExt->ulSubSeq = ++m_DemuxedPktInfoList[index].m_uSubSeq;
            break;
        }
    }

    if (!bIndexFound)
    {
        DemuxedPktInfo pktInfo(pkt.stream_index, 0, _mediaSpec);
        m_DemuxedPktInfoList.push_back(pktInfo);

        pMediaPktExt->ulSubSeq = pktInfo.m_uSubSeq;
    }

    *_ppData = pData;

    return RET_OK;
}

ULONG IDemuxer::SetPacket(BYTE *_pPkt, ULONG _uPktLen, ULONG _uHeaderLen)
{
    if (!_pPkt || !_uPktLen)
        return RET_INVALID_ARG;

    BYTE *pData = &_pPkt[_uHeaderLen];

    ULONG cbWrite,
          cbToWrite;

    cbToWrite = _uPktLen - _uHeaderLen;

    CHECK_ERROR(m_StreamBuf.WriteData(pData, cbToWrite, cbWrite), RET_OK)

    return RET_OK;
}

//--------------------------------ASFDemuxer methods definition--------------------------------//

ULONG ASFDemuxer::Init()
{
    unsigned ret = RET_OK;

    av_register_all();

    m_urlProtocol.url_open  = OpenASF;
    m_urlProtocol.url_read  = ReadASF;
    m_urlProtocol.url_close = CloseASF;

    m_urlCtx.flags = URL_RDONLY;
    m_urlCtx.is_streamed = 1;
    m_urlCtx.prot = &m_urlProtocol;
    m_urlCtx.max_packet_size = 0;
    m_urlCtx.priv_data = this;

    memset(&m_byteCtx, 0, sizeof(m_byteCtx));

    url_fdopen(&m_byteCtx, &m_urlCtx);

    BYTE *pBufTmp = new BYTE[m_byteCtx.buffer_size];

    if (pBufTmp)
    {
        get_buffer(&m_byteCtx, pBufTmp, 2048);
        delete pBufTmp;

        m_byteCtx.buf_ptr = m_byteCtx.buffer;

        AVFormatParameters fmtParam;
        memset(&fmtParam, 0, sizeof(AVFormatParameters));

        fmtParam.initial_pause = 1; /* we force a pause when starting an RTSP stream */
        fmtParam.width  = 0;
        fmtParam.height = 0;
        fmtParam.time_base.num = 1;
        fmtParam.time_base.den = 25;
        fmtParam.pix_fmt = PIX_FMT_NONE;

        AVInputFormat* pInputFmt = av_find_input_format("asf");

        if (pInputFmt)
        {
            if (av_open_input_stream(&m_pFmtCtx, &m_byteCtx, "", pInputFmt, &fmtParam) == 0)
            {
                m_bInit = true;
            }
            else
            {
                ret = RET_ERROR;
            }
        }
        else
        { 
            ret = RET_ERROR;
        }
    }
    else
    {
        ret = RET_LOW_MEMORY;
    }

    return ret;
}

Decoder *ASFDemuxer::GetDecoder(MediaSpec _mediaSpec)
{
    if (!m_bInit)
        return NULL;

    AVCodecContext *pCodecCtx;

    Decoder *pASFDecoder = NULL;

    for (int index=0; index < m_DemuxedPktInfoList.size(); index++)
    {
        if (m_DemuxedPktInfoList[index].m_mediaSpec == _mediaSpec)
        {
            int iStreamIndex = m_DemuxedPktInfoList[index].m_iStreamIndex;
            AVCodecContext *pCodecCtx = m_pFmtCtx->streams[iStreamIndex]->codec;
 
            if (_mediaSpec == VIDEO)
            {
                pASFDecoder = new ASFVideoDecoder(pCodecCtx);
            }
            else
            if (_mediaSpec == AUDIO)
            {
                pASFDecoder = new ASFAudioDecoder(pCodecCtx);
            }

            break;
        }
    }

    if (!pASFDecoder)
    { 
        return NULL;
    }

    if (pASFDecoder->Init() != RET_OK)
    {
        delete pASFDecoder;
        return NULL;
    }

    return pASFDecoder;
}

int ASFDemuxer::OpenASF(URLContext *h, const char *_pUrlPath, int flags) 
{
    return 0;
}

int ASFDemuxer::CloseASF(URLContext *h) 
{
    return 0;
}

int ASFDemuxer::ReadASF(URLContext *h, unsigned char *buf, int size)
{
    ASFDemuxer * pDemuxer =  (ASFDemuxer *) h->priv_data;

    int ret = -1;

    ULONG cbRead;
    pDemuxer->m_StreamBuf.ReadData(buf, size, cbRead);

    if (cbRead)   
    {    
        ret = cbRead;
    }
 
    return ret;
}
