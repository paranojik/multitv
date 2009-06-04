#ifndef ISIMPOSIUM_SINK_H
#define ISIMPOSIUM_SINK_H

class ISimposiumSink
{

public:

    virtual ULONG OnHeader(BYTE *_pHeader, ULONG _ulHeaderLen) = 0;
    virtual ULONG OnPacket(BYTE *_pPacket, ULONG _ulPktLen) = 0;
    virtual ULONG OnEOF() = 0;

};

#endif

