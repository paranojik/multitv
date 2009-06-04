#ifndef PIXEL_CONVERSION_H
#define PIXEL_CONVERSION_H

#include <string>
#include "avcodec.h"
#include "pixel_formats.h"

unsigned GetPixFmtDesc(PixFmt _pixFmt, std::string & _pixFmtDesc);

unsigned TranslateFromFFmpegPixFmtId(PixelFormat _srcPixFmt, PixFmt & _dstPixFmt);
unsigned TranslateToFFmpegPixFmtId(PixFmt _srcPixFmt, PixelFormat & _dstPixFmt);

unsigned CheckPixFmt(PixFmt _pixFmt);

unsigned ConvertPixFmt(unsigned char *_pSrcData,  
                       unsigned _srcDatalen,
                       PixFmt _srcPixFmt,
                       unsigned char *_pDstData, 
                       unsigned & _dstDatalen,
                       PixFmt _dstPixFmt,
                       unsigned _width, 
                       unsigned _height);


unsigned ConvertToRGB24(unsigned char *_pSrcData, 
                        unsigned _srcDatalen,
                        PixFmt   _srcPixFmt,
                        unsigned char *_pDstData, 
                        unsigned & _dstDatalen,
                        unsigned _width, 
                        unsigned _height);

unsigned ConvertToYUV420(unsigned char *_pSrcData,  
                        unsigned _srcDatalen,
                        PixFmt   _srcPixFmt,
                        unsigned char *_pDstData, 
                        unsigned & _dstDatalen,
                        unsigned _width, 
                        unsigned _height);


unsigned StretchPicture(unsigned char *_pSrcData, 
                        unsigned _uSrcDatalen, 
                        unsigned _uSrcWidth, 
                        unsigned _uSrcHeight, 
                        PixFmt _pixFmt, 
                        unsigned char *_pDstFrame, 
                        unsigned & _uDstDatalen, 
                        unsigned   _uDstWidth, 
                        unsigned   _uDstHeight);

#endif
