#include "compat.h"
#include "CThreadSafeDeque.h"
#include "Kernel.h"
#include "CViewer.h"
#include "log.h"

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, PSTR szCmdLine, int iCmdShow)
{ 
#ifdef _DEBUG
    g_FileLog.SetLog("Entering WinMain...");
#endif
   
    Kernel  kernel;
    CViewer viewer(hInstance);

    kernel.SetGraphicModule(&viewer);
    viewer.SetKernelNotify(&kernel);

    viewer.CreateMainWnd();

  
#ifdef _DEBUG
    g_FileLog.SetLog("Leaving WinMain...");
#endif

    return 0;
}
