#include "INSSBuffer.h"
#include "global_error.h"
#include "media_utilities.h"
#include <malloc.h>

const GUID_W WM_SampleExtensionGUID_OutputCleanPoint = {0xf72a3c6f, 0x6eb4, 0x4ebc, {0xb1, 0x92, 0x9, 0xad, 0x97, 0x59, 0xe8, 0x28}};

INSSBuffer::INSSBuffer(LPBYTE p, DWORD cb) :
    BufManager(p, cb),
    //m_pSample(NULL),
    m_bKeyframe(FALSE)
{
    //_TRACE1(DsLOGLEVEL5 | DsLOG_MEMORY_REF, TEXT("CMCINSSBuffer::CMCINSSBuffer(total: %d)"), 
    //    InterlockedIncrement(&cStaticBuffers));
}

INSSBuffer::~INSSBuffer()
{
   // _TRACE1(DsLOGLEVEL5 | DsLOG_MEMORY_REF, TEXT("CMCINSSBuffer::~CMCINSSBuffer(total: %d)"), 
   //     InterlockedDecrement(&cStaticBuffers));
}

unsigned INSSBuffer::GetBuffer(LPBYTE *ppBuf)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCINSSBuffer::GetBuffer"));

    if (!ppBuf)
    {
        //_WARN0C(E_POINTER, DsCAT_WARNING, TEXT("Invalid pointer"));
        return RET_INVALID_ARG;//E_POINTER;
    }

    /*if (m_pSample)
    {
        return m_pSample->GetPointer(ppBuf);
    }*/

    if (!m_p)
    {
        //_WARN0(DsCAT_WARNING, TEXT("No buffer was allocated"));
        return RET_INIT_ERROR;
    }

    *ppBuf = m_p;

    return RET_OK;
}

unsigned INSSBuffer::GetBufferAndLength(LPBYTE* ppBuf, LPDWORD pdwLen)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCINSSBuffer::GetBufferAndLength"));

    CHECK_ERROR(GetBuffer(ppBuf), RET_OK)

    return GetLength(pdwLen);
}

unsigned INSSBuffer::GetLength(LPDWORD pdwLen)
{
    //_TRACE1(LOGLEVEL5, TEXT("CMCINSSBuffer::GetLength (returns %d)"), 
    //	m_pSample ? m_pSample->GetActualDataLength() : Size());

    if (!pdwLen)
    {
        //_WARN0C(E_POINTER, DsCAT_WARNING, TEXT("Invalid pointer"));
        return RET_INVALID_ARG;//E_POINTER;
    }

    *pdwLen = Size();

    return RET_OK;
}

unsigned INSSBuffer::GetMaxLength(LPDWORD pdwLen)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCINSSBuffer::GetMaxLength"));

    if (!pdwLen)
    {
        //_WARN0C(E_POINTER, DsCAT_WARNING, TEXT("Invalid pointer"));
        return RET_INVALID_ARG;//E_POINTER;
    }	

    *pdwLen = _HEAP_MAXREQ;	

    return RET_OK;
}

unsigned INSSBuffer::SetLength(DWORD dwLen)
{
    //_TRACE1(LOGLEVEL5, TEXT("CMCINSSBuffer::SetLength (%d)"), dwLen);

    /*if (m_pSample)
    {
        return m_pSample->SetActualDataLength(dwLen);
    }*/

    SetSize(dwLen);
    if (!m_p)
    {
        //_WARN0C(E_OUTOFMEMORY, DsCAT_ERROR, TEXT("Out of memory"));
        return RET_ERROR; //E_OUTOFMEMORY;
    }

    return RET_OK;
}

unsigned INSSBuffer::GetProperty(GUID_W guidBufferProperty, void*  pvBufferProperty, DWORD* pdwBufferPropertySize)
{
    if (memcmp(&guidBufferProperty, &WM_SampleExtensionGUID_OutputCleanPoint, sizeof(GUID_W)) == 0)
    {
        if (!pdwBufferPropertySize)
        {
            //_WARN0C(E_POINTER, DsCAT_ERROR, TEXT("Invalid Pointer"));
            return RET_ERROR; //E_POINTER;
        }

        if (pvBufferProperty)
        {
            if (*pdwBufferPropertySize >= 1)
            {
                * (LPBYTE) pvBufferProperty = (BYTE) m_bKeyframe;
                *pdwBufferPropertySize = 1;
            } 
            else
            {
                //_WARN0C(E_INVALIDARG, DsCAT_ERROR, TEXT("Invalid arguments"));
                return RET_ERROR; //E_INVALIDARG;
            }
        } 
        else
        {
            *pdwBufferPropertySize = 1;
        } // end if (pvBufferProperty)

        return RET_OK;
    }

    return RET_ERROR; //NS_E_UNSUPPORTED_PROPERTY;
}

unsigned INSSBuffer::SetProperty(
	GUID_W  guidBufferProperty,
	void* pvBufferProperty,
	DWORD dwBufferPropertySize)
{
    if (!pvBufferProperty)
    {
        //_WARN0C(E_POINTER, DsCAT_WARNING, TEXT("Invalid Pointer"));
        return RET_ERROR; //E_POINTER;
    }

    if (memcmp(&guidBufferProperty, &WM_SampleExtensionGUID_OutputCleanPoint, sizeof(GUID_W)) == 0)
    {
        m_bKeyframe = * (LPBYTE) pvBufferProperty;
    }

    return RET_OK;
}
