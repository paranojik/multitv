#include <wx/wx.h>

#include "Channel.h"

/** Constructor.
 * @param[in] name		Channel name. Default: wxEmptyString.
 * @param[in] topic		Channel topic. Default: wxEmptyString.
 * @param[in] userCount	User count. Default: 0
 */
Channel::Channel(const wxString &name, const wxString &topic, unsigned userCount)
{
	m_name = name;
	m_topic = topic;
	m_userCount = userCount;
}

/** Destructor.
 */
Channel::~Channel()
{
	m_modeList.Clear();
	m_userList.Clear();
}

/** Set channel name.
 * @param[in] name	Name to set.
 */
void Channel::SetName(const wxString &name)
{
	m_name = name;
}

/** Set channel topic.
 * @param[in] topic	Topic string to set.
 */
void Channel::SetTopic(const wxString &topic)
{
	m_topic = topic;
}

/** Set user count.
 * @param[in] count	Count to set.
 */
void Channel::SetUserCount(unsigned count)
{
	m_userCount = count;
}

/** Get channel name.
 * @return The channel name.
 */
const wxString &Channel::GetName() const
{
	return m_name;
}

/** Get channel topic.
 * @return The channel topic.
 */
const wxString &Channel::GetTopic() const
{
	return m_topic;
}

/** Get the number of users in channel.
 *
 * This function returns the users count received from channel listing if m_userList is empty,
 * otherwise returns the number of users in the channel's user list.
 *
 * @return The user count.
 */
unsigned Channel::GetUserCount() const
{
	unsigned userListCount = m_userList.GetUserCount();
	if(!userListCount)
		return m_userCount;

	return userListCount;
}

/** Get the user list.
 * @return A reference to the user list.
 * @remarks This reference is non-const because there are cases where the list must be manipulated
 * outside this class.
 */
UserList &Channel::GetUserList()
{
	return m_userList;
}

/** Get the list of channel modes for this channel.
 */
const ModeList &Channel::GetModeList() const
{
	return m_modeList;
}

/** Find out if there is media voice in the channel.
 * @return true if media voice is active in the channel.
 */
bool Channel::IsMediaVoiceActive() const
{
	if(m_userList.FindUserWithMediaVoice())
		return true;
	return false;
}

/** Add an User to the channel.
 * @param[in] user	ChannelUser object to add.
 */
void Channel::AddUser(const ChannelUser &user)
{
	m_userList.AddUser(user);
}

/** Add an User to the channel.
 * @param[in] nickname	Nickname of the user to add. This version creates a new User object.
 */
void Channel::AddUser(const wxString &nickname)
{
	m_userList.AddUser(User(nickname));
}

/** Remove an User from channel.
 * @param[in] user	ChannelUser object to remove.
 */
void Channel::RemoveUser(const ChannelUser &user)
{
	m_userList.RemoveUser(user);
}

/** Remove an User from channel.
 * @param[in] nickname	Nickname of the user to remove.
 */
void Channel::RemoveUser(const wxString &nickname)
{
	m_userList.RemoveUser(nickname);
}

/** Set a channel mode.
 * @param[in] mode	Mode to set.
 */
void Channel::AddMode(const Mode &mode)
{
	m_modeList.AddMode(mode);
}

/** Unset a channel mode.
 * @param[in] mode	Mode to unset.
 */
void Channel::RemoveMode(const Mode &mode)
{
	m_modeList.RemoveMode(mode.GetID());
}
