#include "ASFHeader.h"

const GUID_W ASFHeader::ASF_Header_Stream_Video_Media = 
{ 0xBC19EFC0, 0x5B4D, 0x11CF, { 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B } };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASFHeader::ASFHeader() : 
    m_dwPacketSize(0), 
    m_dwBitrate(0), 
    m_wVideoStream(0),
    m_qwTimeOffset(0),
    m_qwPreroll(0),
    m_bSeekable(FALSE),
    m_qwTotalPackets(0)
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFHeader::CMCASFHeader"));
	m_Type = MC_ASF_HEADER;
}

ASFHeader::~ASFHeader()
{
	//_TRACE0(LOGLEVEL5, TEXT("CMCASFHeader::~CMCASFHeader"));

}

void ASFHeader::Refresh()
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFHeader::Refresh"));

    LPBYTE p = FindObjectInBuffer(&ASFObject::ASF_File_Properties_Object);

    if (!p)
    {
        //_ERROR0(MCMSG("Header is invalid"));
    }

    m_qwTotalPackets = * ((QWORD*) &p[56]);
    m_dwPacketSize = * ((DWORD*) &p[92]);
    m_qwPreroll = * ((QWORD*) &p[80]);
    m_dwBitrate = (DWORD) * ((QWORD*) &p[100]);

    m_bSeekable = (* ((DWORD*) &p[88])) & 0x2;

    // set broadcast and not seekable flag
    * ((DWORD*) &p[88]) &= 0xFFFFFFFD;

    p = FindObjectInBuffer(&ASFObject::ASF_Stream_Properties_Object, TRUE);

    if (p)
    {
        GUID_W type = * ((GUID_W*) &p[24]);

        if (memcmp(&ASF_Header_Stream_Video_Media, &type, sizeof(GUID_W)) == 0)
        {
            m_wVideoStream = (* ((WORD*) &p[72])) & 0x007F;
        }

	m_qwTimeOffset = * ((QWORD*) &p[56]);
    }
}

void ASFHeader::SetTimeOffset(QWORD qwTimeOffset)
{
    //_TRACE1(LOGLEVEL5, TEXT("CMCASFHeader::SetTimeOffset(%.0f)"), (double) (LONGLONG) qwTimeOffset);

    m_p = FindObjectInBuffer(&ASFObject::ASF_Stream_Properties_Object);
    * ((QWORD*) &m_p[56]) = m_qwTimeOffset = qwTimeOffset;
}

void ASFHeader::SetPreroll(QWORD qwPreroll)
{
    //_TRACE1(LOGLEVEL5, TEXT("CMCASFHeader::SetPreroll(%.0f)"), (double) (LONGLONG) qwPreroll);

    LPBYTE p = FindObjectInBuffer(&ASFObject::ASF_File_Properties_Object);
    if (!p)
    {
        //_ERROR0(MCMSG("Header is invalid"));
    }

    * ((QWORD*) &p[80]) = m_qwPreroll = qwPreroll;
}

LPBYTE ASFHeader::FindObjectInBuffer(const GUID_W* pObjGuid, BOOL bVideoStream)
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFHeader::FindObjectInBuffer"));

    if (!m_p)
    {
        //_ERROR1(MCMSG("Header object (%d) is null"), m_Type);
    }

    // skip top level Header object
    QWORD offset = 30;

    ASFObject obj;

    obj.Attach(m_p + offset);

    while (obj.GetObjectSize() + offset < GetObjectSize())
    {
        // found it
        if (memcmp(pObjGuid, obj.m_p, sizeof(GUID_W)) == 0)
        {
            if (bVideoStream)
            {
                LPBYTE pStreamType = ((LPBYTE) obj.m_p) + 24;

                // look for video stream object
                if (memcmp(&ASF_Header_Stream_Video_Media, pStreamType, sizeof(GUID_W)) == 0)
                {
                    return obj.m_p;
                }
            }
            else
            {
                return obj.m_p;
            }
        }
        offset += obj.GetObjectSize();
        obj.Attach(obj.m_p + obj.GetObjectSize());
    } // end while

    return NULL;
}
