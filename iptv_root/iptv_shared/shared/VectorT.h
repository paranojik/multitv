#ifndef _VECTORT_STR
#define _VECTORT_STR

    

#define  VECT_STEP_ALLOC  100


/*
Class definition

template <class T> class VectorT
{
   private:

      T   *ptable;

      T    objNull;

      BOOL flKillRegs;

      UINT wInitSize,
           nelem,
           nelemNotNull,
           nmax_elem,
           pos;                        // indice p/ prox. posicao do ptr de consulta

      void operator=   (T obj)  { }  // nao pode ser usada; deve-se utilizar insert()

   public:

      virtual void kill_elem(T obj) { }

      VectorT (int _flKill=TRUE, UINT _wInitSize=VECT_STEP_ALLOC);
      virtual ~VectorT (void);

      inline UINT    get_nelem        (void) { return(nelem); }
      inline UINT    get_nmaxelem     (void) { return(nmax_elem); }
      inline UINT    get_nelemNotNull (void) { return(nelemNotNull); }
      inline UINT    get_nitens       (void) { return(nelemNotNull); }
      inline void *  get_table        (void) { return(ptable); }

      void init       (BOOL _flKill, UINT _wInitSize);
      void kill       (void);
      void reset      (void);
      void set_pos    (UINT new_pos);
      UINT get_pos    (void);
      UINT insert     (T obj, UINT indForced=0, BOOL flSearchEmpSlot=FALSE);

      T    popFirst   (void);
      T    popLast    (void);

      T    get_elem   (UINT ind);
      T    first      (void);
      T    last       (void);
      T    next       (void);
      T    prev       (void);
      BOOL end        (void);
      UINT search     (T obj);
      BOOL remove     (UINT ind, BOOL _flKill);
      BOOL erase      (T obj);
      BOOL eraseIndex (UINT ind);

      T    operator[] (UINT ind) const;
      T   &operator[] (UINT ind);
};
*/


template <class T> class VectorT
{
   private:

      T     *ptable;

      T      objNull;

      BOOL flKillRegs;

      UINT wInitSize,
           nelem,
           nelemNotNull,
           nmax_elem,
           pos;                        // indice p/ prox. posicao do ptr de consulta

      void operator=   (T obj);  // nao pode ser usada; deve-se utilizar insert()

      void init(BOOL _flKill, UINT _wInitSize)
      {
         wInitSize      = _wInitSize;
         flKillRegs     = _flKill;

         ptable         = NULL;
         pos            = 1;
         nelem          = 0;
         nelemNotNull   = 0;
         nmax_elem      = 0;

         memset(&objNull, '\0', sizeof(T));

         if (wInitSize)
         {
            ptable = (T *) calloc(wInitSize, sizeof(T)); 

            if (ptable)
               nmax_elem = wInitSize;
         }
      }


   public:

      virtual void kill_elem(T obj) {  }

      VectorT (BOOL _flKill=TRUE, UINT _wInitSize=VECT_STEP_ALLOC)
      {
         init(_flKill, _wInitSize);
      }

      virtual ~VectorT (void)
      {
         kill();
      }

      inline UINT    get_nelem        (void) { return(nelem); }
      inline UINT    get_nmaxelem     (void) { return(nmax_elem); }
      inline UINT    get_nelemNotNull (void) { return(nelemNotNull); }
      inline UINT    get_nitens       (void) { return(nelemNotNull); }
      inline void *  get_table        (void) { return(ptable); }


      void kill(void)
      {
         UINT i;

         if (ptable)
         {
            if (flKillRegs)
            {
               for (i=0; i<nelem; i++)
               {
                  if (ptable[i]!=objNull)
                     kill_elem(ptable[i]);
               }
            }

            free(ptable);

            ptable         = NULL;
            pos            = 1;
            nelem          = 0;
            nelemNotNull   = 0;
            nmax_elem      = 0;
         }
      }

      void reset(void)
      {
         UINT i;

         //kill();
         //init(flKillRegs, wInitSize);

         if (ptable)
         {
            for (i=0; i<nelem; i++)
            {
               if (ptable[i]!=objNull)
               {
                  if (flKillRegs)
                     kill_elem(ptable[i]);

                  ptable[i] = objNull;
               }
            }

            pos            = 1;
            nelem          = 0;
            nelemNotNull   = 0;
         }
      }


      /*
         Retorna indice do objeto inserido no vetor
         Se erro retorna 0
      */

      UINT insert(T obj, UINT indForced=0, BOOL flSearchEmpSlot=FALSE)
      {
         BOOL  flOk;
         UINT  ind;
         long  tsize;

         T *pnew_table;


         flOk = FALSE;
         ind  = 0;

         if (ptable)
         {
            if (indForced)
               ind = indForced - 1;
            else
            {
               if (flSearchEmpSlot)
               {
                  ind = 0;
      
                  while (ind<nmax_elem)
                  {
                     if (ptable[ind] == objNull)
                        break;
                     else
                        ind++;
                  }
               }
               else
                  ind = nelem;
            }

            if (ind >= nmax_elem)
            {
               UINT nBlocks;

               nBlocks = ((ind-nmax_elem)/VECT_STEP_ALLOC) + 1;
         
               tsize = (nmax_elem + nBlocks*VECT_STEP_ALLOC) * sizeof(T);

               pnew_table = (T *) realloc(ptable, tsize);

               if (pnew_table)
               {
                  void *pBegin;
                  long  sz;

                  pBegin = &pnew_table[nmax_elem];
                  sz     = nBlocks * VECT_STEP_ALLOC * sizeof(T);

                  memset(pBegin, '\0', sz);

                  flOk        = TRUE;
                  nmax_elem  += nBlocks * VECT_STEP_ALLOC;
                  ptable      = pnew_table;
               }
            }
            else
               flOk = TRUE;

            if (flOk)
            {
               if (flKillRegs && ptable[ind]!=objNull)
               {
                  kill_elem(ptable[ind]);
                  nelemNotNull--;
               }

               ptable[ind] = obj;

               if (nelem <= ind)
                  nelem = ind + 1;

               if (obj!=objNull)
                  nelemNotNull++;
            }
         }

         if (flOk)
            ind++;
         else
            ind = 0;

         return(ind);
      }


      T  get_elem(UINT ind)
      {
         T   obj;

         obj = objNull;

         if (ptable && ind && ind<=nelem)
            obj = ptable[ind-1];

         return obj;
      }


      BOOL remove(UINT ind, BOOL _flKill)
      {
         BOOL  ret;
         long  sz;

         ret = FALSE;

         if (ptable && ind && ind<=nelem)
         {
            if (_flKill && flKillRegs && (ptable[ind-1]!=objNull))
               kill_elem(ptable[ind-1]);

            if (ind < nelem)
            {
               sz = (nelem - ind) * sizeof(T);
               memmove(&ptable[ind-1], &ptable[ind], sz);
            }

            ptable[nelem-1] = objNull;
            nelem--;
            nelemNotNull--;

            ret = TRUE;
         }

         return ret;
      }


      /*
         Retorna o 1o elemento do Vetor, removendo-o da estrutura
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
         Permite a implementacao de fila (FIFO)
      */

      T  popFirst(void)
      {
         T     obj;
         UINT  CurInd;

         obj = first();

         if (obj!=objNull)
         {
            CurInd = pos - 1;
            remove(CurInd, FALSE);
         }

         return obj;
      }


      /*
         Retorna o ultimo elemento do Vetor, removendo-o da estrutura
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
         Permite a implementacao de pilha (Last In First Out)
      */

      T  popLast(void)
      {
         T     obj;
         UINT  CurInd;

         obj = last();

         if (obj!=objNull)
         {
            CurInd = pos + 1;
            remove(CurInd, FALSE);
         }

         return obj;
      }


      T  first(void)
      {
         T obj;

         pos = 1;
         obj = next();

         return obj;
      }


      T  last(void)
      {
         T obj;

         pos = nelem;
         obj = prev();

         return obj;
      }


      T  next(void)
      {
         T obj;

         obj = objNull;

         while(!end() && obj==objNull)
         {
            obj = get_elem(pos);
            pos++;
         }

         return obj;
      }


      T  prev(void)
      {
         T obj;

         obj = objNull;

         while(pos>0 && obj==objNull)
         {
            obj = get_elem(pos);
            pos--;
         }

         return obj;
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
      T &operator[] (UINT ind)
      {
         if (ptable && ind && ind<=nelem)
            return ptable[ind-1];
         else
            return objNull;
      }


      /*
         Retorna indice do elemento no vetor (1..nelem)
         (considerando que nao ha duplicidade de objetos)
         Caso nao seja encontrado retorna 0
      */

      UINT search(T obj)
      {
         UINT ind;

         if (ptable && obj)
         {
            for (ind=0; ind<nelem; ind++)
            {
               if (ptable[ind]==obj)
                  break;
            }

            if (ind == nelem)
               ind = 0;
            else
               ind++;
         }
         else
            ind = 0;

         return ind;
      }


      BOOL erase(T obj)
      {
         BOOL  ret;
         UINT  ind;


         ret = FALSE;

         if (obj!=objNull)
         {
            ind = search(obj);

            if (ind && insert(objNull, ind))
               ret = TRUE;
         }

         return ret;
      }


      BOOL eraseIndex(UINT ind)
      {
         BOOL  ret;


         if (ind && (get_elem(ind)!=objNull) && insert(objNull, ind))
            ret = TRUE;
         else
            ret = FALSE;

         return ret;
      }
};



#endif  // _VECTORT_STR



