#ifndef __DATE_MNG
#define __DATE_MNG


#define TDATA_DISPLAY 30


#ifdef __cplusplus
  extern "C" {
#endif


LPCSTR GetFormatDate(time_t _ctime, LPSTR _saida=NULL, UINT _size=0, LPCSTR _format=NULL);
LPCSTR GetUptime    (time_t _reftime, LPSTR _saida=NULL, UINT _size=0);
LPCSTR GetSysTime   (LPSTR _saida=NULL, UINT _size=0, LPCSTR _format=NULL);


#ifdef __cplusplus
  }
#endif


#endif

