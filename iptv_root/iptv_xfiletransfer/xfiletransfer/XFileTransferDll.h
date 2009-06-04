#ifndef __XFILETRANSFERDLL__
#define __XFILETRANSFERDLL__


// #include <NCTKernel/NCTKernelPublic.h>
typedef const XStr & NCT_IRM_NET_TYPE;

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XFILETRANSFER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XFILETRANSFER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32
#ifdef XFILETRANSFER_EXPORTS
#define X_OBJPROXY_EXPORT
#define XFILETRANSFER_API __declspec(dllexport)
#else
#define XFILETRANSFER_API __declspec(dllimport)
#endif
#else
#define XFILETRANSFER_API
#endif

#include <shared/XObjFactory.h>


class XFILETRANSFER_API XFileTransferProxy : public XProxy
{
public:

    ~XFileTransferProxy ( void );

    bool   Initialize   ( IXFTPNotify *_p );
    void   Build        ( void );
    void   Destroy      ( void );

    void   Reset        ( void );
    bool   BeginReceive ( UINT32 nId, NCT_IRM_NET_TYPE type, LPCSTR szHost, UINT16 nPort, LPCSTR szUser, LPCSTR szChannel, LPCSTR szFilePath, UINT uFileSize, USHORT uPktDataLen, BOOL _flPtP, BOOL _flListen );
    bool   BeginSend    ( UINT32 nId, LPCSTR szHost, UINT16 nPort, LPCSTR szFilePath, ULONG uBitRate, LPCSTR protocol, BOOL _flPtP, BOOL _flListen );

    bool   IsSendEnabled( void );
    bool   IsSending    ( void );
    bool   IsRcvEnabled ( void );
    bool   IsReceiving  ( void );
    ULONG  GetId        ( void );
    USHORT GetSndDataLen( void );
    USHORT GetSndPktSize( void );

    void   SetSndSeq    ( ULONG _seq );
    void   PktRequest   ( ULONG _seqi, ULONG _seqf );
    void   NotifyEOF    ( void );

    void   RunTX        ( void );
    void   StopTX       ( void );
    void   RunRX        ( void );
    void   StopRX       ( void );
};


#endif      // __XFILETRANSFERDLL__

