#ifndef _ALLOC_X
#define _ALLOC_X


#include "StackT.h"


//#ifdef __cplusplus
//extern "C" {
//#endif

// Classe aceita NMAX_BLOCKS ateh 64k - 1 blocos de memoria
// Valor utilizado: 16 kb blocos x 32 bits (sizeof void far *) == 64kb
#define NMAX_BLOCKS   0x4000

//#define NMAX_BLOCKS   500


typedef struct PtrStr_
{
   HGLOBAL   hBlock;
   void far *lpPtr;
   DWORD     size;
} PtrStr;


class far MemoryManager : public StackT<HGLOBAL>
{
   private:
      PtrStr far *lpVect;

      UINT wLastPos,
           wMaxBlocks;

   public:
      MemoryManager(UINT nBlocks = NMAX_BLOCKS);

      UINT       GetLastPos(void) { return wLastPos; }

      HGLOBAL    GlobalAlloc_x       (DWORD size);
      HGLOBAL    GlobalReAlloc_x     (HGLOBAL mHandle, DWORD size);
      HGLOBAL    GlobalFree_x        (HGLOBAL mHandle);
      void far * GlobalLock_x        (HGLOBAL mHandle);
      int        GlobalUnlock_x      (HGLOBAL mHandle);
};




//#ifdef __cplusplus
//}
//#endif

#endif

