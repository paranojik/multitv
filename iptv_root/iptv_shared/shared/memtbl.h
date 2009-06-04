#ifndef __MEM_TBL__
#define __MEM_TBL__


#include "SVectorT.h"
#include "semaphore.h"


class MemBlock
{
public:

    char   filename[TNOME_OBJ+1];
    ULONG  line,
           size;
    void  *ptr;

    MemBlock   (void *_ptr, ULONG _size, const char *_filename, ULONG _line);
    void update(void *_ptr, ULONG _size, const char *_filename, ULONG _line);
};


typedef SVectorT<ULONG, MemBlock *> MemVector;

class MemTable : public MemVector
{
private:
    ULONG maxblocks;

    _SEMAPHORE *pSemaph;

protected:
    void kill_elem(ULONG ptr, MemBlock *pMemBlock);

public:
    MemTable    (void);
   ~MemTable    (void);

    BOOL Wait   (void)   { return (pSemaph ? pSemaph->Wait():FALSE); }
    void Signal (void)   { if (pSemaph) pSemaph->Signal();           }

    BOOL add    (void *ptr, ULONG size, const char *filename, ULONG line);
    void ListAll(void);
};


#endif  // __MEM_TBL__

