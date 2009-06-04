#include "compat.h"
#include "Codec.h"
#include "global_error.h"

bool MediaCodec::m_avcodecInited = false;

AVCodec *Encoder::InitContext(std::string _codec)
{
    if (!(m_pCodecCtx = avcodec_alloc_context()))
        return NULL;

    AVCodec *pCodec = avcodec_find_encoder_by_name(_codec.c_str());

    return pCodec;
}

unsigned Encoder::SetEncoder(std::string _codecType, DsCScriptValue & _tableParam)
{        
    unsigned ret;

    if ((ret = GetTableParam(GetTransmission(), 
                             GetNetwork(), 
                             GetBitrate(),
                             _codecType,
                             GetCodec(), 
                             _tableParam)) == RET_OK)
    {
        DsCString codec = _tableParam.GetTableValue("codec").GetStr();
        
        if (!(m_pCodec = InitContext(codec.GetStr())))
            ret = RET_ERROR;
    }

    return ret;
}

AVCodec * Decoder::InitContext(std::string _codec)
{
    if (!(m_pCodecCtx = avcodec_alloc_context()))
        return NULL;

    AVCodec *pCodec = avcodec_find_decoder_by_name(_codec.c_str());

    return pCodec;
}

unsigned Decoder::SetDecoder(DsCScriptValue & _tableParam)
{    
    unsigned ret;

    if ((ret = GetTableParam(m_MediaType, _tableParam)) == RET_OK) 
    {
        DsCString codec = _tableParam.GetTableValue("codec").GetStr();
        
        if (!(m_pCodec = InitContext(codec.GetStr())))
            ret = RET_ERROR;
    }

    return ret;
}
