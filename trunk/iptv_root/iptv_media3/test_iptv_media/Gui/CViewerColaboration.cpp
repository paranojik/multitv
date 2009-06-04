#include "compat.h"
#include <gl/gl.h>
#include "data_reg.h"
#include "CVideoFrame.h"
#include "CViewer.h"
#include "CViewerColaboration.h"
#include "Global_functions.h"
#include "Global_error.h"
#include "media_utilities.h"



CViewerColaboration::CViewerColaboration(CViewer *_pParent, Rect & _rcRect, ULONG _id, VideoFrame_reg *_pVideoReg)
{
	m_id			= _id;
	m_bInit			= FALSE;
	m_hWnd			= NULL;
	m_hDbgWnd		= NULL;	

	GetTime(&m_ulLastRepaintTime);
	GetTime(&m_ulLastVideoTime);
	GetTime(&m_ulLastAudioTime);

    m_pVideoReg	= _pVideoReg;

	if (m_pVideoReg)
	{	
		if  (MakeWindow(_pParent, _rcRect) == RET_OK) 
		{	
			if (InitDrawing() == RET_OK)
				m_bInit = TRUE;
		}
	}
}

CViewerColaboration::~CViewerColaboration()
{
	if (m_hWnd)
		::DestroyWindow(m_hWnd);

	if (m_hDbgWnd)
		::DestroyWindow(m_hDbgWnd);
}

ULONG CViewerColaboration::InitDrawing()
{
	ULONG ret = RET_OK;
    ret = InitGdi();

	return ret;
}

ULONG CViewerColaboration::InitGdi()
{
#if 0
    HDC hDC = ::GetDC(m_hWnd);

	PIXELFORMATDESCRIPTOR pfd;
    
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	//pfd.iPixelType = PFD_TYPE_RGBA;
	//pfd.cColorBits = 32;

	GLuint PixelFormat;
	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, PixelFormat, &pfd);

	::ReleaseDC(m_hWnd, hDC);
#endif

    return RET_OK;
}

ULONG CViewerColaboration::InitOpenGL()
{
#if 0
    HDC hDC = ::GetDC(m_hWnd);

	PIXELFORMATDESCRIPTOR pfd;
    
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL /*| PFD_DOUBLEBUFFER*/;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	GLuint PixelFormat;
	PixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, PixelFormat, &pfd);

	m_hRC = wglCreateContext(hDC);
	::ReleaseDC(m_hWnd, hDC);

#endif
    return RET_OK;
}

ULONG CViewerColaboration::DrawCurrentFrame()
{
	if (m_CurFrame.FrameOk())
	{
		BYTE *pData;

		ULONG ulDatalen,
			  ulFrameWidth,
			  ulFrameHeight;

		CHECK_ERROR(m_CurFrame.GetFrameData(&pData, &ulDatalen), RET_OK)		
		CHECK_ERROR(m_CurFrame.GetFrameDimensions(&ulFrameWidth, &ulFrameHeight), RET_OK)
	    CHECK_ERROR(GdiRepaintWnd(pData, ulDatalen, ulFrameWidth, ulFrameHeight), RET_OK)
	}
	else
		return RET_ERROR;

	GetTime(&m_ulLastRepaintTime);

	return RET_OK;
}


ULONG CViewerColaboration::DrawFrame()
{
    if (!m_bInit)
        return RET_INIT_ERROR;	
    
	if (m_pVideoReg->NewData())
	{
		m_pVideoReg->ReadData(m_CurFrame);
    
		DrawCurrentFrame();
	}
	else
	{
		if (m_CurFrame.FrameOk())
		{
			ULONG ulCurTime;
			GetTime(&ulCurTime);

			if ((ulCurTime - m_ulLastRepaintTime) >= MAX_REPAINT_WAIT_MS)
				DrawCurrentFrame();
		}
		else
			ShowAudioOnlyMsg();
	}

    return RET_OK;
}

ULONG CViewerColaboration::ShowAudioOnlyMsg()
{
	HDC hDC = ::GetDC(m_hWnd);

	char msg[50] = "NO VIDEO SIGNAL - AUDIO ONLY";
	::TextOut(hDC, 0, 0, msg, lstrlen(msg));
		
	::ReleaseDC(m_hWnd, hDC);		

	return RET_OK;
}

ULONG CViewerColaboration::GdiRepaintWnd(BYTE *_pData, ULONG _ulDatalen, ULONG _ulFrameWidth, ULONG _ulFrameHeight)
{
	if (!_pData || !_ulDatalen || !_ulFrameWidth || !_ulFrameHeight)
		return RET_INVALID_ARG;

	HDC hDC = ::GetDC(m_hWnd);

	RECT rcRect;
	::GetClientRect(m_hWnd, &rcRect);
        
	ULONG ulWndWidth, 
		  ulWndHeight;
		
	ulWndWidth  = rcRect.right  - rcRect.left;
	ulWndHeight = rcRect.bottom - rcRect.top; 

	BITMAPINFO bmpInfo;
	memset(&bmpInfo.bmiHeader, 0, sizeof(bmpInfo.bmiHeader));

	bmpInfo.bmiHeader.biSize     = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth    =  _ulFrameWidth;	
	bmpInfo.bmiHeader.biHeight   =  -(LONG)_ulFrameHeight;	
	bmpInfo.bmiHeader.biPlanes   = 1;	
	bmpInfo.bmiHeader.biBitCount = 24;      

    ::StretchDIBits(hDC, 0, 0, ulWndWidth, ulWndHeight, 0, 0, _ulFrameWidth, _ulFrameHeight, _pData, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
    ::ReleaseDC(m_hWnd, hDC);

	return RET_OK;
}

ULONG CViewerColaboration::OpenGLRepaintWnd(BYTE *_pData, ULONG _ulDatalen, ULONG _ulFrameWidth, ULONG _ulFrameHeight)
{
	if (!_pData || !_ulDatalen || !_ulFrameWidth || !_ulFrameHeight)
		return RET_INVALID_ARG;

#if 0
	HDC hDC = ::GetDC(m_hWnd);

	if (wglMakeCurrent(hDC, m_hRC))
	{
		CHECK_ERROR(TopDownToBottomUp(_pData, _ulDatalen, _ulFrameHeight), RET_OK)
			
		RECT rcRect;
		::GetClientRect(m_hWnd, &rcRect);
			
		ULONG ulWndWidth, ulWndHeight;

		ulWndWidth  = rcRect.right  - rcRect.left;
		ulWndHeight = rcRect.bottom - rcRect.top; 
	
		float xFactor, yFactor;

		xFactor = float (ulWndWidth)  / _ulFrameWidth;
		yFactor = float (ulWndHeight) / _ulFrameHeight;

		::glPixelZoom(xFactor, yFactor);
		::glDrawPixels(_ulFrameWidth, _ulFrameHeight, GL_RGB/*GL_BGRA_EXT*/, GL_UNSIGNED_BYTE, _pData);
	}

	::ReleaseDC(m_hWnd, hDC);
#endif

	return RET_OK;
}

ULONG CViewerColaboration::SetWindowPosition(Rect & _rcRect)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ulWidth, 
          ulHeight;

    ulWidth  = _rcRect.Right  - _rcRect.Left;
    ulHeight = _rcRect.Bottom - _rcRect.Top; 

    ::MoveWindow(m_hWnd, _rcRect.Left, _rcRect.Top, ulWidth, ulHeight, TRUE);

    return RET_OK;
}

ULONG CViewerColaboration::MakeWindow(CViewer *_pParent, Rect & _rcRect)
{	
	if (!_pParent)
		return RET_INVALID_ARG;

	WNDCLASS wndClass = _pParent->GetChildClass();

	ULONG ulWidth, 
          ulHeight;

    ulWidth  = _rcRect.Right  - _rcRect.Left;
    ulHeight = _rcRect.Bottom - _rcRect.Top; 

    m_hWnd = ::CreateWindow(wndClass.lpszClassName,
                            NULL,
                            WS_CHILD | WS_VISIBLE,
                            _rcRect.Left, 
                            _rcRect.Top,
                            ulWidth, 
                            ulHeight,
                            _pParent->GetWindowHandle(), 
                            (HMENU) m_id, 
                            _pParent->GetInstance(),
                            _pParent);

	if (m_hWnd)
	{
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);  
		::UpdateWindow(m_hWnd);
		
		m_bInit = TRUE;
	}

	#ifdef _DEBUG	
		static ULONG ulDbgId = 0;
		TCHAR szCaption[200];
		
		#define DBG_WND_WIDTH	220
		#define DBG_WND_HEIGHT	400

		ulDbgId++;
		wsprintf(szCaption, "Debug Window %lu", ulDbgId);

		WNDCLASS wcDbg = _pParent->GetDbgClass();

		m_hDbgWnd = ::CreateWindow(wcDbg.lpszClassName,
								   szCaption, 
								   WS_OVERLAPPEDWINDOW,
								   0, 0,
								   DBG_WND_WIDTH, 
								   DBG_WND_HEIGHT,
								   NULL, 
								   NULL,  
								   _pParent->GetInstance(), 
								   _pParent); 
		if (m_hDbgWnd)
		{	    					
			::ShowWindow(m_hDbgWnd, SW_NORMAL);  
			//::UpdateWindow(m_hDbgWnd);
		}
	#endif
	
	return RET_OK;
}

ULONG CViewerColaboration::SetLastMediaTS(MediaSpec _mediaSpec)
{
	switch(_mediaSpec)
	{
		case VIDEO:
			GetTime(&m_ulLastVideoTime);	
		break;

		case AUDIO:
			GetTime(&m_ulLastAudioTime);
		break;

		default:
			return RET_INVALID_ARG;
	}

	return RET_OK;
}

ULONG CViewerColaboration::GetLastMediaTime()
{
	ULONG ulLastMediaTime;

	if (m_ulLastVideoTime > m_ulLastAudioTime)
		ulLastMediaTime = m_ulLastVideoTime;
	else
		ulLastMediaTime = m_ulLastAudioTime;

	return ulLastMediaTime;
}
