#include "compat.h"
#include "VideoEncoder.h"
#include "global_error.h"
#include "parseString.h"
#include "MediaNetwork.h"
#include "mediapkt.h"
#include "media_utilities.h"

VideoEncoder::VideoEncoder(std::string _transmission, 
                           std::string _network, 
                           std::string _bitrate) : Encoder(_transmission, _network, _bitrate)
{
    m_pYuvPicture = avcodec_alloc_frame();
    m_pRgbPicture = avcodec_alloc_frame();

    m_pYuvPicBuf  = NULL;
    m_pRgbPicBuf  = NULL;
}

VideoEncoder::~VideoEncoder()
{
    if (m_pYuvPicture)
        av_free(m_pYuvPicture);

    if (m_pRgbPicture)
        av_free(m_pRgbPicture);

    if (m_pYuvPicBuf)
        delete m_pYuvPicBuf;

    if (m_pRgbPicBuf)
        delete m_pRgbPicBuf;
}

unsigned VideoEncoder::GetWidth()
{
    if (CodecOk())
        return m_pCodecCtx->width;
    else
        return 0;
}

unsigned VideoEncoder::GetHeight()
{
    if (CodecOk())
        return m_pCodecCtx->height;
    else
        return 0;
}


double VideoEncoder::GetFramerate()
{
    if (CodecOk())
        return ((double) m_pCodecCtx->time_base.den) / m_pCodecCtx->time_base.num;
    else
        return 0;
}

unsigned VideoEncoder::GetFrameSize()
{
    return 0;
}

unsigned VideoEncoder::InitVideoEncoder()
{
    if (!m_pCodecCtx)
        return RET_INIT_ERROR;
        
    unsigned uYuvBufSize,
             uRgbBufSize;

    uYuvBufSize = avpicture_get_size(m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height);
    uRgbBufSize = avpicture_get_size(PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

    m_pYuvPicBuf  = new BYTE[uYuvBufSize];    
    m_pRgbPicBuf  = new BYTE[uRgbBufSize];

    avpicture_fill((AVPicture *)m_pYuvPicture, m_pYuvPicBuf, m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height);
    avpicture_fill((AVPicture *)m_pRgbPicture, m_pRgbPicBuf, PIX_FMT_RGB24, m_pCodecCtx->width, m_pCodecCtx->height);

    return RET_OK;
}

unsigned VideoEncoder::InitCommonParameters(CParseString & _parseString)
{
    ParseFieldValue value;

    if ((value = _parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("bit_rate_tolerance")).m_valueOK) m_pCodecCtx->bit_rate_tolerance = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("gop")).m_valueOK) m_pCodecCtx->gop_size = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("rc_max")).m_valueOK) m_pCodecCtx->rc_max_rate = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("rc_min")).m_valueOK) m_pCodecCtx->rc_min_rate = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("rc_buffer_size")).m_valueOK) m_pCodecCtx->rc_buffer_size = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("rc_buffer_aggressivity")).m_valueOK) m_pCodecCtx->rc_buffer_aggressivity = (float)value.m_value;
    if ((value = _parseString.GetFieldValue("qsquish")).m_valueOK) m_pCodecCtx->rc_qsquish = (float)value.m_value;
    if ((value = _parseString.GetFieldValue("qmin")).m_valueOK) m_pCodecCtx->qmin = (int)value.m_value;
    if ((value = _parseString.GetFieldValue("qmax")).m_valueOK) m_pCodecCtx->qmax = (int)value.m_value;

    return RET_OK;
}

unsigned VideoEncoder::InitFormat(DsCScriptValue & _parameters)
{
    m_MediaType  = (unsigned char)_parameters.GetTableValue("payload_type").GetInt();

    m_pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
    m_pCodecCtx->width   = (int)_parameters.GetTableValue("format").GetTableValue("size").GetTableValue("width").GetInt();
    m_pCodecCtx->height  = (int)_parameters.GetTableValue("format").GetTableValue("size").GetTableValue("height").GetInt();
            
    m_pCodecCtx->time_base.den = (int)_parameters.GetTableValue("format").GetTableValue("frame_rate").GetInt();
            
    if ( (m_pCodecCtx->time_base.den == 5) || 
         (m_pCodecCtx->time_base.den == 10)|| 
         (m_pCodecCtx->time_base.den == 15) )
    {
        m_pCodecCtx->time_base.num = 1;
    }
    else
        m_pCodecCtx->time_base.num = (int)_parameters.GetTableValue("format").GetTableValue("frame_rate_base").GetInt();

    return RET_OK;
}

unsigned VideoEncoder::Encode(unsigned char *_pBufIn, 
                              unsigned _bufInLen, 
                              unsigned char *_pBufOut, 
                              unsigned & _bufOutLen, 
                              void *_pHeaderData)
{
    if (!_pBufIn || !_bufInLen || !_pBufOut)
        return RET_INVALID_ARG;

    if (!m_pCodecCtx)
        return RET_INIT_ERROR;
    
    memcpy(m_pRgbPicBuf, _pBufIn, _bufInLen);

    img_convert((AVPicture *)m_pYuvPicture, 
                m_pCodecCtx->pix_fmt,
                (AVPicture *)m_pRgbPicture, 
                PIX_FMT_RGB24, 
                m_pCodecCtx->width, 
                m_pCodecCtx->height);

    int ret;
    ret = avcodec_encode_video(m_pCodecCtx, _pBufOut, _bufOutLen,  m_pYuvPicture);

    if (ret >= 0)
    {
        VideoCodecParam *pVideoParam = (VideoCodecParam *) _pHeaderData;

        _bufOutLen = ret;

        if (m_pCodecCtx->coded_frame->key_frame)
            pVideoParam->MPKT_keyFrame = true;
        else
            pVideoParam->MPKT_keyFrame = false;

        pVideoParam->MPKT_type = m_MediaType;

        ret = RET_OK;
    }
    else
    {    
        _bufOutLen = 0;
        ret = RET_ERROR;
    }

    return ret;
}

unsigned Mpeg4Encoder::Init()
{
    ULONG ret = RET_OK;

    if (!m_Init)
    {
        DsCScriptValue tableParam;

        CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)
        
        DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
            
        std::string stringToParse = codec_properties.GetStr();
        CParseString parseString(stringToParse, ':' );
    
        InitCommonParameters(parseString);
            
        ParseFieldValue value;
        if ((value = parseString.GetFieldValue("mpeg_quant")).m_valueOK) m_pCodecCtx->mpeg_quant = (int)value.m_value;            

        value = parseString.GetFieldValue("4mv");
        if (value.m_valueOK && value.m_booleanValue)
        {
            m_pCodecCtx->flags |= CODEC_FLAG_4MV;
        }
                
        /*value = parseString.GetFieldValue("part");
        if (value.m_valueOK && value.m_booleanValue)
        {
            m_pCodecCtx->flags |= CODEC_FLAG_PART;
        }*/

        value = parseString.GetFieldValue("gmc");
        if (value.m_valueOK && value.m_booleanValue)
        {
            m_pCodecCtx->flags |= CODEC_FLAG_GMC;
        }

        value = parseString.GetFieldValue("mv0");

        if (value.m_valueOK && value.m_booleanValue)
        {
            m_pCodecCtx->flags |= CODEC_FLAG_MV0;
        }

        value = parseString.GetFieldValue("trellis");
        if (value.m_valueOK && value.m_booleanValue)
        {
            m_pCodecCtx->flags |= CODEC_FLAG_TRELLIS_QUANT;
        }

        InitFormat(tableParam);
        InitVideoEncoder();

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return ret;
}


unsigned H264Encoder::Init()
{
    ULONG ret = RET_OK;

    if (!m_Init)
    {
        DsCScriptValue tableParam;
     
        CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

        DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
            
        std::string stringToParse = codec_properties.GetStr();
        CParseString parseString(stringToParse, ':' );

        InitCommonParameters(parseString);

        ParseFieldValue value;
        if ((value = parseString.GetFieldValue("coder")).m_valueOK) m_pCodecCtx->coder_type = (int)value.m_value;    
          
        InitFormat(tableParam);			
        InitVideoEncoder();

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return ret;
}
