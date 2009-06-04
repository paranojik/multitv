#include "ChannelUserList.h"

ChannelUserList::ChannelUserList( void )
{
    lpszChannel  = NULL;
    lpszUserList = NULL;
}

ChannelUserList::~ChannelUserList( void )
{
    if( lpszChannel )
        delete [] lpszChannel;

    if( lpszUserList )
        delete [] lpszUserList;
}
