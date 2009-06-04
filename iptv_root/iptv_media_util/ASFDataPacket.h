#ifndef ASF_DATA_PACKET_H
#define ASF_DATA_PACKET_H

#include "ASFObject.h"
#include "defs.h"

class ASFDataPacket : public ASFObject
{
    struct PAYLOAD_INFO 
    {
        BOOL bVideoKeyFrame;
	     DWORD dwSequence;
	     DWORD dwSendTime;
	     WORD wDuration;
	     WORD wPacketSize;
             WORD wVideoStream;
    };

public:
    ASFDataPacket(DWORD dwSize, DWORD dwSeq = -1, WORD wVideoStream = 0);
    virtual ~ASFDataPacket();
   
    QWORD GetObjectSize()       { return (QWORD) m_info.wPacketSize; }
    DWORD GetPresentationTime() { return m_info.dwSendTime; }
    
    BOOL HasKeyFrame()          { return m_info.bVideoKeyFrame; }

// overriden from ASFObject
    void Refresh();

private:
    PAYLOAD_INFO m_info;
};

#endif

