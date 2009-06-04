// VBLib_dll.cpp

#include "stdafx.h"
#include "VBLib/VBLib.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

#if 0
// This is an example of an exported variable
VBLIB_DLL_API int nVblib_dll=0;

// This is an example of an exported function.
VBLIB_DLL_API int fnVblib_dll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see vblib_dll.h for the class definition
CVblib_dll::CVblib_dll()
{ 
	return; 
}
#endif

////////////////////////////////////////

// usually one does not include a cpp.
// I'm doing this to keep the VBLib.cpp cross-platform
#include "..\src\VBLib\VBLib.cpp"

