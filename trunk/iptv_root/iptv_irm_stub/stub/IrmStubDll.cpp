// IrmStub30.cpp : Defines the entry point for the DLL application.
//

#include "shared/compat.h"
#include "shared/socket/proxyparam.h"

#include "IrmStubDll.h"
#include "IrmStub.h"

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

#ifdef _LINUX
    IrmParam emptyIrmParam; // empty IrmParam to use as default argument
    IrmParam &CreateEmptyIrmParam() { return(emptyIrmParam); }
#endif

/*
// This is an example of an exported variable
IRMSTUB30_API int nIrmStub30=0;

// This is an example of an exported function.
IRMSTUB30_API int fnIrmStub30(void)
{
	return 42;
}

*/


// This is the constructor of a class that has been exported.
// see IrmStubDll.h for the class definition

ProxyIrmStub::ProxyIrmStub(void)
{
    pIrmStub = NULL;
}


ProxyIrmStub::~ProxyIrmStub(void)
{
    if (pIrmStub)
        delete pIrmStub;
}


LPCSTR ProxyIrmStub::GetVersion(void)
{
    LPCSTR pRet;

    if (pIrmStub)
        pRet = pIrmStub->GetVersion();
    else
        pRet = "";

    return pRet;
}


LPCSTR ProxyIrmStub::GetChatVersion(void)
{
    LPCSTR pRet;

    if (pIrmStub)
        pRet = pIrmStub->GetChatVersion();
    else
        pRet = "";

    return pRet;
}


BOOL ProxyIrmStub::Initialize (IrmStubDataSink* _p)
{
    BOOL ret;

    ret = FALSE;

    if (_p && !pIrmStub)
    {
        pIrmStub = new CIrmStub(_p);

        if (pIrmStub)
            ret = TRUE;
    }

    return ret;
}


long ProxyIrmStub::SendIrmMessage(long _cmd, IrmParam& _target, IrmParam& _data, IrmParam& _compl, IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3, IrmParam& _compl4)
{
    long ret;

    if (pIrmStub)
        ret = pIrmStub->SendMessage(_cmd, _target, _data, _compl, _compl1, _compl2, _compl3, _compl4);
    else
        ret = 0;

    return ret;
}

long ProxyIrmStub::Connect(IrmParam& _host, IrmParam& _port, IrmParam& _nick, IrmParam& _email, IrmParam& _name, IrmParam& _clientversion, IrmParam& _password, IrmParam& _flupnp, void *_pproxyparam)
{
    long ret;

    if (pIrmStub)
        ret = pIrmStub->Connect(_host, _nick, _email, _name, _clientversion, _password, _port.GetValue(), _flupnp.GetValue(), (ProxyParam *) _pproxyparam);
    else
        ret = 0;

    return ret;
}


void ProxyIrmStub::Disconnect(void)
{
    if (pIrmStub)
        pIrmStub->Disconnect();
}

BOOL ProxyIrmStub::InitViewer(int _rcvPort, IrmParam& _MulticastIP)
{
    BOOL ret;

    ret = FALSE;

    if (pIrmStub)
        ret = pIrmStub->InitViewer(_rcvPort, _MulticastIP);

    return ret;
}

void ProxyIrmStub::CloseUDP(void)
{
    if (pIrmStub)
        pIrmStub->CloseUDP();
}

void ProxyIrmStub::CloseViewer(void)
{
    if (pIrmStub)
        pIrmStub->CloseViewer();
}

void ProxyIrmStub::Terminate(void)
{
    if (pIrmStub)
        pIrmStub->Terminate();
}

void ProxyIrmStub::AppPong(void)
{
    if (pIrmStub)
        pIrmStub->AppPong();
}

void ProxyIrmStub::UDPPing(LPCSTR _ip, int _port, LPCSTR _data)
{
    if (pIrmStub)
        pIrmStub->UDPPing(_ip, _port, _data);
}

BOOL ProxyIrmStub::GetClientIP(LPSTR _pIPStr, UINT _IPMaxLen)
{
    BOOL ret;

    ret = FALSE;

    if (pIrmStub)
        ret = pIrmStub->GetClientIP(_pIPStr, _IPMaxLen);

    return ret;
}


