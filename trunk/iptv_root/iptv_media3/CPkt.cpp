#include "compat.h"
#include "CPkt.h"
#include "global_error.h"
#include "Global_functions.h"


CPkt::CPkt(BYTE *_pBufPkt, ULONG _ulBufSize) :	
    m_bInit(FALSE), 
    m_pPayload(NULL),
    m_ulTimestamp(0),
    m_ulSubSeq(0),
    m_ulPayloadLen(0),
    m_ulSegments(1),
    m_MediaSpec(NONE)
{

    memset(&m_MediaPkt, 0, sizeof(MediaPkt));

    if (_pBufPkt && _ulBufSize && _ulBufSize > sizeof(MediaPktExt))
    {
        memcpy(&m_MediaPkt, _pBufPkt, sizeof(MediaPkt));
		
        MediaPktExt *pMediaPktExt = (MediaPktExt *) _pBufPkt;	
		
        m_ulTimestamp = pMediaPktExt->dwMsTimestamp;
        m_ulSubSeq    = pMediaPktExt->ulSubSeq;

        if (::GetMediaSpec(m_MediaPkt.type, &m_MediaSpec) == RET_OK)
        {
            m_ulPayloadLen = _ulBufSize - sizeof(MediaPktExt);

            m_pPayload = new BYTE [m_ulPayloadLen];
            BYTE *pPayload = &_pBufPkt[_ulBufSize - m_ulPayloadLen];

            if (m_pPayload)
            {	
                memcpy(m_pPayload, pPayload, m_ulPayloadLen);
                m_bInit = TRUE;
            }
        }
    }
}

CPkt::~CPkt()
{
    if (m_pPayload) 
        delete m_pPayload;
}
