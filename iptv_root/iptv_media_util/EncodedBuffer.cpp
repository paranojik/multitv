#include "shared/compat.h"
#include "EncodedBuffer.h"
#include "global_error.h"
#include "INSSBuffer.h"
#include "ASFDataPacket.h"
#include "mediapkt.h"
#include "shared/ithread.h"
#include "ASFPktSink.h"
#include "media_utilities.h"
#include "shared/common.h"
#include "SimposiumSink.h"

EncodedBuffer::EncodedBuffer() : m_pSimpSink(NULL)
{ 
    Reset();
}

EncodedBuffer::~EncodedBuffer()
{
}

void EncodedBuffer::Reset()
{
    m_wVideoStream  = 0;
    m_bLastKeyFrame = FALSE;
    m_ulLastPacketTime = 0;
    m_ulPktsDelivered  = 0;
	m_ulMediaId = 0;
    m_ulSeq = 0;
}

unsigned EncodedBuffer::AllocateDataUnit(DWORD cbDataUnit, INSSBuffer** ppDataUnit)
{
    if (!ppDataUnit)
	{
        return RET_INVALID_ARG;
    }

    INSSBuffer * pNSSBuf = new INSSBuffer();
    if (!pNSSBuf || pNSSBuf->SetLength(cbDataUnit) != RET_OK)
    {
        return RET_LOW_MEMORY;
    }
    
    *ppDataUnit = pNSSBuf;

    return RET_OK;
}

void EncodedBuffer::SetVideoStream(WORD _wVideoStream)
{
    m_wVideoStream = _wVideoStream;
}

unsigned EncodedBuffer::OnDataUnit(INSSBuffer* pDataUnit)
{
    if (!pDataUnit)
        return RET_INVALID_ARG;

    DWORD dwLen;
    CHECK_ERROR(pDataUnit->GetLength(&dwLen), RET_OK)

    LPBYTE pPayload;
    CHECK_ERROR(pDataUnit->GetBuffer(&pPayload), RET_OK)

    ASFDataPacket packet(dwLen, -1, m_wVideoStream);
    packet.Attach(pPayload);
    packet.Refresh();

    if (GetTickCount() <= m_ulLastPacketTime + MC_ENCODER_ASF_INTERVAL_BETWEEN_PACKETS)
    {     
        //printf("EncodedBuffer.cpp - EncodedBuffer::OnDataUnit - Waiting MC_ENCODER_ASF_INTERVAL_BETWEEN_PACKETS = %u ms...\n", MC_ENCODER_ASF_INTERVAL_BETWEEN_PACKETS);
        Wait(MC_ENCODER_ASF_INTERVAL_BETWEEN_PACKETS);
    }

    m_ulLastPacketTime = GetTickCount();

    DWORD dwFlags = 0;

    if (packet.HasKeyFrame())
    {
        dwFlags |= PKT_KEYFRAME;
    }

    dwFlags |= PKT_VIDEO | PKT_AUDIO;

    BYTE pBuf[MC_MAX_IRM_RAW_PACKETSIZE];

    MediaPkt *pMediaPkt = (MediaPkt *)pBuf;

    pMediaPkt->id       = m_ulMediaId;
    pMediaPkt->datalen  = (WORD) dwLen;
    pMediaPkt->seq      = ++m_ulSeq;
    pMediaPkt->type     = MTYPE_MEDIA;
    pMediaPkt->flags    = (BYTE) dwFlags;

    BYTE *pData = &pBuf[sizeof(MediaPkt)];
    memcpy(pData, pPayload, dwLen);

    unsigned uPktLen = dwLen + sizeof(MediaPkt);

	CHECK_ERROR(m_pSimpSink->OnPacket(pBuf, uPktLen), RET_OK)

    m_ulPktsDelivered++;

    //printf("EncodedBuffer.cpp - EncodedBuffer::OnDataUnit - Packet no. %lu sent, pkt.payload first byte: %x, pPayload first byte: %x\n", m_ulPktsDelivered, *pkt.payload, *pPayload);

    return RET_OK;
}

unsigned EncodedBuffer::OnHeader(INSSBuffer* pHeader)
{
    if (!pHeader)
        return RET_INVALID_ARG;

    DWORD dwLen;
    CHECK_ERROR(pHeader->GetLength(&dwLen), RET_OK)
   
    LPBYTE pBuf;
    CHECK_ERROR(pHeader->GetBuffer(&pBuf), RET_OK)

    BYTE pEnc64Buf[MC_MAX_IRM_RAW_PACKETSIZE];
    int uEnc64BufLen = MC_MAX_IRM_RAW_PACKETSIZE;

    unsigned ret;

    if (encode64((const char *)pBuf, dwLen, (char *)pEnc64Buf, &uEnc64BufLen) == 0)
    {
        pEnc64Buf[uEnc64BufLen] = '\0';
        m_Header.SetData(pEnc64Buf, uEnc64BufLen+1);

		BYTE *pHeader;
		ULONG uHeaderSize;

		if (GetHeader(&pHeader, uHeaderSize) == RET_OK)
		{
            m_pSimpSink->OnHeader(pHeader, uHeaderSize);
		}

        ret = RET_OK;
    }
    else
    {
        ret = RET_ERROR;
    }
    
    return ret;
}

void EncodedBuffer::NotifyEOF()
{
    if (m_pSimpSink)
    {
        m_pSimpSink->OnEOF();
    }
}

unsigned EncodedBuffer::GetHeader(BYTE **_ppHeader, ULONG & _uHeaderLen)
{
    _uHeaderLen = 0;

    if (!_ppHeader)
        return RET_INVALID_ARG;

    *_ppHeader = NULL;

    if (!m_Header.Size())
        return RET_INIT_ERROR;

    *_ppHeader  = m_Header.GetDataPtr();
    _uHeaderLen = m_Header.Size()-1;

    return RET_OK;
}