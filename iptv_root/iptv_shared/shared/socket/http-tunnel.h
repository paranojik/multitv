#ifndef _HTTPTUNNEL
#define _HTTPTUNNEL

#include <time.h>

#include "socketctrl.h"
#include "proxyparam.h"

#define HTTPOP_NULL                 0x00
#define HTTPOP_GET                  0x01
#define HTTPOP_POST                 0x02


class HTTPTunnel
{
public:

    HTTPTunnel(UINT _op=HTTPOP_POST|HTTPOP_GET, SocketCtrl *_pSocketCtrl=NULL, SOCKET _ssock=INVALID_SOCKET);
   ~HTTPTunnel(void);

    BOOL   IsOk  (void) { return m_flOk; }

    BOOL   DoGet (void) { return ((m_op & HTTPOP_GET ) == HTTPOP_GET  ? TRUE : FALSE); }
    BOOL   DoPost(void) { return ((m_op & HTTPOP_POST) == HTTPOP_POST ? TRUE : FALSE); }

    BOOL   connect           (LPCSTR _addr, LPCSTR _localaddr, LPCSTR _srvaddr, DWORD _srvport, DWORD _srvid, ProxyParam *_httpproxy, BOOL _flblock=TRUE);
    BOOL   SendHTTPInitCmd   (LPCSTR _addr, LPCSTR _srvaddr, DWORD _srvport, DWORD _srvid, LPCSTR _user, LPCSTR _passwd, BOOL _flproxy);
    BOOL   WaitForServiceLine(LPSTR _pservice);

    SOCKET GetHTTPSock       (void)   { return m_ssock; }
    BOOL   RemoveSock        (void);

private:

    BOOL SendHTTPCmd(LPCSTR _addr, LPCSTR _srvaddr, DWORD _srvport, DWORD _srvid, LPCSTR _user, LPCSTR _passwd, BOOL _flproxy, LPCSTR *_cmdvet);

    BOOL m_flOk,
         m_flNewSocketCtrl;

    SocketCtrl *m_pSocketCtrl;
    SOCKET m_ssock;

    UINT   m_op;

    time_t m_ctime;
};



#endif  // _HTTPTUNNEL

