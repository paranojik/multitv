#ifndef USER_H
#define USER_H

#include <wx/wx.h>
#include "Mode.h"
#include "ModeList.h"

/** @brief Stores user information.
*
* This class stores user information (basically, the nick name and user modes). It
* also deals with the user list ordering rules (see User::operator <).
*/
class User
{
protected:
	wxString m_nickName;
	ModeList m_modeList;	
public:
	User(const wxString &nickName = wxEmptyString);
	virtual ~User();

	void SetNickName(const wxString &nickName);

	const wxString &GetNickName() const;

	bool HasMode(int modeID) const;
	const Mode &GetMode(int modeID) const;
	const ModeList &GetModeList() const;
	void AddMode(const Mode &mode);
	void AddMode(int modeID, const wxString &param = wxEmptyString);
	void RemoveMode(const Mode &mode);
	void RemoveMode(int modeID);

	bool operator <(const User &rhs) const;
	bool operator ==(const User &rhs) const;
};

#endif
