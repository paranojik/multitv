#include "compat.h"
#include <stdio.h>
#include "avcodec.h"
#include "media_utilities.h"
#include "pixel_conversion.h"
#include "global_error.h"

unsigned GetPixFmtDesc(PixFmt _pixFmt, std::string & _pixFmtDesc)
{
    switch(_pixFmt)
    {
        case NO_PIX_FMT:
            _pixFmtDesc = "NO_PIX_FMT";
        break;

        case RGB24:
            _pixFmtDesc = "RGB24";
        break;

        case BGR24:
            _pixFmtDesc = "BGR24";
        break; 

        case RGBA32:
            _pixFmtDesc = "RGBA32";
        break;

        case RGB565:
            _pixFmtDesc = "RGB565";
        break;

        case RGB555:
            _pixFmtDesc = "RGB555";
        break;

        case YUV420P: 
            _pixFmtDesc = "YUV420P";
        break;

        case YUV420:
            _pixFmtDesc = "YUV420";
        break;

        case YUV422:
            _pixFmtDesc = "YUV422";
        break;

        case YUYV422: 
            _pixFmtDesc = "YUYV422";
        break;

        case UYVY422:
            _pixFmtDesc = "UYVY422";
        break;        
 
        case MJPEG:
            _pixFmtDesc = "MJPEG";
        break;

        case PWC2:
            _pixFmtDesc = "PWC2";
        break;

        case GREY:
            _pixFmtDesc = "GREY";
        break;
    }

    return RET_OK;
}

unsigned TranslateToFFmpegPixFmtId(PixFmt _srcPixFmt, PixelFormat & _dstPixFmt)
{
    switch(_srcPixFmt)
    {
        case RGB24:
            _dstPixFmt = PIX_FMT_RGB24;
        break;

        case BGR24:
            _dstPixFmt = PIX_FMT_BGR24;
        break;

        case RGBA32:
            _dstPixFmt = PIX_FMT_RGB32;
        break;

        case YUV420P:
            _dstPixFmt = PIX_FMT_YUV420P;
        break;

        case YUYV422:
            _dstPixFmt = PIX_FMT_YUYV422;
        break;

        case RGB565:
            _dstPixFmt = PIX_FMT_RGB565;
        break;

        case RGB555:
            _dstPixFmt = PIX_FMT_RGB555;
        break;

        default:
            return RET_ERROR;
    }

    return RET_OK;
}


unsigned TranslateFromFFmpegPixFmtId(PixelFormat _srcPixFmt, PixFmt & _dstPixFmt)
{
    switch(_srcPixFmt)
    {
        case PIX_FMT_RGB24:
            _dstPixFmt = RGB24;
        break;

        case PIX_FMT_BGR24:
            _dstPixFmt = BGR24;
        break; 
 
        case PIX_FMT_RGB32:
            _dstPixFmt = RGBA32;
        break;

        case PIX_FMT_YUV420P:
            _dstPixFmt = YUV420P;
        break;

        case PIX_FMT_YUYV422:
            _dstPixFmt = YUYV422;
        break;

        case PIX_FMT_RGB565:
            _dstPixFmt = RGB565;
        break;

        case PIX_FMT_RGB555:
            _dstPixFmt = RGB555;
        break;

        default:
            return RET_ERROR;
    }

    return RET_OK;
}

unsigned CheckPixFmt(PixFmt _pixFmt)
{
    unsigned ret;

    switch(_pixFmt)
    {
        case RGB24:
        case BGR24:
        case RGBA32:
        case YUV420P:
        case YUYV422:
        case RGB565:
        case RGB555:
            ret = RET_OK;
        break;

        default:
            ret = RET_INVALID_ARG;
    }

    return ret;
}


unsigned ConvertPixFmt(unsigned char *_pSrcData,  
                       unsigned _srcDatalen,
                       PixFmt _srcPixFmt,
                       unsigned char *_pDstData, 
                       unsigned & _dstDatalen,
                       PixFmt _dstPixFmt,
                       unsigned _width, 
                       unsigned _height)
{
    if (_srcPixFmt == _dstPixFmt)
        return RET_OK;

    static bool avcodec_inited = false;

    if (!avcodec_inited)
    {
        avcodec_init();
        avcodec_inited = true;
    }

    PixelFormat ffDstFmt,
                ffSrcFmt;

    CHECK_ERROR(TranslateToFFmpegPixFmtId(_srcPixFmt, ffSrcFmt), RET_OK)

    CHECK_ERROR(TranslateToFFmpegPixFmtId(_dstPixFmt, ffDstFmt), RET_OK)   
      
    unsigned dstImageSize = avpicture_get_size(ffDstFmt, _width, _height);

    if (_dstDatalen < dstImageSize)
        return RET_ERROR;

    AVFrame *pDstPixFmt,
            *pSrcPixFmt;

    pDstPixFmt = avcodec_alloc_frame();
    pSrcPixFmt = avcodec_alloc_frame();

    if (!pDstPixFmt || !pSrcPixFmt)
	    return RET_ERROR;

    avpicture_fill((AVPicture *) pDstPixFmt, 
                    _pDstData, 
                    ffDstFmt, 
                    _width,
                    _height);

    avpicture_fill((AVPicture *) pSrcPixFmt, 
                    _pSrcData, 
                    ffSrcFmt, 
                    _width,
                    _height); 
   

    // fazer a transição para swscale !!!

    if (img_convert( (AVPicture *)pDstPixFmt, ffDstFmt, (AVPicture *)pSrcPixFmt, ffSrcFmt, _width, _height) < 0)
    {    
        av_free(pDstPixFmt);
        av_free(pSrcPixFmt);
        return RET_ERROR;
    }

    _dstDatalen = dstImageSize;

    av_free(pDstPixFmt);
    av_free(pSrcPixFmt);

    return RET_OK;
}

unsigned ConvertToRGB24(unsigned char *_pSrcData, 
                        unsigned _srcDatalen,
                        PixFmt _srcPixFmt,
                        unsigned char *_pDstData, 
                        unsigned & _dstDatalen,
                        unsigned _width, 
                        unsigned _height)
{
    unsigned ret;
    ret = ConvertPixFmt(_pSrcData, _srcDatalen, _srcPixFmt, _pDstData, _dstDatalen, RGB24, _width, _height);

    return ret;
}

unsigned ConvertToYUV420(unsigned char *_pSrcData,  
                        unsigned _srcDatalen,
                        PixFmt _srcPixFmt,
                        unsigned char *_pDstData, 
                        unsigned & _dstDatalen,
                        unsigned _width, 
                        unsigned _height)
{
    unsigned ret;
    ret = ConvertPixFmt(_pSrcData, _srcDatalen, _srcPixFmt, _pDstData, _dstDatalen, YUV420P, _width, _height);

    return ret;
}


unsigned StretchPicture(unsigned char *_pSrcData, 
                        unsigned _uSrcDatalen, 
                        unsigned _uSrcWidth, 
                        unsigned _uSrcHeight, 
                        PixFmt _pixFmt,
                        unsigned char *_pYuvDstFrame, 
                        unsigned & _uDstDatalen, 
                        unsigned   _uDstWidth, 
                        unsigned   _uDstHeight)
{

    if ((!_pSrcData  || !_uSrcDatalen || !_uSrcWidth || !_uSrcHeight || 
         !_pYuvDstFrame || !_uDstDatalen || !_uDstWidth || !_uDstHeight))
    {
        return RET_INVALID_ARG;
    }

    CHECK_ERROR(CheckPixFmt(_pixFmt), RET_OK)

    unsigned uOutDatalen = avpicture_get_size(PIX_FMT_YUV420P, _uDstWidth, _uDstHeight);

    if (_uDstDatalen < uOutDatalen)
        return RET_BUFFER_TOO_SMALL;

    unsigned ret;

    unsigned char *pCurFrame = _pSrcData;
    unsigned uCurFramelen    = _uSrcDatalen;

    unsigned char * pAuxBuf = NULL;
    unsigned uAuxBufLen = 0;

    PixFmt curPixFmt = _pixFmt;

    if  (curPixFmt != YUV420P)
    {      
        uAuxBufLen = avpicture_get_size(PIX_FMT_YUV420P, _uSrcWidth, _uSrcHeight);
        pAuxBuf    = new unsigned char [uAuxBufLen];

        if (!pAuxBuf)
            return RET_LOW_MEMORY;

        if ( (ret = ConvertToYUV420(pCurFrame, uCurFramelen, curPixFmt, pAuxBuf, uAuxBufLen, _uSrcWidth, _uSrcHeight) ) != RET_OK)
        {
            delete pAuxBuf;
            return ret;
        }
    
        pCurFrame    = pAuxBuf;
        curPixFmt    = YUV420P;
        uCurFramelen = uAuxBufLen;
    }

    ImgReSampleContext* resample_ctx = img_resample_init(_uDstWidth, _uDstHeight, _uSrcWidth, _uSrcHeight);

    AVFrame *pTargetResampledPicture = avcodec_alloc_frame();
    AVFrame *pSourceCapturedPicture  = avcodec_alloc_frame();

    avpicture_fill((AVPicture*) pTargetResampledPicture, _pYuvDstFrame, PIX_FMT_YUV420P, _uDstWidth, _uDstHeight);
    avpicture_fill((AVPicture*) pSourceCapturedPicture,  pCurFrame,  PIX_FMT_YUV420P, _uSrcWidth, _uSrcHeight);
            
    img_resample(resample_ctx, (AVPicture*) pTargetResampledPicture, (AVPicture*) pSourceCapturedPicture);
    img_resample_close(resample_ctx);

    _uDstDatalen = uOutDatalen;
            
    av_free(pTargetResampledPicture);
    av_free(pSourceCapturedPicture);  

    if (pAuxBuf)
        delete pAuxBuf; 

    return RET_OK;
}
