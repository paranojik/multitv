#ifndef ASF_FILE_SOURCE_STREAM_H
#define ASF_FILE_SOURCE_STREAM_H

#include <string>
#include "defs.h"
#include "mediapkt.h"
#include "IFilterData.h"
#include "EncodedBuffer.h"

class ASFTransmissionStream;
class FileStream;
class ISimposiumSink;

class ASFFileSourceStream : public IFilterData
{
public:
    ASFFileSourceStream();
    virtual ~ASFFileSourceStream();

    unsigned DeliverPacket();
    unsigned Initialize();
    void Reset();

    void GenerateHeader();

    void OnTimestamp(LPBYTE pPacket, ULONG cbPacket, LONG lPacketSequence, MEDIAPKT_TYPES type, DWORD dwFlags, DWORD dwTime);
    void OnIndex(LPBYTE pIndex, ULONG cbIndexSize);
    void OnHeader(LPBYTE pHeader, ULONG cbHeaderSize, DWORD dwBitrate, DWORD dwPacketSize, QWORD qwPreroll);
    void OnEndOfData() {}

    void SetSink(ISimposiumSink * _pSink);
	
    void SetFilename(LPCSTR sztFilename);
    LPCSTR GetFilename();

    void SetFilename(std::string strFilename);
    std::string & GetFilenameStr();

	void SetId(ULONG _id);

    unsigned GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen)
    {
         return m_Sink.GetHeader(_ppHeader, _uHeaderLen);
    }

private:

    QWORD  m_qwPreroll;
    DWORD  m_dwPacketSize;

    std::string m_strFilename;

    ASFTransmissionStream  *m_pStream;
    FileStream *m_pFileStream;

    EncodedBuffer m_Sink;

    unsigned m_refClockTime,
             m_refPktTime;

    bool m_bFirstPkt;
};

#endif

