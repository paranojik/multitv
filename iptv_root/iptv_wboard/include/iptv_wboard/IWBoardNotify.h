#if !defined(_IWBOARDNOTIFY)
#define _IWBOARDNOTIFY

#include "WBoardInfo.h"


class IWBoardNotify
{
public:
    virtual void OnWhiteBoardMessage( long lEvent, WB_HMSG pData ) = 0;
};

#endif // !defined(_IWBOARDNOTIFY)
