//#define GA_GER_ON
#define TMAX_TITLE     20


typedef struct Ghandle_ger  far  *FPGHGER; 


struct Ghandle_ger
{
   HGLOBAL handle;

   UINT    nlocks;

   WORD    Cs,
			  Ip;
};


class GAlloc_ger
{
   HGLOBAL hdata; 

   char title[TMAX_TITLE+1];

	UINT nhandles,
        nhandles_alloc,
        nhandles_max;

   UINT search (HGLOBAL handle);

   public:

	GAlloc_ger (UINT nmax_handles, LPCSTR pTitle);
   ~GAlloc_ger(void);
   BOOL insert (HGLOBAL handle, WORD Cs = 0, WORD Ip = 0);
   BOOL kill   (HGLOBAL handle);
   BOOL lock   (HGLOBAL handle);
   BOOL unlock (HGLOBAL handle);
};


