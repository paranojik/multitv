#ifndef _FILE_CT
#define _FILE_CT


#include "file_x.h"


#ifdef __cplusplus
extern "C" {
#endif

HFILE open_ap  (LPCSTR path, OFSTRUCT FAR *ofstr, UINT flags, UINT mode=0);
int   close_ap (HFILE hfile, OFSTRUCT FAR *ofstr=NULL);
UINT  read_ap  (HFILE hfile, void FAR *buf, UINT size, OFSTRUCT FAR *ofstr=NULL);
UINT  write_ap (HFILE hfile, void FAR *buf, UINT size, OFSTRUCT FAR *ofstr=NULL);
LONG  seek_ap  (HFILE hfile, LONG offset, int nOrigin, OFSTRUCT FAR *ofstr=NULL);
LONG  tell_ap  (HFILE hfile, OFSTRUCT FAR *ofstr=NULL);


#if defined(_WIN32)

BOOL  lock_ap  (HFILE hFile, DWORD dwFileOffsetLow, DWORD dwFileOffsetHigh,
                DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh, OFSTRUCT FAR *ofstr);
long  hread_ap (HFILE hfile, void _huge *buf, long size, OFSTRUCT FAR *ofstr);
long  hwrite_ap(HFILE hfile, void _huge *buf, long size, OFSTRUCT FAR *ofstr);

#endif  // _WIN32


/* Rotinas contidas em file_ap.c */

//BOOL DeleteFile     (LPCSTR path);

//HGLOBAL GetFileImage    (LPCSTR lpszFileName, long FAR *lpSize, long FAR *lpNumCr);


#ifdef __cplusplus
}
#endif


#endif      //_FILE_CT
