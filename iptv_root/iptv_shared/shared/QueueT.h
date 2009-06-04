#ifndef _QUEUET_STR
#define _QUEUET_STR

    

#define  STEP_ALLOC  100


/*
Class definition

template <class T> class QueueT
{
   private:

      QueueT_reg<T>  *ptable;

      T    objnull;

      BOOL flKillRegs;

      UINT wInitSize,
           nelem,
           nmax_elem,
           pos;                        // indice p/ prox. posicao do ptr de consulta

   public:

      virtual void kill_elem(T  obj) {  }

      QueueT (int _flKill=true, UINT _wInitSize=STEP_ALLOC);
     ~QueueT (void);

      inline UINT     get_nitens(void) { return(nelem); }

      void init       (BOOL _flKill, UINT _wInitSize);
      void kill       (void);
      void reset      (void);
      void set_pos    (UINT new_pos);
      UINT get_pos    (void);
      UINT insert     (T obj, int priority=0);

      T    get_elem   (UINT ind);

      UINT popFirst   (T *pobj);
      UINT popLast    (T *pobj);
      UINT first      (T *pobj);
      UINT last       (T *pobj);
      UINT next       (T *pobj);
      UINT prev       (T *pobj);

      BOOL remove     (UINT ind);
      BOOL erase      (UINT ind);
      BOOL end        (void);

      T  operator[] (UINT ind) const
      T  &operator[] (UINT ind)
};
*/


template <class T> class QueueT_reg
{
    public:

    int priority;

    T obj;
};


template <class T> class QueueT
{
   private:

      QueueT_reg<T>  *ptable;

      T    objnull;

      BOOL flKillRegs;

      UINT wInitSize,
           nelem,
           nmax_elem,
           pos;             // indice p/ prox. posicao do ptr de consulta


      void init(BOOL _flKill, UINT _wInitSize)
      {
         wInitSize      = _wInitSize;
         flKillRegs     = _flKill;

         ptable         = NULL;
         pos            = 1;
         nelem          = 0;
         nmax_elem      = 0;

         memset(&objnull, '\0', sizeof(T));

         if (wInitSize)
         {
            ptable = (QueueT_reg<T> *) calloc(wInitSize, sizeof(QueueT_reg<T>));

            if (ptable)
                nmax_elem = wInitSize;
         }
      }

      void kill(void)
      {
         UINT i;

         if (ptable)
         {
            if (flKillRegs)
            {
               for (i=0; i<nelem; i++)
               {
                  kill_elem(ptable[i]);
               }
            }

            free(ptable);

            ptable         = NULL;
            pos            = 1;
            nelem          = 0;
            nmax_elem      = 0;
         }
      }


   public:

      virtual void kill_elem(T obj) {  }

      QueueT (BOOL _flKill=TRUE, UINT _wInitSize=STEP_ALLOC)
      {
         init(_flKill, _wInitSize);
      }

     ~QueueT (void)
      {
         kill();
      }

      UINT get_nitens(void)
      {
         return(nelem);
      }

      void reset(void)
      {
         kill();
         init(flKillRegs, wInitSize);
      }


      /*
         Retorna indice do objeto inserido no vetor
         Se erro retorna 0
      */

      UINT insert(T obj, int priority)
      {
         BOOL  flOk;
         UINT  ind;
         long  tsize;

         QueueT_reg<T>  *pnew_table;


         flOk = FALSE;

         if (ptable)
         {
            if (priority)
            {
               ind = 0;
  
               // Hierarquia de prioridades: 1 > 2 > .. > n
               while (ind < nelem)
               {
                  if (ptable[ind].priority > priority)
                     break;
                  else
                     ind++;
               }
            }
            else
               ind = nelem;

            if (nelem >= nmax_elem)
            {
               tsize = (nmax_elem + STEP_ALLOC) * sizeof(QueueT_reg<T>);

               pnew_table = (QueueT_reg<T> *) realloc(ptable, tsize);

               if (pnew_table)
               {
                  void *pBegin;
                  long  sz;

                  pBegin = &pnew_table[nmax_elem];
                  sz     = STEP_ALLOC * sizeof(QueueT_reg<T>);

                  memset(pBegin, '\0', sz);

                  flOk        = TRUE;
                  nmax_elem  += STEP_ALLOC;
                  ptable      = pnew_table;
               }
            }
            else
               flOk = TRUE;

            if (flOk)
            {
               if (ind < nelem)
               {
                  // Abre espaco para a inclusao do objeto de acordo com
                  // sua prioridade
                  tsize = (nelem - ind) * sizeof(QueueT_reg<T>);
                  memmove(&ptable[ind+1], &ptable[ind], tsize);
               }

               ptable[ind].priority = priority;
               ptable[ind].obj      = obj;

               nelem++;
            }
         }

         if (flOk)
            ind++;
         else
            ind = 0;

         return ind;
      }


      T  get_elem(UINT ind)
      {
         T  obj;

         obj = objnull;

         if (ptable && ind && ind<=nelem)
            obj = ptable[ind-1].obj;

         return obj;
      }


      /*
         Remove da estrutura o elemento de indice ind
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
      */

      BOOL remove(UINT ind)
      {
         BOOL ret;

         ret = FALSE;

         if (ind && ind<=nelem)
         {
            if (ind < nelem)
            {
               long sz;

               sz = (nelem - ind) * sizeof(QueueT_reg<T>);

               if (ind<pos && pos>1)
                  pos--;

               ind--;
               memmove(&ptable[ind], &ptable[ind+1], sz);
            }

            ptable[nelem-1].obj      = objnull;
            ptable[nelem-1].priority = 0;
            nelem--;

            ret = TRUE;
         }

         return ret;
      }


      /*
         Retorna o indice do 1o elemento do Vetor, removendo-o da estrutura
         Caso nao haja elementos retorna 0
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
         Permite a implementacao de fila (FIFO)
      */

      UINT popFirst(T *pobj)
      {
         T     obj;
         UINT  CurInd;

         CurInd = first(pobj);

         if (CurInd)
            remove(CurInd);

         return CurInd;
      }


      /*
         Retorna o indice do ultimo elemento do Vetor, removendo-o da estrutura
         Caso nao haja elementos retorna 0
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
         Permite a implementacao de pilha (Last In First Out)
      */

      UINT popLast(T *pobj)
      {
         T     obj;
         UINT  CurInd;

         CurInd = last(pobj);

         if (CurInd)
            remove(CurInd);

         return CurInd;
      }


      UINT first(T *pobj)
      {
         UINT ind;

         pos = 1;
         ind = next(pobj);

         return ind;
      }


      UINT last(T *pobj)
      {
         UINT ind;

         pos = nelem;
         ind = prev(pobj);

         return ind;
      }


      UINT next(T *pobj)
      {
         UINT ind;
         T    obj;

         if (!end())
         {
            ind = pos;
            obj = get_elem(pos);
            pos++;
         }
         else
         {
            ind = 0;
            obj = objnull;
         }

         if (pobj)
            *pobj = obj;

         return ind;
      }


      UINT prev(T *pobj)
      {
         UINT ind;
         T    obj;

         if (pos > 0)
         {
            ind = pos;
            obj = get_elem(pos);
            pos--;
         }
         else
         {
            ind = 0;
            obj = objnull;
         }

         if (pobj)
            *pobj = obj;

         return ind;
      }


      BOOL end(void)
      {
         BOOL ret;

         if (pos > nelem)
            ret = TRUE;
         else
            ret = FALSE;

         return ret;
      }


      void set_pos(UINT new_pos)
      {
         pos = new_pos + 1;
      }


      UINT get_pos(void)
      {
         return (pos - 1);
      }


      // Acesso ao item.
      T  operator[] (UINT ind) const
      {
         return (get_elem(ind));
      }


      // Retorna a referencia ao item.
      T  &operator[] (UINT ind)
      {
         if (ptable && ind && ind<=nelem)
            return ptable[ind-1];
         else
            return pnull;
      }


      /*
         Deleta e remove da estrutura o elemento de indice ind
      */

      BOOL erase(UINT ind)
      {
         BOOL  ret;
         T  obj;

         ret = FALSE;

         if (ind && ind<=nelem)
         {
            obj = get_elem(ind);

            remove(ind);
            kill_elem(obj);

            ret = TRUE;
         }

         return ret;
      }
};



#endif  // _QUEUET_STR



