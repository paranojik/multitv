#ifndef DEMUXER_H
#define DEMUXER_H

#include <deque>
#include "BufManager.h"
#include "streamingBuffer.h"
#include "avformat.h"
#include "avio.h"
#include "mediapkt.h"
#include "media_utilities.h"

struct DemuxedPktInfo
{
    MediaSpec m_mediaSpec;
    ULONG     m_uSubSeq;
    int       m_iStreamIndex;

    DemuxedPktInfo(int _iStreamIndex=-1, ULONG _uSubSeq=0, MediaSpec _mediaSpec = NONE)
    {
        m_mediaSpec    = _mediaSpec;
        m_uSubSeq      = _uSubSeq;
        m_iStreamIndex = _iStreamIndex;
    }
};

class Decoder;
class IDemuxer
{
protected:

    ULONG m_uCurSeq;

    URLContext  m_urlCtx;
    URLProtocol m_urlProtocol;

    AVFormatContext *m_pFmtCtx;
    ByteIOContext    m_byteCtx;

    BufManager m_StreamHeader,
               m_DemuxedFrame;

    bool m_bInit,
         m_bHeaderSet;

    std::deque<DemuxedPktInfo> m_DemuxedPktInfoList;
    StreamingBuffer m_StreamBuf;

public:

    IDemuxer();
    virtual ~IDemuxer() {}

    virtual ULONG Init() = 0;
    virtual Decoder *GetDecoder(MediaSpec _mediaSpec) = 0;

    ULONG SetHeader(BYTE *_pHeader, ULONG _uHeaderLen);
    ULONG GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen);

    ULONG GetNextFrame(ULONG _id, BYTE **_ppData, ULONG & _uDatalen, MediaSpec & _mediaSpec);
    ULONG SetPacket(BYTE *_pPkt, ULONG _uPktLen, ULONG _uHeaderLen);

    bool HeaderSet()         { return m_bHeaderSet;                   }
    bool GetStreamsCount()   { return m_DemuxedPktInfoList.size();   }

    void SetMutexOwner()     { m_StreamBuf.SetWriteMutexOwner();      }  
    void ReleaseMutexOwner() { m_StreamBuf.ReleaseWriteMutexOwner();  }
};


class ASFDemuxer : public IDemuxer
{
private:

    static int OpenASF(URLContext *h, const char *_pUrlPath, int flags);
    static int CloseASF(URLContext *h);
    static int ReadASF(URLContext *h, unsigned char *buf, int size);

public:

    ASFDemuxer() {}
    virtual ~ASFDemuxer() {}

    virtual ULONG Init();
    virtual Decoder *GetDecoder(MediaSpec _mediaSpec);
};

#endif
