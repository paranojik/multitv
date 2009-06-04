#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <wx/wx.h>

class ChatMessage
{
private:
	wxString m_channel;
	wxString m_sender;
	wxString m_message;
public:
	ChatMessage(const wxString &channel = wxEmptyString, const wxString &sender = wxEmptyString, const wxString &msg = wxEmptyString);
	virtual ~ChatMessage();

	void SetChannel(const wxString &channel);
	void SetSender(const wxString &sender);
	void SetMessage(const wxString &msg);

	const wxString &GetChannel() const;
	const wxString &GetSender() const;
	const wxString &GetMessage() const;
};

#endif // CHATMESSAGE_H
