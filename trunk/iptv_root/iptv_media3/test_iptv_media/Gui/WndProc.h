#ifndef WNDPROC_H
#define WNDPROC_H

#include <windows.h>

LRESULT CALLBACK MainWndProc (HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
LRESULT CALLBACK ChildWndProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);
LRESULT CALLBACK DbgWndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

#endif