#ifndef _HASHT_STR
#define _HASHT_STR


#include "str_ap.h"


// O hashing é obtido a partir de Mod(HASH_MODE)


#define TNOME_SIMB    30    // Redefinido em def_ap.h
#define HASH_MODE     97
#define NMAX_SINON     5


/*
   Tabela principal
   Contém HASH_MODE linhas (HashT_reg)


template <class T> class HashT
{
   private:

   HashT_reg<T>   *pmain_table;  // Ponteiro para a tabela do hash
   HashT_compl<T> *plcompl;      // Ultima pagina complementar de registros acessada

   int            line,         // Ultima posicao da tabela do hash acessada
                  offset;       // Ultimo registro acessado dentro da pagina corrente

   int            nitens,
                  nItensNotDel;

   BOOL           flKillRegs;   // Indica se no destrutor da classe os itens devem ser deletados

   UINT formula(LPCSTR str);
   void atrib_elem( HashT_elem<T> *phash_elem, LPCSTR nome, T obj, BOOL flDel);
   void atrib_buf_elem( HashT_elem<T> *phash_elem, LPSTR nome, T *pobj, LPINT lpflDel);
   BOOL lsearch(LPCSTR nome, T *pobj = NULL, BOOL flKill = FALSE, BOOL flKillReg = TRUE);
   T get_item(LPSTR nome);

   void init(void);
   void kill(void);

   virtual void kill_reg(T obj);


   public:

   HashT(BOOL _flKillRegs = FALSE);
   virtual ~HashT(void);

   void reset     (void);
   int  get_nitens(void) { return nItensNotDel; }
   UINT get_nrecs (UINT _line, UINT *pndel=NULL);

   BOOL add   (LPCSTR nome, T obj);
   T    search(LPCSTR nome);
   BOOL erase (LPCSTR nome);
   BOOL remove(LPCSTR nome);

   T first(LPSTR nome = NULL);
   T next (LPSTR nome = NULL);
};
*/



template <class T> class HashT_elem
{
   public:

   char name[TNOME_SIMB+1];
   BOOL flDeleted;

   T obj;

   void reset(void)
   {
       *name = '\0';
       flDeleted = FALSE;
       obj = 0;
   }

   HashT_elem(void)
   {
       reset();
   }
};


/*
   Tabela complementar p/ hashing
   Contém até NMAX_SINOM elementos
   Se necessário, aponta p/ outra Tabela complementar 
*/

template <class T> class HashT_compl
{
   public:

   HashT_elem<T>   sinon[NMAX_SINON];
   HashT_compl<T> *pnext;

   UINT ndel;

   HashT_compl(void)
   {
       int i;

       pnext = NULL;
       ndel  = 0;

       for (i=0; i<NMAX_SINON; i++)
           sinon[i].reset();
   }
};
 

/*
   Define as linhas da tabela principal
   nsinon indica o número de ocorrências de elementos
   ndel indica o numero de itens excluidos da linha
   situados naquela linha da tabela (sinônimos)
*/


template <class T> class HashT_reg
{
   public:

   UINT nsinon,
        ndel;

   HashT_compl<T> *pcompl,      // Primeira pagina de registros
                  *plastcompl;  // Ultima pagina de registros


   void reset(void)
   {
       nsinon     = 0;
       ndel       = 0;
       pcompl     = NULL;
       plastcompl = NULL;
   }

   HashT_reg(void)
   {
       reset();
   }
};



template <class T> class HashT
{
    private:

    HashT_reg<T>   *pmain_table;  // Ponteiro para a tabela do hash
    HashT_compl<T> *plcompl;      // Ultima pagina complementar de registros acessada

    int         line,         // Ultima posicao da tabela do hash acessada
                offset;       // Ultimo registro acessado dentro da pagina corrente

    int         nitens,
                nItensNotDel;

    BOOL        flKillRegs;   // Indica se no destrutor da classe os itens devem ser deletados


    void init(void)
    {
       int i;

       nitens       = 0;
       nItensNotDel = 0;

       pmain_table = new HashT_reg<T>[HASH_MODE];

       if (pmain_table)
       {
           for (i=0; i<HASH_MODE; i++)
               pmain_table[i].reset();
       }

       plcompl = NULL;
       line    = 0;
       offset  = 0;
    }


    virtual void kill_reg(T obj)
    {
    }


    UINT formula(LPCSTR str)
    {
       UINT lstr,
            ret,
            code[4];

       lstr = lstrlen(str);

       switch(lstr)
       {
          case 0:
             code[0] = 0;
             code[1] = 0;
             code[2] = 0;
             code[3] = 0;
             break;

          case 1:
             code[0] = str[0];
             code[1] = 0;
             code[2] = str[0];
             code[3] = 0;
             break;

          default:
             code[0] = str[0];
             code[1] = str[1];
             code[2] = str[lstr-1];
             code[3] = str[lstr-2];
       }

       ret = (code[0] + code[1]*3 + code[2]*7 + code[3]*11) % HASH_MODE;

       return ret;
    }


    BOOL lsearch(LPCSTR nome, T *pobj = NULL, BOOL flKill = FALSE, BOOL flKillReg = TRUE)
    {
       char jname[TNOME_SIMB+1];

       UINT code,
            pos,
            ret;

       HashT_compl<T> *pcompl;
       HashT_reg<T>   *linha;
       HashT_elem<T>  *phash_elem;


       if (!pmain_table || !nome || !(*nome))
           return FALSE;

       ajusta_upp_str(nome, jname, TNOME_SIMB);
       code  = formula(jname);

       ret    = FALSE;
       linha  = &pmain_table[code];

       if (linha->nsinon>linha->ndel && linha->pcompl)
       {
          pcompl = linha->pcompl;

          while (pcompl && !ret)
          {
             pos = 0;

             while (!ret && pos<NMAX_SINON && *(pcompl->sinon[pos].name))
             {
                phash_elem = &(pcompl->sinon[pos]);

                if (_fstricmp(jname, phash_elem->name)==0 && !phash_elem->flDeleted)
                {
                   ret = TRUE;

                   if (pobj)
                      *pobj = phash_elem->obj;

                   if (flKill)
                   {
                      phash_elem->flDeleted = TRUE;

                      if (flKillRegs && flKillReg && phash_elem->obj)
                          kill_reg(phash_elem->obj);

                      pcompl->ndel++;
                      linha->ndel++;
                   }
                }
                else
                   pos++;
             }

             pcompl = pcompl->pnext;
          }
       }

       return ret;
    }


    void atrib_elem( HashT_elem<T> *phash_elem, LPCSTR nome, T obj, BOOL flDel)
    {
       if (phash_elem)
       {
           if (nome)
              lstrcpy(phash_elem->name, nome);

           if (obj)
              phash_elem->obj = obj;

           phash_elem->flDeleted = flDel;
       }
    }


    void atrib_buf_elem( HashT_elem<T> *phash_elem, LPSTR nome, T *pobj, LPINT lpflDel)
    {
       if (phash_elem)
       {
           if (nome)
              lstrcpy(nome, phash_elem->name);

           if (pobj)
              *pobj = phash_elem->obj;

           if (lpflDel)
              *lpflDel = phash_elem->flDeleted;
       }
    }


    public:


    HashT(BOOL _flKillRegs=TRUE)
    {
       flKillRegs = _flKillRegs;
       init();
    }


    virtual ~HashT(void)
    {
       kill();
    }


    inline int get_nitens(void)
    {
        return nItensNotDel;
    }


    void reset(void)
    {
       kill();
       init();
    }


    void kill(void)
    {
       UINT i,
            j;

       HashT_compl<T> *pcompl,
                      *pnext;

       HashT_reg<T>   *linha;

       T obj;


       if (pmain_table)
       {
          for (i=0; i<HASH_MODE; i++)
          {
             linha = &pmain_table[i];

             if (linha->nsinon && linha->pcompl)
             {
                pcompl = linha->pcompl;

                while (pcompl)
                {
                   pnext = pcompl->pnext;

                   if (flKillRegs)
                   {
                       j = 0;

                       while ( j<NMAX_SINON && *(pcompl->sinon[j].name) )
                       {
                           if (!pcompl->sinon[j].flDeleted)
                           {
                               obj = pcompl->sinon[j].obj;

                               if (obj)
                                   kill_reg(obj);
                           }

                           j++;
                       }
                   }

                   delete pcompl;
                   pcompl = pnext;
                }
             }
          }

          delete [] pmain_table;
          pmain_table = NULL;
       }
    }


    UINT get_nrecs(UINT _line, UINT *pndel=NULL)
    {
        UINT nrecs;

        nrecs = 0;

        if (pmain_table && _line<HASH_MODE)
        {
            nrecs = pmain_table[_line].nsinon;

            if (pndel)
                *pndel = pmain_table[_line].ndel;
        }

        return nrecs;
    }


    T search(LPCSTR nome)
    {
       int ret;

       T obj;

       obj = 0;
       ret = lsearch(nome, &obj);

       if (!ret)
          obj = 0;

       return obj;
    }


    BOOL erase(LPCSTR nome)
    {
       BOOL ret;

       ret = lsearch(nome, NULL, TRUE);

       if (ret)
          nItensNotDel--;

       return ret;
    }

    BOOL remove(LPCSTR nome)
    {
       BOOL ret;

       ret = lsearch(nome, NULL, TRUE, FALSE);

       if (ret)
          nItensNotDel--;

       return ret;
    }

    BOOL add(LPCSTR nome, T obj)
    {
       BOOL ret;

       char jname[TNOME_SIMB+1];

       UINT code,
            pos;

       HashT_compl<T> *pcompl,
                      *plast;

       HashT_reg<T>   *linha;
       HashT_elem<T>  *phash_elem;


       if (!pmain_table || !nome || !(*nome))
           return FALSE;

       ret = FALSE;

       ajusta_upp_str(nome, jname, TNOME_SIMB);
       code  = formula(jname);

       pcompl = NULL;
       linha  = &pmain_table[code];


       if (!linha->nsinon)
       {
           // Primeiro item da linha
           pcompl = new HashT_compl<T>;

           if (pcompl)
           {
               atrib_elem( &(pcompl->sinon[0]), nome, obj, FALSE);
               linha->pcompl     = pcompl;
               linha->plastcompl = pcompl;

               linha->nsinon++;

               nitens++;
               nItensNotDel++;

               ret = TRUE;
           }
       }
       else
       {
           if (linha->ndel)
           {
               // Existem itens deletados na linha
               // => Utiliza espaco vago

               pcompl = linha->pcompl;

               while (pcompl && !ret)
               {
                   if (pcompl->ndel)
                   {
                       pos = 0;

                       while (pos<NMAX_SINON && !pcompl->sinon[pos].flDeleted)
                           pos++;

                       // pos = posicao a ser incluido o registro

                       if (pos<NMAX_SINON)
                       {
                           phash_elem = &(pcompl->sinon[pos]); 
                           atrib_elem( phash_elem, nome, obj, FALSE );

                           linha->ndel--;
                           pcompl->ndel--;

                           nItensNotDel++;

                           ret = TRUE;
                       } 
                   }
                   else
                       pcompl = pcompl->pnext;
               }
           }
           else
           {
               // Nao existem itens deletados na linha
               // => Inclui novo registro

               plast = linha->plastcompl;

               if (linha->nsinon % NMAX_SINON == 0)
               {
                   pos = 0;

                   pcompl = new HashT_compl<T>;

                   if (pcompl)
                   {
                       plast->pnext = pcompl;
                       linha->plastcompl = pcompl;
                       ret = TRUE;
                   }
               }
               else
               {
                   pos = ((linha->nsinon - 1) % NMAX_SINON) + 1;
                   pcompl = plast;
                   ret = TRUE;
               }

               if (ret)
               {
                   phash_elem = &(pcompl->sinon[pos]); 
                   atrib_elem( phash_elem, nome, obj, FALSE );

                   linha->nsinon++;

                   nitens++;
                   nItensNotDel++;
               }
           }
       }

       return ret;
    }


    T get_item(LPSTR nome)
    {
        BOOL flDeleted,
             flOk;
        T obj;

        HashT_reg<T> *linha;


        flOk = FALSE;
        obj  = NULL;

        while (line<HASH_MODE && !flOk)
        {
            linha = &pmain_table[line];

            // Verifica se ha itens nao deletados na linha
            if (linha->nsinon > linha->ndel)
            {
                // Verifica se deve ser obtido o primeiro registro da linha
                if (!plcompl)
                {
                    plcompl = linha->pcompl;
                    offset  = 0;
                }

                if ( *(plcompl->sinon[offset].name) )
                {
                    if ( !(plcompl->sinon[offset].flDeleted) )
                    {
                        atrib_buf_elem(&(plcompl->sinon[offset]), nome, &obj, &flDeleted);
                        flOk = TRUE;
                    }
                }
                else
                    offset = NMAX_SINON;

                offset++;

                if (offset >= NMAX_SINON)
                {
                    // Posiciona ponteiros para proxima pagina
                    plcompl = plcompl->pnext;
                    offset  = 0;

                    if (!plcompl)
                    {
                        // Se nao existir => posiciona ponteiros para proxima linha
                        line++;
                        plcompl = NULL;
                    }
                }
            }
            else
            {
                // Se nao existirem registros validos na linha
                // => posiciona ponteiros para proxima linha
                line++;
                plcompl = NULL;
                offset  = 0;
            }
        }

        return obj;
    }


    T first(LPSTR nome=NULL)
    {
        T obj;

        line    = 0;
        plcompl = NULL;
        offset  = 0;

        obj = get_item(nome);

        return obj;
    }


    T next(LPSTR nome=NULL)
    {
        T obj;

        obj = get_item(nome);

        return obj;
    }
};


#endif
