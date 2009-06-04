#ifndef PRIVATECHATWINDOWBASE_H
#define PRIVATECHATWINDOWBASE_H

#include <wx/wx.h>

/** @brief Base class for the Private Chat pop-up windows.
 *
 */
class PrivateChatWindowBase
{
private:
	wxString m_userName;

public:
	virtual ~PrivateChatWindowBase();

	void SetUserName(const wxString &name);
	const wxString &GetUserName() const;

	virtual void SetMessage(const wxString &user, const wxString &message) = 0;

	virtual void OpenWindow() = 0;
	virtual void CloseWindow() = 0;
};

#endif
