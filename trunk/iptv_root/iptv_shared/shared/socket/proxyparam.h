#ifndef _PROXYPARAM
#define _PROXYPARAM


#define HTTP_PORT              80
#define SOCKS5_PORT          1080
#define S5XS_PORT           31080

#define PROXYTYPE_NULL          0
#define PROXYTYPE_HTTP          1
#define PROXYTYPE_HTTPPROXY     2
#define PROXYTYPE_SOCKS5        3


class ProxyParam
{
public:
    ProxyParam    (void);
    ProxyParam    (ProxyParam& _p);
    ProxyParam    (LPCSTR _host, DWORD _port, LPCSTR _user, LPCSTR _password, DWORD _proxytype);

    LPCSTR GetHost     (void) { return m_szHost;      }
    LPCSTR GetUser     (void) { return m_szUser;      }
    LPCSTR GetPassword (void) { return m_szPasswd;    }
    DWORD  GetPort     (void) { return m_dwPort;      }
    DWORD  GetProxyType(void) { return m_dwProxyType; }

    BOOL   IsHTTP      (void) { return (m_dwProxyType==PROXYTYPE_HTTP      ? TRUE : FALSE); }
    BOOL   IsHTTPProxy (void) { return (m_dwProxyType==PROXYTYPE_HTTPPROXY ? TRUE : FALSE); }
    BOOL   IsSocks5    (void) { return (m_dwProxyType==PROXYTYPE_SOCKS5    ? TRUE : FALSE); }

    void Copy    (ProxyParam& _p);

    BOOL IsRdy   (void);
    BOOL CanAuth (void);
    void reset   (void);

    void SetHost     (LPCSTR _host);
    void SetUser     (LPCSTR _user);
    void SetPasswd   (LPCSTR _password);
    void SetPort     (DWORD _port);
    BOOL SetProxyType(DWORD _proxytype);

private:
    char  m_szHost  [HOSTLEN+1],
          m_szUser  [NICKLEN+1],
          m_szPasswd[PASSWDLEN+1];

    DWORD m_dwPort,
          m_dwProxyType;
};


#endif  //_PROXYPARAM


