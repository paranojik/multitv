#ifndef _VECTOR_STR
#define _VECTOR_STR

    

#define  VECT_STEP_ALLOC  100


/*
Class definition

template <class T> class Vector
{
   private:

      T     **ptable;

      bool flKillRegs;

      UINT wInitSize,
           nelem,
           nelemNotNull,
           nmax_elem,
           pos;                        // indice p/ prox. posicao do ptr de consulta

      void operator=   (T *pobj)  { }  // nao pode ser usada; deve-se utilizar insert()

   public:

      virtual void kill_elem(T *pobj) { delete pobj; }

      Vector (int _flKill=true, UINT _wInitSize=VECT_STEP_ALLOC);
     ~Vector (void);

      inline UINT    get_nelem        (void) { return(nelem); }
      inline UINT    get_nelemNotNull (void) { return(nelemNotNull); }

      void init       (bool _flKill, UINT _wInitSize);
      void kill       (void);
      void reset      (void);
      void set_pos    (UINT new_pos);
      UINT get_pos    (void);
      UINT insert     (T *pobj, UINT indForced=0, bool flSearchEmpSlot=false);

      T *  popFirst   (void);
      T *  popLast    (void);

      T *  get_elem   (UINT ind);
      T *  first      (void);
      T *  last       (void);
      T *  next       (void);
      T *  prev       (void);
      bool end        (void);
      UINT search     (T *pobj);
      BOOL remove     (UINT ind, bool _flKill);
      bool erase      (T *pobj);
      bool erase      (UINT ind);

      T *  operator[] (UINT ind) const;
      T*  &operator[] (UINT ind);
};
*/


template <class T> class Vector
{
   private:

      T     **ptable;

      T      *pnull;

      bool flKillRegs;

      UINT wInitSize,
           nelem,
           nelemNotNull,
           nmax_elem,
           pos;                        // indice p/ prox. posicao do ptr de consulta

      void operator=   (T *pobj);  // nao pode ser usada; deve-se utilizar insert()

      void init(bool _flKill, UINT _wInitSize)
      {
         wInitSize      = _wInitSize;
         flKillRegs     = _flKill;

         ptable         = NULL;
         pnull          = NULL;
         pos            = 1;
         nelem          = 0;
         nelemNotNull   = 0;
         nmax_elem      = 0;

         if (wInitSize)
         {
            ptable = (T **) calloc(wInitSize, sizeof(T*)); 

            if (ptable)
               nmax_elem = wInitSize;
         }
      }


   public:

      virtual void kill_elem(T *pobj) { if (pobj) delete pobj; }

      Vector (bool _flKill=true, UINT _wInitSize=VECT_STEP_ALLOC)
      {
         init(_flKill, _wInitSize);
      }

     ~Vector (void)
      {
         kill();
      }

      inline UINT    get_nelem        (void) { return(nelem); }
      inline UINT    get_nelemNotNull (void) { return(nelemNotNull); }
      inline UINT    get_nitens       (void) { return(nelemNotNull); }


      void kill(void)
      {
         UINT i;

         if (ptable)
         {
            if (flKillRegs)
            {
               for (i=0; i<nelem; i++)
               {
                  if (ptable[i])
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
               if (ptable[i])
               {
                  if (flKillRegs)
                     kill_elem(ptable[i]);

                  ptable[i] = NULL;
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

      UINT insert(T *pobj, UINT indForced=0, bool flSearchEmpSlot=false)
      {
         bool  flOk;
         UINT  ind;
         long  tsize;

         T **pnew_table;


         flOk = false;

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
                     if (ptable[ind] == NULL)
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
         
               tsize = (nmax_elem + nBlocks*VECT_STEP_ALLOC) * sizeof(T*);

               pnew_table = (T **) realloc(ptable, tsize);

               if (pnew_table)
               {
                  void *pBegin;
                  long  sz;

                  pBegin = &pnew_table[nmax_elem];
                  sz     = nBlocks * VECT_STEP_ALLOC * sizeof(T*);

                  memset(pBegin, '\0', sz);

                  flOk        = true;
                  nmax_elem  += nBlocks * VECT_STEP_ALLOC;
                  ptable      = pnew_table;
               }
            }
            else
               flOk = true;

            if (flOk)
            {
               if (ptable[ind] && flKillRegs)
               {
                  kill_elem(ptable[ind]);
                  nelemNotNull--;
               }

               ptable[ind] = pobj;

               if (nelem <= ind)
                  nelem = ind + 1;

               if (pobj)
                  nelemNotNull++;
            }
         }

         if (flOk)
            ind++;
         else
            ind = 0;

         return(ind);
      }


      T * get_elem(UINT ind)
      {
         T   *pobj;

         pobj = NULL;

         if (ptable && ind && ind<=nelem)
            pobj = ptable[ind-1];

         return(pobj);
      }


      BOOL remove(UINT ind, BOOL _flKill)
      {
         BOOL  ret;
         long  sz;

         ret = FALSE;

         if (ptable && ind && ind<=nelem)
         {
            if (_flKill && flKillRegs && ptable[ind-1])
               kill_elem(ptable[ind-1]);

            if (nelem>1 && nelem>ind)
            {
               sz = (nelem - ind) * sizeof(T *);
               memmove(&ptable[ind-1], &ptable[ind], sz);
            }

            ptable[nelem-1] = NULL;
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

      T * popFirst(void)
      {
         T    *pobj;
         UINT  CurInd;
         long  sz;

         pobj = first();

         if (pobj)
         {
            CurInd = pos - 1;
            remove(CurInd, FALSE);
         }

         return pobj;
      }


      /*
         Retorna o ultimo elemento do Vetor, removendo-o da estrutura
         A aplicacao deverah encarregar-se de deleta-lo apos o uso
         Permite a implementacao de pilha (Last In First Out)
      */

      T * popLast(void)
      {
         T    *pobj;
         UINT  CurInd;

         pobj = last();

         if (pobj)
         {
            CurInd = pos + 1;
            remove(CurInd, FALSE);
         }

         return pobj;
      }


      T * first(void)
      {
         T *pobj;

         pos  = 1;
         pobj = next();

         return pobj;
      }


      T * last(void)
      {
         T *pobj;

         pos  = nelem;
         pobj = prev();

         return pobj;
      }


      T * next(void)
      {
         T *pobj;

         pobj = NULL;

         while(!end() && !pobj)
         {
            pobj = get_elem(pos);
            pos++;
         }

         return pobj;
      }


      T * prev(void)
      {
         T *pobj;

         pobj = NULL;

         while(pos>0 && !pobj)
         {
            pobj = get_elem(pos);
            pos--;
         }

         return pobj;
      }


      bool end(void)
      {
         bool ret;

         if (pos > nelem)
            ret = true;
         else
            ret = false;

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
      T * operator[] (UINT ind) const
      {
         return (get_elem(ind));
      }


      // Retorna a referencia ao item.
      T* &operator[] (UINT ind)
      {
         if (ptable && ind && ind<=nelem)
            return ptable[ind-1];
         else
            return pnull;
      }


      /*
         Retorna indice do elemento no vetor (1..nelem)
         (considerando que nao ha duplicidade de objetos)
         Caso nao seja encontrado retorna 0
      */

      UINT search(T *pobj)
      {
         UINT ind;

         if (ptable && pobj)
         {
            for (ind=0; ind<nelem; ind++)
            {
               if (ptable[ind]==pobj)
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


      bool erase(T *pobj)
      {
         bool  ret;
         UINT  ind;


         ret = false;

         if (pobj)
         {
            ind = search(pobj);

            if (ind && insert(NULL, ind))
               ret = true;
         }

         return ret;
      }


      bool erase(UINT ind)
      {
         bool  ret;


         if (ind && get_elem(ind) && insert(NULL, ind))
            ret = true;
         else
            ret = false;

         return ret;
      }
};



#endif  // _VECTOR_STR



