#include "compat.h"
#include "AudioEncoder.h"
#include "global_error.h"
#include "parseString.h"
#include "MediaNetwork.h"
#include "mediapkt.h"
#include "media_utilities.h"

unsigned AudioEncoder::GetSampleRate()
{
	if (CodecOk())
		return m_pCodecCtx->sample_rate;
	else
		return 0;
}

unsigned AudioEncoder::GetSampleSize()
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

unsigned AudioEncoder::GetFrameSamples()
{
	if (CodecOk())
		return m_pCodecCtx->frame_size;
	else
		return 0;
}

unsigned AudioEncoder::GetChannels()
{
	if (CodecOk())
		return m_pCodecCtx->channels;
	else
		return 0;
}

unsigned AudioEncoder::InitCommonParameters(CParseString & _parseString)
{
	ParseFieldValue value; 

	AVFilterContext *pFilterCtx = avfilter_alloc_context();       
	if ((value = _parseString.GetFieldValue("agc_level")).m_valueOK) pFilterCtx->agc_level = (int)value.m_value;

	value = _parseString.GetFieldValue("agc");
	if (value.m_valueOK && value.m_booleanValue)
	{
		pFilterCtx->flags |= FILTER_FLAG_SPEEX_AGC;
	}

	value = _parseString.GetFieldValue("vad");
	if (value.m_valueOK && value.m_booleanValue)
	{
		pFilterCtx->flags |= FILTER_FLAG_SPEEX_VAD;
	}            

	value = _parseString.GetFieldValue("denoise");
	if (value.m_valueOK && value.m_booleanValue)
	{
		pFilterCtx->flags |= FILTER_FLAG_SPEEX_DENOISER;
	}

	m_pCodecCtx->block_align  = 2;

	if ( (avcodec_open(m_pCodecCtx, m_pCodec)) < 0 )
		return RET_COULD_NOT_OPEN_CODEC;

	AVFilter *pFilter = avfilter_find_by_name("speexpreproc");
	avfilter_add(m_pCodecCtx, pFilterCtx, pFilter);

	return RET_OK;
}

unsigned AudioEncoder::InitFormat(DsCScriptValue & _tableParam)
{
	m_pCodecCtx->sample_rate = (int)_tableParam.GetTableValue("format").GetTableValue("sample_rate").GetInt();
	m_pCodecCtx->channels    = (int)_tableParam.GetTableValue("format").GetTableValue("channels").GetInt();

	m_uFramesPkt = (unsigned)_tableParam.GetTableValue("network").GetTableValue("frames_packet").GetInt();
	m_MediaType  = (unsigned char)_tableParam.GetTableValue("payload_type").GetInt();

	return RET_OK;
}

unsigned AudioEncoder::Encode(unsigned char *_pBufIn, 
							  unsigned _bufInLen, 
							  unsigned char *_pBufOut, 
							  unsigned & _bufOutLen, 
							  void *_pHeaderData)
{
	if (!_pBufIn || !_bufInLen || !_pBufOut || !_bufOutLen)
		return RET_INVALID_ARG;

	if (!m_Init)
		return RET_INIT_ERROR;

	int ret,
		uBytesWritten,
		uRemainingBuf,
		uFrameSize,
		uBytesUsed;

	ret = RET_OK;

	uBytesUsed    = 0;
	uFrameSize    = _bufInLen / m_uFramesPkt;
	uRemainingBuf = _bufOutLen;

	for (int iPos=0; iPos < (int)m_uFramesPkt; iPos++)
	{
		//uBytesWritten = avcodec_encode_audio(m_pCodecCtx, &_pBufOut[uBytesUsed], uRemainingBuf, (short *)&_pBufIn[iPos * uFrameSize]);
		uBytesWritten = m_pCodecCtx->codec->encode(m_pCodecCtx, &_pBufOut[uBytesUsed], uRemainingBuf, &_pBufIn[iPos * uFrameSize]);

		if (uBytesWritten)
		{
			if (uRemainingBuf >= uBytesWritten)
			{   
				uRemainingBuf -= uBytesWritten;
				uBytesUsed    += uBytesWritten;
			}
			else
			{
				ret = RET_BUFFER_TOO_SMALL;
				break;
			}
		}
		else
		{
			ret = RET_ERROR;
			break;
		}
	}

	if (ret == RET_OK)
	{
		CodecParam * pCodecParam = (CodecParam *) _pHeaderData;
		pCodecParam->MPKT_type = m_MediaType;

		_bufOutLen = uBytesUsed;
	}

	return ret;
}


//----------------------------------------------//
// Speex encoder class definiton
//----------------------------------------------//

unsigned SpeexEncoder::Init()
{
	unsigned ret = RET_OK;

	if (!m_Init)
	{
		DsCScriptValue tableParam;

		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

			InitFormat(tableParam);

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse = codec_properties.GetStr();
		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;        
		if ((value = parseString.GetFieldValue("quality")).m_valueOK) m_pCodecCtx->global_quality = (int)value.m_value;

		CHECK_ERROR(InitCommonParameters(parseString), RET_OK)

			m_Init = true;
	}

	return ret;
}

//----------------------------------------------//
// G711 encoder class definiton
//----------------------------------------------//

unsigned G711Encoder::Init()
{
	if (!m_Init)
	{
		DsCScriptValue tableParam;

		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

	    InitFormat(tableParam);

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse  = codec_properties.GetStr();

		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;                    
		if ((value = parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;

		CHECK_ERROR(InitCommonParameters(parseString), RET_OK)

			m_Init = true;
	}

	return RET_OK;
}

//----------------------------------------------//
// G723 encoder class definiton
//----------------------------------------------//

unsigned G723Encoder::Init()
{

	if (!m_Init)
	{
		DsCScriptValue tableParam;
		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

			InitFormat(tableParam);

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse  = codec_properties.GetStr();

		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;                    
		if ((value = parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;

		CHECK_ERROR(InitCommonParameters(parseString), RET_OK)

			m_Init = true;       
	}

	return RET_OK;
}

//----------------------------------------------//
// G728 encoder class definiton
//----------------------------------------------//

unsigned G728Encoder::Init()
{
	if (!m_Init)
	{
		DsCScriptValue tableParam;
		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

			InitFormat(tableParam);

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse  = codec_properties.GetStr();

		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;                    
		if ((value = parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;

		CHECK_ERROR(InitCommonParameters(parseString), RET_OK)

			m_Init = true;
	}

	return RET_OK;
}

//----------------------------------------------//
// G729 encoder class definiton
//----------------------------------------------//

unsigned G729Encoder::Init()
{
	if (!m_Init)
	{
		DsCScriptValue tableParam;

		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

			InitFormat(tableParam);

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse  = codec_properties.GetStr();

		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;                    
		if ((value = parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;

		CHECK_ERROR(InitCommonParameters(parseString), RET_OK)

			m_Init = true;
	}

	return RET_OK;
}

//----------------------------------------------//
// AAC encoder class definiton
//----------------------------------------------//

unsigned AACEncoder::Init()
{
	if (!m_Init)
	{
		DsCScriptValue tableParam;

		CHECK_ERROR(SetEncoder(GetCodecType(), tableParam), RET_OK)

		InitFormat(tableParam);
		m_uFrameSize = (unsigned)tableParam.GetTableValue("network").GetTableValue("frame_size").GetInt();

		DsCString codec_properties = tableParam.GetTableValue("codec_properties").GetStr();
		std::string stringToParse  = codec_properties.GetStr();

		CParseString parseString(stringToParse, ':' );

		ParseFieldValue value;                    
		if ((value = parseString.GetFieldValue("bit_rate")).m_valueOK) m_pCodecCtx->bit_rate = (int)value.m_value;

		value = parseString.GetFieldValue("global_header");
		if (value.m_valueOK && value.m_booleanValue)
		{
			m_pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
		} 

		if ( (avcodec_open(m_pCodecCtx, m_pCodec)) < 0 )
			return RET_COULD_NOT_OPEN_CODEC;
		m_Init = true;
	}

	return RET_OK;
}
