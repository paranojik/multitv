// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ASMODULE_API_*
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ASMODULE_API_* functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
////////////////////////////////////////////////////////////////
#pragma once

// Generates DLL for Cross Platform Client
#ifdef ASMODULE_EXPORTS_CROSS
#define ASMODULE_API_CROSS __declspec(dllexport)
#else
#define ASMODULE_API_CROSS 
#endif // ASMODULE_EXPORTS_CROSS

// Generates DLL for Windows only Client
#ifdef ASMODULE_EXPORTS_WIN
#define ASMODULE_API_WIN __declspec(dllexport)
#else
#define ASMODULE_API_WIN 
#endif // ASMODULE_EXPORTS_WIN
