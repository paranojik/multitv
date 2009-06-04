#ifndef DATA_FILTER_H
#define DATA_FILTER_H

#include "mediapkt.h"

class IFilterData
{
public:
	virtual void OnTimestamp(LPBYTE pPacket, ULONG cbPacket, LONG lPacketSequence, MEDIAPKT_TYPES type, DWORD dwFlags, DWORD dwTime) = 0;
	virtual void OnHeader(LPBYTE pHeader, ULONG cbHeaderSize, DWORD dwBitrate, DWORD dwPacketSize, QWORD qwPreroll) = 0;
	virtual void OnIndex(LPBYTE pIndex, ULONG cbIndexSize) = 0;
	virtual void OnEndOfData() = 0;

    //virtual BOOL OnSourceChanged(UINT32 _nId) = 0;
    //virtual BOOL Reconnect() = 0;
    //virtual void SetArchivalFileSink(IMCStorageSink* pArchivalSink) = 0;
    //virtual void OnDiscontinuity(BOOL bJustConnected) = 0;
};

#endif

