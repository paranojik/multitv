#include "compat.h"
#include "functions.h"
#include "Global_error.h"
#include "DbgWnd.h"

ULONG EnterMessageLoop()
{
    MSG msg;
    
    while(GetMessage(&msg, NULL, 0, 0))
    {    
        ::TranslateMessage(&msg); 
        ::DispatchMessage(&msg);
    }

    return (ULONG) msg.wParam;
}

ULONG WriteDebugInfo(HDC _hDC, MediaDbgInfo & _mediaDbgInfo)
{	
	TEXTMETRIC tm;
	GetTextMetrics(_hDC, &tm);

	ULONG cyChar = tm.tmHeight + tm.tmExternalLeading;
	TCHAR szString[200];

	wsprintf(szString, TEXT("Media id: %lu"), _mediaDbgInfo.ulMediaId);
	TextOut(_hDC, 0, MEDIA_ID_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Received Packets: %lu"),  _mediaDbgInfo.ulPktRcv);
	TextOut(_hDC, 0, RCV_PKT_LINE*cyChar, szString, lstrlen(szString));

	wsprintf(szString, TEXT("Jitter Packets: %lu"), _mediaDbgInfo.ulJitterPkts);
	TextOut(_hDC, 0, JITTER_PKT_LINE*cyChar, szString, lstrlen(szString));

	wsprintf(szString, TEXT("Video Packets: %lu"), _mediaDbgInfo.ulVideoPkts);
	TextOut(_hDC, 0, VIDEO_PKT_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Audio Packets: %lu"), _mediaDbgInfo.ulAudioPkts);
	TextOut(_hDC, 0, AUDIO_PKT_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Video Frames Buffered: %lu"), _mediaDbgInfo.ulBufVideoFrames);
	TextOut(_hDC, 0, VIDEO_FRAME_BUF_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Audio Frames Buffered: %lu"), _mediaDbgInfo.ulBufAudioFrames);
	TextOut(_hDC, 0, AUDIO_FRAME_BUF_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Video Frames Processed: %lu"), _mediaDbgInfo.ulProcVideoFrames);
	TextOut(_hDC, 0, VIDEO_FRAME_PROC_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Audio Frames Processed: %lu"), _mediaDbgInfo.ulProcAudioFrames);
	TextOut(_hDC, 0, AUDIO_FRAME_PROC_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Current Timestamp: %lu"), _mediaDbgInfo.ulCurTS);
	TextOut(_hDC, 0, CUR_TIMESTAMP_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Video Timestamp: %lu"), _mediaDbgInfo.ulVideoTS);
	TextOut(_hDC, 0, VIDEO_TIMESTAMP_LINE*cyChar, szString, lstrlen(szString));
	
	wsprintf(szString, TEXT("Audio Timestamp: %lu"), _mediaDbgInfo.ulAudioTS);
	TextOut(_hDC, 0, AUDIO_TIMESTAMP_LINE*cyChar, szString, lstrlen(szString));

    if (_mediaDbgInfo.bVideoStarted)
    {            
        if (_mediaDbgInfo.bVideoPaused)
            wsprintf(szString, TEXT("Video started - Paused"));
        else
            wsprintf(szString, TEXT("Video started - Running"));
    }
    else  
        wsprintf(szString, TEXT("Video not started"));

    TextOut(_hDC, 0, VIDEO_THREAD_STATUS*cyChar, szString, lstrlen(szString));

    if (_mediaDbgInfo.bAudioStarted)
    {    
        if (_mediaDbgInfo.bAudioPaused)
            wsprintf(szString, TEXT("Audio started - Paused"));
        else
            wsprintf(szString, TEXT("Audio started - Running"));
    }
    else  
        wsprintf(szString, TEXT("Audio not started"));

    TextOut(_hDC, 0, AUDIO_THREAD_STATUS*cyChar, szString, lstrlen(szString));

    if (_mediaDbgInfo.bBuffering)
    {
        wsprintf(szString, "Buffering: %lu%%", _mediaDbgInfo.ulBufferingPercent);
        TextOut(_hDC, 0, BUFFERING_STATUS*cyChar, szString, lstrlen(szString));
    }

	return RET_OK;
}