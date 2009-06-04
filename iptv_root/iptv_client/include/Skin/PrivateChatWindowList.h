#ifndef PRIVATECHATWINDOWLIST_H
#define PRIVATECHATWINDOWLIST_H

#include <list>
#include <wx/wx.h>

#include "PrivateChatWindowBase.h"

typedef std::list<PrivateChatWindowBase *> PvtChatList;
typedef PvtChatList::iterator PvtChatIterator;

/** @brief Manages a list of the Private Chat windows.
 *
 */
class PrivateChatWindowList
{
private:
	PvtChatList m_list;

public:
	void AddWindow(PrivateChatWindowBase *win);
	// Remove may be either by user name or explicit
	void RemoveWindow(PrivateChatWindowBase *win);
	void RemoveWindow(const wxString &userName);
	// Find a window by user name
	PrivateChatWindowBase *GetWindowByUserName(const wxString &userName);
	// Clear the whole list
	void ClearList();
};

#endif
