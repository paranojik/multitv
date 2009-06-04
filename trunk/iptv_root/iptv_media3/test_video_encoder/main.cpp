#include <process.h>
#include "CapCam.h"
#include "test_encoder_functions.h"
#include "mediapkt.h"
#include "log.h"
#include "VideoEncoder.h"
#include "MediaNetwork.h"
#include "VideoDecoder.h"

BYTE encodedBuf[ENCODEDBUFSIZE];

_SEMAPHORE        g_Semaph;

CapCam            g_CapCam;
CVideoFrame       g_Frame;

BYTE *g_pBufToEncode = NULL,
     *g_pDecodedBuf  = NULL;

VideoEncoder *g_pVideoEncoder = NULL;
VideoDecoder *g_pVideoDecoder = NULL;

bool g_CodecRunning = true;
bool g_NewFrame     = false;

unsigned g_uDstWidth,
         g_uDstHeight,
         g_uDstDatalen,
         g_uFrameSize;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;

	WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));

	wc.style         = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDC_ARROW);
	wc.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = TEXT("Image Capture");

	RegisterClass(&wc);

	hwnd = CreateWindow(wc.lpszClassName, 
                        wc.lpszClassName, 
                        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		                0, 0, 0, 0, 0, 0, hInstance, 0);
	
    ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    
	g_CapCam.Stop();
	g_CapCam.WaitCompletion();

    if (g_pDecodedBuf)
        delete g_pDecodedBuf;
	
    CoUninitialize();
	return msg.wParam;
}

unsigned WINAPI StartEncoding(void *_pParam)
{    
    unsigned encodedBufSize,
             decodedBufSize;

    while(g_CodecRunning)
    {
        g_Semaph.Wait(); 

		if (g_CapCam.GetLastFrame(g_Frame))
        {
            if (g_Frame.FrameOk())
            {
                ULONG uWidth, 
                      uHeight,
                      uDatalen;
         
                g_Frame.GetFrameData(&g_pBufToEncode, &uDatalen);
                g_Frame.GetFrameDimensions(&uWidth, &uHeight);

                VideoCodecParam videoParam;
                memset(&videoParam, 0, sizeof(videoParam));

                encodedBufSize = ENCODEDBUFSIZE;
                g_pVideoEncoder->Encode(g_pBufToEncode, 
                                        g_uFrameSize, 
                                        encodedBuf, 
                                        encodedBufSize, 
                                        &videoParam);

                decodedBufSize = g_uFrameSize;
                g_pVideoDecoder->Decode(encodedBuf, 
					                    encodedBufSize, 
										g_pDecodedBuf, 
										decodedBufSize, 
										&videoParam);
				                
                g_NewFrame = true;
            
                Sleep(1000.0 / g_pVideoEncoder->GetFramerate());
            } 
        }

		g_Semaph.Signal();
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static bool encoderStarted = false;

	switch(message)
	{
        case WM_CREATE:
        {
            std::string codec,
                        bitrate;
            
            codec   = "mpeg4";
            bitrate = "1024k";

            TCHAR szCaption[50];

            wsprintf(szCaption, "%s, %s", codec.c_str(), bitrate.c_str());
            SetWindowText(hWnd, szCaption);

            InitCodec(codec, bitrate);
            g_CapCam.Start(g_uDstWidth, g_uDstHeight);
                        
            SetTimer(hWnd, 1, 30, NULL);
        }
        break;

        case WM_TIMER:
				  
            if (!encoderStarted)
            {
                g_Semaph.Wait();

                _beginthreadex(NULL, 0, StartEncoding, 0, 0, 0);
                encoderStarted = true;
            }
            else
            {
                g_FileLog.SetLog("Timer...");
                
                g_Semaph.Signal();
                g_Semaph.Wait();

                if (g_NewFrame)
                {
                    ClientResize(hWnd, g_uDstWidth, g_uDstHeight);
                    DrawFrame(hWnd, g_pDecodedBuf, g_uDstWidth, g_uDstHeight);
                    
                    g_NewFrame = false;
                }
            }

        break;

        case WM_DESTROY:
			g_CodecRunning = false;
		    PostQuitMessage(0);
	    break;
	
        default:
	        return(DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 1;
}