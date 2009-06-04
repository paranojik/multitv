#include "shared/compat.h"
#include "defs.h"
#include "ASFFileSourceStream.h"
#include "ASFStream.h"
#include "EncodedBuffer.h"
#include "global_error.h"
#include "mediapkt.h"
#include "SimposiumSink.h"
#include "ASFDataPacket.h"
#include "ithread.h"
#include "log.h"

ASFFileSourceStream::ASFFileSourceStream() : 
//    m_qwPreroll(0),
//    m_dwPacketSize(0),
//    m_refClockTime(0),
//    m_refPktTime(0),
    m_pStream(NULL),
    m_pFileStream(NULL)
//    m_bFirstPkt(true)
{
    Reset();
}

ASFFileSourceStream::~ASFFileSourceStream()
{
    Reset();
}

void ASFFileSourceStream::SetFilename(LPCSTR szFilename)
{
    m_strFilename = szFilename;
}

void ASFFileSourceStream::SetFilename(std::string strFilename)
{
    m_strFilename = strFilename;
}

LPCSTR ASFFileSourceStream::GetFilename()
{
    return m_strFilename.c_str();
}

std::string & ASFFileSourceStream::GetFilenameStr()
{
    return m_strFilename;
}

void ASFFileSourceStream::SetSink(ISimposiumSink * _pSink)
{
    m_Sink.SetSink(_pSink);
}

void ASFFileSourceStream::SetId(ULONG _id)
{
    m_Sink.SetId(_id);
}

unsigned ASFFileSourceStream::Initialize()
{
    if (!m_strFilename.size())    {
        return RET_ERROR;
    }

    Reset();

    FileStream* pFileStream = new FileStream(m_strFilename.c_str());
    if (!pFileStream)
    {
        return RET_LOW_MEMORY;
    }

    ASFTransmissionStream* pASFStream = new ASFTransmissionStream(pFileStream);
    if (!pASFStream)
    {
        return RET_LOW_MEMORY;
    }

    m_pStream     = pASFStream;
    m_pFileStream = pFileStream;

    pASFStream->AddDataFilter(this);

    return RET_OK;
}

void ASFFileSourceStream::Reset()
{
    if (m_pStream)
    {    
        delete m_pStream;
        m_pStream = NULL;
    }

    if (m_pFileStream)
    {    
        delete m_pFileStream;
        m_pFileStream = NULL;
    }

    m_qwPreroll    = 0;
    m_dwPacketSize = 0;
    m_refClockTime = 0;
    m_refPktTime   = 0;

    m_bFirstPkt    = true;

    m_Sink.Reset();
}

void ASFFileSourceStream::GenerateHeader()
{

    m_pFileStream->OpenUrl();

    LPBYTE pHeader;
    ULONG cbRead = MC_MAX_IRM_PACKETSIZE;

    unsigned ret = RET_ERROR;
	
    INSSBuffer * pNSSBuffer = NULL;
    ret = m_Sink.AllocateDataUnit(cbRead, &pNSSBuffer);
    
    if (ret == RET_OK)
    {
        ret = pNSSBuffer->GetBuffer(&pHeader);

        if (ret == RET_OK)
        {
	        ret = m_pStream->Read(pHeader, cbRead, &cbRead);

            if (ret == RET_OK)
            {
                ret = pNSSBuffer->SetLength(cbRead);
            }
        }
    }

    if (ret == RET_OK)
	{
        m_Sink.OnHeader(pNSSBuffer);
	}

    if (pNSSBuffer)
        delete pNSSBuffer;
}

unsigned ASFFileSourceStream::DeliverPacket()
{
    LPBYTE pBuf;
    ULONG cbRead = m_dwPacketSize + sizeof(MediaPkt);

    ULONG cbToRead = cbRead;
    
    unsigned ret;

    INSSBuffer * pNSSBuffer = NULL;
    if ((ret = m_Sink.AllocateDataUnit(MC_MAX_IRM_RAW_PACKETSIZE, &pNSSBuffer)) == RET_OK)   
    {
        ret = pNSSBuffer->GetBuffer(&pBuf);

        if (ret == RET_OK)
        {
            if (((ret = m_pStream->Read(pBuf, cbToRead, &cbRead)) == RET_OK) && (cbRead == cbToRead))
            {
                MediaPkt *pMediaPkt = (MediaPkt *)pBuf;
                BYTE *pData = &pBuf[sizeof(MediaPkt)];

                ASFDataPacket asfpkt(cbRead);
                asfpkt.Attach(pData);
                asfpkt.Refresh();

                unsigned uCurTime = GetTickCount();
                unsigned uPktTime = asfpkt.GetPresentationTime();
                            
                if (m_bFirstPkt)
                {
                    m_refPktTime   = uPktTime;
                    m_refClockTime = uCurTime;

                    m_bFirstPkt = false;
                }    

                unsigned uClockTimeElapsed = uCurTime - m_refClockTime;
                unsigned uPktTimeElapsed   = uPktTime - m_refPktTime;

                if (uClockTimeElapsed < uPktTimeElapsed)
                {
                    unsigned uTimeToWait = uPktTimeElapsed - uClockTimeElapsed;
                    Wait(uTimeToWait);
                }
                
                ret = pNSSBuffer->SetLength(pMediaPkt->datalen);

                // move asf payload to beginning of buffer
                memmove(pBuf, pData, pMediaPkt->datalen);
            }
            else
            {
                ret = RET_ERROR;
            }
        }
    }

    if ( (ret == RET_OK) && (cbRead == cbToRead) )
    {
        m_Sink.OnDataUnit(pNSSBuffer);
    }
    else
    {
#ifdef _DEBUG
        g_FileLog.SetLog("m_Sink.NotifyEOF");
#endif
        m_Sink.NotifyEOF();
    }

    if (pNSSBuffer)
        delete pNSSBuffer;

    return ret;
}

void ASFFileSourceStream::OnTimestamp(LPBYTE pPacket, ULONG cbPacket, LONG lPacketSequence, MEDIAPKT_TYPES type, DWORD dwFlags, DWORD dwTime)
{

}

void ASFFileSourceStream::OnHeader(LPBYTE pHeader, ULONG cbHeaderSize, DWORD dwBitrate, DWORD dwPacketSize, QWORD qwPreroll)
{
    m_qwPreroll = qwPreroll;
    m_dwPacketSize = dwPacketSize;

    if (pHeader)
    {
        ASFHeader Header;
        Header.Attach(pHeader);

        Header.Refresh();
        WORD wVideoStream = Header.GetVideoStream();
        m_Sink.SetVideoStream(wVideoStream);
    }
}

void ASFFileSourceStream::OnIndex(LPBYTE pIndex, ULONG cbIndexSize)
{

}
