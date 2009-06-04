#ifndef TEST_ENCODER_FUNCTIONS_H
#define TEST_ENCODER_FUNCTIONS_H

#include <windows.h>
#include <string>

#define ENCODEDBUFSIZE  50000
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

unsigned WINAPI StartEncoding(void *_pParam);

unsigned DrawFrame(HWND _hWnd, 
                   BYTE *_pData, 
                   unsigned _width, 
                   unsigned _height);

unsigned ResizeFrame(BYTE *_pSrcData, 
                     unsigned _uSrcWidth, 
                     unsigned _uSrcHeight, 
                     unsigned _uSrcDatalen,
                     BYTE *_pDstData, 
                     unsigned _uDstWidth, 
                     unsigned _uDstHeight,
                     unsigned & _uDstDatalen);


void InitCodec(std::string _codec, std::string _bitrate);
void ClientResize(HWND hWnd, int nWidth, int nHeight);


#endif