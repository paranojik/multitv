#include "test_encoder_functions.h"
#include "semaphore.h"
#include "CapCam.h"
#include "global_functions.h"
#include "mediapkt.h"
#include "VideoEncoder.h"
#include "VideoDecoder.h"

extern BYTE *g_pBufToEncode;
extern BYTE *g_pDecodedBuf;

extern BYTE encodedBuf[ENCODEDBUFSIZE];

extern CapCam            g_CapCam;
extern CVideoFrame       g_Frame;

extern VideoEncoder     *g_pVideoEncoder;
extern VideoDecoder     *g_pVideoDecoder;

extern bool codecRunning;

extern unsigned g_uDstWidth;
extern unsigned g_uDstHeight;
extern unsigned g_uDstDatalen;
extern unsigned g_uFrameSize;

unsigned DrawFrame(HWND _hWnd, BYTE *_pData, unsigned _width, unsigned _height)
{
    if (!_pData)
        return 0;
                
    BITMAPINFO bmpInfo;
    memset(&bmpInfo, 0, sizeof(bmpInfo));

    bmpInfo.bmiHeader.biSize     = sizeof( bmpInfo.bmiHeader );
    bmpInfo.bmiHeader.biWidth    = _width;
    bmpInfo.bmiHeader.biHeight   = _height;
    bmpInfo.bmiHeader.biPlanes   = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
	            
    HDC hDC = GetDC(_hWnd);
    SetDIBitsToDevice(hDC, 0, 0, _width, _height, 0, 0, 0, _height, _pData, &bmpInfo, 0);
    ReleaseDC(_hWnd, hDC);

    return 0;
}


unsigned ResizeFrame(BYTE *_pSrcData, 
                     unsigned _uSrcWidth, 
                     unsigned _uSrcHeight, 
                     unsigned _uSrcDatalen,
                     BYTE *_pDstData, 
                     unsigned _uDstWidth, 
                     unsigned _uDstHeight,
                     unsigned & _uDstDatalen)
{
    if ((_uSrcWidth < _uDstWidth) || (_uSrcHeight < _uDstHeight))
        return -1;

    ULONG uSrcLineLen,
          uDstLineLen;
          
    uSrcLineLen = _uSrcDatalen / _uSrcHeight;
    uDstLineLen = _uDstWidth * 3;

    for (int line=0; line < _uDstHeight; line++)
        memcpy(&_pDstData[line * uDstLineLen], &_pSrcData[uSrcLineLen * line], uDstLineLen);

    _uDstDatalen = _uDstWidth * _uDstHeight * 3;

    return 0;
}

void ClientResize(HWND hWnd, int nWidth, int nHeight)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    ULONG uClientWidth,
          uClientHeight;

    uClientWidth  = rcClient.right - rcClient.left;
    uClientHeight = rcClient.bottom - rcClient.top;

    if ( ( uClientWidth != nWidth ) || ( uClientHeight != nHeight ))
    {
        RECT  rcWindow;
        GetWindowRect(hWnd, &rcWindow);

        POINT ptDiff;
        ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
        ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
        
        MoveWindow(hWnd, rcWindow.left, rcWindow.top, nWidth + ptDiff.x, nHeight + ptDiff.y, TRUE);
    }
}

void InitCodec(std::string _codec, std::string _bitrate)
{
    if ((_codec != "mpeg4") && (_codec != "h264"))
        return;

    std::string transmission,
                network,
                mediaType;

    transmission = "conf";     // "conf", "ptp"
    network      = "internet"; //"internet", "satellite"
    mediaType    = "video";    // "video", "audio"
   
    BYTE type;

    if (g_pVideoEncoder)
        delete g_pVideoEncoder;

    if (_codec == "mpeg4") 
    {
        if (_bitrate == "33k" || _bitrate == "64k")
            type = MTYPE_MEDIA_VIDEO_MPEG4_176x144;
        else
        {
            type = MTYPE_MEDIA_VIDEO_MPEG4_320x240;
        }

        g_pVideoEncoder = new Mpeg4Encoder(transmission, network, _bitrate);
    }
    else
    {
        if (_bitrate == "33k" || _bitrate == "64k")
            type = MTYPE_MEDIA_VIDEO_H264_QCIF;
        else
        {
            type = MTYPE_MEDIA_VIDEO_H264_QVGA;
        }

        g_pVideoEncoder = new H264Encoder(transmission, network, _bitrate);
    }

    if (g_pVideoEncoder)
        g_pVideoEncoder->Init();

    //bitrates : "33k", "64k", "128k", "256k", "384k", "512k", "1024k"
    //codecs   : "mpeg4", "h264" 
    
    GetFrameDimensions(type, (ULONG *)&g_uDstWidth, (ULONG *)&g_uDstHeight);
     
    g_uFrameSize = g_uDstWidth * g_uDstHeight * 3;
    
    if (g_pDecodedBuf)
        delete [] g_pDecodedBuf;

    g_pDecodedBuf = new BYTE[g_uFrameSize];

    if (g_pVideoDecoder)
        delete g_pVideoDecoder;
	
	g_pVideoDecoder = new VideoDecoder(type);

	if (g_pVideoDecoder)
        g_pVideoDecoder->Init();
}