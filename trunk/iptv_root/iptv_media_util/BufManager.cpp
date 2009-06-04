#include "BufManager.h"
#include "global_error.h"

BYTE* BufManager::Attach(BYTE* lp)
{
    Release();

    return (BYTE*) (m_p = lp);
}

BYTE* BufManager::Detach()
{
    BYTE* lp   = m_p;

    m_p        = NULL;
    m_lSize    = 0;
    m_lMaxSize = 0;	

    return lp;
}

LONG BufManager::Size()
{
    return labs(m_lSize);
}

LONG BufManager::AllocatedSize()
{
    return m_lMaxSize;
}

void BufManager::SetSize(ULONG ulSize)
{
    if (ulSize == 0)
    {        Release();
    } 
    else 
    if (ulSize > (ULONG) m_lMaxSize)
    {
        Release();
        Alloc(ulSize);
    }

    m_lSize = m_p ? ulSize : 0;
}

ULONG BufManager::SetData(BYTE *_pData, ULONG _uLen)
{
    unsigned ret = RET_OK;

    if (_pData && _uLen) 
    {
        SetSize(_uLen);

        if (m_p && m_lSize)
        {
            memcpy(m_p, _pData, m_lSize);
        }
        else
        {
            ret = RET_LOW_MEMORY;
        }
    }
    else
    {
        ret = RET_ERROR;
    }

    return ret;
}

void BufManager::Release()
{
    if (m_lMaxSize > 0)
    {
        Free();
        m_lSize = 0;
    }
    
    m_p = NULL;
}

void BufManager::Alloc(LONG lSize)
{
    m_p = new BYTE[lSize];
    
    if (m_p)
        m_lMaxSize = lSize;
}

void BufManager::Free()
{
    if (m_p)
        delete [] m_p;

    m_lMaxSize = 0;
}

