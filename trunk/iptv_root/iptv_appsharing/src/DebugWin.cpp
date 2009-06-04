#include "DebugWin.h"
#include "tchar.h"

#define LISTID    0x0001

// Creates a pop-up window
LRESULT g_DebugWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    long ret;
    CREATESTRUCT *pCS;
    RECT rc;
    DebugWin *pThis = (DebugWin *)GetWindowLong( hWnd, GWL_USERDATA );

    switch( message )
    {
        case WM_CREATE:
            pCS = (CREATESTRUCT *)lParam;
            SetWindowLong( hWnd, GWL_USERDATA, (LONG)(pCS->lpCreateParams) );
            pThis = (DebugWin *)pCS->lpCreateParams;

            GetClientRect( hWnd, &rc );

            pThis->m_hList = CreateWindow( _T("LISTBOX"), NULL, WS_VISIBLE | WS_CHILD| WS_VSCROLL | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
                                            0, 0, rc.right - rc.left, rc.bottom - rc.top,
                                            hWnd, (HMENU)LISTID, pThis->m_hInstance, NULL );

            ShowWindow( pThis->m_hList, SW_SHOW );
            SendMessage( pThis->m_hList, WM_SETFONT, (WPARAM)GetStockObject( SYSTEM_FIXED_FONT ), TRUE );

            ret = 0;
            break;

        case WM_SIZE:
            SetWindowPos( pThis->m_hList, hWnd, 0, 0, LOWORD( lParam ), HIWORD( lParam ), SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
            ret = 0;
            break;

        case WM_DESTROY:
            pThis->m_hWnd = NULL;
            ret = 0;
            break;

        default:
            ret = ::DefWindowProc( hWnd, message, wParam, lParam );
    }

    return ret;
}

DebugWin::DebugWin( HINSTANCE hInstance )
{
    WNDCLASS wndclass;

    m_hWnd  = NULL;
    m_hList = NULL;
    m_hInstance = hInstance;

    if( !GetClassInfo( hInstance, _T("CxDebugWin"), &wndclass) )
    {
        m_wndclass.style         = CS_HREDRAW | CS_VREDRAW;
        m_wndclass.lpfnWndProc   = (WNDPROC)g_DebugWindowProc;
        m_wndclass.cbClsExtra    = 0;
        m_wndclass.cbWndExtra    = 0;
        m_wndclass.hInstance     = m_hInstance;
        m_wndclass.hIcon         = NULL;
        m_wndclass.hCursor       = LoadCursor( NULL, IDC_ARROW );
        m_wndclass.hbrBackground = GetSysColorBrush( COLOR_WINDOW );
        m_wndclass.lpszMenuName  = NULL;
        m_wndclass.lpszClassName = _T("CxDebugWin");

        RegisterClass( &m_wndclass );
    }


}

DebugWin::~DebugWin( void )
{

}

bool DebugWin::Create( void )
{
    m_hWnd = CreateWindow( _T("CxDebugWin"), _T("CxDebugWin"), WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           GetDesktopWindow( ), NULL, m_hInstance, (LPVOID)this );

    ShowWindow( m_hWnd, SW_SHOW );

    return m_hWnd != NULL;
}

bool DebugWin::Destroy( void )
{
    return DestroyWindow( m_hWnd ) != 0;
}

bool DebugWin::AddString( char *pStr )
{
    long index;

    if( !m_hWnd )
        return false;

    index = SendMessage( m_hList, LB_ADDSTRING, 0, (LPARAM) pStr );
    SendMessage( m_hList, LB_SETCURSEL, index, (LPARAM) 0 );

    return true;
}