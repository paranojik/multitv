// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CHAT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CHAT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.


#ifndef __DLL_X_CHAT
#define __DLL_X_CHAT

#ifdef WIN32
	#ifdef CHAT_EXPORTS
	#define CHAT_API __declspec(dllexport)
	#else
	#define CHAT_API __declspec(dllimport)
	#endif

	#include <windows.h>
#endif

#ifdef _LINUX
	#include "shared/compat.h"
	#define CHAT_API
#endif

#include "MessageData.h"
#include "nOpCodes.h"

class cxChat;
class IThread;
class CxSemaphore;
class IMessageDataSink;
//class CHAT_API MessageData;

class CHAT_API cxProxyChat
{
    private:
        cxChat *cx;
    public:
        cxProxyChat( IMessageDataSink* p=NULL );
        virtual ~cxProxyChat( void );

        /* Public Functions */

        LPCSTR GetVersion ( void );

        BOOL IsConnected  ( void );

        //void SetServer     ( LPCSTR _lpszStr );
        void SetClientVersion( LPCSTR _lpszStr );
        void SetNickName     ( LPCSTR _lpszStr );
        void SetRealName     ( LPCSTR _lpszStr );
        void SetEmail        ( LPCSTR _lpszStr );
        void SetHost         ( LPCSTR _lpszStr );
        void SetPortNumber   ( int nNum );

        /* CHAT Functions */
        BOOL GetClientIP( LPSTR _pIPStr, UINT _IPMaxLen );
        BOOL Connect    ( LPCSTR lpszHost, int nPort, BOOL _fluPnP, void *_pproxyparam );
        BOOL ConnectIRC ( LPCSTR _lpszNickname, LPCSTR _lpszEmail, LPCSTR _lpszFullName, LPCSTR _lpszClientVersion, LPCSTR _lpszPassword );
        void Disconnect ( void );
        BOOL InitViewer ( int _rcvPort, LPCSTR _MulticastIP=NULL );
        void CloseViewer( void );
        void CloseUDP   ( void );

        void Terminate  ( void );
        void AppPong    ( void );
        void UDPPing    ( LPCSTR _ip, int _port, LPCSTR _data );

        BOOL SendMessage( UINT nOpcode, long Param, int nSize = 0 );
        //BOOL GetMessage( MessageData *lpMsgData );

        BOOL GetNextMediaQuery( MediaQuery *vq );

        // Provisoriamente
        BOOL Send(  LPSTR lpszStr );
};

#endif /* __DLL_X_CHAT */
