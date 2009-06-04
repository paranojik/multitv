
/*
  DsTypes.h

  Declares basic types

  $Id: dstypes.h 472 2006-10-31 21:39:05Z dario $

  19/02/2003
*/

#ifndef _DS_TYPES_H
#define _DS_TYPES_H

#define DsNULL					0

typedef void					*DsPVOID;

typedef signed short int		DsINT16;
typedef unsigned short int		DsUINT16;
typedef signed long int			DsINT32;
typedef unsigned long int		DsUINT32;

#if defined ( WIN32 )
typedef unsigned __int64		DsUINT64;
typedef __int64					DsINT64;
#elif defined( _LINUX )
typedef unsigned long long int 		DsUINT64;
typedef long long int			DsINT64;
#endif /* _LINUX */

typedef char					DsINT8, DsCHAR;
typedef unsigned char			DsUINT8, DsBYTE;

typedef wchar_t				    DsUNICHAR;

typedef DsUINT32                DsBOOL;

typedef DsCHAR					*DsPSTR;
typedef const DsCHAR			*DsPCSTR;
typedef DsUNICHAR				*DsPUNISTR;
typedef const DsUNICHAR			*DsPUNICSTR;
typedef DsBYTE                  *DsPBYTE;
typedef const DsBYTE            *DsPCBYTE;

typedef float                   DsFLOAT;
typedef double                  DsDOUBLE;

#define DsFALSE					0
#define DsTRUE					1

#define DSLIB_MIN( a, b ) ( (a) > (b) ? (b) : (a) )

// string
#define DSLIB_STRING_MAX_STRING                 16384 // 16k chars max
#define DSLIB_STRING_MAX_STATIC_STRING           8192

// IO
#define DSLIB_IO_MAX_VERSION_BUFFER               512
#define DS_MAX_PATH                               260

#define DSLIB_FILE_TEMP_PREFIX                  "_DS"

#endif // _DS_TYPES_H
