// XFileTransferDll.cpp : Defines the entry point for the DLL application.
//

#define SOURCE_STRING "XFileTransferDll"
#include <shared/debug.h>


#include "XFileTransfer.h"
#include "XFileTransferDll.h"


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

//////////////////////////////////////////////
// Implementation of XFileTransferProxy
//////////////////////////////////////////////

XFileTransferProxy::~XFileTransferProxy( void )
{
    Destroy();
}

bool XFileTransferProxy::Initialize( IXFTPNotify *_p )
{
    bool ret;

    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->Initialize(_p) )
        ret = true;

    return ret;
}

void XFileTransferProxy::Build( void )
{
    XFileTransfer *pFileTransfer;

    if ( !TestObj() )
    {
        pFileTransfer = new XFileTransfer;

        SetObj( pFileTransfer );
    }
}

void XFileTransferProxy::Destroy( void )
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
    {
        delete pFileTransfer;
        ResetObj();
    }
}

void XFileTransferProxy::Reset(void)
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->Reset();
}

bool XFileTransferProxy::BeginReceive(UINT32 _nId, NCT_IRM_NET_TYPE _type, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szUser, LPCSTR _szChannel, LPCSTR _szFilePath, UINT _uFileSize, USHORT _uPktDataLen, BOOL _flPtP, BOOL _flListen )
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->BeginReceive(_nId, _type, _szHost, _nPort, _szUser, _szChannel, _szFilePath, _uFileSize, _uPktDataLen, _flPtP, _flListen) )
        ret = true;

    return ret;
}

bool XFileTransferProxy::BeginSend(UINT32 _nId, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szFilePath, ULONG _uBitRate, LPCSTR _protocol, BOOL _flPtP, BOOL _flListen )
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->BeginSend( _nId, _szHost, _nPort, _szFilePath, _uBitRate, _protocol, _flPtP, _flListen ) )
        ret = true;

    return ret;
}

bool XFileTransferProxy::IsSendEnabled(void)
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->IsSendEnabled() )
        ret = true;

    return ret;
}

bool XFileTransferProxy::IsSending(void)
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->IsSending() )
        ret = true;

    return ret;
}

bool XFileTransferProxy::IsRcvEnabled(void)
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->IsRcvEnabled() )
        ret = true;

    return ret;
}

bool XFileTransferProxy::IsReceiving(void)
{
    bool ret;
    XFileTransfer *pFileTransfer;

    ret = false;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer && pFileTransfer->IsReceiving() )
        ret = true;

    return ret;
}

ULONG XFileTransferProxy::GetId(void)
{
    ULONG ret;
    XFileTransfer *pFileTransfer;

    ret = 0;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        ret = pFileTransfer->GetId();

    return ret;
}

USHORT XFileTransferProxy::GetSndPktSize(void)
{
    USHORT ret;
    XFileTransfer *pFileTransfer;

    ret = 0;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        ret = pFileTransfer->GetSndPktSize();

    return ret;
}

USHORT XFileTransferProxy::GetSndDataLen( void )
{
    USHORT ret;
    XFileTransfer *pFileTransfer;

    ret = 0;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        ret = pFileTransfer->GetSndDataLen();

    return ret;
}

void XFileTransferProxy::SetSndSeq( ULONG _seq )
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->SetSndSeq( _seq );
}

void XFileTransferProxy::PktRequest( ULONG _seqi, ULONG _seqf )
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->PktRequest( _seqi, _seqf );
}

void XFileTransferProxy::NotifyEOF( void )
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->NotifyEOF();
}

void XFileTransferProxy::RunTX(void)
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->RunTX();
}

void XFileTransferProxy::StopTX(void)
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->StopTX();
}

void XFileTransferProxy::RunRX(void)
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->RunRX();
}

void XFileTransferProxy::StopRX(void)
{
    XFileTransfer *pFileTransfer;

    pFileTransfer = (XFileTransfer *) GetObj();

    if ( pFileTransfer )
        pFileTransfer->StopRX();
}

