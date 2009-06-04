#ifndef CHANNEL_WINDOW_H
#define CHANNEL_WINDOW_H

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "AppInterface.h"
#include "SettingsManager.h"
#include "Profile.h"

enum
{
	ID_CW_FIRST = wxID_HIGHEST + 100,
	ID_CW_MAIN_PANEL,
	ID_CW_REFRESH_BTN,
	ID_CW_CHANNEL_TXT,
	ID_CW_JOIN_CHANNEL_BTN,
	ID_CW_DISCONNECT_BTN,
	ID_CW_LAST,
	ID_CW_SESSION_TIME_TIMER
};

/** @brief Class thats implements channels list choice window.
*
*/
class ChannelWindow: public wxFrame
{
private:
	AppInterface *m_appInterface;	// Avoids horrid, inintelligible type-casts
	wxPanel *m_mainPanel;
	wxListCtrl *m_channelList;
	wxButton *m_btnRefresh, *m_btnJoinChannel, *m_btnDisconnect;
	wxTextCtrl *m_txtChannel, *m_txtPassword;
	wxTimer m_timer;

	void DoJoinChannel(const wxString &channel, const wxString &password);
public:
	ChannelWindow();
	ChannelWindow(AppInterface *iface, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Channels"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));
	virtual ~ChannelWindow();

	bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Channels"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxT("frame"));

	void CreateControls();

	void UpdateChannelList();
	void SetAppInterface(AppInterface *appInterface);

	// Event handlers
	void OnClose(wxCloseEvent &event);
	void OnListItemSelect(wxListEvent &event);
	void OnListItemDblClick(wxListEvent &event);
	void OnRefreshBtnClick(wxCommandEvent &event);
	void OnJoinChannelBtnClick(wxCommandEvent &event);
	void OnDisconnectBtnClick(wxCommandEvent &event);
	void OnTimer(wxTimerEvent &event);
	void OnChannelTextChange(wxCommandEvent &event);

	void DisableAllControls();
	void EnableAllControls();

	DECLARE_EVENT_TABLE()
};

#endif
