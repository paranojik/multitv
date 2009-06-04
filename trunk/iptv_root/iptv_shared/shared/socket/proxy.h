#ifndef __XPROXY__
#define __XPROXY__


#include "socketctrl.h"
#include "proxyparam.h"


#define S5_NMAXMETHODS            255
#define S5_NMETHODS                 5
#define S5_VERSION               0x05
#define S5_RESERVED              0x00

#define S5_METHOD_NOAUTH         0x00
#define S5_METHOD_GSSAPI         0x01
#define S5_METHOD_USERPASS       0x02
#define S5_METHOD_NONE           0xFF

#define S5_AUTH_USERPASSOK       0x00
#define S5_AUTH_USERPASSVERSION  0x01
#define S5_AUTH_TMAXUSERNAME      255
#define S5_AUTH_TMAXPASSWORD      255

#define S5_CMD_CONNECT           0x01
#define S5_CMD_BIND              0x02
#define S5_CMD_UDPASSOCIATE      0x03
#define S5_CMD_HTTPTUNNEL        0x04

#define S5_ATYP_IPV4             0x01
#define S5_ATYP_DOMAIN           0x03
#define S5_ATYP_IPV6             0x04

#define S5_TMAXHOSTNAME           255

#define S5_REP_SUCCEEDED         0x00
#define S5_REP_SRVFAILURE        0x01
#define S5_REP_CONNNOTALLOWED    0x02
#define S5_REP_NETUNREACHABLE    0x03
#define S5_REP_HOSTUNREACHABLE   0x04
#define S5_REP_CONNREFUSED       0x05
#define S5_REP_TTLEXPIRED        0x06
#define S5_REP_CMDNOTSUPPORTED   0x07
#define S5_REP_INVALIDADDRTYPE   0x08



/*
  o  REP    Reply field:
     o  X'00' succeeded
     o  X'01' general SOCKS server failure
     o  X'02' connection not allowed by ruleset
     o  X'03' Network unreachable
     o  X'04' Host unreachable
     o  X'05' Connection refused
     o  X'06' TTL expired
     o  X'07' Command not supported
     o  X'08' Address type not supported
     o  X'09' to X'FF' unassigned
*/


/*
The values currently defined for METHOD are:

          o  X'00' NO AUTHENTICATION REQUIRED
          o  X'01' GSSAPI
          o  X'02' USERNAME/PASSWORD
          o  X'03' to X'7F' IANA ASSIGNED
          o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
          o  X'FF' NO ACCEPTABLE METHODS
*/


/* Disabled
class XProxy : public SocketCtrl
{
public:

    XProxy(BOOL _flTextMode, ULONG _maxTxtblen);

    BOOL connect(LPCSTR addr, int port, SOCKET *pNewSock=NULL, LPCSTR _localaddr=NULL);


protected:

private:

};
*/

struct s5_methods
{
    BYTE version,
         nmethods;

    BYTE mseq[S5_NMETHODS];
};

struct s5_methodrpl
{
    BYTE version,
         method;
};

struct s5_authuserpassrpl
{
    BYTE version,
         status;
};

struct s5_reqcmdrpl
{
    BYTE version,
         rplcode,
         reserved,
         atype;
};


class Socks5
{
public:

    //Socks5(BOOL _flTextMode=FALSE, ULONG _maxTxtblen=TBUF_TXTDATA);
    Socks5(SocketCtrl *_pSocketCtrl=NULL);
   ~Socks5(void);

    BOOL   IsAuthOk    (void) { return m_flAuth; }

    SOCKET GetTCPSock  (void) { return m_TCPSock; }
    SOCKET GetUDPSock  (void) { return m_UDPSock; }

    BOOL   IsConnected (void);
    BOOL   RemoveSock  (BOOL _flTCP=TRUE);

    BOOL   xconnect    (LPCSTR _addr, int _port);
    BOOL   xconnecthttp(LPCSTR _addr, int _port, LPCSTR _httptsaddr, int _httptsport, LPCSTR _httpproxy=NULL, int _httpproxyport=0);
    BOOL   xbind       (int _remport, LPCSTR _remaddr, USHORT *_pbndport, LPSTR _pbndaddr, UINT _lbndaddr);
    BOOL   xudp        (int _remport, LPCSTR _remaddr, USHORT *_pbndport, LPSTR _pbndaddr, UINT _lbndaddr);
    BOOL   xudpsend    (LPCSTR _dstaddr, USHORT _dstport, LPCSTR _data, UINT _ldata);

    BOOL   connect     (LPCSTR _addr, int _port, LPCSTR _username=NULL, LPCSTR _password=NULL, SOCKET *_pNewSock=NULL, LPCSTR _localaddr=NULL);
    BOOL   connect     (ProxyParam *_pproxyparam, SOCKET *_pNewSock=NULL, LPCSTR _localaddr=NULL);

private:
    
    BOOL   m_flAuth,
           m_flNewSocketCtrl;

    SOCKET m_TCPSock,
           m_UDPSock;

    SocketCtrl *m_pSocketCtrl;

    BOOL auth   (LPCSTR _username=NULL, LPCSTR _password=NULL);
    BOOL cmd    (BYTE _cmd, LPCSTR _dstaddr, int _dstport, LPCSTR _httptsaddr=NULL, int _httptsport=0, LPCSTR _httpproxy=NULL, int _httpproxyport=0);
    BOOL rpl    (USHORT *_pbndport=NULL, LPSTR _pbndaddr=NULL, UINT _lbndaddr=0);
    void putaddr(LPCSTR _host, int _port, LPSTR _buf, UINT &_offset);
};


#endif      // __XPROXY__


