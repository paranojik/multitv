#include <wx/wx.h>

#include <list>

#include "ChannelUser.h"
#include "UserList.h"

/** Default constructor.
 */
UserList::UserList()
{
	m_subList = NULL;
}

/** Destructor.
 */
UserList::~UserList()
{
	if(m_subList)
	{
		m_subList->m_list.clear();
		delete m_subList;
	}
	Clear();
}

/** Add an User to the list.
 * @param[in] user	User to be added.
 */
void UserList::AddUser(const ChannelUser &user)
{
	m_list.push_back(user);
	Sort();
}

/** Find user by name.
 * @param[in] nickname	Nickname of user to find.
 * @return Pointer to the ChannelUser object if found, or NULL if not found.
 */
ChannelUser *UserList::FindUserByName(const wxString &nickname)
{
	UserListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetNickName() == nickname)
			return &(*it);
	}
	return NULL;
}

/** Find user by media voice flag.
 * @return Pointer to the ChannelUser object if found, or NULL if not found.
 */
const ChannelUser *UserList::FindUserWithMediaVoice() const
{
	UserListType::const_iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->HasMediaVoice())
			return &(*it);
	}
	return NULL;
}

/** Remove user from list.
 * @param[in] user	User to be removed.
 */
void UserList::RemoveUser(const User &user)
{
	RemoveUser(user.GetNickName());
}

/** Remove user from list.
 * @param[in] user	User to be removed.
 */
void UserList::RemoveUser(const ChannelUser &user)
{
	UserListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(*it == user)
		{
			m_list.erase(it);
			break;
		}
	}
	Sort();
}

/** Remove user from list.
 * @param[in] nickname	Nickname of user being removed.
 */
void UserList::RemoveUser(const wxString &nickname)
{
	ChannelUser *user = FindUserByName(nickname);
	if(user)
		RemoveUser(*user);
}

/** Returns the number of users in list.
 * @return The user count.
 */
unsigned UserList::GetUserCount() const
{
	return (unsigned)m_list.size();
}

/** Returns an iterator to the beginning of the list.
 * @return An iterator to the beginning of the list.
 */
UserIterator UserList::GetBeginIterator() const
{
	return m_list.begin();
}

/** Returns an iterator to the end of the list.
 * @return An iterator to the end of the list.
 */
UserIterator UserList::GetEndIterator() const
{
	return m_list.end();
}

/** Returns an iterator to the reverse beginning of the list.
 * @return An iterator to the reverse beginning of the list.
 */
UserReverseIterator UserList::GetReverseBeginIterator() const
{
	return m_list.rbegin();
}

/** Returns an iterator to the reverse end of the list.
 * @return An iterator to the reverse end of the list.
 */
UserReverseIterator UserList::GetReverseEndIterator() const
{
	return m_list.rend();
}

/** Returns a sub-list based on criteria (enumerated in UserList.h, defined within).
 * @param[in] criteria	Criteria to use to make the sub-list (see UserList.h).
 * @return A pointer to the generated sub-list.
 * @remarks Do not delete the pointer returned!! The sub-list is managed internally.
 */
const UserList *UserList::GetSubList(SubListCriteria criteria)
{
	UserIterator it;
	if(m_subList)
		m_subList->m_list.clear();
	else
		m_subList = new UserList;

	switch(criteria)
	{
		case SLC_NONE:		// No criteria -- subList equals m_list
			return this;

		case SLC_HAS_COLLABORATION:		// List all users who are currently collaborating - does not include media voice
		{
			for(it = m_list.begin(); it != m_list.end(); it++)
			{
				if(it->IsChannelCollaborator() && !(it->HasMediaVoice()))
					m_subList->AddUser(*it);
			}
		}
		break;

		case SLC_SERIES_COLLAB_ENABLED:		// Users who may participate in Series Collaboration
		{
			for(it = m_list.begin(); it != m_list.end(); it++)
			{
				if(!(it->HasMediaVoice()) && it->HasMode(IRM_MODE_USER_CAMERA_ON)) // To participate, one must have camera enabled and not have media voice
					m_subList->AddUser(*it);
			}
		}
		break;

		case SLC_SERIES_COLLAB_SELECTED:	// Users who were selected by the 'mediator' to participate in Series Collaboration
		{
			for(it = m_list.begin(); it != m_list.end(); it++)
			{
				if(it->IsInSeriesCollaboration())
					m_subList->AddUser(*it);
			}
		}
		break;

		default:
			return NULL;
	}
	return m_subList;
}

/** Clears the "series collaboration" flag for all users in list.
 */
void UserList::ClearSeriesCollaboration()
{
	UserListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		it->SetSeriesCollaboration(false);
	}
}

/** Resets the "media voice" flag for all users in list.
 */
void UserList::ResetMediaVoice()
{
	UserListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->HasMediaVoice())
			it->SetMediaVoice(false);
	}
}

/** Sorts the list. For sorting criteria, see ChannelUser::operator <.
 */
void UserList::Sort()
{
	m_list.sort();
}

/** Clears the list.
 */
void UserList::Clear()
{
	ChannelUser::ResetOrderingCounters();
	m_list.clear();
}
