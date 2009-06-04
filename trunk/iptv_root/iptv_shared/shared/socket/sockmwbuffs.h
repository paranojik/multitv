#ifndef __SOCK_MWBUFFS__
#define __SOCK_MWBUFFS__


#include "shared/ithread.h"
#include "shared/semaphore.h"
#include "shared/mediabuf.h"

#include "shared/irm/cmd.h"

#include "socketctrl.h"
#include "isockbuff.h"
#include "notifypkt.h"


#define MW_PKTTYPE_ANY      0
#define MW_PKTTYPE_AUDIO    1
#define MW_PKTTYPE_VIDEO    2

#define MW_DEFRAGMODE_OFF           0x00
#define MW_DEFRAGMODE_PKT           0x01
#define MW_DEFRAGMODE_THREAD        0x02
#define MW_DEFRAGMODE_LMEDIAPKT     0x04
#define MW_DEFRAGMODE_PKTEXT        0x08


class SockMWBuffs : private SocketCtrl, public ISockBuff
{
public:

    SockMWBuffs (UINT _bitrate, UINT _nintervals, UINT _nmsecsbuff, BOOL _flmsecQOS, BOOL _flAudioPresent, DWORD _dwDefragMode);
   ~SockMWBuffs (void);

    void  reset (void);

    void  SetUrl           (LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen, ProxyParam *_pproxyparam);
    BOOL  OpenUrl          (void);
    BOOL  CloseUrl         (void);
    void  SetIdleMode      (BOOL _flag);
    void  Abort            (void);

    BOOL  SetINotify       (ISockBuffNotify *_pINotify, UINT _notifyfreq, DWORD _notifyid=0);
    void  ResetNotify      (void);

    BOOL  IsOk             (void) { return m_flOk;   }
    BOOL  IsOpened         (void) { return m_flOpen; }

    BOOL  IsDefragOn       (void) { return ( ((m_dwDefragMode & MW_DEFRAGMODE_PKT) == MW_DEFRAGMODE_PKT) ? TRUE : FALSE); }
    BOOL  IsDefragThread   (void) { return ( ((m_dwDefragMode & MW_DEFRAGMODE_THREAD) == MW_DEFRAGMODE_THREAD) ? TRUE : FALSE ); }
    BOOL  IsDefragLMediaPkt(void) { return ( ((m_dwDefragMode & MW_DEFRAGMODE_LMEDIAPKT) == MW_DEFRAGMODE_LMEDIAPKT) ? TRUE : FALSE ); }
    BOOL  IsDefragPktExt   (void) { return ( ((m_dwDefragMode & MW_DEFRAGMODE_PKTEXT) == MW_DEFRAGMODE_PKTEXT) ? TRUE : FALSE ); }

    BOOL  SetBitrate       (UINT _bitrate);
    void  SetFileMWObj     (ISockBuff *_pFileMW) { m_pFileMW = _pFileMW; }

    STDMETHODIMP Read( /* [length_is][size_is][out] */ void *pv,
                       /* [in] */ ULONG cb,
                       /* [out] */ ULONG *pcbRead);
    STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                       /* [in] */ ULONG cb,
                       /* [out] */ ULONG *pcbWritten);

private:

    BOOL   m_flOk,                // Classe foi inicializada com sucesso
           m_flOpen,              // Socket estah pronto para tx / rx ?
           m_flListen,            // connect() / listen() ?
           m_flSockError,         // indica se ocorreu erro na escrita do socket
           m_flExit,              // Abort thread
           m_flAbort,             // Abort send engine
           m_flUDP,               // TCP / UDP ?
           m_flMCast,             // Unicast / Multicast
           m_flIdleMode,          // Indica que a engine estah em estado de idle (nao deve executar reconnect)
           m_flWaitingForData,    // Indica que a thread de envio estah aguardando por dados a serem enviados
           m_flMsecQOS,           // Indica que o controle de bitrate tambem deverah ser instantaneo (a nivel de msecs)
           m_flAudioPresent;      // Indica se haverah stream de audio associado ao video

    DWORD  m_dwPort,              // Porta de conexao / listen
           m_tlastpkt,            // Instante de envio do ultimo pacote
           m_tlasttcpping,        // Instante de chegada do ultimo tcp ping (server->user)
           m_dwMaxBufSize,        // Tamanho dos buffers de media (audio e video)
           m_nbytesAudioBuf,      // Numero de bytes contidos no buffer de audio
           m_nbytesVideoBuf,      // Numero de bytes contidos no buffer de video
           m_nAudioPktsInBuf,     // Numero de pacotes contidos no buffer de audio
           m_nVideoPktsInBuf,     // Numero de pacotes contidos no buffer de video
           m_dwCurPktSeq,         // Sequencial corrente caso _flDefrag==TRUE
           m_dwDefragMode;        // Indica se o pacote deve ser quebrado (caso pktsize > MAKPKTSIZE) antes de ser enviado

    ULONG  m_uLastPktSeq;         // Sequencial do ultimo pacote enviado

    UINT   m_bitrate,             // Bitrate corrente desejado (se = 0 => nao deve haver controle de bitrate)
           m_maxbyterate,         // Numero maximo de bytes permitidos em 1 sec
           m_maxbytesonemsec,     // Numero maximo de bytes permitidos em 1 msec
           m_nintervals,          // Numero de intervalos de tempo a serem utilizados em 1 sec para analise
           m_tinterval,           // Duracao (em msecs) de 1 intervalo
           m_maxbytesinterval,    // Numero maximo de bytes permitidos em 1 intervalo
           m_curbyterate,         // Byterate corrente
           m_npktinterval,        // Numero de pacotes enviados no intervalo corrente
          *m_pnBytesInterval;     // Vetor contendo o numero de bytes enviados por intervalo

    SOCKET m_RemoteSock;          // Socket retornado por accept (TCP / listen)

    ISockBuffNotify *m_pINotify;  // Indica se deve ser notificado o envio de pacotes de media
    UINT        m_uNotifyFreq;    // Frequencia minima de notificacao de envio de pacotes de media
    DWORD       m_dwNotifyId;     // Identificador a ser informado na funcao de notificacao
    NotifyVect *m_pNotifyVector;  // Vetor que correlaciona Media IDs com os respectivos tempos de notificacao e flags das medias recebidas no intervalo

    ISockBuff  *m_pFileMW;        // Ponteiro para objeto de gravacao das medias enviadas

    IThread *m_pWriteThread;      // Thread para a leitura de dados do socket
    IThread *m_pDefragThread;     // Thread para juncao de segmentos de pacotes fragmentados
    IThread *m_pTCPPingThread;    // Thread para verificacao da chegada de MEDIAPING po servidor

    char   m_szProtocol [PROTOCOLLEN+1],
           m_szHost     [HOSTLEN+1],
           m_szConnStr  [TMAX_MSG+1];

    ProxyParam m_proxyparam;  // Dados para inicializao do http proxy (se houver)

    _SEMAPHORE *m_pMutex;
    _SEMAPHORE *m_pSeqCountMutex;
    _SEMAPHORE *m_pWaitForDataSemaph;

    LPSTR  m_pAudioBuff,
           m_pVideoBuff;

    MediaBuf *m_pDefragTmpBuf,    // Buffer para o recebimento dos pacotes do client (apenas no caso de defrag)
             *m_pTCPPingBuf;      // Buffer para a leitura dos pacotes de MEDIAPING enviados pelo servidor
                                  // para informar que o stream de media estah chegando

    BOOL   IsSockError   (void)            { return m_flSockError; }
    //BOOL   IsAudioPkt    (MediaPkt *_ppkt) { return ( (_ppkt->flags & (PKT_REALTIME | PKT_AUDIO)) == (PKT_REALTIME | PKT_AUDIO) ? TRUE : FALSE); }
    BOOL   IsAudioPkt    (BYTE _flags) { return ( (_flags & (PKT_REALTIME | PKT_AUDIO)) == (PKT_REALTIME | PKT_AUDIO) ? TRUE : FALSE); }

    void   ResetWaitingForData(void);

    BOOL   CanSendPkt    (ULONG _sz, DWORD _ctime, UINT *_pSleepTime);
    void   SetLastPktTime(DWORD _ctime=0);
    void   SetLastTCPPingTime(DWORD _ctime=0);

    BOOL   write         (MediaPkt *_pMediaPkt, UINT _pktlen, DWORD _ctime);
    BOOL   SendPkt       (const void *_pdata, ULONG *_pszdata);

    BOOL   SavePkt       (const void *_pdata);
    UINT   GetPkt        (MediaPkt *_pMediaPkt=NULL, UINT _blen=0, BOOL _flPop=FALSE, UINT _uPktType=MW_PKTTYPE_ANY);

    void   NotifyPktWrite(ULONG _mediaid, BYTE _flags);

    BOOL   StartWriteSockThread(void);
    BOOL   StartDefragThread   (void);
    BOOL   StartTCPPingThread  (void);
    static int _DoWriteSock(IThread *pthread, void* ctx);
    static int _DoDefrag   (IThread *pthread, void* ctx);
    static int _DoTCPPing  (IThread *pthread, void* ctx);
};


#endif  // __SOCK_MWBUFFS__


