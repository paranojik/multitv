#include "compat.h"
#include <gl/gl.h>
#include "WndProc.h"
#include "CViewer.h"
#include "WndNotify.h"
#include "functions.h"


#define FILE_PATH "c:\\ArquivosIMC\\arquivoIMC36.imc"

LRESULT CALLBACK MainWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{      
    static CViewer *pViewer;
	CREATESTRUCT *pCreateStruct;
	ULONG id;

    switch (_msg)
    {
        case WM_CREATE:

			pCreateStruct = (CREATESTRUCT *) _lParam;
			pViewer = (CViewer *) pCreateStruct->lpCreateParams;
			//"229.1.1.1"
            pViewer->InitConference(TEXT("MCAST"), TEXT("228.1.1.1"), 8011, NULL, TRUE);
            //pViewer->InitConference(TEXT("MCAST"), TEXT("229.1.1.5"), 8011, NULL, TRUE);
			//pViewer->InitConference(FILE_PATH);
		break;
        
		case WM_ACTIVATE_WND:
			id = (ULONG)_lParam;
			
			if (pViewer->ActivateWnd(id) == RET_TOO_MANY_WINDOWS)
                ::MessageBox(_hWnd, TEXT("Too many windows"), TEXT("Error!"), MB_OK);
		break;

        case WM_DEACTIVATE_WND:
            id = (ULONG)_lParam;        
			pViewer->DeactivateWnd(id);	
        break;
        
        case WM_DESTROY:        
            pViewer->CloseApp();
            PostQuitMessage(0);
        break;

		default:
			return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
    }
    
	return 0;
}


LRESULT CALLBACK ChildWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{      
    static CViewer *pViewer = NULL;
    static UINT_PTR ulTimerId = 0;
	CREATESTRUCT *pCreateStruct;

	switch (_msg)
    {
        case WM_CREATE:

			if (!pViewer)
			{
				pCreateStruct = (CREATESTRUCT *) _lParam;
				pViewer = (CViewer *) pCreateStruct->lpCreateParams;
			}
			ulTimerId += 2;
			::SetTimer(_hWnd, ulTimerId, 30, NULL);
		break;
    
		case WM_TIMER:
			pViewer->Draw(_hWnd);
		break;

		default:
			return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
    }
    
	return 0;
}

LRESULT CALLBACK DbgWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
    static CViewer *pViewer = NULL;
	static UINT_PTR ulTimerId = 1;
	CREATESTRUCT *pCreateStruct;

	switch(_msg)
	{
        case WM_CREATE:
			
			if (!pViewer)
			{
				pCreateStruct = (CREATESTRUCT *) _lParam;
				pViewer = (CViewer *) pCreateStruct->lpCreateParams;
			}
			
			ulTimerId += 2;
			::SetTimer(_hWnd, ulTimerId, 100, NULL);
		
		break;

		case WM_TIMER:
			InvalidateRect(_hWnd, NULL, TRUE);
		break;

		case WM_PAINT:
		{	
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(_hWnd, &ps);

			MediaDbgInfo mediaDbgInfo;
			HWND hColaborationWnd;
			
            pViewer->GetDbgWndMap(&hColaborationWnd, _hWnd);
			pViewer->GetDbgInfo(hColaborationWnd, mediaDbgInfo);

			SetTextColor(hDC, RGB(255, 255, 255));
			SetBkColor(hDC, RGB(0, 0, 0));

			WriteDebugInfo(hDC, mediaDbgInfo);

			::EndPaint(_hWnd, &ps);
		}
		break;

		default:
			return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
	}

	return 0;
}