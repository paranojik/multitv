#ifndef __CX_CHAT__
#define __CX_CHAT__

#include "shared/socket/socketctrl.h"
#include "shared/socket/proxyparam.h"
#include "shared/ithread.h"
#include "shared/irm/srv_defs.h"
#include "shared/irm/irc_defs.h"
#include "shared/hash_cmd.h"
#include "shared/ListT.h"
#include "shared/semaphore.h"
#include "MessageData.h"
#include "ChannelUserList.h"
#include "IMessageDataSink.h"



class MediaQueryList;

class cxChat
{
    friend class cxProxyChat;

    private:

        static BOOL _DoChatMessage( IThread *pThread, void *lpctx );
        static int  _DoUDP        ( IThread *pThread, void *lpctx );
        static int  _DoAppPing    ( IThread *pTread , void *lpctx );

        IMessageDataSink* pSink;

        SocketCtrl *pSock,
                   *pSockUDP,
                   *pSockUDPSnd;

        IThread    *CmdControlThread;
        IThread    *ChatThread;
        IThread    *UDPThread;
        IThread    *AppPingThread;

        _SEMAPHORE *pSemaph;
        _SEMAPHORE *pUDPSemaph;
        _SEMAPHORE *pMsgQSemaph;

        char lpszNickName    [NICKLEN+1];
        char lpszCliVersion  [VERSIONLEN+1];
        char lpszRealName    [REALLEN+1];
        char lpszEmail       [EMAILLEN+1];
        char lpszUser        [USERLEN+1];
        char lpszHost        [HOSTLEN+1];
        char lpszPassword    [PASSWDLEN+1];
        char lpszMCastChannel[CHANNELLEN+1];

        int  nPort;
        BOOL flConnected,
             fluPnP,
             flUDPListening;

        BOOL flAppPing,
             flSrvPing;

        time_t LastAppPingTime,
               LastSrvPingTime,
               LastCmdTime,
               LastCmdSentTime;


        Hash_cmd                *pHashCmd,
                                *pHashServicesCmd;
        ListT<MessageData>      *pMessageList;
        ListT<ChannelUserList>  *pChannelUserList;

        // TODO:
        // TODO:
        // TODO:
        // ConnectIRC. desalocar estruturas de Messagens, queries, etc
        // ConnectIRC. desalocar estruturas de Messagens, queries, etc
        // ConnectIRC. desalocar estruturas de Messagens, queries, etc
        // Listas de controle do MediaQuery e MediaRcvList com ID
        ListT<MediaQueryList>   *pMediaQueryList;

        /* Private Functions */
        BOOL AddMessage   ( UINT nCode, LPCSTR lpszSource, LPCSTR lpszTarget, long *lParam, LPCSTR lpszFullCmd);
        BOOL DeleteMessage( MessageData *lpMd );
        //BOOL CopyMessage  ( MessageData *lpMsgData, MessageData *lpMd );

        BOOL CreateChannelUserList( LPSTR lpszChannel, LPSTR lpszUserList );
        BOOL DeleteChannelUserList( LPSTR lpszChannel );
        BOOL GetChannelUserList( LPSTR lpszChannel, LPSTR *lpszUserList );

        BOOL AddMediaQuery ( MediaQuery *vq );

        BOOL Wait          (void)   { return pSemaph->Wait( ); }
        void Signal        (void)   { pSemaph->Signal( );      }
        BOOL IsUDPListening(void)   { return flUDPListening;   }

        BOOL InitUDPThread (void);
        void KillThreads   (void);
        BOOL InitMulticastChannel(LPCSTR _channel, LPCSTR _MulticastIP, int _port);
        void UpdateLastCmdTime    (void);
        void UpdateLastCmdSentTime(void);

        BOOL ListenUDP     (int _rcvPort, LPCSTR _MulticastIP=NULL);
        void CloseUDP      (void);

        void UDPPing       (LPCSTR _ip=NULL, int _port=0, LPCSTR _data=NULL);

        BOOL InituPnP      (void);
        void RemoveuPnP    (void);

    public:
	    cxChat( IMessageDataSink* p = NULL );
        virtual ~cxChat( void );

        friend BOOL RecvCallback  (SocketCtrl *pSock, SOCKET ssock, char *buf, unsigned int blen);
        friend void CloseCallback (SocketCtrl *_psock, SOCKET _ssock, LPCSTR _pCloseMsg);
        friend BOOL RcvUDPCallback(SocketCtrl *_psock, struct sockaddr_in *_psin, char *buf, unsigned int blen);

        /* Public Functions */
        BOOL   IsConnected (void)       { return flConnected;  }
        void   SetConnected(BOOL _flag) { flConnected = _flag; }
        LPCSTR GetVersion  (void);

        LPCSTR GetMCastChannel(void)  { return lpszMCastChannel; }
        LPCSTR GetNickName    (void)  { return lpszNickName;     }

        void SetClientVersion ( LPCSTR _lpszStr );
        void SetUserName      ( LPCSTR _lpszStr );
        void SetNickName      ( LPCSTR _lpszStr );
        void SetRealName      ( LPCSTR _lpszStr );
        void SetEmail         ( LPCSTR _lpszStr );
        void SetHost          ( LPCSTR _lpszStr );
        void SetPassword      ( LPCSTR _lpszStr );
        void SetPortNumber    ( int nNum );

        /* CHAT Functions */
        BOOL GetClientIP( LPSTR _pIPStr, UINT _IPMaxLen );
        BOOL Connect    ( LPCSTR lpszHost, int nPort, BOOL _fluPnP, ProxyParam *_pproxyparam );
        BOOL ConnectIRC ( LPCSTR _lpszNickname, LPCSTR _lpszEmail, LPCSTR _lpszFullName, LPCSTR _lpszClientVersion, LPCSTR _lpszPassword );
        void Disconnect ( void );
        void Terminate  ( void );
        void AppPong    ( void );

        BOOL InitViewer (int _rcvPort, LPCSTR _MulticastIP=NULL);
        void CloseViewer(void);

        BOOL SendMessage( UINT nOpcode, long Param, int nSize = 0 );
        //BOOL GetMessage( MessageData *lpMsgData );
        MessageData * GetMessage( void );

        BOOL GetNextMediaQuery( MediaQuery *vq );

        BOOL Send(  LPSTR lpszStr );
        BOOL Sendf( char *lpszBuffer, ... );
};

class MediaQueryList
{
    private:
        ListT<MediaQuery> *pList;

    public:
        MediaQueryList( void );
        ~MediaQueryList( void );
        void AddMediaQuery(     MediaQuery *vq );
        void DelMediaQuery(     MediaQuery *vq );
        BOOL GetNextMediaQuery( MediaQuery *vq );

};

#endif /* __CX_CHAT__ */
