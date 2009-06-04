#ifndef _CHANNEL_USER_LIST_H
#define _CHANNEL_USER_LIST_H

#include "shared/compat.h"

class ChannelUserList
{
    public:
        char *lpszChannel;
        char *lpszUserList;

        ChannelUserList(  void );
        ~ChannelUserList( void );
};

#endif  // _CHANNEL_USER_LIST_H

