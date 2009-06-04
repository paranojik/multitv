#include <wx/wx.h>

#include "ChatMessage.h"

/** Constructor.
 * @param[in] channel	Channel on which this chat message has been sent.
 * @param[in] sender	Nickname of the sender.
 * @param[in] msg	Message text.
 */
ChatMessage::ChatMessage(const wxString &channel, const wxString &sender, const wxString &msg)
{
	SetChannel(channel);
	SetSender(sender);
	SetMessage(msg);
}

/** Destructor.
 */
ChatMessage::~ChatMessage()
{

}

/** Returns the nickname of the sender.
 * @return The sender's nickname.
 */
const wxString &ChatMessage::GetSender() const
{
	return m_sender;
}

/** Returns the channel on which the message has been sent.
 * @return Channel name.
 */
const wxString &ChatMessage::GetChannel() const
{
	return m_channel;
}

/** Returns the message text.
 * @return Message text.
 */
const wxString &ChatMessage::GetMessage() const
{
	return m_message;
}

/** Set channel name.
 * @param[in] Channel name.
 */
void ChatMessage::SetChannel(const wxString &channel)
{
	m_channel = channel;
}

/** Set sender nickname.
 * @param[in] Sender nickname.
 */
void ChatMessage::SetSender(const wxString &sender)
{
	m_sender = sender;
}

/** Set message text.
 * @param[in] Message text.
 */
void ChatMessage::SetMessage(const wxString &msg)
{
	m_message = msg;
}
