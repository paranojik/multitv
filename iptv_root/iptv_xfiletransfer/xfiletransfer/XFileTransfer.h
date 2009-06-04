#ifndef __XFILETRANSFER__
#define __XFILETRANSFER__


#include "shared/VectorT.h"
#include "shared/semaphore.h"
#include "shared/ithread.h"

#include "shared/irm/cmd.h"

#include "IXFileTransferNotify.h"


#define FTPSNDPKTDATALEN       1354      // Tamanho em bytes da area de dados do pacote para que o tamanho total seja multiplo de 49 (!!)
//#define FTPSNDPKTDATALEN       354      // Tamanho em bytes da area de dados do pacote para que o tamanho total seja multiplo de 49 (!!)

//#define FTPSNDPKTDATALEN      1356      // Tamanho em bytes da area de dados do pacote para que o tamanho total seja multiplo de 49 (!!)
//#define FTPSNDPKTDATALEN      4051      // Tamanho em bytes da area de dados do pacote para que o tamanho total seja multiplo de 49 (!!)


/*
struct MediaPkt
{
    ULONG  id,
           seq;
    USHORT datalen;
    BYTE   type,
           flags;
};
*/

#pragma pack(1)

struct FTPHeader
{
    MediaPkt m;
    ULONG chksum;
};

#pragma pack()



class SocketCtrl;

class XFileTransfer
{
public:
    XFileTransfer(void);
   ~XFileTransfer(void);

    BOOL   Initialize   ( IXFTPNotify *_p );
    void   Reset        ( void );

    BOOL   IsSendEnabled( void ) { return m_bSendEnabled; }
    BOOL   IsRcvEnabled ( void ) { return m_bRcvEnabled;  }
    BOOL   IsSending    ( void ) { return m_bSending;     }
    BOOL   IsReceiving  ( void ) { return m_bReceiving;   }
    ULONG  GetId        ( void ) { return m_uId;          }
    USHORT GetSndDataLen( void ) { return FTPSNDPKTDATALEN; }
    USHORT GetSndPktSize( void ) { return (sizeof(FTPHeader) + FTPSNDPKTDATALEN); }
    //USHORT GetPktSize   ( void ) { return m_uPktSize;     }

    BOOL   BeginReceive ( UINT32 _nId, LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szUser, LPCSTR _szChannel, LPCSTR _szFilePath, UINT _uFileSize, USHORT _uPktDataLen, BOOL _flPtP=FALSE, BOOL _flListen=FALSE );
    BOOL   BeginSend    ( UINT32 _nId, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szFilePath, ULONG _uBitRate, LPCSTR _protocol, BOOL _flPtP=FALSE, BOOL _flListen=FALSE );

    void   SetSndSeq    ( ULONG _seq );
    void   PktRequest   ( ULONG _seqi, ULONG _seqf );
    void   NotifyEOF    ( void );

    void   SndPause     ( void );
    void   SndResume    ( void );

    BOOL   RunTX        ( void );
    BOOL   StopTX       ( void );
    BOOL   RunRX        ( void );
    BOOL   StopRX       ( void );


protected:

    int TransferThreadFunc(void);

    static int _DoPlayThread(IThread* _pthread, void* _pctx)
    {
        int ret;
        
        ret = ((XFileTransfer *) _pctx)->TransferThreadFunc();

        //_pthread->Exit(ret);

        return ret;
    }


private:

    BOOL  VerifyProtocol      ( LPCSTR _protocol );
    BOOL  InitializeRcvNetwork( ULONG _nId, LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, LPCSTR _szUser, LPCSTR _szChannel, BOOL _flListen );
    BOOL  InitializeSndNetwork( LPCSTR _protocol, LPCSTR _szHost, UINT16 _nPort, BOOL _flListen );
    BOOL  ConnectTCP          ( SocketCtrl *_pSock, ULONG _nId, BOOL _flSend );
    BOOL  ListenTCP           ( SocketCtrl *_pSock );

    void  SendEvent           ( XFTP_CODE _code, DWORD _dwData1, DWORD _dwData2 );
    BOOL  SendPkt             ( void );
    BOOL  ReceivePkt          ( void );
    BOOL  SaveRcvPkt          ( LPCSTR _buf, UINT _blen );

    BOOL  StartTransferThread ( void );
    void  EndTransferThread   ( void );

    void  FlushTX             ( void );
    void  FlushRX             ( BOOL _flResetBufRdy );

    void  ResetSnd            ( void );
    void  ResetRcv            ( void );


    IXFTPNotify    *m_pFTPNotify;        // Ponteiro para o objeto que receberah notificacoes

    SocketCtrl     *m_pSockSnd,          // Socket para envio de pacotes
                   *m_pSockRcv;          // Socket para recebimento de pacotes

    HFILE           m_hFile;             // Handle do arquivo recebido ou enviado

    USHORT          m_uPktSize,          // Tamanho do pacote a ser enviado/recebido (header + dados)
                    m_uPktDataLen;       // Tamanho da area de dados dos pacotes (enviados/recebidos)

    ULONG           m_uFileSize,         // Tamanho do arquivo enviado ou recebido
                    m_uCurSeq,           // Sequencial do ultimo pacote enviado / recebido
                    m_uTotPkts,          // Numero de pacotes que irao compor o arquivo a ser enviado
                    m_nRcvPkts,          // Numero de pacotes consistentes recebidos
                    m_nSndTotPktReq,     // Numero total de pacotes requisitados por PKTREQUEST
                    m_nSndPktReq;        // Numero de pacotes requisitados por PKTREQUEST que ja foram enviados

    VectorT<ULONG>  m_vPktLost;          // Vetor contendo sequenciais dos pacotes perdidos

    _SEMAPHORE      m_Semaph;
    _SEMAPHORE      m_SemaphSndPause;    // Para forcar pausa na transmissao
    IThread        *m_pTransferThread;   // Thread para controlar o envio / recebimento de pacotes

    BOOL            m_bSendEnabled;      // Indica que BeginSend foi executado com sucesso
    BOOL            m_bRcvEnabled;       // Indica que BeginReceive foi executado com sucesso
    BOOL            m_bSending;          // Indica que StartTX foi executado com sucesso => estao sendo enviados dados ao server
    BOOL            m_bReceiving;        // Indica que StartRX foi executado com sucesso => os dados recebidos do server estao sendo tocados
    BOOL            m_bSndUDP;           // Indica se os dados estao sendo enviados por UDP ou TCP
    BOOL            m_bRcvUDP;           // Indica se os dados estao sendo recebidos por UDP ou TCP
    BOOL            m_bResetting;        // Para evitar reentranca no Reset()
    BOOL            m_bSndPaused;        // Indica se o semaforo <m_SemaphSndPause> estah bloqueado
    BOOL            m_flNotifyEOF;       // Indica que estah executando NotifyEOF()
    BOOL            m_flAbort;           // Indica que processo de abort em RX foi iniciado
    BOOL            m_flEOF;             // Indica que o fim do arquivo foi alcancado
    BOOL            m_flRcvComplete;     // Indica que o arquivo foi recebido com sucesso
    BOOL            m_flPtP;             // Indica se o modo de transmissao / recepcao eh PTP
    BOOL            m_flListen;          // Indica se o client deverah conectar-se ao host remoto ou aguardar por conexao

    ULONG           m_uId,               // Id do stream de que estah sendo enviado ou recebido
                    m_tSndWait,          // Tempo de espera (em msecs) entre o envio de 2 pacotes (para garantir o bitrate)
                    m_uMaxRcvPktLost;    // Se o numero de pacotes perdidos ultrapassar este valor => aborta recebimento
    time_t          m_tFirstPktTime,     // Instante em que o primeiro pacote deverah ser enviado para garantir o seu recebimento por todos os usuarios
                    m_tSndPause;         // Tempo de pausa no envio de pacotes
    DWORD           m_tSndEnd;           // Instante previsto para encerramento do processo de send
                                         // caso nao hajam mais pacotes a serem enviados

    XStrDyn         m_strFilename;       // Nome do arquivo enviado / recebido
    XStrDyn         m_strUser;           // Nick do usuario para inicializacao de MEDIARCVALLRUN em caso de TCP
    XStrDyn         m_strChannel;        // Canal da transmissao para inicializacao de MEDIARCVALLRUN em caso de TCP
    XStrDyn         m_strHost;           // Armazena ip do server para re-conexao em caso de perda de conexao TCP
    int             m_port;              // Armazena porta do server para re-conexao em caso de perda de conexao TCP
};


#endif      // __XFILETRANSFER__



