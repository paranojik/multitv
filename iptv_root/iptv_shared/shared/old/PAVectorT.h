#ifndef _PAVECTORT_STR
#define _PAVECTORT_STR

    
#include "alloc_ct.h"


#define  STEP_ALLOC  100


template <class K> class PAVectorT_reg
{
    public:

    K key;
    UINT sz;
    char obj[1];
};


/*
Class definition

template <class K> class PAVectorT
{
    private:

    HGLOBAL htable;
    PAVectorT_reg<K> *ptable;

    UINT wInitSize,
         maxobjsize,
         nelemNotNull,
         nmax_elem,
         pos;                        // indice p/ prox. posicao do ptr de consulta


    PAVectorT_reg<K> * GetRegAddr(UINT ind);
    K GetKey(UINT ind);

    public:

    virtual int compare(K elem1, K elem2);

    PAVectorT (UINT _wInitSize=STEP_ALLOC);
   ~PAVectorT (void);

    inline UINT    get_nitens        (void) { return(nelemNotNull); }

    void init    (UINT _objsize, UINT _wInitSize);
    void kill    (void);
    void reset   (void);
    void set_pos (UINT new_pos);
    UINT get_pos (void);
    UINT insert  (K key, void *pobj, UINT _size);

    void * get_elem (UINT ind, UINT *_size=NULL);
    void * first    (UINT *_size=NULL);
    void * next     (UINT *_size=NULL);
    BOOL end        (void);
    UINT search     (K key, void **pobj=NULL, UINT *_size=NULL);
    BOOL erase      (K key);
    BOOL eraseIndex (UINT ind);

    T   operator[] (UINT ind) const;
    T  &operator[] (UINT ind);
};
*/


template <class K> class PAVectorT
{
    private:

    HGLOBAL  htable;
    PAVectorT_reg<K> *ptable;

    UINT wInitSize,
         maxobjsize,
         regsize,
         nelemNotNull,
         nmax_elem,
         pos;                        // indice p/ prox. posicao do ptr de consulta


    /*
        Retorno da funcao:

        < 0 elem1 less than elem2 
        0 elem1 equivalent to elem2 
        > 0 elem1 greater than elem2 
    */

    virtual int compare(K elem1, K elem2)
    {
        int ret;

        if (elem1 < elem2)
            ret = -1;
        else if (elem1 > elem2)
            ret = 1;
        else
            ret = 0;

        return ret;
    }


    void init(UINT _objsize, UINT _wInitSize)
    {
        long tsize;

        wInitSize      = _wInitSize;

        ptable         = NULL;
        pos            = 1;
        nelemNotNull   = 0;
        nmax_elem      = 0;
        maxobjsize     = 0;
        regsize        = 0;

        if (wInitSize && _objsize)
        {
            regsize = sizeof(K) + sizeof(UINT) + _objsize;
            tsize   = wInitSize * regsize;
            htable  = GAllocPtr(tsize, (void **) &ptable);

            if (ptable)
            {
                nmax_elem  = wInitSize;
                maxobjsize = _objsize;
            }
        }
    }


    void kill(void)
    {
        if (htable)
        {
            GFreePtr(htable);

            htable         = NULL;
            ptable         = NULL;
            pos            = 1;
            nelemNotNull   = 0;
            nmax_elem      = 0;
        }
    }


    public:


    inline UINT get_nitens(void) { return(nelemNotNull); }


    PAVectorT (UINT _objsize, UINT _wInitSize=STEP_ALLOC)
    {
        init(_objsize, _wInitSize);
    }


   ~PAVectorT (void)
    {
        kill();
    }


    void reset(void)
    {
        kill();
        init(maxobjsize, wInitSize);
    }


    PAVectorT_reg<K> * GetRegAddr(UINT ind)
    {
        PAVectorT_reg<K> *preg;
        ULONG offset;

        preg = NULL;

        if (ptable && ind>0 && ind<=nelemNotNull)
        {
            ind--;

            offset = ind * regsize;
            preg   = (PAVectorT_reg<K> *) (((char *) ptable) + offset);
        }

        return preg;
    }


    K GetKey(UINT ind)
    {
        K key;
        PAVectorT_reg<K> *preg;

        preg = GetRegAddr(ind);

        if (preg)
            key = preg->key;
        else
            key = NULL;

        return key;
    }


    /*
        Retorna indice do objeto inserido no vetor (1..n)
        Se erro retorna 0
    */

    UINT insert(K key, void *pobj, UINT _size)
    {
        BOOL  flOk;
        UINT  ind,
              ibegin,
              imid,
              iend;

        long  tsize;

        HGLOBAL  hnew_table;


        flOk = false;
        ind  = 0;

        if (ptable && pobj && _size && _size<=maxobjsize)
        {
            // Verifica se elemento eh o primeiro a ser incluido ou
            // se o mesmo deve ser incluido no inicio do vetor
            if ( nelemNotNull && compare(key, GetKey(0)) >=0 )
            {
                ibegin = 0;
                iend   = nelemNotNull-1;

                // Verifica se o elemento deve ser incluido no fim do vetor
                if ( compare(key, GetKey(iend)) >=0 )
                    ind = iend + 1;
                else
                {
                    // Elemento estah entre ibegin e iend
                    while (iend - ibegin > 1)
                    {
                        // Compara com elemento entre ibegin e iend
                        imid = (iend + ibegin) / 2;

                        if ( compare(key, GetKey(imid)) >=0 )
                            ibegin = imid;
                        else
                            iend   = imid;
                    }

                    // Elemento deve ser incluido entre ibegin e iend
                    ind = iend;
                }
            }
            else
                ind = 0;

            // Verifica se a area de memoria eh capaz de armazenar o novo item
            // Caso contrario a mesma eh realocada
            if (ind >= nmax_elem)
            {
                tsize = (nmax_elem + STEP_ALLOC) * regsize;

                GlobalUnlock_ap(htable);
                hnew_table = GlobalReAlloc_ap(htable, tsize);

                if (hnew_table)
                {
                    flOk        = true;
                    nmax_elem  += STEP_ALLOC;
                    htable      = hnew_table;
                }

                ptable = (PAVectorT_reg<K> *) GlobalLock_ap(htable);
            }
            else
                flOk = true;

            if (flOk)
            {
                PAVectorT_reg<K> *porig,
                                 *pdest;

                porig = GetRegAddr(ind);

                // Se necessario abre espaco para a insercao do novo item
                if ( ind < nelemNotNull )
                {
                    pdest = GetRegAddr(ind+1);

                    tsize  = nelemNotNull - ind;
                    tsize *= sizeof(regsize);

                    memmove( pdest, porig, tsize );
                }

                porig->key = key;
                memmove(porig->obj, pobj, _size);
                porig->sz  = _size;

                ind++;      // Apenas para que o retorno da funcao seja (1..n)
                nelemNotNull++;
            }
            else
                ind = 0;
        }

        return ind;
    }



    /*
       Retorna indice do elemento no vetor (1..nelem),
       o ponteiro para a area de dados e seu tamanho
       Caso nao seja encontrado retorna 0
    */

    UINT search(K key, void **pobj=NULL, UINT *_size=NULL)
    {
        BOOL  flOk;
        UINT  ind,
              ibegin,
              imid,
              iend;



        flOk = false;
        ind  = 0;

        if (pobj)
            *pobj = NULL;

        if (_size)
            *_size = 0;

        if (ptable)
        {
            if ( nelemNotNull )
            {
                // Verifica se elemento eh o primeiro do vetor
                if ( compare(key, GetKey(0)) == 0 )
                {
                    obj = ptable[0].obj;
                    ind = 1;
                }
                // Verifica se elemento eh o ultimo do vetor
                else if ( compare(key, GetKey(nelemNotNull-1)) == 0 )
                {
                    obj = ptable[nelemNotNull-1].obj;
                    ind = nelemNotNull;
                }
                else if (nelemNotNull > 2)
                {
                    ibegin = 0;
                    iend   = nelemNotNull-1;

                    // Elemento estah entre ibegin e iend
                    while (iend-ibegin > 1)
                    {
                        // Compara com elemento entre ibegin e iend
                        imid = (iend + ibegin) / 2;

                        if ( compare(key, GetKey(imid)) == 0 )
                        {
                            obj = ptable[imid].obj;
                            ind = imid + 1;
                            break;
                        }
                        if ( compare(key, GetKey(imid)) > 0 )
                            ibegin = imid;
                        else
                            iend   = imid;
                    }
                }

                if (ind)
                {
                    if (pobj)
                    {
                        PAVectorT_reg<K> *preg;

                        preg = GetRegAddr(ind);

                        if (preg)
                        {
                            *pobj = preg->obj;

                            if (_size)
                                *size = preg->sz;
                        }
                    }
                }
            }
        }

        return ind;
    }


    BOOL eraseIndex(UINT ind)
    {
        BOOL  ret;


        ret = FALSE;

        if (ind && ind<=nelemNotNull)
        {
            if (ind < nelemNotNull)
            {
                long tsize;

                PAVectorT_reg<K> *porig,
                                 *pdest,
                                 *plast;
                     
                tsize  = nelemNotNull - ind;
                tsize *= regsize;

                ind--;

                porig = GetRegAddr(ind+1);
                pdest = GetRegAddr(ind);

                memmove( pdest, porig, tsize );
            }

            plast = GetRegAddr(nelemNotNull-1);

            memset( plast, '\0', regsize );

            nelemNotNull--;

            ret = TRUE;
        }

        return ret;
    }


    void * get_elem(UINT ind, UINT *_size=NULL)
    {
        void *pobj;
        PAVectorT_reg<K> *preg;


        if (_size)
            *_size = 0;

        preg = GetRegAddr(ind);

        if (preg)
        {
            pobj = preg->obj;

            if (_size)
                *_size = preg->sz;
        }
        else
            pobj = NULL;

        return pobj;
    }


    void * first(UINT *_size=NULL)
    {
        void *pobj;

        pos  = 1;
        pobj = next(_size);

        return pobj;
    }


    void * next(UINT *_size=NULL)
    {
        void *pobj;

        pobj = NULL;

        if (pos <= nelemNotNull)
        {
            pobj = get_elem(pos, _size);
            pos++;
        }

        return pobj;
    }


    BOOL end(void)
    {
        BOOL ret;

        if (pos > nelemNotNull)
            ret = TRUE;
        else
            ret = FALSE;

        return ret;
    }


    void set_pos(UINT new_pos)
    {
        pos = new_pos;
    }


    inline UINT get_pos(void)
    {
        return pos;
    }


    BOOL erase(K key)
    {
        BOOL  ret;
        UINT  ind;


        ret = FALSE;

        ind = search(key);

        if (ind)
           ret = eraseIndex(ind);

        return ret;
    }
};



#endif  // _PAVECTORT



