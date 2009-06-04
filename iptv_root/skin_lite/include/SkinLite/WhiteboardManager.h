#ifndef WHITEBOARD_MANAGER_H
#define WHITEBOARD_MANAGER_H

#include <stdio.h>
#include "iptv_wboard/IWBoardNotify.h"
#include "iptv_wboard/WBoardDll.h"
#include "iptv_wboard/WBoardInfo.h"
#include "AppInterface.h"
#include "SessionWindow.h"
#include <wx/wx.h>

class SessionWindow;

/** @brief Class thats manages the whiteboard.
*
*/
class WhiteboardManager : public IWBoardNotify
{
public:
	WhiteboardManager(AppInterface *iface, SessionWindow *sessionWindow);
	~WhiteboardManager();

	void CreateWhiteboardFrame(wxWindow* parent, int id = wxID_ANY);
	void ShowWhiteboardFrame();
	void HideWhiteboardFrame();
	void Enable();
	void Disable();
	void EnableImageControl();
	void DisableImageControl();
	void OnWhiteboardChannelMessage(const wxString &channel, const wxString &user, const wxString &data);

private:
	void OnWhiteBoardMessage(long lEvent, WB_HMSG pData);
	WBoardModule *m_whiteboardModule;
	AppInterface *m_appInterface;
	SessionWindow *m_sessionWindow;
	static const long m_maxWhiteboardMessageSize;
};

#endif // WHITEBOARD_MANAGER_H
