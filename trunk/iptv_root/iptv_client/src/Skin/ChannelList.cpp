#include <wx/wx.h>
#include <list>

#include "Channel.h"
#include "ChannelList.h"

/** Add a channel to the list.
 * @param[in] Channel object to add.
 */
void ChannelList::AddChannel(const Channel &channel)
{
	ChannelListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetName() == channel.GetName())
		{
			*it = channel;
			return;
		}
	}
	m_list.push_back(channel);
}

/** Add a channel to the list.
 * @param[in] Channel name. This creates a new Channel object.
 */
void ChannelList::AddChannel(const wxString &channelName)
{
	AddChannel(Channel(channelName));
}

/** Find a Channel object in the list by its name.
 * @param[in]  channelName	Channel name
 * @param[out] result		Search result. Is left untouched if not found.
 * @return true if found, false if not found.
 */
bool ChannelList::FindChannelByName(const wxString &channelName, Channel &result)
{
	ChannelListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetName() == channelName)
		{
			result = *it;
			return true;
		}
	}
	return false;
}

/** Remove channel from the list.
 * @param[in] channel	Channel object to remove.
 */
void ChannelList::RemoveChannel(const Channel &channel)
{
	RemoveChannel(channel.GetName());
}

/** Remove channel from the list.
 * @param[in] channelName	Name of the channel to remove.
 */
void ChannelList::RemoveChannel(const wxString &channelName)
{
	ChannelListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetName() == channelName)
		{
			m_list.erase(it);
			return;
		}
	}
}

/** Returns an iterator to the beginning of the channel list.
 * @return The begin iterator.
 */
ChannelIterator ChannelList::GetBeginIterator() const
{
	return m_list.begin();
}

/** Returns an iterator to the end of the channel list.
 * @return The end iterator.
 */
ChannelIterator ChannelList::GetEndIterator() const
{
	return m_list.end();
}

/** Clear the list.
 */
void ChannelList::Clear()
{
	m_list.clear();
}
