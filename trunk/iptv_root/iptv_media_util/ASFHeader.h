#ifndef ASF_HEADER_H
#define ASF_HEADER_H

#include "ASFObject.h"

class ASFHeader : public ASFObject  
{
public:
    ASFHeader();
    virtual ~ASFHeader();

    DWORD GetPacketSize() { return m_dwPacketSize; }
    QWORD GetTimeOffset() { return m_qwTimeOffset; }
    void SetTimeOffset(QWORD qwTimeOffset);
    QWORD GetPreroll() { return m_qwPreroll; }
    void SetPreroll(QWORD qwPreroll);
    DWORD GetBitrate() { return m_dwBitrate; }
    WORD GetVideoStream() { return m_wVideoStream; }
    BOOL IsSeekable() { return m_bSeekable; }
    QWORD GetTotalPackets() { return m_qwTotalPackets; }

    void Refresh();

    // ASF_Video_Media	
    // {BC19EFC0-5B4D-11CF-A8FD-00805F5C442B}
	
    static const GUID_W ASF_Header_Stream_Video_Media;

protected:
    
    LPBYTE FindObjectInBuffer(const GUID_W* pObjGuid, BOOL bVideoStream = FALSE);

    DWORD m_dwPacketSize;
    QWORD m_qwTimeOffset;
    QWORD m_qwPreroll;
    DWORD m_dwBitrate;
    WORD m_wVideoStream;
    BOOL m_bSeekable;
    QWORD m_qwTotalPackets;
};

#endif
