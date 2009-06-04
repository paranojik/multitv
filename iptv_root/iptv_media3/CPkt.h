#ifndef CPKT_H
#define CPKT_H

#include "media_utilities.h"
#include "mediapkt.h"

class CPkt
{
private:

    BOOL      m_bInit;
    
    MediaPkt  m_MediaPkt;
    MediaSpec m_MediaSpec;
    
    BYTE     *m_pPayload;
    
    ULONG     m_ulTimestamp,
              m_ulPayloadLen,
              m_ulSegments,
              m_ulSubSeq;

public:

    CPkt(BYTE *_pBufPkt, ULONG _bufSize);
    virtual ~CPkt();
	
    // this function must be called to ensure data consistency
    BOOL   PacketOk()        { return m_bInit;            }
    ULONG  GetId()           { return m_MediaPkt.id;      }
    ULONG  GetSeq()          { return m_MediaPkt.seq;	  }
    ULONG  GetDataLen()	     { return m_MediaPkt.datalen; }
    BYTE   GetType()         { return m_MediaPkt.type;	  }
    BYTE   GetFlags()        { return m_MediaPkt.flags;   }
    ULONG  GetTimestamp()    { return m_ulTimestamp;      }
    ULONG  GetSubSeq()       { return m_ulSubSeq;         }
    ULONG  GetSegments()     { return m_ulSegments;       }
    ULONG  GetPayloadLen()   { return m_ulPayloadLen;     }	
    BYTE * GetPayload()	     { return m_pPayload;	  }

    MediaSpec GetMediaSpec() { return m_MediaSpec;        }
};

#endif
