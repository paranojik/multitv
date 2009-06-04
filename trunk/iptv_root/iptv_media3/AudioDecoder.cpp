#include "compat.h"
#include "AudioDecoder.h"
#include "global_error.h"
#include "media_utilities.h"
#include "log.h"

unsigned AudioDecoder::GetSampleRate()
{
    if (CodecOk())
        return m_pCodecCtx->sample_rate;
    else
        return 0;
}

unsigned AudioDecoder::GetSampleSize()
{
    if (CodecOk())
    {
        switch(m_pCodecCtx->sample_fmt)
        {
            case SAMPLE_FMT_U8:
            return 1;

            case SAMPLE_FMT_S16:
            return 2;

            case SAMPLE_FMT_S24:
            return 3;

            case SAMPLE_FMT_S32:
            return 4;

            default:
                return 0;
        }
    }
    else
        return 0;
}

unsigned AudioDecoder::GetFrameSamples()
{
    if (CodecOk())
        return m_pCodecCtx->frame_size;
    else
        return 0;
}

unsigned AudioDecoder::GetChannels()
{
    if (CodecOk())
        return m_pCodecCtx->channels;
    else
        return 0;
}

unsigned AudioDecoder::Decode(unsigned char *_pBufIn, 
                              unsigned _bufInLen, 
                              unsigned char *_pBufOut, 
                              unsigned & _bufOutLen, 
                              void *_pHeaderData)
{
    if (!_pBufIn || !_bufInLen || !_pBufOut || !_bufOutLen)
        return RET_INVALID_ARG;

    if (!m_Init)
        return RET_INIT_ERROR;

    int iBytesRead,
        iFrameSize,
        iEncodedFrameSize,
        ret;

    ret = RET_OK;

    _bufOutLen = 0;

    iEncodedFrameSize = _bufInLen / m_uFramesPkt;

    int16_t *pDecodedSamples = (int16_t *)_pBufOut;

    BYTE *pAudioSliceIn;

    for (unsigned uPos=0; uPos < m_uFramesPkt; uPos++)
    {        
        pAudioSliceIn  = &_pBufIn[uPos*iEncodedFrameSize];

        iBytesRead = avcodec_decode_audio(m_pCodecCtx, 
                                          &pDecodedSamples[uPos*m_pCodecCtx->frame_size],
                                          &iFrameSize,
                                          pAudioSliceIn, 
                                          iEncodedFrameSize);
        
        if (iBytesRead < 0)
        {
            ret = RET_ERROR;
            break;
        }
        else
            _bufOutLen += iFrameSize;
    }

    return ret;
}

//----------------------------------------------//
// Speex decoder class definiton
//----------------------------------------------//

unsigned SpeexDecoder::Init()
{    
    if (!m_Init)
    {
        DsCScriptValue tableParam;
       
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        m_pCodecCtx->channels    = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
    
        m_uFramesPkt = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();

        if (tableParam.GetTableValue("codec_properties").GetStr().Equals("enh"))
            m_pCodecCtx->flags2 |= CODEC_FLAG2_SPEEX_ENH;

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }    

    return RET_OK;
}

//----------------------------------------------//
// G711 decoder class definiton
//----------------------------------------------//

unsigned G711Decoder::Init()
{
    if (!m_Init)
    {
        DsCScriptValue tableParam;
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        // no codec properties

        m_pCodecCtx->channels    = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
        m_uFramesPkt             = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();  
        m_uFrameDuration         = (unsigned)tableParam.GetTableValue("network").GetTableValue("frame_duration").GetInt();
      
        m_pCodecCtx->block_align = 2;

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return RET_OK;
}

//----------------------------------------------//
// G723 decoder class definiton
//----------------------------------------------//

unsigned G723Decoder::Init()
{
    if (!m_Init)
    {
        DsCScriptValue tableParam;
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        DsCString codecProperties = tableParam.GetTableValue("codec_properties").GetStr();
        //m_pCodecCtx->bit_rate     = 5300;
        m_pCodecCtx->bit_rate     = 6300;
            
        m_pCodecCtx->channels     = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate  = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
        m_uFramesPkt              = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();
        m_uFrameDuration          = (unsigned)tableParam.GetTableValue("network").GetTableValue("frame_duration").GetInt();
      
        m_pCodecCtx->block_align  = 2;

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return RET_OK;
}

//----------------------------------------------//
// G728 decoder class definiton
//----------------------------------------------//

unsigned G728Decoder::Init()
{
    if (!m_Init)
    {
        DsCScriptValue tableParam;
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        DsCString codecProperties = tableParam.GetTableValue("codec_properties").GetStr();
        //m_pCodecCtx->bit_rate     = 9600;
        //m_pCodecCtx->bit_rate     = 12800;
        m_pCodecCtx->bit_rate     = 16000;

        m_pCodecCtx->channels     = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate  = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
        m_uFramesPkt              = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();
        m_uFrameDuration          = (unsigned)tableParam.GetTableValue("network").GetTableValue("frame_duration").GetInt();
      
        m_pCodecCtx->block_align = 2;

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return RET_OK;
}


//----------------------------------------------//
// G729 decoder class definiton
//----------------------------------------------//

unsigned G729Decoder::Init()
{
    if (!m_Init)
    {
        DsCScriptValue tableParam;
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        DsCString codecProperties = tableParam.GetTableValue("codec_properties").GetStr();
        m_pCodecCtx->bit_rate     = 8000;

        m_pCodecCtx->channels     = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate  = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
        m_uFramesPkt              = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();
        m_uFrameDuration          = (unsigned)tableParam.GetTableValue("network").GetTableValue("frame_duration").GetInt();
      
        m_pCodecCtx->block_align  = 2;

        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return RET_OK;
}


//----------------------------------------------//
// AAC decoder class definiton
//----------------------------------------------//

unsigned AACDecoder::Init()
{
    if (!m_Init)
    {
        DsCScriptValue tableParam;     
        CHECK_ERROR(SetDecoder(tableParam), RET_OK)

        DsCString extraData = tableParam.GetTableValue("extradata").GetStr(); 
        DsPSTR pExtraData   = extraData.GetBuffer();
            
        m_pCodecCtx->extradata_size = extraData.GetSize();
        m_pCodecCtx->extradata      = new BYTE[m_pCodecCtx->extradata_size];

        memcpy(m_pCodecCtx->extradata, pExtraData, m_pCodecCtx->extradata_size);

        m_pCodecCtx->channels    = (int)tableParam.GetTableValue("format").GetTableValue("channels").GetInt();
        m_pCodecCtx->sample_rate = (int)tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
        m_uFramesPkt             = (unsigned)tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();
      
        if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
            return RET_COULD_NOT_OPEN_CODEC;

        m_Init = true;
    }

    return RET_OK;
}
