#ifndef ENCODED_BUFFER_H
#define ENCODED_BUFFER_H

#include "BufManager.h"

class INSSBuffer;
class ISimposiumSink;

class EncodedBuffer
{
private:

	void SetMediaId(ULONG _ulMediaId) {m_ulMediaId = _ulMediaId; }

    WORD m_wVideoStream;
    BOOL m_bLastKeyFrame;
    ULONG m_ulSeq;
    ULONG m_ulLastPacketTime;
    ULONG m_ulPktsDelivered;
	ULONG m_ulMediaId;

    ISimposiumSink *m_pSimpSink;

    BufManager m_Header;

public:

	EncodedBuffer();
	virtual ~EncodedBuffer();

    void Reset();

    unsigned AllocateDataUnit(DWORD  cbDataUnit, INSSBuffer**  ppDataUnit);
    unsigned OnDataUnit(INSSBuffer* pDataUnit);
    unsigned OnHeader(INSSBuffer* pHeader);
    unsigned GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen);

    void NotifyEOF();
    void SetVideoStream(WORD _wVideoStream);
	void SetId(ULONG _ulMediaId)         { m_ulMediaId = _ulMediaId; }
	void SetSink(ISimposiumSink *_pSink) { m_pSimpSink = _pSink; }

};

#endif

