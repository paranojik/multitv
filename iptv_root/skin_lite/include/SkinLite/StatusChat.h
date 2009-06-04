#ifndef _STATUSCHAT_H_
#define _STATUSCHAT_H_

#include <wx/wx.h>

/**
*@brief This Class checks if chat frame was already opened
*/
class StatusChat
{
private:
	bool m_openedChatFrame;
	wxString m_nameChatFrame;
	
public:
	StatusChat();
	virtual ~StatusChat();

	void SetOpenedChatFrame(const bool &openedChatFrame);
	bool GetOpenedChatFrame();
	
	void SetNameChatFrame(const wxString &nameChatFrame);
	wxString GetNameChatFrame();
};

#endif
