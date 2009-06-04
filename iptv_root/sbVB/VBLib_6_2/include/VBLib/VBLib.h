// VBLib.h version 6.1
#ifndef _VBLIB_H_
#define _VBLIB_H_

#ifdef VBLIB_DLL_EXPORTS
	#define VBLIB_DLL_API __declspec(dllexport)
#else
	#define VBLIB_DLL_API __declspec(dllimport)
#endif

#if !defined(_MSC_VER) || defined(VBLIB_STATIC)
	// if unix or if (window and static library)
	#undef VBLIB_DLL_API
	#define VBLIB_DLL_API
#endif

#ifdef _MSC_VER	// These pragmas are MSVC-only!
#pragma warning(disable: 4251)
#pragma warning(disable: 4290) // throws ignored
#pragma warning(disable: 4996) // strcpy deprecated to strcpy_s
#endif

#include "VBLib_raw.h"

#endif	// _VBLIB_H_

