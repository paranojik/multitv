#ifndef CFRAME_H
#define CFRAME_H

#include "media_utilities.h"

class CFrame
{
private:

    void SetDefaultValues(MediaSpec _mediaSpec);

protected:

    BOOL   m_bInit;

    BYTE  *m_pData;

    ULONG  m_ulDatalen,
	   m_ulTimestamp,
           m_ulBufferLen,
           m_Id;

    MediaSpec m_MediaSpec;

    ULONG SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp);

    CFrame(MediaSpec _mediaSpec, BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp);
    CFrame(CFrame & Frame);
    CFrame(MediaSpec _mediaSpec);

public:

    virtual ~CFrame();

    CFrame & operator=(CFrame & Frame);
    ULONG GetFrameData(BYTE **_ppData, ULONG *_pDatalen);
    ULONG GetTimestamp(ULONG *_pulTimestamp);
    ULONG GetDatalen  (ULONG *_pulDatalen);
    ULONG GetMediaSpec(MediaSpec * _pMediaSpec); 

    void  AttachData(BYTE *_pData, ULONG _ulDatalen)
    {
        m_bInit     = TRUE;
	    m_pData     = _pData;
        m_ulDatalen = _ulDatalen;
    }

    void SetId(ULONG _id)  { m_Id = _id; }
    ULONG GetId()          { return m_Id; }

    BOOL FrameOk() {return m_bInit; }
};

#endif
