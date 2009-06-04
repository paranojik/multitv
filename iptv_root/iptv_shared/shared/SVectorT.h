#ifndef _SVECTORT_STR
#define _SVECTORT_STR

    

#define  STEP_ALLOC  100


template <class K, class T> class SVectorT_reg
{
    public:

    K key;
    T obj;
};


/*
Class definition

template <class K, class T> class SVectorT
{
    private:

    SVectorT_reg<K, T> *ptable;

    BOOL flKillRegs,
         flAscend;                  // indica se o vetor estah sendo percorrido (first() / next())
                                    // de forma ascendente ou descendente (last() / prev())

    T    objNull;
    K    keyNull;

    UINT wInitSize,
         nelemNotNull,
         nmax_elem,
         pos;                        // indice p/ prox. posicao do ptr de consulta


    public:

    virtual void kill_elem(K key, T pobj) { }
    virtual int compare(K elem1, K elem2);

    SVectorT (int _flKill=true, UINT _wInitSize=STEP_ALLOC);
   ~SVectorT (void);

    UINT get_nitens (void) { return(nelemNotNull); }

    void init       (BOOL _flKill, UINT _wInitSize);
    void kill       (void);
    void reset      (void);
    void set_pos    (UINT new_ind, int _flAscend=-1);
    UINT get_pos    (void);
    UINT insert     (K key, T obj);
    BOOL update     (K key, T obj);
    BOOL updateIndex(UINT ind, T obj);

    T    get_elem   (UINT ind, K *pKey=NULL);
    T    first      (K *pKey=NULL);
    T    last       (K *pKey=NULL);
    T    next       (K *pKey=NULL);
    T    prev       (K *pKey=NULL);
    BOOL end        (void);
    UINT search     (K key, T *pobj=NULL);
    BOOL erase      (K key, BOOL _flKill=TRUE);
    BOOL eraseIndex (UINT ind, BOOL _flKill=TRUE);
    BOOL eraseCur   (BOOL _flKill=TRUE);

    T   operator[]  (UINT ind) const;
    T  &operator[]  (UINT ind);
};
*/


template <class K, class T> class SVectorT
{
    private:

    SVectorT_reg<K, T> *ptable;

    BOOL flKillRegs,
         flAscend;

    T    objNull;
    K    keyNull;

    UINT wInitSize,
         nelemNotNull,
         nmax_elem,
         pos;       // indice p/ prox. posicao do ptr de consulta


    virtual void kill_elem(K key, T pobj)
    {
    }


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


    void init(BOOL _flKill, UINT _wInitSize)
    {
        long tsize;

        wInitSize      = _wInitSize;
        flKillRegs     = _flKill;

        ptable         = NULL;
        pos            = 1;
        nelemNotNull   = 0;
        nmax_elem      = 0;
        flAscend       = TRUE;

        memset(&objNull, '\0', sizeof(T));
        memset(&keyNull, '\0', sizeof(K));

        if (wInitSize)
        {
            tsize  = wInitSize * sizeof(SVectorT_reg<K, T>);
            ptable = (SVectorT_reg<K, T> *) calloc(1, tsize);

            if (ptable)
                nmax_elem = wInitSize;
        }
    }


    public:


    SVectorT (BOOL _flKill=TRUE, UINT _wInitSize=STEP_ALLOC)
    {
        init(_flKill, _wInitSize);
    }


    virtual ~SVectorT (void)
    {
        kill();
    }


    UINT get_nitens(void)
    {
        return(nelemNotNull);
    }

    void kill(void)
    {
        UINT i;

        if (ptable)
        {
            if (flKillRegs)
            {
                for (i=0; i<nelemNotNull; i++)
                {
                    kill_elem(ptable[i].key, ptable[i].obj);
                }
            }

            free(ptable);

            ptable         = NULL;
            pos            = 1;
            nelemNotNull   = 0;
            nmax_elem      = 0;
            flAscend       = TRUE;
        }
    }

    void reset(void)
    {
        kill();
        init(flKillRegs, wInitSize);
    }


    /*
        Retorna indice do objeto inserido no vetor (1..n)
        Se erro retorna 0
    */

    UINT insert(K key, T obj)
    {
        BOOL  flOk;
        UINT  ind,
              ibegin,
              imid,
              iend;

        long  tsize;

        SVectorT_reg<K, T> *pnew_table;


        flOk = false;
        ind  = 0;

        if (ptable)
        {
            // Verifica se elemento eh o primeiro a ser incluido ou
            // se o mesmo deve ser incluido no inicio do vetor
            if ( nelemNotNull && compare(key, ptable[0].key) >=0 )
            {
                ibegin = 0;
                iend   = nelemNotNull-1;

                // Verifica se o elemento deve ser incluido no fim do vetor
                if ( compare(key, ptable[iend].key) >=0 )
                    ind = iend + 1;
                else
                {
                    // Elemento estah entre ibegin e iend
                    while (iend - ibegin > 1)
                    {
                        // Compara com elemento entre ibegin e iend
                        imid = (iend + ibegin) / 2;

                        if ( compare(key, ptable[imid].key) >=0 )
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
            if (nelemNotNull >= nmax_elem)
            {
                tsize = (nmax_elem + STEP_ALLOC) * sizeof(SVectorT_reg<K, T>);

                pnew_table = (SVectorT_reg<K, T> *) realloc(ptable, tsize);

                if (pnew_table)
                {
                    void *pBegin;
                    long  sz;

                    pBegin = &pnew_table[nmax_elem];
                    sz     = STEP_ALLOC * sizeof(SVectorT_reg<K, T>);

                    memset(pBegin, '\0', sz);

                    flOk        = true;
                    nmax_elem  += STEP_ALLOC;
                    ptable      = pnew_table;
                }
            }
            else
                flOk = true;

            if (flOk)
            {
                // Se necessario abre espaco para a insercao do novo item
                if ( ind < nelemNotNull )
                {
                    SVectorT_reg<K, T> *porig,
                                       *pdest;
                         
                    porig = &ptable[ind];
                    pdest = &ptable[ind+1];

                    tsize  = nelemNotNull - ind;
                    tsize *= sizeof(SVectorT_reg<K, T>);

                    memmove( pdest, porig, tsize );
                }

                ptable[ind].key = key;
                ptable[ind].obj = obj;

                ind++;      // Apenas para que o retorno da funcao seja (1..n)
                nelemNotNull++;
            }
            else
                ind = 0;

            if (ind && ind<pos)
                pos++;
        }

        return ind;
    }


    /*
       Retorna indice do elemento no vetor (1..nelem)
       Caso nao seja encontrado retorna 0
    */

    UINT search(K key, T  *pobj=NULL)
    {
        T     obj;

        BOOL  flOk;
        UINT  ind,
              ibegin,
              imid,
              iend;



        obj  = objNull;

        flOk = false;
        ind  = 0;

        if (pobj)
            *pobj = objNull;

        if (ptable)
        {
            if ( nelemNotNull )
            {
                // Verifica se elemento eh o primeiro do vetor
                if ( compare(key, ptable[0].key) ==0 )
                {
                    obj = ptable[0].obj;
                    ind = 1;
                }
                // Verifica se elemento eh o ultimo do vetor
                else if ( compare(key, ptable[nelemNotNull-1].key) ==0 )
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

                        if ( compare(key, ptable[imid].key) ==0 )
                        {
                            obj = ptable[imid].obj;
                            ind = imid + 1;
                            break;
                        }
                        if ( compare(key, ptable[imid].key) > 0 )
                            ibegin = imid;
                        else
                            iend   = imid;
                    }
                }
                
                if (pobj)
                    *pobj = obj;
            }
        }

        return ind;
    }


    BOOL updateIndex(UINT ind, T  obj)
    {
        BOOL ret;


        ret = FALSE;

        if (ind && ind<=nelemNotNull)
        {
            if (obj != objNull)
            {
                BOOL flKill;
                K    key;
                T    oldobj;

                flKill = FALSE;
                key    = keyNull;
                oldobj = objNull;

                if (flKillRegs && ptable[ind-1].obj!=objNull)
                {
                    flKill = TRUE;
                    key    = ptable[ind-1].key;
                    oldobj = ptable[ind-1].obj;
                }

                ptable[ind-1].obj = obj;
                ret = TRUE;

                if (flKill)
                    kill_elem(key, oldobj);
            }
            else
                ret = eraseIndex(ind);
        }

        return ret;
    }


    BOOL update(K key, T  obj)
    {
        BOOL ret;
        UINT ind;


        ret = FALSE;
        ind = search(key);

        if (ind)
            ret = updateIndex(ind, obj);

        return ret;
    }


    BOOL eraseIndex(UINT ind, BOOL _flKill=TRUE)
    {
        BOOL  ret;


        ret = FALSE;

        if (ind && ind<=nelemNotNull)
        {
            K key;
            T obj;

            key = keyNull;
            obj = objNull;

            if (flKillRegs && _flKill)
            {
                key = ptable[ind-1].key;
                obj = ptable[ind-1].obj;
            }
            else
                _flKill = FALSE;

            if (ind < nelemNotNull)
            {
                long tsize;

                SVectorT_reg<K, T> *porig,
                                   *pdest;
                     
                tsize  = nelemNotNull - ind;
                tsize *= sizeof(SVectorT_reg<K, T>);

                if (ind<pos && pos>1)
                    pos--;

                ind--;

                porig = &ptable[ind+1];
                pdest = &ptable[ind];

                memmove( pdest, porig, tsize );
            }

            memset( &ptable[nelemNotNull-1], '\0', sizeof(SVectorT_reg<K, T>) );

            nelemNotNull--;

            ret = TRUE;

            if (_flKill)
                kill_elem(key, obj);
        }

        return ret;
    }


    BOOL eraseCur(BOOL _flKill=TRUE)
    {
        BOOL ret;
        UINT ind;

        ind = get_pos();
        ret = eraseIndex(ind, _flKill);

        return ret;
    }


    T  get_elem(UINT ind, K *pKey=NULL)
    {
        T  obj;

        obj = objNull;

        if (pKey)
            *pKey = keyNull;

        if (ptable && ind && ind<=nelemNotNull)
        {
            obj = ptable[ind-1].obj;

            if (pKey)
                *pKey = ptable[ind-1].key;
        }

        return obj;
    }


    T  first(K *pKey=NULL)
    {
        T obj;


        pos = 1;
        obj = next(pKey);

        return obj;
    }


    T  last(K *pKey=NULL)
    {
        T obj;


        pos = nelemNotNull;
        obj = prev(pKey);

        return obj;
    }


    T  next(K *pKey=NULL)
    {
        T  obj;


        if (pKey)
            *pKey = keyNull;

        flAscend = TRUE;

        obj = objNull;

        if (pos <= nelemNotNull)
        {
            obj = get_elem(pos, pKey);
            pos++;
        }

        return obj;
    }


    T  prev(K *pKey=NULL)
    {
        T  obj;


        if (pKey)
            *pKey = keyNull;

        flAscend = FALSE;

        obj = objNull;

        if (pos >= 1)
        {
            obj = get_elem(pos, pKey);
            pos--;
        }

        return obj;
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


    /*
        Posiciona ponteiro em new_ind => Proximo next() ou prev() irah retornar
        o registro seguinte (nao o de indice new_ind!)

        _flAscend: -1 => Mantem atual
                    0 => Descendente
                    1 => Ascendente
    */

    void set_pos(UINT new_ind, int _flAscend=-1)
    {
        if (_flAscend != -1)
            flAscend = _flAscend;

        if ( (flAscend && new_ind<=nelemNotNull) ||
             (!flAscend && new_ind>=1 && new_ind<=nelemNotNull+1) )
        {
            // O proximo next() ou prev() returnarah o item seguinte a new_pos
            pos = (flAscend ? new_ind+1:new_ind-1);
        }
        else
        {
            if (flAscend)
                pos = nelemNotNull + 1;     // O proximo next() retornarah objNull
            else
            {
                if (new_ind == 0)           // O proximo prev() retornarah objNull
                    pos = 0;
                else
                    pos = nelemNotNull;     // O proximo prev() retornarah o ultimo item
            }
        }
    }


    /*
        Retorna indice do registro corrente
    */

    UINT get_pos(void)
    {
        UINT ret;

        if (flAscend)
            ret = (pos>0 ? pos-1:0);
        else
            ret = pos + 1;

        return ret;
    }


    // Acesso ao item.
    T  operator[] (UINT ind) const
    {
        return (get_elem(ind));
    }


    // Retorna a referencia ao item.
    T &operator[] (UINT ind)
    {
        if (ptable && ind && ind<=nelemNotNull)
            return &ptable[ind-1].obj;
        else
            return &objNull;
    }


    BOOL erase(K key, BOOL _flKill=TRUE)
    {
        BOOL  ret;
        UINT  ind;


        ret = FALSE;

        ind = search(key);

        if (ind)
            ret = eraseIndex(ind, _flKill);

        return ret;
    }
};



#endif  // _SVECTORT



