// VBLib_raw.h version 6.0
#ifndef __VBLIB__
#define __VBLIB__

/**************************************************************** 
VBLib stands for "Villas-Boas Library"
VBLib is cross-platform (works exactly equal in windows and unix)
by sbVB: Sergio Barbosa Villas-Boas (sbvb@sbvb.com.br)
sbVB home page: http://www.sbvb.com.br
VBLib home page: http://www.vbmcgi.org/vblib
****************************************************************/

// comment out this line to get rid of debug
//#define VBSTRINGDEBUG

/////////////////////////////////////////////////////////////////////
//
// includes of VBLib
//
/////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER		// pragmas are MSVC-only
#pragma warning( disable : 4786 )
#pragma warning( disable : 4275 ) // complex in dll
#endif

#include <deque>
#include <complex> // namespace version of header

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>  // fabs
#include <ctime>  // time

#ifdef _MSC_VER  // Visual C++ only !!!
//	#include <crtdbg.h> // include necessary to handle memory leak debug (win32 only)
	#include <atlbase.h> // BSTR
//	#include <windows.h> // for sys info
//	#include <wincon.h> // to use wincon, link project with netapi32.lib
#else // unix only
	#include <sys/stat.h> // struct stat, S_ISDIR
	#include <dirent.h> // DIR
#endif

/////////////////////////////////////////////////////////////////////
//
// defines of VBLib
//
/////////////////////////////////////////////////////////////////////

// typedef complex<float> fcomplex;
// typedef complex<double> dcomplex;
// typedef complex<long double> dlcomplex;


#define VB_SAFEDELETE(x) {if (x) {delete x; x=0;} }
#define VB_SAFEDELETE_A(x) {if (x) {delete [] x; x=0;} }
#ifndef NULL
	#define NULL 0
#endif

#ifdef DEFINE_GLOBAL_VARIABLES_HERE
#define EXTERN
#else
#define EXTERN extern
#endif


////////////////////////////////////////////////////////////////////
//
// Definition of complex types
//
/////////////////////////////////////////////////////////////////////

typedef std::complex<float> VBComplex_F;
typedef std::complex<double> VBComplex_D;
typedef std::complex<long double> VBComplex_LD;


/////////////////////////////////////////////////////////////////////
//
// include modules of VBLib
//
/////////////////////////////////////////////////////////////////////



#include "VBString.h" // before VBMD5, before VBException, because it uses VBString
#include "VBException.h" // before the others, to allow derived classes
#include "VBClock.h"
#include "VBList.h"
#include "VBShowSTLContainer.h"
#include "VBBinaryData.h" // before VBMD5
#include "VBMD5.h"
#include "VBDate.h"
#include "VBFileDirectory.h"
#include "VBCrossDLL.h"
#include "VBProcessArgs.h"

// that include is now part of VBMath
// #include "VBGenMath.h"


// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// global functions
//
/////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
// functions for version control
VBLIB_DLL_API const char *getBuildId();
VBLIB_DLL_API const char *getBuildDate();
VBLIB_DLL_API const char *getBuildStructDate();
VBLIB_DLL_API const char *getVersion();


VBLIB_DLL_API char *strsrep(char *str,const char *search,const char *replace);
VBLIB_DLL_API char *strinc(const char *str1,char *str2);
VBLIB_DLL_API char *strschg(char *str,const char *find,const char *replace);
VBLIB_DLL_API char *strdel(const char *substr,char *str);
VBLIB_DLL_API char *strins(const char *instr,char *str,unsigned st_pos);

// global variable to point to custom assert function
VBLIB_DLL_API void VBAssert(bool b, const char * message ="(undefined)");
// extern VBLIB_DLL_API int myGlobal;
extern VBLIB_DLL_API void (*g_customAssertFunction)
  (bool b, const char * message); // function pointer


// VBLIB_DLL_API DWORD CPU_info();
// VBLIB_DLL_API VBString getSystemHash();

////////////////////////////////////////////////////////////////////
// functions to convert ascii and related

VBLIB_DLL_API unsigned char charInt2ascii(unsigned char in);
VBLIB_DLL_API unsigned char charInt2asciiReverse(unsigned char in);
VBLIB_DLL_API unsigned char charIntString2ascii(::br::com::sbVB::VBLib::VBString in);
VBLIB_DLL_API ::br::com::sbVB::VBLib::VBString charInt2asciiString(unsigned char in);
VBLIB_DLL_API ::br::com::sbVB::VBLib::VBString ascii2charIntString(unsigned char in);
VBLIB_DLL_API unsigned char asciiString2charInt(::br::com::sbVB::VBLib::VBString in);

////////////////////////////////////////////////////////////////////
// other global functions

// return true if the file exists. leave the file not oppened
VBLIB_DLL_API bool fileExist(const char *fileName);

// place the contents of a file to a string, an throw exception if can't open file
VBLIB_DLL_API void getFileContentsToString(const char *fileName, VBString & contents)
throw (::br::com::sbVB::VBLib::VBException);

// return true if char is a space char (or similar, like tab)
VBLIB_DLL_API  bool isSpaceChar(char c);


VBLIB_DLL_API bool nearEqual(double x, double y);

VBLIB_DLL_API int VBmin(int x, int y);
VBLIB_DLL_API int VBmax(int x, int y);


////////////////////////////////////////////////////////////////////
// global functions for identification in VBDllReport

VBLIB_DLL_API const char *getBuildID();
VBLIB_DLL_API const char *getBuild_dateTime();
VBLIB_DLL_API const char *getBuild_dateTime_str();
VBLIB_DLL_API const char *getUserVersion();
VBLIB_DLL_API const char *getLibraryDescription();


VBLIB_DLL_API void buildID();

}}}} // close namespace ::br::com::sbVB::VBLib



#endif // __VBLIB__

