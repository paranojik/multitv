#ifndef CHANNEL_H
#define CHANNEL_H

#include <wx/wx.h>

#include "ModeList.h"
#include "UserList.h"

/** @brief Represents a channel, and manages information about it.
 *
 * This class represents a channel. It stores and manages information about a channel
 * such as number of users, topic, and active channel modes.
 */
class Channel
{
private:
	wxString m_name;
	wxString m_topic;
	unsigned m_userCount;
	UserList m_userList;
	ModeList m_modeList;
public:
	Channel(const wxString &name = wxEmptyString,
			const wxString &topic = wxEmptyString,
			unsigned userCount = 0);
	virtual ~Channel();

	void SetName(const wxString &name);
	void SetTopic(const wxString &topic);
	void SetUserCount(unsigned count);

	const wxString &GetName() const;
	const wxString &GetTopic() const;
	unsigned GetUserCount() const;

	UserList &GetUserList();
	const ModeList &GetModeList() const;

	bool IsMediaVoiceActive() const;

	void AddUser(const ChannelUser &user);
	void AddUser(const wxString &nickname);
	void RemoveUser(const ChannelUser &user);
	void RemoveUser(const wxString &nickname);

	void AddMode(const Mode &mode);
	void RemoveMode(const Mode &mode);
};

#endif
