#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#include <wx/wx.h>
#include <list>

#include "Channel.h"

typedef std::list<Channel> ChannelListType;
typedef ChannelListType::const_iterator ChannelIterator;

/** @brief Stores the list of channels.
 *
 * This class stores the list of channels. Can search by name and is iteratable on the outside.
 *
 */
class ChannelList
{
private:
	ChannelListType m_list;
public:
	void AddChannel(const Channel &channel);
	void AddChannel(const wxString &channelName);
	bool FindChannelByName(const wxString &channelName, Channel &result);
	void RemoveChannel(const Channel &channel);
	void RemoveChannel(const wxString &channelName);
	ChannelIterator GetBeginIterator() const;
	ChannelIterator GetEndIterator() const;
	void Clear();
};

#endif
