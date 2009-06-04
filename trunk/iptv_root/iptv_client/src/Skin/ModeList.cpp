#include <wx/wx.h>
#include <list>

#include "Mode.h"
#include "ModeList.h"

/** Default constructor.
 */
ModeList::~ModeList()
{

}

// Private methods
ModeListType::iterator ModeList::GetModeListItem(int modeID)
{
	ModeListType::iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetID() == modeID)
			return it;
	}
	return it;
}

ModeListType::const_iterator ModeList::GetModeListItem(int modeID) const
{
	ModeListType::const_iterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(it->GetID() == modeID)
			return it;
	}
	return it;
}
// Public methods

/** Tries to find a mode in list with given ID.
 * @param[in]  modeID	Mode ID to look for.
 * @param[out] result	Mode object that receives the result. Left unchanged if not found.
 * @return true if mode is found, false otherwise.
 */
bool ModeList::FindMode(int modeID, Mode &result) const
{
	ModeListType::const_iterator it = GetModeListItem(modeID);
	// If GetModeListItem didn't reach m_list.end(), we found it.
	if(it != m_list.end())
	{
		result = *it;
		return true;
	}
	return false;
}

/** Returns true if a mode with given ID is in the list.
 * @param[in] modeID	Mode ID.
 * @return true if mode is found.
 */
bool ModeList::HasMode(int modeID) const
{
	Mode mode;
	return FindMode(modeID, mode);
}

/** Adds a mode to the list. If a mode with the same ID is present, updates its parameter.
 * @param[in] mode	Mode to add.
 */
void ModeList::AddMode(const Mode &mode)
{
	// If mode is already present in list, update parameter only.
	ModeListType::iterator it = GetModeListItem(mode.GetID());
	if(it != m_list.end())
	{
		if(mode.GetParameter() != wxEmptyString)	// If parameter comes in blank, leave current value as is
			it->SetParameter(mode.GetParameter());
	}
	else	// If mode is not in list, add it.
		m_list.push_back(mode);
}

/** Adds a mode to the list. If a mode with the same ID is present, updates its parameter.
 * @param[in] modeID	ID of the mode to add.
 * @param[in] parameter	Mode parameter string.
 */
void ModeList::AddMode(int modeID, const wxString &param)
{
	AddMode(Mode(modeID, param));
}

/** Removes a mode from the list.
 * @param[in] modeID	ID of the mode to remove.
 */
void ModeList::RemoveMode(int modeID)
{
	ModeListType::iterator it = GetModeListItem(modeID);
	if(it != m_list.end())
		m_list.erase(it);
}

/** Clears the mode list.
 */
void ModeList::Clear()
{
	m_list.clear();
}
