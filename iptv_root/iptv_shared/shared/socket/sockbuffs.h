#ifndef __SOCK_BUFFS__
#define __SOCK_BUFFS__

#include <time.h>

#include "shared/ithread.h"
#include "shared/semaphore.h"
#include "shared/BufVectorT.h"
#include "shared/jitterbuffer.h"

#include "socketctrl.h"
#include "isockbuff.h"
#include "proxyparam.h"
#include "notifypkt.h"


#define TINDEX_ID             20
#define TINDEX_VERSION        63
#define NMAX_INDEXES        1000


#define SB_PLAYSTATE_STOP   0x00
#define SB_PLAYSTATE_PLAY   0x01
#define SB_PLAYSTATE_PAUSE  0x02
#define SB_PLAYSTATE_PAUSED 0x04


#define SB_UNDEF_MEDIAID    (ULONG)-1L

#define SB_MEDIAPKTTYPE_NORMAL  0x00
#define SB_MEDIAPKTTYPE_LPKT    0x01
#define SB_MEDIAPKTTYPE_EXT     0x02



struct IndexStr
{
    char  m_szHeaderID[TINDEX_ID+1],
          m_szVersion [TINDEX_VERSION+1];

    DWORD m_dwChecksum;

    ULONG m_uMaxIndexes,
          m_nIndexes,
          m_uOffsets[NMAX_INDEXES];

    ULONG m_uMaxIndexesInit,
          m_tHeader;


    IndexStr(ULONG _nmax_index=NMAX_INDEXES);

    ULONG GetnIndex (void) { return m_nIndexes; }

    void  reset        (void);
    BOOL  CreateIndexes(LPCSTR _filename);
    BOOL  AddIndex     (ULONG _offset);
    ULONG GetIndex     (ULONG _index);
    void  FillIndexes  (void);
    BOOL  SaveIndexes  (HFILE _hfile);
    BOOL  ReadIndexes  (HFILE _hfile);
};


class SockBuffs;
class SockBuf_reg2;
class SockBuf_defrag;

class SockBuf_reg : public BufVectorReg, public ISockBuff
{
public:

    SockBuf_reg(ULONG _maxblen, BOOL _flRealloc=FALSE);
   ~SockBuf_reg(void);

    BOOL  IsOk (void) { return ((m_pszBuf && m_pMutex && m_pWaitForDataSemaph && m_pGettingDataSemaph) ? TRUE:FALSE); }

    void  reset(void);
    void  flush(void);

    BOOL  IsEnable       (void)    { return TRUE; }
    BOOL  SetEnable      (BOOL _f) { return TRUE; }
    ULONG GetBytesLeft   (void)    { return (m_dwmaxblen - m_dwblen);  }
    BOOL  CanAbort       (void)    { return m_flAbort; }
    void  Abort          (void);

    ISockBuff * GetObj   (void);

    BOOL  SaveData       (LPVOID _pdataBuf, ULONG _szdata);
    BOOL  GetData        (LPVOID _pdataBuf, ULONG *_szdataBuf);

    void  SetMediaId     (ULONG _mediaid);
    void  SetTSFstPkt    (ULONG _mstime);
    void  GetTSFstPkt    (ULONG *_pTSFstPkt, ULONG *_ptFstPkt);

    void  SetSockBuffs   (SockBuffs *_pSockBuffs)       { m_pSockBuffs    = _pSockBuffs;   }
    void  SetId          (UINT _id)                     { m_uId           = _id;           }
    void  SetBlock       (BOOL _flBlock)                { m_flBlock       = _flBlock;      }
    BOOL  CanBlock       (void)                         { return m_flBlock;                }
    BOOL  ShouldBlock    (void)                         { return m_flShouldBlock;          }

    UINT  GetId          (void)                         { return m_uId;                    }
    ULONG GetMediaId     (void)                         { return m_dwmediaid;              }

    ULONG GetBufLen      (void)                         { return m_dwblen;                 }
    ULONG GetMaxBufLen   (void)                         { return m_dwmaxblen;              }

    ULONG GetReadBitrate (void);
    ULONG GetSaveBitrate (void);

    BOOL  OpenUrl        (void);
    BOOL  CloseUrl       (void);
    BOOL  IsOpened       (void);
    BOOL  IsSockError    (void);

    BOOL  SetOptions     (UINT _uMaxPktSize, UINT _nPktWaitForDelayed, BOOL _flDefrag, ULONG _uMediaPktType);

    STDMETHODIMP Read ( /* [length_is][size_is][out] */ void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbRead);

    STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbWritten);

private:

    FILE  *m_fDump;

    BOOL   m_flOpen,
           m_flAbort,
           m_flBlock,
           m_flShouldBlock,
           m_flWaitingForData,
           m_flRealloc;

    LPSTR  m_pszBuf;

    UINT   m_uId;

    ULONG  m_dwblen,
           m_dwmaxblen,
           m_dwmediaid,
           m_nPktLost,
           m_dwLastPktSeq,
           m_dwMsTSFstPkt,
           m_dwMstFstPkt;

    ULONG  m_unBytesSaved,
           m_unBytesRead,
           m_uSaveBitrate,
           m_uReadBitrate;

    time_t m_tlastSave,
           m_tlastRead;

    SockBuffs      *m_pSockBuffs;       // Main Class
    SockBuf_reg2   *m_pSockBuf_reg2;    // Aux class for pky delay drop
    SockBuf_defrag *m_pSockBuf_defrag;  // Aux class for pky defrag

    _SEMAPHORE *m_pMutex;
    _SEMAPHORE *m_pWaitForDataSemaph;
    _SEMAPHORE *m_pGettingDataSemaph;

    UINT IsPacketRdy(void);

    void ResetWaitingForData(void);
};


class SockBuf_reg2 : public ISockBuff
{
public:

    SockBuf_reg2   (void);
    virtual ~SockBuf_reg2   (void);

    BOOL IsOk      (void) { return m_flOk; }

    void reset     (void);
    BOOL SetOptions(SockBuf_reg *_pSockBufReg, UINT _uMaxPktSize, UINT _nPktWaitForDelayed);

    BOOL CanAbort  (void) { return (m_pSockBufReg ? m_pSockBufReg->CanAbort() : FALSE);    }
    BOOL OpenUrl   (void) { return (m_pSockBufReg ? m_pSockBufReg->OpenUrl()  : FALSE);    }
    BOOL CloseUrl  (void) { return (m_pSockBufReg ? m_pSockBufReg->CloseUrl() : FALSE);    }
    BOOL IsOpened  (void) { return (m_pSockBufReg ? m_pSockBufReg->IsOpened() : FALSE);    }
    void Abort     (void) { if (m_pSockBufReg) m_pSockBufReg->Abort();                     }

    void SetBlock  (BOOL _f) { if (m_pSockBufReg) m_pSockBufReg->SetBlock(_f);             }
    BOOL CanBlock  (void)    { return (m_pSockBufReg ? m_pSockBufReg->CanBlock() : FALSE); }

    STDMETHODIMP Read ( /* [length_is][size_is][out] */ void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbRead);

    STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbWritten);
private:

    BOOL  SaveData  (void *_pv, ULONG _ldata);
    BOOL  GetData   (void *_pv, ULONG *_pldata);
    BOOL  CanSendPkt(void *_pv, ULONG _ldata, BOOL *_pbPktError);


    BOOL  m_flOk;

    UINT  m_uMaxPktSize,
          m_nPktWaitForDelayed;

    ULONG m_dwCurMediaId,
          m_dwFirstPktInBuf,
          m_dwLastPktInBuf,
          m_dwNextSndPkt;

    JitterBuffer *m_pJitterBuffer;

    SockBuf_reg *m_pSockBufReg;
};


class SockBuf_defrag: public ISockBuff
{
public:

    SockBuf_defrag (SockBuf_reg2 *_pSockBufReg2, ULONG _uMediaPktType);
    virtual ~SockBuf_defrag (void);

    BOOL IsOk      (void) { return m_flOk; }

    void reset     (void);
    void flush     (void);

    BOOL OpenUrl   (void) { return (m_pSockBufReg2 ? m_pSockBufReg2->OpenUrl()  : FALSE);    }
    BOOL CloseUrl  (void) { return (m_pSockBufReg2 ? m_pSockBufReg2->CloseUrl() : FALSE);    }
    BOOL IsOpened  (void) { return (m_pSockBufReg2 ? m_pSockBufReg2->IsOpened() : FALSE);    }
    void Abort     (void) { if (m_pSockBufReg2) m_pSockBufReg2->Abort();                     }

    void SetBlock  (BOOL _f) { if (m_pSockBufReg2) m_pSockBufReg2->SetBlock(_f);             }
    BOOL CanBlock  (void)    { return (m_pSockBufReg2 ? m_pSockBufReg2->CanBlock() : FALSE); }

    STDMETHODIMP Read ( /* [length_is][size_is][out] */ void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbRead);

    STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                        /* [in] */ ULONG cb,
                        /* [out] */ ULONG *pcbWritten);
private:

    BOOL  SaveData  (void *_pv, ULONG _ldata);
    BOOL  GetData   (void *_pv, ULONG *_pldata);
    BOOL  CanSendPkt(void *_pv, ULONG _ldata, BOOL *_pbPktError);

    void  TracePkt  (void *_pv);

    BOOL  m_flOk;
          

    ULONG m_uMediaPktType,      // Defines if datalen type is USHORT or ULONG and if pkt has EXT data or not
          m_dwCurMediaId,       // Current Media Id
          m_dwBufSize,          // Memory block size allocated to buffer
          m_dwnBytesInBuf,      // nbytes saved in buffer
          m_dwTotPktSegs,       // Number of segments needed to complete current packet
          m_dwContSegs,         // Number of segments already saved (current packet)
          m_dwFirstSeqFrame,    // First packet's sequential of current subseq
          m_dwLastSeq,          // Last Pkt sequential in buffer (main seq)
          m_dwCurSubSeq,        // Current Pkt SubSeq
          m_dwCurMsTimestamp,   // Current timestamp
          m_dwCurSubSeqPktLost; // Number of packets lost in current subseq

    BYTE  m_CurPktType,         // Current Pkt Type
          m_CurPktFlags;        // Current Pkt Flags

    void *m_pBuffer;

    SockBuf_reg2 *m_pSockBufReg2;
};


class SockBufVect : public BufSVectorT<UINT, SockBuf_reg>
{
public:

    SockBufVect(UINT _maxlines, ULONG _maxblen);

    void  reset(void)       { BufSVectorT<UINT, SockBuf_reg>::reset(); }
    void  reset(ULONG _id);
    void  flush(ULONG _id);

    BOOL  SetMediaId     (ULONG _id, ULONG _mediaid);
    BOOL  SetTSFstPkt    (ULONG _id, ULONG _tsFstPkt);
    BOOL  GetTSFstPkt    (ULONG _id, ULONG *_pTSFstPkt, ULONG *_ptFstPkt);
    void  SetSockBuffs   (ULONG _id, SockBuffs *_pSockBuffs);
    BOOL  SetOptions     (ULONG _id, UINT _uMaxPktSize, UINT _nPktWaitForDelayed, BOOL _flDefrag, ULONG _uMediaPktType);
    ULONG GetId          (ULONG _mediaid);
    BOOL  OpenUrl        (ULONG _id);
    ULONG GetBufLen      (ULONG _id);

    ULONG GetReadBitrate (ULONG _id);
    ULONG GetSaveBitrate (ULONG _id);
    BOOL  SetBlock       (ULONG _id, BOOL _flBlock);

    ISockBuff * GetObj   (ULONG _id);
};


class SockBuffs : private SocketCtrl
{

friend class SockBuf_reg;

public:

    SockBuffs  (UINT _maxids, ULONG _maxblen);
   ~SockBuffs  (void);

    void  reset(void);

    BOOL  SetINotify     (ISockBuffNotify *_pINotify, UINT _notifyfreq, DWORD _notifyid=0);
    void  ResetNotify    (void);

    BOOL  SetUrl         (LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen, ProxyParam *_pproxyparam);
    BOOL  SetUrl         (LPCSTR _szReadFile);
    BOOL  SetReadOffset  (double _fpercent);

    BOOL  OpenUrl        (void);
    void  CloseUrl       (void);
    void  SetIdleMode    (BOOL _flag);

    BOOL  Play           (void);
    void  Stop           (void);
    BOOL  Pause          (BOOL _flOn);
    void  Abort          (void);
    void  Abort          (ULONG _wndid);

    BOOL  IsPlaying      (void) { return ((m_dwPlayState & SB_PLAYSTATE_PLAY)   ? TRUE : FALSE); }
    BOOL  IsPauseOn      (void) { return ((m_dwPlayState & SB_PLAYSTATE_PAUSE)  ? TRUE : FALSE); }
    BOOL  IsPaused       (void) { return ((m_dwPlayState & SB_PLAYSTATE_PAUSED) ? TRUE : FALSE); }
    BOOL  IsStopped      (void) { return (m_dwPlayState ? FALSE : TRUE); }

    void  SetState       (DWORD _flag) { m_dwPlayState |= _flag;  }
    void  ResetState     (DWORD _flag) { m_dwPlayState &= ~_flag; }

    BOOL  SetSaveFileName(LPCSTR _szFileName);

    BOOL  CreateWnd      (ULONG _wndid, UINT _uMaxPktSize=0, UINT _nPktWaitForDelayed=0, BOOL _flDefrag=FALSE, ULONG _uMediaPktType=SB_MEDIAPKTTYPE_NORMAL, ULONG _mediaid=SB_UNDEF_MEDIAID);
    ULONG GetWndId       (ULONG _mediaid);
    BOOL  SetWndMediaId  (ULONG _wndid, ULONG _mediaid);
    void  DestroyWnd     (ULONG _wndid);
    void  DestroyAllWnds (void);

    ULONG GetReadBitrate (ULONG _wndid);
    ULONG GetSaveBitrate (ULONG _wndid);

    BOOL  SetWndBlock    (ULONG _wndid, BOOL _flBlock);
    ISockBuff * GetWndObj(ULONG _wndid);
    void  SetFileMWObj   (ISockBuff *_pFileMW) { m_pFileMW = _pFileMW; }


private:

    BOOL   m_flOk,              // Classe foi inicializada com sucesso
           m_flUrlSet,          // A funcao SetUrl() foi executada com sucesso ?
           m_flOpen,            // Socket estah pronto para tx / rx ?
           m_flListen,          // connect() / listen() ?
           m_flSockError,       // indica se ocorreu erro na leitura do socket
           m_flExit,            // Abort thread
           m_flUDP,             // TCP / UDP ?
           m_flMCast,           // Unicast / Multicast
           m_flIdleMode;        // Indica que a engine estah em estado de idle (nao deve executar reconnect)

    UINT   m_nMaxIds,           // Numero maximo de IDs de media a serem bufferizados
           m_nIds,              // Numero corrente de IDs de media
           m_uNotifyFreq;       // Frequencia minima de notificacao de recebimento de pacotes de media

    DWORD  m_dwPort,            // Porta de conexao / listen
           m_dwPlayState;       // Indica se estah em estado de PLAY / STOP / PAUSE

    time_t m_tlastpkt;          // Instante de chegada do ultimo pacote

    long   m_uReadFileSize,     // Tamanho do arquivo de leitura de media
           m_uReadOffset;       // Offset corrente na leitura do arquivo de leitura de media

    SOCKET m_RemoteSock;        // Socket retornado por accept (TCP / listen)
    SockBufVect *m_pBufVect;    // Vetor de buffers associados a cada ID

    IThread *m_pReadThread;     // Thread para a leitura de dados do socket

    SockBuf_reg  *m_pBuffer;    // Buffer intermediario para a leitura dos pacotes de rede

    SVectorT<ULONG, ULONG>     *m_pWndVector;    // Vetor que correlaciona Window IDs com Media IDs
    NotifyVect                 *m_pNotifyVector; // Vetor que correlaciona Media IDs com os respectivos tempos de notificacao e flags das medias recebidas no intervalo

    HFILE  m_hSaveFile,         // Handle do arquivo para gravar medias recebidas
           m_hReadFile;         // Handle do arquivo para leitura de media ( SetUrl() )

    ISockBuff *m_pFileMW;       // Ponteiro para objeto de gravacao das medias recebidas

    IndexStr m_Index;           // Estrutura para o armazenamento de indices do arquivo de medias recebidas

    char   m_szProtocol [PROTOCOLLEN+1],
           m_szHost     [HOSTLEN+1],
           m_szConnStr  [TMAX_MSG+1],
           m_szReadFile [TPATH+1];

    ProxyParam m_proxyparam;    // Dados para inicializao do proxy (se houver)

    _SEMAPHORE *m_pMutex;

    ISockBuffNotify *m_pINotify;    // Indica se MediaIDs nao cadastrados devem ser notificados (para serem incluidos se for o caso)
    DWORD            m_dwNotifyId;  // Identificador a ser informado na funcao de notificacao

    BOOL   IsOpened     (void)   { return m_flOpen;      }
    BOOL   IsSockError  (void)   { return m_flSockError; }

    BOOL   VerifyConnection   (void);
    void   NotifyPktRead      (ULONG _mediaid, BYTE _flags);
    void   PauseMedias        (void);

    void   CloseSaveFile      (void);
    BOOL   WriteSaveFile      (LPVOID _buf, UINT _ldata);

    void   CloseReadFile      (void);

    ULONG  SearchMediaID      (ULONG _mediaid);
    ULONG  FirstMediaID       (ULONG *_pmediaid);
    ULONG  NextMediaID        (ULONG *_pmediaid);

    void   SetLastPktTime     (time_t _ctime=0);
    BOOL   MaxIdleTimeReached (void);

    BOOL   StartReadSockThread(void);
    static int _DoReadSock(IThread *pthread, void* ctx);
};


#endif


