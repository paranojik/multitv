#ifndef __NOTIFY_PKT__
#define __NOTIFY_PKT__

#include <time.h>

#include "shared/SVectorT.h"


struct NotifyRec
{
    time_t  m_ctime;
    BYTE    m_flags;

    NotifyRec(time_t _ctime=0, BYTE _flags=0);

    void init(time_t _ctime, BYTE _flags);

    NotifyRec& operator= (const NotifyRec& obj);
    NotifyRec& operator= (int i);
    bool operator== (time_t _ctime) const;
    bool operator== (const NotifyRec& obj) const;
    bool operator!= (const NotifyRec& obj) const;
    bool operator!= (int i) const;
};


class NotifyVect : public SVectorT<ULONG, NotifyRec>
{
public:

    UINT insert(ULONG _mediaid, time_t _ctime, BYTE _flags);
    BOOL update(ULONG _mediaid, time_t _ctime);
    BOOL update(ULONG _mediaid, time_t _ctime, BYTE _flags);
};


#endif  //__NOTIFY_PKT__
