#include <wx/wx.h>

#include "PrivateChatWindowList.h"

void PrivateChatWindowList::AddWindow(PrivateChatWindowBase *win)
{
	m_list.push_back(win);
}

void PrivateChatWindowList::RemoveWindow(const wxString &userName)
{
	RemoveWindow(GetWindowByUserName(userName));
}

void PrivateChatWindowList::RemoveWindow(PrivateChatWindowBase *win)
{
	m_list.remove(win);
}

PrivateChatWindowBase *PrivateChatWindowList::GetWindowByUserName(const wxString &userName)
{
	PvtChatIterator it;

	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if((*it)->GetUserName() == userName)
			return *it;
	}
	
	return NULL;	// not found
}
