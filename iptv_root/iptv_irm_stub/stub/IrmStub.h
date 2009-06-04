// IrmStub.h : Declaration of the CIrmStub

#ifndef __IRMSTUB_H_
#define __IRMSTUB_H_

#include "shared/compat.h"
#include "shared/def_ap.h"
#include "shared/expr.h"
#include "shared/socket/proxyparam.h"

#include "chat/nOpCodes.h"
#include "chat/IMessageDataSink.h"
#include "chat/MessageData.h"

#include "shared/irm/irc_defs.h"
#include "shared/ithread.h"

#include "IrmStubDataSink.h"



#define CONNECT_THREAD      9


/////////////////////////////////////////////////////////////////////////////
// CIrmStub
class CIrmStub : public IMessageDataSink
{
    cxProxyChat     *pChat;
    IThread         *ConnectThread;
    IrmStubDataSink *pSink;

    BOOL flConnecting;
    BOOL flDisconnecting;
    BOOL fluPnP;

    char host      [HOSTLEN+1],
         nick      [NICKLEN+1],
         email     [EMAILLEN+1],
         realname  [REALLEN+1],
         cliversion[VERSIONLEN+1],
         password  [PASSWDLEN];

    long port;

    ProxyParam m_proxyparam;

    static int _DoConnect(IThread *pthread, void *_pCtx);

    void OnMessage(MessageData *lpMd);

public:
	CIrmStub (IrmStubDataSink *_pDataSink);
    virtual ~CIrmStub(void);

    LPCSTR GetVersion    (void);
    LPCSTR GetChatVersion(void);
    BOOL   GetClientIP   (LPSTR _pIPStr, UINT _IPMaxLen);

	long   SendMessage(long _cmd, IrmParam& _target, IrmParam& _data, IrmParam& _compl, IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3, IrmParam& _compl4);
	long   Connect    (IrmParam& _host, IrmParam& _nick, IrmParam& _email, IrmParam& _name, IrmParam& _clientversion, IrmParam& _password, long _port, BOOL _flupnp, ProxyParam *_pproxyparam);
    void   Disconnect (void);

    BOOL   InitViewer (int _rcvPort, IrmParam& _MulticastIP);
    void   CloseViewer(void);
    void   CloseUDP   (void);

    void   Terminate  (void);

    void   AppPong    (void);
    void   UDPPing    (LPCSTR _ip, int _port, LPCSTR _data);
};

#endif //__IRMSTUB_H_
