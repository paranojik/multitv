#ifndef _BUFVECTORT_STR
#define _BUFVECTORT_STR


#include "VectorT.h"
#include "SVectorT.h"


/*
template <class T> class BufVectorT
{

public:

    BufVectorT (UINT _maxlines, ULONG _maxblen);
   ~BufVectorT (void);

    BOOL IsOk  (void) { return flOk; }

    void reset (void);
    UINT append(void);
    BOOL erase (UINT _ind);
    T *  search(UINT _ind);
    T *  first (void);
    T *  next  (void);

    BOOL  SaveData (UINT _ind, LPVOID _pdataBuf, ULONG _szdata);
    BOOL  GetData  (UINT _ind, LPVOID _pdataBuf, ULONG *_szdataBuf);

    ULONG GetBytesLeft(UINT _ind);
    BOOL  IsEnable    (UINT _ind);
    BOOL  SetEnable   (UINT _ind, BOOL _f);
};

template <class K, class T> class BufSVectorT
{

public:

    BufSVectorT (UINT _maxlines, ULONG _maxblen);
   ~BufSVectorT (void);

    BOOL IsOk  (void) { return flOk; }

    void reset (void);
    UINT append(K _id);
    BOOL erase (K _id);
    T *  search(K _id);
    T *  first (void);
    T *  next  (void);

    BOOL  SaveData (K _id, LPVOID _pdataBuf, ULONG _szdata);
    BOOL  GetData  (K _id, LPVOID _pdataBuf, ULONG *_szdataBuf);

    ULONG GetBytesLeft(K _id);
    BOOL  IsEnable    (K _id);
    BOOL  SetEnable   (K _id, BOOL _f);
};
*/


class BufVectorReg
{
public:
    virtual ~BufVectorReg(void) {}

    virtual BOOL IsOk (void) = 0;
    virtual void reset(void) = 0;

    virtual BOOL  SaveData    (LPVOID _pdataBuf, ULONG _szdata) = 0;
    virtual BOOL  GetData     (LPVOID _pdataBuf, ULONG *_szdataBuf) = 0;
    virtual ULONG GetBytesLeft(void) = 0;
    virtual BOOL  IsEnable    (void) = 0;
    virtual BOOL  SetEnable   (BOOL _f) = 0;
};


template <class T> class BufVect : public VectorT<T *>
{

public:

     BufVect(UINT _maxlines) : VectorT<T *>(TRUE, _maxlines) {}
    ~BufVect(void) { VectorT<T *>::kill(); }

protected:
    void kill_elem(T *_pReg) { if (_pReg) delete _pReg; }
};


template <class K, class T> class BufSVect : public SVectorT<K, T *>
{

public:

    BufSVect(UINT _maxlines) : SVectorT<K, T *>(TRUE, _maxlines) {}
    ~BufSVect(void) { SVectorT<K, T *>::kill(); }

protected:
    void kill_elem(K _id, T *_pReg) { if (_pReg) delete _pReg; }

};



template <class T> class BufVectorT
{

private:

    BufVect<T>  *pVectActiveBuf,
                *pVectDeletedBuf;

    UINT  maxlines;     // Numero maximo de elementos do vetor
    ULONG maxblen;      // Tamanho maximo de cada buffer

    BOOL  flOk;


public:

    BufVectorT(UINT _maxlines, ULONG _maxblen)
    {
        pVectActiveBuf  = NULL;
        pVectDeletedBuf = NULL;

        flOk     = FALSE;
        maxlines = 0;
        maxblen  = 0;

        if (_maxlines && _maxblen)
        {
            pVectActiveBuf  = new BufVect<T>(_maxlines);
            pVectDeletedBuf = new BufVect<T>(_maxlines);

            if (pVectActiveBuf && pVectDeletedBuf)
            {
                flOk     = TRUE;
                maxlines = _maxlines;
                maxblen  = _maxblen;
            }
        }
    }


   ~BufVectorT (void)
    {
        if (pVectActiveBuf)
            delete pVectActiveBuf;

        if (pVectDeletedBuf)
            delete pVectDeletedBuf;
    }


    BOOL IsOk  (void) { return flOk; }

    void reset (void)
    {
        if (pVectActiveBuf)
            pVectActiveBuf->reset();

        if (pVectDeletedBuf)
            pVectDeletedBuf->reset();
    }


    // _ind = 1..n

    T * search(UINT _ind)
    {
        T  *pReg;

        pReg = NULL;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);
        }

        return pReg;
    }


    T * first(void)
    {
        T *pReg;

        pReg = NULL;

        if (flOk)
            pReg = pVectActiveBuf->first(); 

        return pReg;
    }


    T * next(void)
    {
        T *pReg;

        pReg = NULL;

        if (flOk)
            pReg = pVectActiveBuf->next(); 

        return pReg;
    }


    UINT append(void)
    {
        UINT ret;

        T *pReg;


        ret = 0;

        if (flOk)
        {
            if (pVectActiveBuf->get_nelem() < maxlines)
            {
                // Verifica se ha itens no vetor de deletados
                pReg = pVectDeletedBuf->popLast();

                // Se nao houver => cria novo registro
                if (!pReg)
                {
                    pReg = new T(maxblen);

                    if (!pReg->IsOk())
                    {
                        delete pReg;
                        pReg = NULL;
                    }
                }

                if (pReg)
                {
                    // O Objeto agora eh resetado no instante de sua desativacao
                    //pReg->reset();

                    // Adiciona na lista de registros ativos
                    ret = pVectActiveBuf->insert(pReg);

                    if (!ret)
                    {
                        // Erro ao adicionar registro => adiciona na 
                        // lista de deletados
                        if (  pVectDeletedBuf->get_nelem() >= maxlines ||
                             !pVectDeletedBuf->insert(pReg) )
                        {
                            // Erro ao adicionar registro => apaga registro
                            delete pReg;
                        }
                    }
                }
            }
        }

        return ret;
    }


    // ind = 1..n
    BOOL erase(UINT _ind)
    {
        BOOL ret;
        T  *pReg;

        ret  = FALSE;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
            {
                // Reseta objeto para possivel utilizacao posterior
                pReg->reset();

                // Remove objeto da lista de registros ativos
                if ( pVectActiveBuf->remove(_ind, FALSE) )
                {
                    ret = TRUE;

                    // Insere na lista de registros deletados
                    if (  pVectDeletedBuf->get_nelem() >= maxlines ||
                         !pVectDeletedBuf->insert(pReg) )
                    {
                        // Erro ao adicionar registro => apaga registro
                        delete pReg;
                    }
                }
            }
        }

        return ret;
    }

    BOOL SaveData(UINT _ind, LPVOID _pdataBuf, ULONG _szdata)
    {
        BOOL ret;
        T  *pReg;

        ret = FALSE;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
                ret = pReg->SaveData(_pdataBuf, _szdata);
        }

        return ret;
    }

    BOOL GetData(UINT _ind, LPVOID _pdataBuf, ULONG *_szdataBuf)
    {
        BOOL ret;
        T  *pReg;


        ret  = FALSE;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
                ret = pReg->GetData(_pdataBuf, _szdataBuf);
        }

        return ret;
    }

    ULONG GetBytesLeft(UINT _ind)
    {
        ULONG ret;
        T  *pReg;

        ret = 0;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
                ret = pReg->GetBytesLeft();
        }

        return ret;
    }

    BOOL IsEnable(UINT _ind)
    {
        BOOL ret;
        T *pReg;

        ret = FALSE;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
                ret = pReg->IsEnable();
        }

        return ret;
    }

    BOOL SetEnable(UINT _ind, BOOL _f)
    {
        BOOL ret;
        T *pReg;

        ret = FALSE;

        if (flOk)
        {
            pReg = pVectActiveBuf->get_elem(_ind);

            if (pReg)
                ret = pReg->SetEnable(_f);
        }

        return ret;
    }
};


template <class K, class T> class BufSVectorT
{

private:

    BufSVect<K, T>  *pVectActiveBuf;
    BufVect<T>      *pVectDeletedBuf;

    UINT  maxlines;     // Numero maximo de elementos do vetor
    ULONG maxblen;      // Tamanho maximo de cada buffer

    BOOL  flOk;


public:

    BufSVectorT(UINT _maxlines, ULONG _maxblen)
    {
        pVectActiveBuf  = NULL;
        pVectDeletedBuf = NULL;

        flOk     = FALSE;
        maxlines = 0;
        maxblen  = 0;

        if (_maxlines && _maxblen)
        {
            pVectActiveBuf  = new BufSVect<K, T>(_maxlines);
            pVectDeletedBuf = new BufVect<T>(_maxlines);

            if (pVectActiveBuf && pVectDeletedBuf)
            {
                flOk     = TRUE;
                maxlines = _maxlines;
                maxblen  = _maxblen;
            }
        }
    }


   ~BufSVectorT (void)
    {
        if (pVectActiveBuf)
            delete pVectActiveBuf;

        if (pVectDeletedBuf)
            delete pVectDeletedBuf;
    }


    BOOL IsOk  (void) { return flOk; }

    void reset (void)
    {
        if (pVectActiveBuf)
            pVectActiveBuf->reset();

        if (pVectDeletedBuf)
            pVectDeletedBuf->reset();
    }


    T * search(K _id)
    {
        UINT ind;

        T *pReg;


        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (!ind)
                pReg = NULL;
        }

        return pReg;
    }


    T * first(void)
    {
        T *pReg;


        pReg = NULL;

        if (flOk)
            pReg = pVectActiveBuf->first(); 

        return pReg;
    }


    T * next(void)
    {
        T *pReg;


        pReg = NULL;

        if (flOk)
            pReg = pVectActiveBuf->next(); 

        return pReg;
    }


    UINT append(K _id)
    {
        UINT ret;

        T *pReg;


        ret = 0;

        if (flOk)
        {
            if ( pVectActiveBuf->get_nitens()<maxlines && !pVectActiveBuf->search(_id) )
            {
                // Verifica se ha itens no vetor de deletados
                pReg = pVectDeletedBuf->popLast();

                // Se nao houver => cria novo registro
                if (!pReg)
                {
                    pReg = new T(maxblen);

                    if (!pReg->IsOk())
                    {
                        delete pReg;
                        pReg = NULL;
                    }
                }

                if (pReg)
                {
                    // O Objeto agora eh resetado no instante de sua desativacao
                    //pReg->reset();

                    // Adiciona na lista de registros ativos
                    ret = pVectActiveBuf->insert(_id, pReg);

                    if (!ret)
                    {
                        // Erro ao adicionar registro => adiciona na 
                        // lista de deletados
                        if (  pVectDeletedBuf->get_nelem() >= maxlines ||
                             !pVectDeletedBuf->insert(pReg) )
                        {
                            // Erro ao adicionar registro => apaga registro
                            delete pReg;
                        }
                    }
                }
            }
        }

        return ret;
    }


    BOOL erase(K _id)
    {
        BOOL ret;
        UINT ind;
        T  *pReg;

        ret  = FALSE;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
            {
                // Reseta objeto para possivel utilizacao posterior
                pReg->reset();

                // Remove objeto da lista de registros ativos
                if ( pVectActiveBuf->eraseIndex(ind, FALSE) )
                {
                    ret = TRUE;

                    // Insere na lista de registros deletados
                    if (  pVectDeletedBuf->get_nelem() >= maxlines ||
                         !pVectDeletedBuf->insert(pReg) )
                    {
                        // Erro ao adicionar registro => apaga registro
                        delete pReg;
                    }
                }
            }
        }

        return ret;
    }


    BOOL SaveData(K _id, LPVOID _pdataBuf, ULONG _szdata)
    {
        BOOL ret;
        UINT ind;

        T  *pReg;


        ret  = FALSE;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
                ret = pReg->SaveData(_pdataBuf, _szdata);
        }

        return ret;
    }


    BOOL GetData(K _id, LPVOID _pdataBuf, ULONG *_szdataBuf)
    {
        BOOL ret;
        UINT ind;

        T  *pReg;


        ret  = FALSE;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
                ret = pReg->GetData(_pdataBuf, _szdataBuf);
        }

        return ret;
    }

    ULONG GetBytesLeft(K _id)
    {
        ULONG ret;
        UINT  ind;

        T  *pReg;


        ret  = 0;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
                ret = pReg->GetBytesLeft();
        }

        return ret;
    }

    BOOL IsEnable(K _id)
    {
        BOOL ret;
        UINT ind;

        T  *pReg;


        ret  = FALSE;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
                ret = pReg->IsEnable();
        }

        return ret;
    }

    BOOL SetEnable(K _id, BOOL _f)
    {
        BOOL ret;
        UINT ind;
        T  *pReg;

        ret  = FALSE;
        pReg = NULL;

        if (flOk)
        {
            ind = pVectActiveBuf->search(_id, &pReg);

            if (ind && pReg)
                ret = pReg->SetEnable(_f);
        }

        return ret;
    }
};


#endif  // _BUFVECTORT_STR



