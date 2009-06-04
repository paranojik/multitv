#ifndef __MEDIA_BUF__
#define __MEDIA_BUF__

#include <time.h>

#include "semaphore.h"


class MediaBuf
{
public:

    MediaBuf(ULONG _maxblen, BOOL _flLMediaPkt=FALSE, BOOL _flRealloc=FALSE, BOOL _flBlock=TRUE);
   ~MediaBuf(void);

    BOOL  IsOk (void) { return (m_pszBuf ? TRUE:FALSE); }

    void  reset(void);

    void  SetMediaId     (ULONG _mediaid);

    ULONG GetMediaId     (void) { return m_dwmediaid;              }
    ULONG GetBytesLeft   (void) { return (m_dwmaxblen - m_dwblen); }
    BOOL  CanAbort       (void) { return m_flAbort; }
    BOOL  CanBlock       (void) { return m_flBlock; }
    void  Abort          (void);

    BOOL  SaveData       (const void *_pdataBuf, ULONG _szdata);
    BOOL  GetData        (void **_pdataBuf, ULONG *_szdataBuf, BOOL _flRealloc);

private:

    BOOL   m_flAbort,
           m_flBlock,
           m_flRealloc,
           m_flLMediaPkt,
           m_flWaitingForData;

    LPSTR  m_pszBuf;

    ULONG  m_dwblen,
           m_dwmaxblen,
           m_dwmediaid,
           m_nPktLost,
           m_dwLastPktSeq;

    _SEMAPHORE *m_pMutex;
    _SEMAPHORE *m_pWaitForDataSemaph;
    _SEMAPHORE *m_pGettingDataSemaph;


    void ResetWaitingForData(void);

    UINT IsPacketRdy(void);
};


#endif  // __MEDIA_BUFF__
