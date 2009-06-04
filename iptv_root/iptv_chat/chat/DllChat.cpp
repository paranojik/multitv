#include "shared/compat.h"
#include "shared/socket/proxyparam.h"

#include "chat.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
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
///////////////////////////////////////////////////////////////////////////////
cxProxyChat::cxProxyChat( IMessageDataSink* p )
{
    cx = new cxChat(p);
}

cxProxyChat::~cxProxyChat( void )
{
    if( cx )
        delete cx;
}

LPCSTR cxProxyChat::GetVersion (void)
{
    return cx->GetVersion();
}

BOOL cxProxyChat::IsConnected(void)
{
    return cx->IsConnected();
}

void cxProxyChat::SetClientVersion( LPCSTR _lpszStr )
{
    cx->SetClientVersion( _lpszStr );
}

void cxProxyChat::SetNickName( LPCSTR _lpszStr )
{
    cx->SetNickName( _lpszStr );
}

void cxProxyChat::SetRealName( LPCSTR _lpszStr )
{
    cx->SetRealName( _lpszStr );
}

void cxProxyChat::SetEmail( LPCSTR _lpszStr )
{
    cx->SetEmail( _lpszStr );
}

void cxProxyChat::SetHost( LPCSTR _lpszStr )
{
    cx->SetHost( _lpszStr );
}

void cxProxyChat::SetPortNumber( int nNum )
{
    cx->SetPortNumber( nNum );
}

BOOL cxProxyChat::Connect( LPCSTR _lpszHost, int nPort, BOOL _fluPnP, void *_pproxyparam )
{
    return cx->Connect( _lpszHost, nPort, _fluPnP, (ProxyParam *) _pproxyparam );
}

BOOL cxProxyChat::ConnectIRC( LPCSTR _lpszNickname, LPCSTR _lpszEmail, LPCSTR _lpszFullName, LPCSTR _lpszClientVersion, LPCSTR _lpszPassword )
{
    return cx->ConnectIRC( _lpszNickname, _lpszEmail, _lpszFullName, _lpszClientVersion, _lpszPassword );
}

void cxProxyChat::Disconnect( void )
{
    cx->Disconnect();
}

BOOL cxProxyChat::InitViewer( int _rcvPort, LPCSTR _MulticastIP )
{
    return cx->InitViewer(_rcvPort, _MulticastIP);
}

void cxProxyChat::CloseViewer( void )
{
    cx->CloseViewer();
}

void cxProxyChat::CloseUDP( void )
{
    cx->CloseUDP();
}

void cxProxyChat::Terminate( void )
{
    cx->Terminate();
}

void cxProxyChat::AppPong( void )
{
    cx->AppPong();
}

void cxProxyChat::UDPPing( LPCSTR _ip, int _port, LPCSTR _data)
{
    cx->UDPPing( _ip, _port, _data);
}

BOOL cxProxyChat::SendMessage( UINT nOpcode, long Param, int nSize )
{
    return cx->SendMessage( nOpcode, Param, nSize );
}

//BOOL cxProxyChat::GetMessage( MessageData *lpMsgData )
//{
//    return cx->GetMessage( lpMsgData );
//}

BOOL cxProxyChat::Send(  LPSTR lpszStr )
{
    return cx->Send( lpszStr );
}

BOOL cxProxyChat::GetNextMediaQuery( MediaQuery *vq )
{
    return cx->GetNextMediaQuery( vq );
}

BOOL cxProxyChat::GetClientIP( LPSTR _pIPStr, UINT _IPMaxLen )
{
    return cx->GetClientIP( _pIPStr, _IPMaxLen );
}


