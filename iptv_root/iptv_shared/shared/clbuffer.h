#ifndef _CLBUFFER_STR_
#define _CLBUFFER_STR_


#include "VectorT.h"
#include "SVectorT.h"


#define  CLBUF_NITENS_STEP_ALLOC  1024


#pragma pack(1)

class CLDataBuf
{
public:

    ULONG size;

    CLDataBuf *pnext,
              *pprev;

    LPSTR      pdata;

    void  init   (void);
    void  SetNext(CLDataBuf *_pnext) { pnext = _pnext; }
    void  SetPrev(CLDataBuf *_pprev) { pprev = _pprev; }

    ULONG AddData   (LPCSTR _pbuffer, ULONG _sz, ULONG _maxsize, BOOL _flFragPacket, LPCSTR *_pDataAdded);
    ULONG RemoveData(LPSTR _pbuffer , ULONG _sz, ULONG _offset);
};

#pragma pack()


// Cada registro aponta para uma lista de slots, associada ao identificador <id>

class CLCtrlData
{
public:

    ULONG id,       // Identificador da lista
          totsize,  // Total de bytes armazenados na lista
          maxsize;  // Numero maximo de bytes permitidos para a lista

    CLDataBuf *pfirst,    // Ponteiro para o primeiro slot da lista
              *plast;     // Ponteiro para o ultimo slot da lista

    CLDataBuf *pcurrent;  // Ponteiro para o slot corrente (para fins de consulta)
    long       userdata;  // Permite fornecer dados adicionais


    CLCtrlData(ULONG _id=0, ULONG _maxsize=0);
    CLCtrlData& operator= (const CLCtrlData& obj);
    CLCtrlData& operator= (int i);
    bool  operator==  (void *p) const;
    bool  operator==  (const CLCtrlData& obj) const;
    bool  operator!=  (const CLCtrlData& obj) const;
    bool  operator!=  (int i) const;

    void  init        (ULONG _id, ULONG _maxsize);
    BOOL  CanAddPacket(ULONG _sz);
    ULONG AppendPacket(CLDataBuf *_pslot, LPCSTR _pdata, ULONG _sz, ULONG _maxpacketsize, BOOL _flFragPacket, LPCSTR *_pPacketBuf);
    ULONG RemoveData  (CLDataBuf *_pslot, LPSTR _pbuffer, ULONG _sz, ULONG _offset);
    void  AddSlot     (CLDataBuf *_pslot, CLDataBuf *_pslotMain, BOOL _flAppend);
    void  RemoveSlot  (CLDataBuf *_pslot);

    void  SetPos      (CLDataBuf *_pcurrent) { pcurrent = _pcurrent; }
    void  SetUserData (long _userdata      ) { userdata = _userdata; }
};


class CLBufBlocks: public VectorT<void *>
{
protected:
    void kill_elem(void *pBlock) { if (pBlock) free(pBlock); }

public:

    ~CLBufBlocks(void) { kill(); }
};


/*
    _SlotSize    : Tamanho dos slots do Vetor (o tamanho do pacote nao poderah
                   exceder este valor)
    _nSlots      : Numero de slots inicial do Vetor
    _flOverwrite : Se for atingido tamanho maximo de bytes na lista associada a um dado
                   id, apaga primeiro registro da lista e reutiliza seu espaco
    _flOnePacketPerSlot : Permite colocar mais de 1 pacote em um slot
    _flFragPacket: Se um slot estiver full, quebra o pacote de forma a ocupar mais de 1 slot

    _flAutoId: Ao incluir novo pacote, se id=0 => id=lastid
    _IncSlots: Quando nao houver mais espaco disponivel no vetor corrente
               eh alocado um novo bloco com <IncSlots> slots
*/

class CLBuffer
{
private:

    ULONG SlotSize,         // Tamanho dos registros de dados
          InitSlots,        // Numero inicial de registros
          IncSlots,         // Numero de slots a serem alocados quando ocorrer Buffer Full
          TotSlots;         // Total de slots ocupados (em todos os blocos)
    
    ULONG lastId;           // Identificador do ultimo registro incluido

    BOOL  flOverwrite,      // Indica se ao ocorrer buffer full se o proximo registro a ser incluido associado a um id deverah sobrescrever o primeiro (da lista)
          flAutoId,         // Indica se os identificadores de registros devem ser atribuidos automaticamente
          flOk;

    BOOL  flOnePacketPerSlot, // Permite colocar mais de 1 pacote em um slot
          flFragPacket;       // Se um slot estiver full, quebra o pacote de forma a ocupar mais de 1 slot

    LPSTR pCurBlock;          // Ponteiro para o Bloco corrente (um bloco contem varios slots)
    ULONG nMaxSlotsBlock,     // Numero maximo de slots do bloco corrente
          nSlotsBlock;        // Numero de slots incluidos no bloco corrente

    CLCtrlData LCtrlDataLast; // Guarda informacoes da ultima operacao de consulta realizada (first)
    ULONG      nBytesLast;    // Para otimizar o pop em seguida (se for o caso)
    ULONG      IdLastOp;

    VectorT   <CLDataBuf *> *pDeletedSlots;  // Vetor de Slots excluidos
    CLBufBlocks  *pBlocks;                   // Vetor de Blocos alocados (cada bloco contem N Slots nos quais sao armazenados os registros de dados)


    BOOL  AllocBlock(ULONG _nSlots);
    void  kill      (void);

    ULONG getdata   (CLCtrlData& LCtrlData, LPSTR _pdata, ULONG *_pszdata, long *_puserdata=NULL);

    CLDataBuf * NewSlot(void);


protected:

    ULONG MaxPacketSize;    // Tamanho maximo do pacote de dados

    SVectorT  <ULONG, CLCtrlData> *pTable;   // Relaciona identificadores com o registro de controle de slots correspondente


public:

    CLBuffer  (ULONG _MaxPacketSize, ULONG _nSlots=CLBUF_NITENS_STEP_ALLOC, BOOL _flOverwrite=TRUE, BOOL _flOnePacketPerSlot=FALSE, BOOL _flFragPacket=FALSE, BOOL _flAutoId=FALSE, ULONG _IncSlots=0);
   ~CLBuffer  (void);

    BOOL  IsOk(void) { return flOk; }

    //BOOL  newid    (ULONG _id, ULONG _maxbufsize, long _userdata, CLCtrlData &LCtrlData);
    BOOL  reset     (void);

    ULONG insert    (ULONG _id, LPCSTR _pdata, ULONG _szdata, ULONG _maxbufsize, LPCSTR *_pBuffer=NULL, long _userdata=0, CLDataBuf **_pSlot=NULL, BOOL _flMustNewSlot=FALSE, CLDataBuf *_pSlotMain=NULL, BOOL _flAppendSlotMain=TRUE, BOOL _flMustNotFragPacket=FALSE);
    BOOL  search    (ULONG _id, LPSTR _pdata=NULL, ULONG *_pszdata=NULL, long *_puserdata=NULL);
    BOOL  erase     (ULONG _id);
    BOOL  erase     (CLCtrlData *_pLCtrlData);
    void  erase_all (void);
    BOOL  set_pos   (ULONG _id);
    ULONG get_nitens(void);
    ULONG first     (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);
    ULONG last      (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);
    ULONG next      (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);
    ULONG prev      (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);

    ULONG popfirst  (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);
    ULONG poplast   (LPSTR _pdata=NULL, ULONG *_pszdata=NULL);
    ULONG first     (ULONG _id, LPSTR _pdata=NULL, ULONG *_pszdata=NULL, long *_puserdata=NULL, BOOL flPop=FALSE, BOOL *_pflPktRemoved=NULL);
    void  poplastop (ULONG _id, ULONG _nbytes);

    ULONG getszdata (ULONG _id);
};


#endif  // _CLBUFFER_STR_

