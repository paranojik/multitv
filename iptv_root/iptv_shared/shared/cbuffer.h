#ifndef _CBUFFER_STR_
#define _CBUFFER_STR_


#include "VectorT.h"
#include "SVectorT.h"


#define  CBUF_NITENS_STEP_ALLOC  1000


class CBufRec
{
public:

    ULONG id,
          size;

    void *ptr;


    CBufRec(ULONG _id=0, ULONG _size=0, void *_ptr=NULL)
    {
        id   = _id;
        size = _size;
        ptr  = _ptr;
    }

    CBufRec& operator= (const CBufRec& obj)
    {
        id   = obj.id;
        size = obj.size;
        ptr  = obj.ptr;

        return *this;
    }

    CBufRec& operator= (int i)
    {
        if (i == 0) 
        {
            id   = 0;
            size = 0;
            ptr  = NULL;
        }

        return *this;
    }

    bool operator== (void *p) const
    {
        if (p == ptr)
            return true;
            
        return false;
    }

    bool operator== (const CBufRec& obj) const
    {
        if (ptr == obj.ptr)
            return true;
            
        return false;
    }

    bool operator!= (const CBufRec& obj) const
    {
        if (ptr != obj.ptr)
            return true;
            
        return false;
    }
};


class CBufBlocks: public VectorT<void *>
{
protected:
    void kill_elem(void *pBlock) { if (pBlock) free(pBlock); }

public:

    ~CBufBlocks(void) { kill(); }
};



class CBuffer
{
private:

    ULONG SlotSize,         // Tamanho dos registros de dados
          InitSlots,        // Numero inicial de registros
          IncSlots,         // Numero de slots a serem alocados quando ocorrer Buffer Full
          TotSlots;         // Total de slots ocupados (em todos os blocos)
    
    ULONG lastId;           // Identificador do ultimo registro incluido

    BOOL  flOverwrite,      // Indica se ao ocorrer buffer full se o proximo registro a ser incluido deverah sobrescrever o primeiro
          flAutoId,         // Indica se os identificadores de registros devem ser atribuidos automaticamente
          flOk;

    LPSTR    pCurBlock;     // Ponteiro para o Bloco corrente
    ULONG    nMaxSlotsBlock,// Numero maximo de slots do bloco corrente
             nSlotsBlock;   // Numero de slots incluidos no bloco corrente


    SVectorT  <ULONG, CBufRec> *pTable;   // Relaciona identificadores de registros com o slot correspondente
    VectorT   <void *> *pDeletedSlots;    // Vetor de Slots excluidos
    CBufBlocks  *pBlocks;                 // Vetor de Blocos alocados (cada bloco contem N Slots nos quais sao armazenados os registros de dados)


    BOOL  AllocBlock(ULONG _nSlots);
    void  kill      (void);
    void  reset     (void);

    ULONG getdata   (CBufRec& SlotData, LPCSTR *_pdata, ULONG *_pszdata);

public:

    CBuffer  (ULONG _SlotSize, ULONG _nSlots=CBUF_NITENS_STEP_ALLOC, BOOL _flOverwrite=TRUE, BOOL _flAutoId=FALSE, ULONG _IncSlots=0);
   ~CBuffer  (void);

    BOOL IsOk(void) { return flOk; }

    ULONG insert  (ULONG _id, LPCSTR _pdata, ULONG _szdata, void **_pBuffer=NULL);
    BOOL  search  (ULONG _id, LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
    BOOL  erase   (ULONG _id);
    BOOL  set_pos (ULONG _id);
    ULONG first   (LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
    ULONG last    (LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
    ULONG next    (LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
    ULONG prev    (LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);

    ULONG popfirst(LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
    ULONG poplast (LPCSTR *_pdata=NULL, ULONG *_pszdata=NULL);
};


#endif  // _CBUFFER_STR_

