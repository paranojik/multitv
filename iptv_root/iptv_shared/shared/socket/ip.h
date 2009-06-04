#ifndef __XIPADDR
#define __XIPADDR


#include "shared/def_ap.h"


class XIpAddr
{
public:

    XIpAddr(LPCSTR _host=NULL, ULONG _port=0);
    XIpAddr(XIpAddr &_ipaddr);

    void   SetPort  (ULONG _port) { m_port = _port;  }
    ULONG  GetPort  (void)        { return m_port;   }
    LPCSTR GetHost  (void)        { return m_host;   }
    ULONG  GetIpLong(void)        { return m_iplong; }

    BOOL   IsOk     (void);
    void   reset    (void);
    BOOL   SetIp    (ULONG _ip);
    BOOL   SetIp    (LPCSTR _ip);
    void   SetHost  (LPCSTR _host);
    void   SetHost  (LPCSTR _host, UINT _thoststr);
    BOOL   SetAddr  (LPCSTR _ip, ULONG _port);
    BOOL   SetAddr  (XIpAddr *_pipaddr);

private:

    ULONG m_iplong,
          m_port;

    char  m_host[HOSTLEN+1];
};



#endif  //__XIPADDR


