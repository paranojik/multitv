#include "ASFDataPacket.h"

ASFDataPacket::ASFDataPacket(DWORD dwSize, DWORD dwSeq, WORD wVideoStream)
{
    m_Type = MC_ASF_DATAPACKET;
    m_info.dwSequence = dwSeq;
    m_info.wDuration = 0;
    m_info.dwSendTime = -1;
    m_info.wPacketSize = (WORD) dwSize;
    m_info.wVideoStream = wVideoStream;
    m_info.bVideoKeyFrame = FALSE;
}

ASFDataPacket::~ASFDataPacket()
{

}

#define get_byte(p) ((DWORD) * ((BYTE*) (p + offset))); offset++;
#define get_le16(p) ((DWORD) * ((WORD*) (p + offset))); offset += 2;
#define get_le32(p) ((DWORD) * ((DWORD*) (p + offset))); offset += 4;

#define DO_2BITS(bits, var, defval) \
    switch (bits & 3) \
    { \
    case 3: var = get_le32(m_p); break; \
    case 2: var = get_le16(m_p); break; \
    case 1: var = get_byte(m_p); break; \
    default: var = defval; break; \
	}

void ASFDataPacket::Refresh()
{
    //_TRACE0(LOGLEVEL5, TEXT("CMCASFDataPacket::Refresh()"));
	
    int offset = 0;
    ULONG packet_length, packet_flags, padsize, payload_flags;
    ULONG payload_stream, cPayloads, payload_rep_data;
    ULONG payload_length;

    int c = get_byte(m_p);

    if (c != 0x82) 
    {
        //_ERROR2(MCMSG("BAD HRD %x  at:%Ld\n"), c, offset);
    }

    if ((c & 0x0f) == 2) 
    { // always true for now
        int z = get_le16(m_p);
        if (z != 0) 
        {
            //_ERROR0(MCMSG("mc asf BAD NO ZERO"));
        }
    }

    packet_flags = get_byte(m_p);

    //packet_property
    get_byte(m_p);

    DO_2BITS(packet_flags >> 5, packet_length, m_info.wPacketSize);
    DO_2BITS(packet_flags >> 1, padsize, 0); // sequence ignored
    DO_2BITS(packet_flags >> 3, padsize, 0); // padding length

    m_info.dwSendTime = get_le32(m_p);
    m_info.wDuration = (WORD) get_le16(m_p); /* duration */

    payload_flags = get_byte(m_p);

    // multiple payloads
    if (packet_flags & 0x1)
    {
        cPayloads = payload_flags & 0x3F;

        payload_flags = get_byte(m_p);
    }
    else
    {
        cPayloads = 1;
    }

    while (cPayloads--)
    {
        payload_stream = payload_flags & 0x7F;

        // offset to next payload
        // Media Object Number
        c = get_byte(m_p);

        // Offset Into Media Object
        c = get_le32(m_p);

        // means it is the first payload fragment
        // belonging to a specific media object
        // we should not count all packets that contain
        // a keyframe media object as "keyframe", only
        // the first one
        if ((payload_stream == m_info.wVideoStream)
            && (payload_flags & 0x80)
            && (c == 0))
        {
            m_info.bVideoKeyFrame = TRUE;
            break;
        }
        
        // Replicated Data Length
        payload_rep_data = get_byte(m_p);

        offset += payload_rep_data;

        payload_length = get_le16(m_p);
        
        offset += payload_length;

        if (cPayloads)
        {
            // get next payload flags
            payload_flags = get_byte(m_p);
        }
    }

    //_TRACE3(LOGLEVEL3, TEXT("Read packet with timestamp (%d ms) and packetsize (%d) (%d keyframe)"),
    //	m_info.dwSendTime, m_info.wPacketSize, m_info.bVideoKeyFrame);
    // rsize has at least 11 bytes which have to be present
}

