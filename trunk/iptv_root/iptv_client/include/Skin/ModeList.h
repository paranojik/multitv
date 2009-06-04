#ifndef MODELIST_H
#define MODELIST_H

#include <wx/wx.h>
#include <list>

#include "Mode.h"

typedef std::list<Mode> ModeListType;

/** @brief Used to store lists of modes for both channels and users.
 *
 */
class ModeList
{
private:
	 ModeListType m_list;

	 ModeListType::iterator GetModeListItem(int modeID);
	 ModeListType::const_iterator GetModeListItem(int modeID) const;

public:
	virtual ~ModeList();

	bool FindMode(int modeID, Mode &result) const;
	bool HasMode(int modeID) const;
	void AddMode(const Mode &mode);
	void AddMode(int modeID, const wxString &param = wxEmptyString);
	void RemoveMode(int modeID);
	void Clear();
};

#endif
