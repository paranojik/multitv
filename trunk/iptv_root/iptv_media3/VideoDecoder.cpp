#include "compat.h"
#include "VideoDecoder.h"
#include "global_error.h"
#include "MediaNetwork.h"
#include "media_utilities.h"

unsigned VideoDecoder::GetWidth()
{
    if (CodecOk())
        return m_pCodecCtx->width;
    else
        return 0;
}

unsigned VideoDecoder::GetHeight()
{
    if (CodecOk())
        return m_pCodecCtx->height;
    else
        return 0;
}

unsigned VideoDecoder::GetFrameSize()
{
    if (CodecOk())
        return m_uOutputFrameLen;
    else
        return 0;
}

double VideoDecoder::GetFramerate()
{
    if (CodecOk())
        return ((double) m_pCodecCtx->time_base.den) / m_pCodecCtx->time_base.num;
    else
        return 0;
}

unsigned VideoDecoder::InitBuffers()
{
    unsigned ret = RET_ERROR;
   
    m_pDecodedFrame = avcodec_alloc_frame();
    m_pOutputFrame  = avcodec_alloc_frame();
			
    if (m_pDecodedFrame && m_pOutputFrame)
    {
        m_uDecodedFrameLen = avpicture_get_size(m_pCodecCtx->pix_fmt, m_pCodecCtx->width, m_pCodecCtx->height);
        m_uOutputFrameLen  = avpicture_get_size( m_outputPixFmt, m_pCodecCtx->width, m_pCodecCtx->height);

        m_pDecodedFrameBuf = new unsigned char[m_uDecodedFrameLen];

        if (m_pDecodedFrameBuf) 
        {
            // setup pointers on AVFrame to correcty point to buffer (pDecodedFrameBuffer)

            avpicture_fill((AVPicture *) m_pDecodedFrame,      // AVFrame that will contain the decoded picture 
			                m_pDecodedFrameBuf,   // buffer that holds AVFrame data
			                m_pCodecCtx->pix_fmt, // codec context default pixel format
			                m_pCodecCtx->width,   // picture width
			                m_pCodecCtx->height); // picture height

            ret = RET_OK;            
        }
    }

    return ret;
}

unsigned VideoDecoder::Init()
{
    unsigned ret = RET_OK;

    if (!m_Init)
    {
        DsCScriptValue tableParam;

        if ((ret = GetTableParam(m_MediaType, tableParam)) != RET_OK) 
            return ret;

        DsCString codec = tableParam.GetTableValue("codec").GetStr();
        m_pCodec = InitContext(codec.GetStr());

        if (m_pCodec)
        {
            DsCScriptValue frameSize = tableParam.GetTableValue("format").GetTableValue("size");

            m_pCodecCtx->width   = (int)frameSize.GetTableValue("width").GetInt();
            m_pCodecCtx->height  = (int)frameSize.GetTableValue("height").GetInt();
            
            m_pCodecCtx->pix_fmt = PIX_FMT_YUV420P;

            if ( avcodec_open(m_pCodecCtx, m_pCodec) < 0 )
                return RET_COULD_NOT_OPEN_CODEC;

            if (( ret = InitBuffers() ) == RET_OK) 
                m_Init = true;
	    }
        else
        {    
            ret = RET_ERROR;
        }
    }

    return ret;
}

unsigned VideoDecoder::Decode(unsigned char *_pBufIn, 
                              unsigned _bufInLen, 
                              unsigned char *_pBufOut, 
                              unsigned & _bufOutLen, 
                              void *_pHeaderData)
{

    if (!m_Init)
        return RET_INIT_ERROR;
	
    if (!_pBufIn || !_bufInLen || !_pBufOut || !_bufOutLen || !_pHeaderData)
        return RET_INVALID_ARG;

    if (_bufOutLen < m_uOutputFrameLen)
        return RET_BUFFER_TOO_SMALL; 

    VideoCodecParam *pVideoParam = (VideoCodecParam *)_pHeaderData;

    if (!m_keyFrameFound)   
    {
        if (pVideoParam->MPKT_keyFrame)
        {    
            m_keyFrameFound = true;
        } 
        else
        {    
            return RET_KEYFRAME_NEEDED;
        }
    }  

    int iGotPicture,
        iDecodedBytes;
	
    iDecodedBytes = avcodec_decode_video(m_pCodecCtx,     //codec context
	                                     m_pDecodedFrame, //AVFrame in which the decoded video will be stored in its default pixel format
                                         &iGotPicture,    //positive if a frame is decompressed
                                         _pBufIn,         //input buffer
                                         _bufInLen);      //size of the input buffer in bytes
    
    if ((iDecodedBytes < 0) || (!iGotPicture))
        return RET_DECODE_ERROR;
 
    CHECK_ERROR(ConvertToOutputFmt((AVPicture *)m_pDecodedFrame, _pBufOut, _bufOutLen), RET_OK)
    
    return RET_OK;
}

unsigned VideoDecoder::ConvertToOutputFmt(AVPicture *_pDecodedFrame,
                                          unsigned char *_pDstBuf,
                                          unsigned  &_uDstBufLen)
{         
    unsigned ret = RET_OK;

    _uDstBufLen = m_uOutputFrameLen;

    avpicture_fill((AVPicture *) m_pOutputFrame, 
                    _pDstBuf, 
                    m_outputPixFmt, 
                    m_pCodecCtx->width,
                    m_pCodecCtx->height);
        
    if (img_convert((AVPicture *)m_pOutputFrame,
                     m_outputPixFmt, 
                     _pDecodedFrame, 
                     m_pCodecCtx->pix_fmt, 
                     m_pCodecCtx->width, 
                     m_pCodecCtx->height) < 0)
    {   
        _uDstBufLen = 0;
        ret = RET_COULD_NOT_CONVERT_PIX_FMT;
    }
    
    return ret;
}


//----------------------ASFVideoDecoder methods definition---------------------//

unsigned ASFVideoDecoder::Init()
{
    unsigned ret = RET_ERROR;

    if (m_pCodecCtx) 
    {    
        if (m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id))
        {
            if (avcodec_open(m_pCodecCtx, m_pCodec) == 0)
            {
                if ( (ret = InitBuffers()) == RET_OK)
                    m_Init = true;
            }
        }
    }

    return ret;
}

