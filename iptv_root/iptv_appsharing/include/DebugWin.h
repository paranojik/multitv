#include <windows.h>
#include "AsModuleWin.h"

#ifndef DEBUGWIN__H
#define DEBUGWIN__H

//
LRESULT g_DebugWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

class DebugWin
{
    private:
        HWND m_hWnd, m_hList;
        WNDCLASS m_wndclass;
        HINSTANCE m_hInstance;

        HFONT m_hFont;

    public:
        DebugWin( HINSTANCE hInstance );
        ~DebugWin( void );

        bool Create( void );
        bool Destroy( void );

        bool AddString( char *pStr );

        friend LRESULT g_DebugWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
};

#endif