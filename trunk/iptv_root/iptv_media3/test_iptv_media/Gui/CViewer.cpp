#include "compat.h"
#include "data_reg.h"
#include "CVideoFrame.h"
#include "CViewer.h"
#include "WndProc.h"
#include "functions.h"
#include "WndNotify.h"
#include "CViewerColaboration.h"
#include "Global_functions.h"
#include "IThread.h"
#include "IInterfaceKernelNotify.h"
#include "log.h"

#define MAX_TRANSM 4

CViewer::CViewer(HINSTANCE _hInstance)
{
    m_bInit				= FALSE;
	m_bKillApp			= FALSE;
    m_hAppWnd			= NULL;
    m_pKernelNotify		= NULL;
	m_pMediaStateThread = NULL;
    m_bMainWndCreated	= FALSE;
    m_hInstance			= _hInstance;
    m_ulMaxTransm		= MAX_TRANSM;

    memset(&m_wcParent, 0, sizeof(WNDCLASS));
    memset(&m_wcChild,  0, sizeof(WNDCLASS));
	memset(&m_wcDbg,    0, sizeof(WNDCLASS));
 
	m_pSemaph = new _SEMAPHORE();

	if (m_hInstance && m_pSemaph)
	{
		if ((CreateChildWndClass()  == RET_OK) && 
			(CreateParentWndClass() == RET_OK) && 
			(CreateDbgWndClass()    == RET_OK))
		{      
            m_bInit = TRUE;
		}
	} 
}

CViewer::~CViewer()
{
    if (m_pSemaph)
		delete m_pSemaph;

	if (m_pMediaStateThread)
		delete m_pMediaStateThread;

	while (m_WndList.size())
    {
        if (m_WndList[0])
            delete m_WndList[0];

        m_WndList.pop_front();
    }
}

ULONG CViewer::CloseApp()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ret = m_pKernelNotify->TerminateConference();

	m_bKillApp = TRUE;
	m_pMediaStateThread->WaitForMe(INFINITE_TIME);

    return ret;
}


ULONG CViewer::CreateMainWnd()
{
    if (!m_bInit)
        return RET_INIT_ERROR;    
    
    if (m_bMainWndCreated)
        return RET_ERROR;

	m_hAppWnd = ::CreateWindow(m_wcParent.lpszClassName, 
                               m_wcParent.lpszClassName, 
                               WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
                               0, 0,
                               1024, 768,
                               NULL, NULL, m_hInstance, this);
        
    if (m_hAppWnd)
    {    
        ::ShowWindow(m_hAppWnd, SW_NORMAL);  
        ::UpdateWindow(m_hAppWnd);
        ::SetFocus(m_hAppWnd);

        m_bMainWndCreated = TRUE;
    }
    else
        return RET_ERROR;

	m_pMediaStateThread = CreateIThread(_CheckMediaState, this, TRUE);

	if (m_pMediaStateThread)
		EnterMessageLoop();
	else
		return RET_LOW_MEMORY;

    return RET_OK;
}

ULONG CViewer::CreateParentWndClass()
{ 
    m_wcParent.style         = CS_HREDRAW | CS_VREDRAW;
    m_wcParent.lpfnWndProc   = MainWndProc;
    m_wcParent.cbClsExtra    = 0;
    m_wcParent.cbWndExtra    = 0;
    m_wcParent.hInstance     = m_hInstance;
    m_wcParent.hIcon         = ::LoadIcon(m_hInstance, IDI_APPLICATION);
    m_wcParent.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    m_wcParent.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);   
    m_wcParent.lpszMenuName  = TEXT("");
    m_wcParent.lpszClassName = TEXT("Parent Window");

    if (!::RegisterClass(&m_wcParent))
        return RET_ERROR;

    return RET_OK;
}

ULONG CViewer::CreateChildWndClass()
{
    m_wcChild.style         = CS_HREDRAW | CS_VREDRAW;
    m_wcChild.lpfnWndProc   = ChildWndProc;
    m_wcChild.cbClsExtra    = 0;
    m_wcChild.cbWndExtra    = 0;
    m_wcChild.hInstance     = m_hInstance;
    m_wcChild.hIcon         = ::LoadIcon(m_hInstance, IDI_APPLICATION);
    m_wcChild.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    m_wcChild.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);   
    m_wcChild.lpszMenuName  = TEXT("");
    m_wcChild.lpszClassName = TEXT("Child Window");

    if (!::RegisterClass(&m_wcChild))
        return RET_ERROR;

    return RET_OK;
}
ULONG CViewer::CreateDbgWndClass()
{
    m_wcDbg.style         = CS_HREDRAW | CS_VREDRAW;
    m_wcDbg.lpfnWndProc   = DbgWndProc;
    m_wcDbg.cbClsExtra    = 0;
    m_wcDbg.cbWndExtra    = 0;
    m_wcDbg.hInstance     = m_hInstance;
    m_wcDbg.hIcon         = ::LoadIcon(m_hInstance, IDI_APPLICATION);
    m_wcDbg.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
    m_wcDbg.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);   
    m_wcDbg.lpszMenuName  = TEXT("");
    m_wcDbg.lpszClassName = TEXT("Dbg Window");

    if (!::RegisterClass(&m_wcDbg))
        return RET_ERROR;

    return RET_OK;
}


ULONG CViewer::CreateMediaWnd(ULONG _id)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	ULONG ret = RET_OK;

	if (!::PostMessage(m_hAppWnd, WM_ACTIVATE_WND, NULL, _id))
		ret = RET_ERROR;

	return ret;
}

ULONG CViewer::DestroyMediaWnd(ULONG _id)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	ULONG ret = RET_OK;

	if (!::PostMessage(m_hAppWnd, WM_DEACTIVATE_WND, NULL, _id))
		ret =  RET_ERROR;

#ifdef _DEBUG
    TCHAR szBuf[200];
    wsprintf(szBuf, TEXT("CViewer::DestroyMediaWnd - id: %lu"), _id);
    g_FileLog.SetLog(szBuf);
#endif

    return ret;
}

ULONG CViewer::ActivateWnd(ULONG _id)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	m_pSemaph->Wait();

	ULONG ret;

    VideoFrame_reg *pVideoReg;
    
	if ( (ret = m_pKernelNotify->GetVideoReg(_id, &pVideoReg)) == RET_OK)
	{
		Rect rcRect(0, 0, 0, 0);
		CViewerColaboration *pWnd = new CViewerColaboration(this, rcRect, _id, pVideoReg); 

		if (pWnd)
		{	
			m_WndList.push_back(pWnd);
            
            if ((ret = PopQueuedWndId(_id)) == RET_OK)
                ret = ResizeWindows();
        }
		else
			ret = RET_LOW_MEMORY;    			
	}

	m_pSemaph->Signal();

    return ret;

}

ULONG CViewer::PopQueuedWndId(ULONG _id)
{
    ULONG ulListSize,
          ret;

    ulListSize = (ULONG)m_NewIdList.size();

    if (!ulListSize)
        return RET_ERROR;

    ret = RET_INVALID_ARG;

    std::deque<ULONG>::iterator it;
    it = m_NewIdList.begin();

    for (ULONG i=0; i < ulListSize; i++, it++)
    {
        if (m_NewIdList[i] == _id)
        {    
            m_NewIdList.erase(it);
            ret = RET_OK;
            break;
        }
    }

    return ret;
}

ULONG CViewer::DeactivateWnd(ULONG _id)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

    ULONG ulListSize,
          ret;

	m_pSemaph->Wait();

    ulListSize = (ULONG)m_WndList.size();
    
    if (ulListSize)
    {
	    ret = RET_INVALID_ARG;

	    std::deque<CViewerColaboration *>::iterator it;
	    it = m_WndList.begin();

	    for(ULONG i=0; i < ulListSize; i++, it++)
	    {	
		    if (m_WndList[i]->GetId() == _id)
		    {	
			    delete m_WndList[i];
			    m_WndList.erase(it);
	
		    	ret = ResizeWindows();		
			    break;
		    }
	    }
    }
    else
        ret = RET_ERROR;

	m_pSemaph->Signal();

	return ret;
}

ULONG CViewer::ResizeWindows()
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	ULONG ulWndCount = (ULONG) m_WndList.size();

	Rect rcRect(0, 0, 0, 0);

    if (ulWndCount < m_ulMaxTransm)
    {
        for (ULONG ulIndex=0; ulIndex < ulWndCount; ulIndex++)
        {    
			CalcWndPos(ulWndCount, ulIndex, rcRect);
			m_WndList[ulIndex]->SetWindowPosition(rcRect);
		}
    }
    else
        return RET_TOO_MANY_WINDOWS;

	return RET_OK;
}


ULONG CViewer::Draw(HWND _hWnd)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	if (!_hWnd)
		return RET_INVALID_ARG;

	m_pSemaph->Wait();

	ULONG ret = RET_INVALID_ARG;

	for(ULONG i=0; i<m_WndList.size(); i++)
	{	
		if (m_WndList[i]->GetWindowHandle() == _hWnd)
		{	
			ret = m_WndList[i]->DrawFrame();
			break;
		}
	}

	m_pSemaph->Signal();

	return ret;
}

ULONG CViewer::InitConference(LPCSTR _szFilename)
{
    if (!m_bInit)
        return RET_INIT_ERROR; 

	m_pSemaph->Wait();

    ULONG ret = m_pKernelNotify->InitNetwork(_szFilename);

	m_pSemaph->Signal();

    return ret;
}

ULONG CViewer::InitConference(LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen)
{
    if (!m_bInit)
        return RET_INIT_ERROR; 

	m_pSemaph->Wait();	

    ULONG ret = m_pKernelNotify->InitNetwork(_szProtocol, _szHost, _dwPort, _szConnStr, _flListen);

	m_pSemaph->Signal();
    
	return ret;
}

ULONG CViewer::GetDbgInfo(HWND _hWnd, MediaDbgInfo & _mediaDbgInfo)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	if (!_hWnd)
		return RET_INVALID_ARG;

	m_pSemaph->Wait();

	ULONG id,
		  ret;

	ret = RET_INVALID_ARG;

	for(ULONG i=0; i<m_WndList.size(); i++)
	{	
		if (m_WndList[i]->GetWindowHandle() == _hWnd)
		{	
			id = m_WndList[i]->GetId();
			ret = RET_OK;
			break;
		}
	}

	if (ret == RET_OK)
		ret = m_pKernelNotify->GetMediaDbgInfo(id, _mediaDbgInfo);

	m_pSemaph->Signal();

	return RET_OK;
}

ULONG CViewer::GetDbgWndMap(HWND *_phColaborationWnd, HWND _hDbgWnd)
{
    if (!m_bInit)
        return RET_INIT_ERROR;

	if (!_phColaborationWnd || !_hDbgWnd)
		return RET_INVALID_ARG;

	*_phColaborationWnd = 0;

	m_pSemaph->Wait();
	
	ULONG ret = RET_INVALID_ARG;

	for(ULONG i=0; i<m_WndList.size(); i++)
	{	
		if (m_WndList[i]->GetDbgWndHandle() == _hDbgWnd)
		{	
			*_phColaborationWnd = m_WndList[i]->GetWindowHandle();
			ret = RET_OK;
			break;
		}
	}

	m_pSemaph->Signal();

	return ret;
}

ULONG CViewer::DestroyIdleWnd()
{
	if (!m_bInit)
		return RET_INIT_ERROR;
	
	ULONG ulLastMediaTime,
		  ulCurTime,
		  ret,
		  id;

	m_pSemaph->Wait();

	ret = RET_OK;

    for (ULONG i=0; i<m_WndList.size(); i++)
    {           		
		ulLastMediaTime = m_WndList[i]->GetLastMediaTime();
		GetTime(&ulCurTime);

		if ((ulCurTime - ulLastMediaTime) >= MAX_WAIT_BEFORE_DESTROY_WND_MS)
		{	
			id = m_WndList[i]->GetId();
			
			if ( (ret = DestroyMediaWnd(id)) != RET_OK)
				break;
		}
	}
		
	m_pSemaph->Signal();

	return ret;
}


int CViewer::_CheckMediaState(IThread *_pthread, void* _pCtx)
{
    CViewer *pCtx = (CViewer *) _pCtx;

#ifdef _DEBUG
	g_FileLog.SetLog("Entering CViewer::_CheckMediaState");
#endif

    while (!pCtx->m_bKillApp)
    {
		pCtx->DestroyIdleWnd();
		Wait(500);
    }

#ifdef _DEBUG
	g_FileLog.SetLog("Leaving CViewer::_CheckMediaState");
#endif

    return RET_OK;
}

ULONG CViewer::SetMediaAlive(ULONG _id, MediaSpec _mediaSpec)
{

	if (!m_bInit)
		return RET_INIT_ERROR;

	m_pSemaph->Wait();

    ULONG ret = RET_OK;

	BOOL bNewId = TRUE;

	for (ULONG i=0; i < m_WndList.size(); i++)
	{
		if (m_WndList[i]->GetId() == _id)
		{
			m_WndList[i]->SetLastMediaTS(_mediaSpec);
			bNewId = FALSE;
			break;
		}
	}

	if (bNewId)
	{	
		BOOL bIdFound = FALSE;
		for (ULONG i=0; i < m_NewIdList.size(); i++)
		{
			if (m_NewIdList[i] == _id)
				bIdFound = TRUE;
		}

		if (!bIdFound)
        {		
            m_NewIdList.push_back(_id);
            ret = CreateMediaWnd(_id);
        }
    }

	m_pSemaph->Signal();

	return ret;
}

ULONG CViewer::CalcWndPos(ULONG _ulWndCount, ULONG _ulIndex, Rect & _rcRect)
{
    if ((_ulWndCount > 4) || (_ulIndex > 3))
        return RET_ERROR;

    if (_ulWndCount == 1)
        _rcRect =  Rect(46-1, 0, 932+46-1, 700-1); 
    else
    if (_ulWndCount == 2)
    {
        switch(_ulIndex)
        {
        case 0:
            _rcRect = Rect(30-1, 181-1, 30+466-1, 181+350-1);
        break;

        case 1:
            _rcRect = Rect(30+466+32-1, 181-1, 30+466+32+466-1, 181+350-1);
        break;
        }
    }
    else
    if ((_ulWndCount == 3) || (_ulWndCount == 4))
    {
        switch(_ulIndex)
        {
        case 0:
            _rcRect = Rect(30, 0, 30+466-1, 350-1);
        break;

        case 1:
            _rcRect = Rect(30+466+32-1, 0, 30+466+32+466-1, 350-1);
        break;

        case 2:
            _rcRect = Rect(30, 350+12-1, 30+466-1, 350+12+350-1);
        break;

        case 3:
            _rcRect = Rect(30+466+32-1, 350+12-1, 30+466+32+466-1, 350+12+350-1);
        break;
        }
    }

    return RET_OK;
}



