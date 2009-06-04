#include "StatusChat.h"

StatusChat::StatusChat()
{
	m_openedChatFrame = false;
	m_nameChatFrame = wxEmptyString;
}

StatusChat::~StatusChat()
{
}

void StatusChat::SetOpenedChatFrame(const bool &openedChatFrame)
{
	m_openedChatFrame = openedChatFrame;
}

bool StatusChat::GetOpenedChatFrame()
{
	return m_openedChatFrame;
}
	
void StatusChat::SetNameChatFrame(const wxString &nameChatFrame)
{
	m_nameChatFrame = nameChatFrame;
}

wxString StatusChat::GetNameChatFrame()
{
	return m_nameChatFrame;
}
