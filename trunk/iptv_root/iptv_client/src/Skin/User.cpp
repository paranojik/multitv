#include <wx/wx.h>
#include "User.h"
#include "Mode.h"
#include "ModeList.h"

/** Constructor.
 * @param[in] nickName	User nickname. Default: wxEmptyString.
 */
User::User(const wxString &nickName)
{
	SetNickName(nickName);
}

/** Destructor.
 */
User::~User()
{
}

/** Set user nickname.
 * @param[in] nickName	Nickname to set.
 */
void User::SetNickName(const wxString &nickName)
{
	m_nickName = nickName;
}

/** Get user nickname.
 * @return User nickname.
 */
const wxString &User::GetNickName() const
{
	return m_nickName;
}

/** Get the list of user modes currently set.
 * @return Reference to the user's mode list.
 */
const ModeList &User::GetModeList() const
{
	return m_modeList;
}

/** Searches for a specific user mode.
 * @param[in] modeID	ID of the mode.
 * @return true if user has this mode set.
 */
bool User::HasMode(int modeID) const
{
	return m_modeList.HasMode(modeID);
}

/** Sets an user mode.
 * @param[in] mode	Mode to set.
 */
void User::AddMode(const Mode &mode)
{
	m_modeList.AddMode(mode);
}

/** Sets an user mode.
 * @param[in] modeID	ID of the mode to set. (See Mode.h)
 * @param[in] param		Mode parameter string.
 */
void User::AddMode(int modeID, const wxString &param)
{
	m_modeList.AddMode(modeID, param);
}

/** Unsets an user mode.
 * @param[in] mode	Mode to unset.
 */
void User::RemoveMode(const Mode &mode)
{
	m_modeList.RemoveMode(mode.GetID());
}

/** Unsets an user mode.
 * @param[in] modeID	ID of the mode to unset.
 */
void User::RemoveMode(int modeID)
{
	m_modeList.RemoveMode(modeID);
}

/** Base sorting operator for Users. This just sorts alphabetically.
 * @param[in] rhs	Right-hand side of the operator.
 * @return	True if "this" object precedes the right-hand object.
 */
bool User::operator <(const User &rhs) const
{
	// Generic Users are sorted alphabetically
	return (m_nickName < rhs.GetNickName());
}

/** Equality operator. Just compares by nickname.
 */
bool User::operator ==(const User &rhs) const
{
	return (m_nickName == rhs.GetNickName());
}
