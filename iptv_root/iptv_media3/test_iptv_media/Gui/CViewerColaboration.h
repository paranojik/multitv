#ifndef CVIEWER_COLABORATION_H
#define CVIEWER_COLABORATION_H

#include "Media_const.h"

class CViewerColaboration
{
private:

	typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

    BOOL   m_bInit;

    ULONG  m_id,
		   m_ulLastRepaintTime,
		   m_ulLastVideoTime,
		   m_ulLastAudioTime;

    HWND   m_hWnd,
		   m_hDbgWnd;

    HGLRC  m_hRC;

    VideoFrame_reg *m_pVideoReg;
	CVideoFrame     m_CurFrame;
    
	ULONG MakeWindow(CViewer *_pParent, Rect & _rcRect);
	
    ULONG InitDrawing();
    ULONG InitOpenGL();
    ULONG InitGdi();

	ULONG OpenGLRepaintWnd(BYTE *_pData, ULONG _ulDatalen, ULONG _ulFrameWidth, ULONG _ulFrameHeight);
	ULONG GdiRepaintWnd(BYTE *_pData, ULONG _ulDatalen, ULONG _ulFrameWidth, ULONG _ulFrameHeight);
	
    ULONG DrawCurrentFrame();
	ULONG ShowAudioOnlyMsg();

public:

    CViewerColaboration(CViewer *_pParent, Rect & _rcRect, ULONG  _id, VideoFrame_reg *_pVideoReg);   
	virtual ~CViewerColaboration();

    ULONG SetWindowPosition(Rect & _rcRect);
    ULONG DrawFrame();
	
	ULONG SetLastMediaTS(MediaSpec _mediaSpec);

	HWND  GetDbgWndHandle()		{return m_hDbgWnd;			}
	HWND  GetWindowHandle()		{return m_hWnd;				}
    ULONG GetId()				{return m_id;				}
	ULONG GetLastVideoTime()	{return m_ulLastVideoTime;	}
	ULONG GetLastAudioTime()	{return m_ulLastAudioTime;	}
	
	ULONG GetLastMediaTime();

	void  SetDbgWndMap(HWND _hDbgWnd) {m_hDbgWnd = _hDbgWnd;}
};


#endif