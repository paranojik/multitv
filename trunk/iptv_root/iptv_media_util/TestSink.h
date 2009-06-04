#ifndef TEST_SINK_H
#define TEST_SINK_H

#include "SimposiumSink.h"
#include "ASFPktSink.h"

class TestSink : public ISimposiumSink
{
private:

	ASFPktSink m_pktSink;

public:

	ASFPktSink *GetSink() {return &m_pktSink; }

	virtual ULONG OnHeader(BYTE *_pHeader, ULONG _ulHeaderLen)
	{
        return m_pktSink.WriteHeader(_pHeader, _ulHeaderLen);

	}
    virtual	ULONG OnPacket(BYTE *_pPacket, ULONG _ulPktLen)
	{
        return m_pktSink.WritePacket((MediaPkt *)_pPacket,  _ulPktLen);
	}

    virtual ULONG OnEOF()
    {
        return 0;
    }
};

#endif

