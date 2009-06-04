#ifndef _CLSBUFFER_STR_
#define _CLSBUFFER_STR_


#include "clbuffer.h"



/*
    Estrutura de armazenamento de pacotes (sequencia de bytes a serem bufferizados)
*/

struct CLSPktRec
{
    ULONG   id,
            size,
            type;

    ULONG64 userdata;

    LPCSTR ptr;

    CLDataBuf *pSlot;


    CLSPktRec(ULONG _id=0, ULONG _size=0, ULONG _type=0, LPCSTR _ptr=NULL, CLDataBuf *_pSlot=NULL, ULONG64 _userdata=0);

    void init(ULONG _id, ULONG _size, ULONG _type, LPCSTR _ptr, CLDataBuf *_pSlot, ULONG64 _userdata);

    CLSPktRec& operator= (const CLSPktRec& obj);
    CLSPktRec& operator= (int i);
    bool operator== (unsigned long _id) const;
    bool operator== (const CLSPktRec& obj) const;
    bool operator!= (const CLSPktRec& obj) const;
    bool operator!= (int i) const;

    void SetUserData(ULONG64 _userdata) { userdata = _userdata; }
};


/*
    Tabela que associa pacotes a seus identificadores (sequenciais)
*/

class CLSPktTable : public SVectorT<ULONG, CLSPktRec>
{
friend class CLSBuffer;

private:

    ULONG BufSize,
          lastId;

public:

    CLSPktTable(ULONG _MaxBufSize);
   ~CLSPktTable(void);

    void SetLastId(ULONG _lastId) { lastId = _lastId; }

    BOOL SetPktUserData(ULONG _idpkt, ULONG64 _userdata);
};


/*
    Tabela que associa Identificadores a suas respectivas tabelas de pacotes
    CLSIdTable -> N Ids
    1 Id -> 1 Tabela de Pacotes -> N Pacotes
*/

class CLSIdTable : public SVectorT<ULONG, CLSPktTable *>
{
protected:
    void kill_elem(ULONG _id, CLSPktTable *_pPktTable) { if (_pPktTable) delete _pPktTable; }

public:
   ~CLSIdTable(void) { kill(); }
};


enum CLSOp
{
    CLS_OP_GET    = 1,
    CLS_OP_FIRST  = 2,
    CLS_OP_LAST   = 3,
    CLS_OP_NEXT   = 4,
    CLS_OP_PREV   = 5,
};

class CLSBuffer : public CLBuffer
{
private:

    CLSIdTable *pIdTable;   // Relaciona identificadores com suas tabelas de pacotes

    CLSPktTable *pLastPktTable;     // Guarda ultima tabela de pacotes percorrida
    ULONG  lastTableId;             // e seu respectivo id para fins de otimizacao


    BOOL  _AddPacket(ULONG _id, CLSPktTable *_pPktTable, CLSPktRec &PktRec, void *_pdata, BOOL _flMustNewSlot, CLDataBuf *_pSlotMain, BOOL _flAppendSlotMain);
    ULONG _GetPacket(CLSOp _op, ULONG _id, void *_pbuffer, UINT *_pszdata, ULONG *_type, ULONG _idPkt, ULONG64 *_userdata);


protected:

    CLSPktTable * GetPktTable(ULONG _id);


public:

    CLSBuffer(ULONG _MaxPacketSize, ULONG _nSlots, ULONG _IncSlots);
   ~CLSBuffer(void);

    BOOL  isOk(void)   { return (pIdTable ? TRUE:FALSE); }

    BOOL  NewId (ULONG _id, ULONG _MaxBufSize);
    void  KillId(ULONG _id);

    BOOL  AddPacket   (ULONG _id, ULONG _idPkt, void *_pdata, UINT _szdata, ULONG _type, BOOL _flMustNewSlot);
    ULONG GetPacket   (ULONG _id, ULONG _idPkt, void *_pbuffer, UINT *_pszdata, ULONG *_type, ULONG64 *_userdata=NULL);
    ULONG GetLastPktId(ULONG _id);

    ULONG firstPacket (ULONG _id, void *_pbuffer, UINT *_pszdata, ULONG *_type=NULL);
    ULONG lastPacket  (ULONG _id, void *_pbuffer, UINT *_pszdata, ULONG *_type=NULL);
    ULONG nextPacket  (ULONG _id, void *_pbuffer, UINT *_pszdata, ULONG *_type=NULL);
    ULONG prevPacket  (ULONG _id, void *_pbuffer, UINT *_pszdata, ULONG *_type=NULL);

    ULONG64 GetPktUserData(ULONG _id, ULONG _idpkt);
    BOOL    SetPktUserData(ULONG _id, ULONG _idpkt, ULONG64 _userdata);

    BOOL  SetPos      (ULONG _id, ULONG _idpkt);
};


#endif  // _CLSBUFFER_STR_

