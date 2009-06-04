#ifndef CVIEWER_H
#define CVIEWER_H

#include <deque>
#include "IKernelInterfaceNotify.h"
#include "types.h"
#include "Data_reg.h"

class IThread;
class _SEMAPHORE;
class CViewerColaboration;
class CVideoFrame;
class IInterfaceKernelNotify;

class CViewer : public IKernelInterfaceNotify
{
private:

	typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

    BOOL  m_bInit,
          m_bMainWndCreated,
		  m_bKillApp;

    ULONG m_ulMaxTransm;

    HWND  m_hAppWnd;
    
    HINSTANCE m_hInstance; 
    
    WNDCLASS  m_wcParent, 
              m_wcChild,
			  m_wcDbg;
    
	_SEMAPHORE *m_pSemaph;
	IThread	   *m_pMediaStateThread;

    IInterfaceKernelNotify *m_pKernelNotify;

    std::deque<CViewerColaboration *> m_WndList;
	std::deque<ULONG> m_NewIdList;

    ULONG CreateParentWndClass();
    ULONG CreateChildWndClass();
	ULONG CreateDbgWndClass();

	ULONG ResizeWindows();
    ULONG CalcWndPos(ULONG _ulWndCount, ULONG _ulIndex, Rect & _rcRect);

	ULONG CreateMediaWnd(ULONG _id);
    ULONG DestroyMediaWnd(ULONG _id);
    ULONG PopQueuedWndId(ULONG _id);

	static int _CheckMediaState(IThread *_pthread, void* _pCtx);

    // IKernelInterfaceNotify overrides
	virtual ULONG SetMediaAlive(ULONG _id, MediaSpec _mediaSpec);


public:

	friend class IKernelInterfaceNotify;

    CViewer(HINSTANCE _hInstance);
    ~CViewer();

    ULONG CreateMainWnd();

    ULONG InitConference(LPCSTR _szFilename);
    ULONG InitConference(LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen);

	ULONG ActivateWnd(ULONG _id);
	ULONG DeactivateWnd(ULONG _id);

	ULONG Draw(HWND _hWnd);

    ULONG CloseApp();

	ULONG GetDbgInfo(HWND _hWnd, MediaDbgInfo & _mediaDbgInfo);
	ULONG GetDbgWndMap(HWND *_phColaborationWnd, HWND  _hDbgWnd);
	
	ULONG DestroyIdleWnd();

	void SetKernelNotify(IInterfaceKernelNotify *_pKernelNotify) {m_pKernelNotify = _pKernelNotify; }

	HWND GetWindowHandle()	  {return m_hAppWnd;	}
	HINSTANCE GetInstance()   {return m_hInstance;	}
	
	WNDCLASS  GetChildClass() {return m_wcChild;	}
	WNDCLASS  GetDbgClass()	  {return m_wcDbg;		}
};

#endif