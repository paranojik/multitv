#ifndef __DLL_X_IRMSTUB
#define __DLL_X_IRMSTUB


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IRMSTUB30_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IRMSTUB30_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef _WIN32
    #ifdef IRMSTUB30_EXPORTS
        #define IRMSTUB_API __declspec(dllexport)
    #else
        #define IRMSTUB_API __declspec(dllimport)
    #endif
#endif

#ifdef _LINUX
    #define IRMSTUB_API
#endif

/*
extern IRMSTUB_API int nIrmStub30;

IRMSTUB_API int fnIrmStub30(void);
*/

#include "IrmStubDataSink.h"

class CIrmStub;

#ifdef _LINUX
    IrmParam &CreateEmptyIrmParam();// empty IrmParam to use as default argument
#endif

class IRMSTUB_API ProxyIrmStub
{
private:
    CIrmStub *pIrmStub;

public:
    ProxyIrmStub (void);
    virtual ~ProxyIrmStub(void);

    LPCSTR GetVersion    (void);
    LPCSTR GetChatVersion(void);
    BOOL   GetClientIP   (LPSTR _pIPStr, UINT _IPMaxLen);

    BOOL   Initialize (IrmStubDataSink* _p);
#ifdef _WIN32
	long   SendIrmMessage( long _cmd, IrmParam& _target = IrmParam(), IrmParam& _data = IrmParam(), IrmParam& _compl = IrmParam(), IrmParam& _compl1 = IrmParam(), IrmParam& _compl2 = IrmParam(), IrmParam& _compl3 = IrmParam(), IrmParam& _compl4 = IrmParam() );
    long   Connect       ( IrmParam& _host, IrmParam& _port, IrmParam& _nick, IrmParam& _email = IrmParam(), IrmParam& _name = IrmParam(), IrmParam& _clientversion = IrmParam(), IrmParam& _password = IrmParam(), IrmParam& _flupnp = IrmParam(), void *_pproxyparam = NULL );
#endif
#ifdef _LINUX
    long   SendIrmMessage(long _cmd, IrmParam& _target = CreateEmptyIrmParam(), IrmParam& _data = CreateEmptyIrmParam(), IrmParam& _compl = CreateEmptyIrmParam(), IrmParam& _compl1 = CreateEmptyIrmParam(), IrmParam& _compl2 = CreateEmptyIrmParam(), IrmParam& _compl3 = CreateEmptyIrmParam(), IrmParam& _compl4 = CreateEmptyIrmParam());
    long   Connect    (IrmParam& _host, IrmParam& _port, IrmParam& _nick, IrmParam& _email = CreateEmptyIrmParam(), IrmParam& _name = CreateEmptyIrmParam(), IrmParam& _clientversion = CreateEmptyIrmParam(), IrmParam& _password = CreateEmptyIrmParam(), IrmParam& _flupnp = CreateEmptyIrmParam(), void *_pproxyparam = NULL );
#endif

    void   Disconnect (void);

    BOOL   InitViewer (int _rcvPort, IrmParam& _MulticastIP);
    void   CloseViewer(void);
    void   CloseUDP   (void);

    void   Terminate  (void);

    void   AppPong    (void);
    void   UDPPing    (LPCSTR _ip, int _port, LPCSTR _data);
};



#endif  // __DLL_X_IRMSTUB



