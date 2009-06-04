#ifndef __UPNP
#define __UPNP


#ifdef __cplusplus
  extern "C" {
#endif


void ResetuPnP     (LPCSTR _protocol, UINT _port);
BOOL AdduPnPPort   (LPCSTR _protocol, LPCSTR _localip, UINT _port, LPCSTR _desc);
BOOL GetuPnPLocalIP(LPCSTR _protocol, UINT _port, LPSTR _localip, UINT _bufsize);


#ifdef __cplusplus
  }
#endif


#endif   // __UPNP



