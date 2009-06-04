#ifndef _STACK_STR
#define _STACK_STR


class Stack
{
   UINT size,
        level;

   // HGLOBAL hdata;
   LPHANDLE pdata;

   public:

   Stack (UINT maxelem);
  ~Stack (void);

   int     push (HGLOBAL handle);
   HGLOBAL pop  (void);
   void    kill (void);
   void    init (UINT maxelem);
   void    reset(void);

   HGLOBAL get_elem(UINT nivel);
   UINT    get_level(void) { return level; }
};


#endif


