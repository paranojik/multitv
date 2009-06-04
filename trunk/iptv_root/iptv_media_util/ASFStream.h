#ifndef ASF_STREAM_H
#define ASF_STREAM_H

#include "shared/compat.h"
#include "FileStream.h"
#include "ASFFullHeader.h"
#include "defs.h"

class ASFObject;
class ASFIndexObject;
class ASFHeader;
class IFilterData;

#define MC_ASF_STREAM_SEEK_MEDIA	(SEEK_END + 10)

class ASFTransmissionStream
{
public:
    ASFTransmissionStream(FileStream *_pFileStream, BOOL bIrmStream = FALSE);
    ~ASFTransmissionStream();

    void SetHeader(ASFHeader& _asfHeader);
    void AddDataFilter(IFilterData * pDataFilter);
    //IUnknown* ReadObject();

    unsigned Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
    unsigned Read(void *pv, ULONG cb, ULONG *pcbRead);

    // IWMIStreamProps
    // The GetProperty method retrieves a named property from the IStream.
    //STDMETHOD(GetProperty)( LPCWSTR pszName, WMT_ATTR_DATATYPE* pType, BYTE* pValue, DWORD* pdwSize);

private:

    ASFFullHeader m_Header;
    ASFIndexObject* m_pIndex;

    ULONG m_ulHeaderRead;

    FileStream * m_pStream;

    void SetIndex(ASFHeader* pHeader);

    ASFObject* GetObject();
    unsigned GetPacket(void *_pv, ULONG _cb, ULONG *_pcbRead);

    LONG  m_lDataPacketSequence;
    WORD  m_wPacketSize;
    QWORD m_qwPreroll;
    BOOL  m_bIrmStream;

    IFilterData* m_pDataFilter;

    void FilterObject(ASFObject& obj);
};

#endif

