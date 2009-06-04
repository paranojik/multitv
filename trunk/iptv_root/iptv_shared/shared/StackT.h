#ifndef _STACKT_STR
#define _STACKT_STR


#include "compat.h"


/*
template <class T> class StackT
{
   UINT size,
        level;

   T   *pdata;

   public:

   Stack (UINT maxelem, BOOL _KillRegs=TRUE);
  ~Stack (void);

   int   push (T obj);
   T     pop  (void);
   void  kill (void);
   void  init (UINT maxelem);
   void  reset(void);

   T     get_elem(UINT nivel);
   UINT  get_nitens(void) { return level; }
};

*/



template <class T> class StackT
{
    private:


    UINT size,
         level;

    BOOL flKillRegs;

    T   *pdata;

    public:


    void reset(void)
    {
        if (flKillRegs)
        {
            if (pdata && size)
            {
                while (level > 0)
                    delete (pdata[--level]);
            }
        }
        else
            level = 0;
    }


    void init(UINT maxelem)
    {
        if (!pdata)
        {
            pdata = (T *) farcalloc(maxelem, sizeof(T));
            size  = maxelem;
            reset();
        }
    }


    void kill(void)
    {
       reset();

       if (pdata)
       {
          farfree(pdata);
          pdata = NULL;
          size  = 0;
       }
    }


    StackT(UINT maxelem, BOOL _KillRegs=TRUE)
    {
       level      = 0;
       pdata      = NULL;
       flKillRegs = _KillRegs;

       init(maxelem);
    }


   ~StackT(void)
    {
       kill();
    }


    UINT get_nitens(void)
    {
       return level;
    }


    /*
       Se Retorno = 0 => Erro senao retorna level (1..n)
    */

    int push(T obj)
    {
       int nivel;


       nivel = 0;

       if (pdata && size && level<size)
       {
          pdata[level++] = obj;

          nivel = level;
       }

       return(nivel);
    }


    T pop(void)
    {
       T hret;

       if (pdata && size && level>0)
       {
          hret = pdata[--level];
       }
       else
          hret = NULL;

       return(hret);
    }


    T get_elem(UINT nivel)
    {
       T hret;

       if (pdata && size && level>0 && nivel>0 && nivel<=level)
       {
          hret = pdata[nivel-1];
       }
       else
          hret = NULL;

       return(hret);
    }

};



#endif
