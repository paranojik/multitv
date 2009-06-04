#ifndef BUF_MANAGER_H
#define BUF_MANAGER_H

#include "shared/compat.h"

class BufManager
{
private:
    LONG   m_lSize,
           m_lMaxSize;

    void Release();
    void Alloc(LONG lSize);
    void Free();

protected:

    BYTE * m_p;

public:

    BufManager() : m_lSize(0), m_lMaxSize(0), m_p(NULL) {}
    BufManager(LONG lSize) : m_lSize(0), m_lMaxSize(0), m_p(NULL)
    {
        SetSize(lSize);
    }

    BufManager(LPVOID pVoid, ULONG ulSize) : m_lSize(ulSize), m_p((BYTE *) pVoid)
    {
        // invert signal
        // it represents it does not own the memory
        // allocated buffer
        
        m_lSize *= -1;
    }

    ~BufManager()
    {
        Release();
    }
  
    ULONG SetData(BYTE *_pData, ULONG _uLen);

    BYTE *GetDataPtr() { return m_p; }

    BYTE* Attach(BYTE* lp);
    BYTE* Detach();

    LONG  Size();
    LONG AllocatedSize();

    void  SetSize(ULONG ulSize);
};

#endif

