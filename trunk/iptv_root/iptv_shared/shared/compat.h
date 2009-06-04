#ifndef _COMPAT_STR
#define _COMPAT_STR


/* Macros usadas no Projeto Imagem - Encontrei referencia sobre elas no arquivo NWSQLINT.c*/
/* Nao vejo muita utilidade para o FP_OFF */
#undef FP_SEG
#undef FP_OFF
#define FP_SEG(fp) ( * ( (unsigned *) &(fp) + 1 ) )
#define FP_OFF(fp) ( * ( (unsigned *) &(fp) ) )


typedef void * * VOID_FPFP;

#if defined(_LINUX)
#define __FD_SETSIZE    1024
#else
// Definir aqui o numero maximo de conexoes por socket
#define FD_SETSIZE      1024

// Diretiva para evitar warnings de seguranca na manipulacao de strings
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#endif

// Para desabilitar Send Queue comentar a linha abaixo
#define _SENDQ_SLOTS     1024
#define _SENDQ_PKTSIZE   8192


#define WAITFORENDTHREADSTIME    1000


#if defined(__BORLANDC__)
    #include <alloc.h>
    #include <mem.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <process.h>
#elif defined (_LINUX)
    //#include <alloc.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <time.h>
#else
    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <process.h>
    #include <time.h>
#endif  // __BORLANDC__



#if defined(_WIN32)


#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>


#define ENOBUFS         WSAENOBUFS
#define EWOULDBLOCK     WSAEWOULDBLOCK
#define EINPROGRESS     WSAEINPROGRESS

#ifndef EAGAIN
#define EAGAIN          WSAEWOULDBLOCK
#endif  // EAGAIN


#else

// ! _WIN32

//#include <sys/socket.h>
#include <netinet/in.h>


/* Chose only one of NBLOCK_POSIX, NBLOCK_BSD, and NBLOCK_SYSV */
/* Define if you have posix non-blocking sockets (O_NONBLOCK) */
#define NBLOCK_POSIX 1

/* Define if you have BSD non-blocking sockets (O_NDELAY) */
/* #undef NBLOCK_BSD */

/* Define if you have SYSV non-blocking sockets (FIONBIO) */
/* #undef NBLOCK_SYSV */
 

#define FAR
#define __cdecl


typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;

typedef const char    *LPCTSTR;
typedef const char    *LPCSTR;
typedef char          *LPSTR;
typedef char          *LPTSTR;

typedef long           LONG;
typedef unsigned long  ULONG;
typedef unsigned long  DWORD;

typedef int            BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef float          FLOAT;
typedef double         DOUBLE;

typedef FLOAT         *PFLOAT;
typedef BOOL          *LPBOOL;
typedef BYTE          *LPBYTE;
typedef int           *LPINT;
typedef WORD          *LPWORD;
typedef long          *LPLONG;
typedef void          *PVOID;
typedef void          *LPVOID;
typedef const void    *LPCVOID;

typedef int            HFILE;
typedef void          *HANDLE;
typedef HANDLE         HGLOBAL;
typedef HANDLE        *PHANDLE;

typedef LONG           HRESULT;
#define STDMETHODIMP   LONG

#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)

#define STG_E_INVALIDPOINTER    _HRESULT_TYPEDEF_(0x80030009L)
#define STG_E_INCOMPLETE        _HRESULT_TYPEDEF_(0x80030201L)
#define STG_E_ACCESSDENIED      _HRESULT_TYPEDEF_(0x80030005L)
#define STG_E_WRITEFAULT        _HRESULT_TYPEDEF_(0x8003001DL)
#define STG_S_BLOCK             _HRESULT_TYPEDEF_(0x00030201L)

#define S_OK                    ((HRESULT)0x00000000L)
#define S_FALSE                 ((HRESULT)0x00000001L)

#ifndef HFILE_ERROR
#define HFILE_ERROR     -1
#endif

typedef unsigned int   SOCKET;

/*
 * This is used instead of -1, since the
 * SOCKET type is unsigned.
 */
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)


// Temporario!
typedef void           OFSTRUCT;


#ifndef INT64
typedef long long int  INT64;
#endif
#ifndef ULONG64
typedef unsigned long long int ULONG64;
#endif

#define INFINITE      0xFFFFFFFF

// Este parametro nao existe em Linux
#define O_BINARY      0x00

#define _O_RDONLY     O_RDONLY
#define _O_BINARY     O_BINARY
#define _O_WRONLY     O_WRONLY
#define _O_RDWR       O_RDWR
#define _O_CREATE     O_CREATE
#define _O_EXCL       O_EXCL
#define _O_TRUNC      O_TRUNC
#define _O_APPEND     O_APPEND


#endif  // _WIN32


#ifndef INT8
typedef signed char    INT8;
#endif
#ifndef INT16
typedef short          INT16;
#endif
#ifndef INT32
typedef int            INT32;
#endif
#ifndef SHORT
typedef short          SHORT;
#endif
#ifndef USHORT
typedef unsigned short USHORT;
#endif
#ifndef UINT8
typedef unsigned char  UINT8;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
#endif
#ifndef UINT32
typedef unsigned int   UINT32;
#endif
#ifndef UINT
typedef unsigned int   UINT;
#endif


// Memory Manager (Para desativar comentar as 3 linhas abaixo)

//#if defined(_DEBUG) && !defined(_AFXDLL)
//    #define _MM_
//#endif


#define XMIN( a, b ) ( (a) > (b) ? (b) : (a) )
#define XMAX( a, b ) ( (a) > (b) ? (a) : (b) )


#include "../shared/def_ap.h"
#include "../shared/str_ap.h"
#include "../shared/memmgr.h"
#include "../shared/XResult.h"
#include "../shared/XStr.h"
#include "../shared/XException.h"



#if !defined(__BORLANDC__)
   // String functions

   #ifdef __cplusplus
      extern "C" {
   #endif

   int    _fstrcmp   ( const char *str1, const char *str2 );
   int    _fstrncmp  ( const char *str1, const char *str2, size_t n );
   char * _fstrncpy  ( char *strDest, const char *strSource, size_t count );
   int    _fstricmp  ( const char *string1, const char *string2 );
   int    _fstrnicmp ( const char *string1, const char *string2, size_t count );
   char * _fstrupr   ( char *string );
   char * _fstrchr   ( char *string, int c );
   char * _fstrstr   ( char *string, const char *strCharSet );

   double _atold     ( const char *string );


   // Memory Management functions

   void * _fmemset   ( void *dest, int c, size_t count );
   void * _fmemmove  ( void *dest, const void *src, size_t count );
   void * _fmemcpy   ( void *dest, const void *src, size_t count );
   int    _fmemcmp   ( const void *buf1, const void *buf2, size_t count );

   #if defined(_LINUX)

       int    lstrlen(LPCTSTR lpString);
       LPTSTR lstrcpy(LPTSTR lpString1, LPCTSTR lpString2);
       LPTSTR lstrcat(LPTSTR lpString1, LPCTSTR lpString2);

       int _stricmp  (LPCSTR string1  , LPCSTR string2);
       int stricmp   (LPCSTR string1  , LPCSTR string2);
       int _strnicmp (LPCSTR string1  , LPCSTR string2, size_t count);
       int strnicmp  (LPCSTR string1  , LPCSTR string2, size_t count);

       int _snprintf (LPSTR buffer, size_t size, LPCSTR format, ... );
       int _vsnprintf(LPSTR buffer, size_t size, LPCSTR format, va_list argptr);

       LPSTR _itoa (int value, LPSTR str, int radix);
       LPSTR _ltoa (long value, LPSTR str, int radix);
       LPSTR _ultoa(unsigned long int value, LPSTR str, int radix);

       LPSTR _gcvt(double value, int digits, LPSTR buf);

       typedef struct timeb _timeb;
       void _ftime(_timeb *timeptr);

       int   _getpid (void);

       DWORD GetCurrentThreadId(void);

       DWORD GetTickCount(void);

       BOOL  DeleteFile(LPCSTR _path);

    #elif defined(_WIN32)

       LONG  random (void);
       void  srandom(UINT seed);

   #endif

   #ifdef __cplusplus
      }
   #endif


#endif  // __BORLANDC__


#endif  // _COMPAT_STR

