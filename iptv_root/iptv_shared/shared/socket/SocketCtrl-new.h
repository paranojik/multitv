#ifndef __SOCKET_CTRL__
#define __SOCKET_CTRL__


#include "../shared/def_ap.h"
#include "../shared/SVectorT.h"

#include "fd_set.h"


#define MAXPKTSIZE      65535
#define SELECT_BLOCK    -1L
#define TBUF_TXTDATA    65535
#define TTL_DEF         10


struct SockOptions
{
    ULONG   maxsendq;       // Tamanho maximo da fila de bytes a serem enviados ao socket
    BOOL    flUDP;

    long    ctime;          // Se !=0 => Tempo inicial da contagem do numero de bytes enviados
    ULONG64 nBytesSent;     // Numero de bytes enviados a partir de ctime

    long    lastctime;      // Se !=0 => Instante em que os ultimos dados foram enviados
    ULONG   uMaxByteRate,   // Se !=0 => O socket fica limitado a enviar <uMaxByteRate> bytes/sec
            nlastBytesSent; // Numero de bytes enviados a partir de <lastctime>

    SockOptions(void);
    SockOptions& operator= (const SockOptions& obj);
    bool  operator!=  (const SockOptions& obj) const;
    //bool  operator==  (const SockOptions& obj) const;
};


/**
 * Classe SocketCtrl
 * =====================
 *
 * Esta classe encapsula uma conexão socket do Cliente
 * com um servidor. A conexão é estabelecida utilizado-se
 * o método connect. Se for bem sucedido, o mesmo retorna
 * TRUE. Para enviar / receber informações do Servidor,
 * utiliza-se os métodos read/readLine/write/writeLine.
 *
 * Os métodos read/write são utilizados para transmitir
 * dados no formato binário, enquanto readLine/writeLine são
 * usados para transmitir dados no formato texto.
 *
 * Existe um terceiro método de leitura, o readInt, que lê uma
 * linha de texto e a converte em um número inteiro, utilizando
 * a função atoi.
 *
 * Para terminar a conexão, destrói-se o objeto. O destrutor
 * chama o método close, responsável por finalizar a conexão.
 * Este método pode ser chamado diretamente. Após esta chamada
 * pode-se reaproveitar a mesma instância de SocketCtrle
 * para estabelecer uma nova conexão com outro servidor.
 */

class TxtBufVect;
class _SEMAPHORE;

class SocketCtrl
{
    private:
        SOCKET socketMain;

        Myfd_set *pafds;   // Vetor de sockets ativos
        Myfd_set *plfds;   // Vetor de sockets em estado de listen
        Myfd_set *pefds;   // Vetor de sockets com erro
        Myfd_set *pewfds;  // Vetor de sockets com erro de escrita

        fd_set *pafdSel;   // Vetor de sockets ativos (para o comando select())
        fd_set *prfdSel;   // Vetor de sockets com requisicoes pendentes (para o comando select())

        struct sockaddr_in sin_conn; // Estrutura IP:porta associada a conexao

        SVectorT<SOCKET, SockOptions> *pSockOptions; // Ponteiro para vetor de opcoes dos sockets associados

        TxtBufVect  *pTxtBufVect;    // Ponteiro para vetor de buffers de dados
                                     // recebidos de streams (text mode)

		BOOL   flExecutingAccept,    // Indica se a funcao accept() estah sendo executada
               flTextMode,           // Indica se o stream eh modo texto (todas as linhas devem terminar com 0d0a)
               flDefUDP;             // Indica se o protocolo default da classe eh UDP

        ULONG  maxblen,
               maxTxtblen;      // Caso o stream seja modo texto define o 
                                // tamanho maximo do buffer de cada sock

        static long nrefs;

        _SEMAPHORE *pSockOptMutex;  // Mutex para evitar concorrencia em <pSockOptions>
        _SEMAPHORE *pConnectSemaph; // Semaforo para evitar concorrencia entre
                                    // connect() / listen() e accept()


        BOOL  AddActiveSock      (SOCKET fd);
        BOOL  RemoveActiveSock   (SOCKET fd);
        void  ResetActiveSocks   (void);

        BOOL  AddListenSock      (SOCKET fd);
        BOOL  RemoveListenSock   (SOCKET fd);
        BOOL  IsListenSock       (SOCKET fd);

        BOOL  SocketError        (SOCKET ssock, Myfd_set *pfd_set);
        void  ProcessCloseSSocks (void);


    public:
        SocketCtrl(BOOL _flTextMode=FALSE, ULONG _maxTxtblen=TBUF_TXTDATA);
        virtual ~SocketCtrl(void);

        inline Myfd_set * GetActiveFDs   (void)  { return pafds;      }
        inline SOCKET     GetMasterSocket(void)  { return socketMain; }

        BOOL  isSockOk   ( void );
        BOOL  IsConnected( SOCKET clientSock=INVALID_SOCKET );
        BOOL  connect    ( LPCSTR addr, LPCSTR _protocol, int port, LPCSTR _service=NULL, SOCKET *pNewSock=NULL );
        BOOL  listen     ( LPCSTR _protocol, int qlen, int port, LPCSTR _localIP=NULL, LPCSTR _service=NULL, SOCKET *pNewSock=NULL );
        BOOL  accept     ( BOOL (__cdecl *acceptCallback )(SocketCtrl *_psock, SOCKET _ssock, char *buf, unsigned int blen),
                           BOOL (__cdecl *connectCallback)(SocketCtrl *_psock, SOCKET _ssock)=NULL,
                           void (__cdecl *closeCallback  )(SocketCtrl *_psock, SOCKET _ssock, LPCSTR _pCloseMsg)=NULL,
                           long _timeout=0);
        BOOL  acceptUDP  ( BOOL (__cdecl *acceptCallback) (SocketCtrl *_psock, struct sockaddr_in *_psin, char *buf, unsigned int blen),
                           void (__cdecl *closeCallback)  (SocketCtrl *_psock, struct sockaddr_in *_psin, LPCSTR _pCloseMsg)=NULL,
                           long _timeout=0);
        int   getmsg     ( LPSTR buf, unsigned int _blen, long _timeout, SOCKET clientSock=INVALID_SOCKET );
        void  close      ( SOCKET clientSock=INVALID_SOCKET );
        int   read       ( LPSTR buf, unsigned int sz, sockaddr_in *fsin=NULL, SOCKET clientSock=INVALID_SOCKET, BOOL flMustFillBuf=FALSE );
        int   readall    ( LPSTR buf, unsigned int sz, BOOL flMustFillBuf=FALSE, BOOL _flAccept=FALSE, long _timeout=SELECT_BLOCK, SOCKET *_psock=NULL );
        BOOL  readLine   ( LPSTR buf, unsigned int sz, SOCKET clientSock=INVALID_SOCKET );
        long  readInt    ( SOCKET clientSock=INVALID_SOCKET );
        int   write      ( LPCSTR buf, unsigned int sz=0, SOCKET clientSock=INVALID_SOCKET, BOOL flDebug=TRUE, BOOL flMustSendAll=TRUE, int flUDP=UNDEFINED_PARAM );
        ULONG _write     ( LPCSTR buf, unsigned int sz, SOCKET clientSock, BOOL flDebug, BOOL flMustSendAll, BOOL flKillSockOnError, BOOL flKillSockOnNoBuf, int *pErrorCode, BOOL flUDP);
        BOOL  writeLine  ( LPCSTR str, SOCKET clientSock=INVALID_SOCKET, EolFormat format=EOL_DOS );
        int   _writeto   ( LPCSTR buf, unsigned int sz, sockaddr_in *fsin=NULL, SOCKET clientSock=INVALID_SOCKET);
        BOOL  closeSSock ( SOCKET ssock );

        BOOL  InitSockOptVector  (void);
        BOOL  GetSockOptions     (SockOptions *pSockOpt, SOCKET clientSock=INVALID_SOCKET);
        BOOL  UpdateSockOptions  (SockOptions *pSockOpt, SOCKET clientSock=INVALID_SOCKET);
        void  WaitSockOpt        (void);
        void  SignalSockOpt      (void);

        void  SetDefSin          (sockaddr_in *fsin);
        void  SetDefSin          (LPCSTR _ip, int _port);
        void  SetDefUDP          (BOOL _flag) { flDefUDP = _flag; }
        void  set_non_blocking   (SOCKET clientSock=INVALID_SOCKET);

        int   GetRcvBufSize      (SOCKET clientSock=INVALID_SOCKET);
        int   GetSndBufSize      (SOCKET clientSock=INVALID_SOCKET);

        BOOL  SetRcvBufSize      (int BufSize, SOCKET clientSock=INVALID_SOCKET);
        BOOL  SetSndBufSize      (int BufSize, SOCKET clientSock=INVALID_SOCKET);
        BOOL  SetTCPNoDelay      (BOOL _flag=TRUE, SOCKET clientSock=INVALID_SOCKET);
        BOOL  SetMaxSendQueue    (ULONG _sz, BOOL flUDP, SOCKET clientSock=INVALID_SOCKET);
        BOOL  SetBytesCount      (BOOL _flOn, ULONG _uMaxBitrate=0, SOCKET clientSock=INVALID_SOCKET );
        ULONG GetBitRate         (BOOL _flBytes=FALSE, SOCKET clientSock=INVALID_SOCKET);

        BOOL  SetMulticast       (int _ttl=TTL_DEF, LPCSTR _localIP=NULL, SOCKET clientSock=INVALID_SOCKET);
        BOOL  AddMembership      (LPCSTR _MulticastIP, LPCSTR _localIP=NULL, SOCKET clientSock=INVALID_SOCKET);
        BOOL  DropMembership     (LPCSTR _MulticastIP, LPCSTR _localIP=NULL, SOCKET clientSock=INVALID_SOCKET);

        BOOL  isSockSndBufFull   (SOCKET clientSock=INVALID_SOCKET);
        ULONG EnqueueData        (LPCSTR _pdata, ULONG _szdata, SOCKET clientSock=INVALID_SOCKET);

        #if defined(_WIN32)
        UINT  GetMaxMsgSize      (SOCKET clientSock=INVALID_SOCKET);
        #endif
};


#ifdef  __cplusplus
extern "C" {
#endif


BOOL  InitSocketLib       (void);
BOOL  CleanupSocketLib    (void);
int   GetLastSocketError  (void);
BOOL  GetMyHostName       (LPSTR lpszBuffer , UINT nLen);
BOOL  GetClientHost       (SOCKET clientSock, LPSTR pHostStr, UINT HostMaxLen=HOSTLEN);
unsigned long IPStrToLong (LPCSTR _ip);
LPCSTR IPLongToStr        (unsigned long _ip);

ULONG GetClientIP         (SOCKET clientSock, LPSTR pIPStr=NULL, UINT IPMaxlen=IPSTR_MAXLEN);
ULONG GetSinIP            (struct sockaddr_in *_psin, LPSTR pIPStr=NULL, UINT IPMaxLen=IPSTR_MAXLEN);
ULONG GetIP               (SOCKET clientSock, struct sockaddr_in *_psin=NULL, LPSTR pIPStr=NULL, UINT IPMaxLen=IPSTR_MAXLEN);

BOOL  SendQueuedData      (BOOL flDebug=TRUE, SOCKET clientSock=INVALID_SOCKET);

BOOL  StartSendQueueThread(void);
ULONG GetSendQBytesLeft   (SOCKET clientSock);

#ifdef  __cplusplus
}
#endif


#endif

