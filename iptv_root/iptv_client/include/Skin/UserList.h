#ifndef USERLIST_H
#define USERLIST_H

#include <wx/wx.h>
#include <list>

#include "ChannelUser.h"

typedef std::list<ChannelUser> UserListType;
typedef UserListType::const_iterator UserIterator;
typedef UserListType::const_reverse_iterator UserReverseIterator;

enum SubListCriteria
{
	SLC_NONE = 0,
	SLC_HAS_COLLABORATION,
	SLC_SERIES_COLLAB_ENABLED,
	SLC_SERIES_COLLAB_SELECTED
};

/** @brief Handles the user list.
 *
 * This class handles the user list. Is outside-iteratable and has a sub-listing capability.
 */
class UserList
{
private:
	UserListType m_list;
	UserList *m_subList;
public:
	UserList();
	virtual ~UserList();

	void AddUser(const ChannelUser &user);
	ChannelUser *FindUserByName(const wxString &nickname);
	const ChannelUser *FindUserWithMediaVoice() const;
	void RemoveUser(const User &user);
	void RemoveUser(const ChannelUser &user);
	void RemoveUser(const wxString &nickname);
	unsigned GetUserCount() const;
	UserIterator GetBeginIterator() const;
	UserIterator GetEndIterator() const;
	UserReverseIterator GetReverseBeginIterator() const;
	UserReverseIterator GetReverseEndIterator() const;
	const UserList *GetSubList(SubListCriteria criteria);
	void ResetMediaVoice();
	void ClearSeriesCollaboration();
	void Sort();
	void Clear();
};

#endif
