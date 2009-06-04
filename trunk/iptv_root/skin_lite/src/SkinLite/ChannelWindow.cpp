#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/settings.h>

#define SKIN_API
#include "Skin.h"
#include "SkinLite.h"
#include "AppInterface.h"
#include "ChannelWindow.h"
#include "SessionWindow.h"
#include "SkinLiteConstants.h"
#include "MessageDialog.h"

#define CW_LISTCTRL_MIN_SIZE	wxSize(-1, 120)
#define CW_MAIN_PANEL_MIN_SIZE	wxSize(480, -1)

BEGIN_EVENT_TABLE(ChannelWindow, wxFrame)
	EVT_CLOSE(ChannelWindow::OnClose)
	EVT_LIST_ITEM_SELECTED(wxID_ANY, ChannelWindow::OnListItemSelect)
	EVT_LIST_ITEM_ACTIVATED(wxID_ANY, ChannelWindow::OnListItemDblClick)
	EVT_BUTTON(ID_CW_REFRESH_BTN, ChannelWindow::OnRefreshBtnClick)
//	EVT_BUTTON(ID_CW_JOIN_CHANNEL_BTN, ChannelWindow::OnJoinChannelBtnClick)
	EVT_BUTTON(ID_CW_DISCONNECT_BTN, ChannelWindow::OnDisconnectBtnClick)
	EVT_TIMER(ID_CW_SESSION_TIME_TIMER, ChannelWindow::OnTimer)
	EVT_TEXT(ID_CW_CHANNEL_TXT, ChannelWindow::OnChannelTextChange)
END_EVENT_TABLE()

/** ChannelWindow default constructor.
*
*/
ChannelWindow::ChannelWindow()
{
	m_appInterface = NULL;
}

/** ChannelWindow constructor.
*
*/
ChannelWindow::ChannelWindow(AppInterface *iface, wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
	SetAppInterface(iface);
	Create(parent, id, title, pos, size, style, name);
}

ChannelWindow::~ChannelWindow()
{
}

/** Sets linkage with application.
* @param[in] appInterface Application interface
*/
void ChannelWindow::SetAppInterface(AppInterface *appInterface)
{
	m_appInterface = appInterface;
}

/** ChannelWindow creation.
* 
*/
bool ChannelWindow::Create(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
	wxASSERT_MSG(m_appInterface, wxT("Please use ChannelWindow::SetAppInterface before calling ChannelWindow::Create."));
	bool ret = wxFrame::Create(parent, id, title, pos, size, style, name);
	if(ret)	
	{
		wxIcon icon = wxIcon(S_LITE_ICON_FILENAME, wxBITMAP_TYPE_ICO);
		SetIcon(icon);

		SetMinSize(CW_WINDOW_SIZE);
		SetWindowStyle(wxDEFAULT_FRAME_STYLE);
		SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		CreateControls();
		Centre();

		m_appInterface->RequestChannelList();
		m_btnRefresh->Disable();
		m_timer.SetOwner(this, ID_CW_SESSION_TIME_TIMER);
		m_timer.Start(CW_SESSIONTIME_TIMER_INTERVAL);
	}
	return ret;
}

void ChannelWindow::CreateControls()
{
	// Create one vertical sizer for the whole window
	wxSizer *windowSizer = new wxBoxSizer(wxVERTICAL);
	// and another for the main panel (contained within the window)
	wxSizer *mainPanelSizer = new wxBoxSizer(wxVERTICAL);

	// Set this as the window's sizer
	SetSizer(windowSizer);
	// Create a status bar
	CreateStatusBar();

	// Create main panel
	m_mainPanel = new wxPanel(this, ID_CW_MAIN_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	windowSizer->Add(m_mainPanel, 1, wxGROW, 5);
	m_mainPanel->SetSizer(mainPanelSizer);

	// Now, we'll create sizers with static boxes around them.
	// Create a vertical sizer for the list and 'Refresh' button...
	wxSizer *listSizer = new wxStaticBoxSizer(wxVERTICAL, m_mainPanel, _("Channel List"));
	// an horizontal sizer, which will contain three vertical ones:
	wxSizer *chanBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, m_mainPanel, _("Selected Channel"));
	// one for the input box labels...
	wxSizer *chanLabelSizer = new wxBoxSizer(wxVERTICAL);
	// one for the "Channel" and "Password" input boxes...
	wxSizer *chanInputSizer = new wxBoxSizer(wxVERTICAL);
	// and the other for the "Join" button.
	wxSizer *chanButtonSizer = new wxBoxSizer(wxVERTICAL);
	// and another one for the 'Disconnect' button.
	wxSizer *discSizer = new wxBoxSizer(wxHORIZONTAL);


	// Create the list control and button
	m_channelList = new wxListCtrl(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
	m_channelList->SetInitialSize(CW_LISTCTRL_MIN_SIZE);
	m_channelList->SetMinSize(CW_LISTCTRL_MIN_SIZE);
	m_btnRefresh = new wxButton(m_mainPanel, ID_CW_REFRESH_BTN, CW_BTN_REFRESH_LABEL);
	// Add them to their sizer
	listSizer->Add(m_channelList, 1, wxALL | wxEXPAND | wxALIGN_CENTER, 0);
	listSizer->Add(m_btnRefresh, 0, wxTOP | wxALIGN_RIGHT, 4);
	listSizer->Layout();

	// Create the Channel controls:
	// a "Channel:" label...
	wxStaticText *lblChannel = new wxStaticText(m_mainPanel, wxID_ANY, CW_LBL_CHANNEL_LABEL);
	wxSize sz = lblChannel->GetSize();
	lblChannel->SetInitialSize(wxSize(-1, sz.GetHeight()));
	lblChannel->SetMaxSize(wxSize(-1, sz.GetHeight()));	// Done so that the sizer only resizes the label horizontally
	// a "Password:" label...
	wxStaticText *lblPassword = new wxStaticText(m_mainPanel, wxID_ANY, _("Password:"));
	sz = lblPassword->GetSize();
	lblPassword->SetInitialSize(wxSize(-1, sz.GetHeight()));
	lblPassword->SetMaxSize(wxSize(-1, sz.GetHeight()));
	// the channel text input control...
	m_txtChannel = new wxTextCtrl(m_mainPanel, ID_CW_CHANNEL_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sz = m_txtChannel->GetSize();
	m_txtChannel->SetInitialSize(wxSize(-1, sz.GetHeight()));
	m_txtChannel->SetMaxSize(wxSize(-1, sz.GetHeight()));	// Done so that the sizer only resizes the label horizontally
	// the password text input control...
	m_txtPassword = new wxTextCtrl(m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxTE_PROCESS_ENTER);
	sz = m_txtPassword->GetSize();
	m_txtPassword->SetInitialSize(wxSize(-1, sz.GetHeight()));
	m_txtPassword->SetMaxSize(wxSize(-1, sz.GetHeight()));	// Done so that the sizer only resizes the label horizontally
	// and the Join Channel button.
	m_btnJoinChannel = new wxButton(m_mainPanel, ID_CW_JOIN_CHANNEL_BTN, CW_BTN_JOINCHANNEL_LABEL);
	sz = m_btnJoinChannel->GetSize();
	m_btnJoinChannel->SetMaxSize(sz);	// Done so that the sizer only resizes the label horizontally
	m_btnJoinChannel->Enable(false);
	m_btnJoinChannel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ChannelWindow::OnJoinChannelBtnClick), NULL, this);
	// Connect the text controls' TEXT_ENTER event to the same function as clicking the "Join" button
	m_txtChannel->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ChannelWindow::OnJoinChannelBtnClick), NULL, this);
	m_txtPassword->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ChannelWindow::OnJoinChannelBtnClick), NULL, this);
	// Add them to their sizers
	chanLabelSizer->AddSpacer(2);
	chanLabelSizer->Add(lblChannel, 1, wxRIGHT | wxBOTTOM | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 4);
	chanLabelSizer->AddSpacer(2);
	chanLabelSizer->Add(lblPassword, 1, wxRIGHT | wxBOTTOM | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 4);
	chanInputSizer->Add(m_txtChannel, 1, wxEXPAND | wxALIGN_CENTER, 0);
	chanInputSizer->AddSpacer(4);
	chanInputSizer->Add(m_txtPassword, 1, wxEXPAND | wxALIGN_CENTER, 0);
	chanButtonSizer->Add(m_btnJoinChannel, 1, wxLEFT | wxFIXED_MINSIZE | wxALIGN_CENTER, 4);
	chanBoxSizer->Add(chanLabelSizer, 0, wxALL | wxEXPAND, 4);
	chanBoxSizer->Add(chanInputSizer, 1, wxALL | wxEXPAND, 4);
	chanBoxSizer->Add(chanButtonSizer, 0, wxALL | wxEXPAND, 4);
	chanBoxSizer->Layout();
	chanButtonSizer->Layout();
	chanInputSizer->Layout();
	chanLabelSizer->Layout();

	// Create the Disconnect button
	m_btnDisconnect = new wxButton(m_mainPanel, ID_CW_DISCONNECT_BTN, CW_BTN_DISCONNECT_LABEL);
	// Add to the sizer
	discSizer->AddStretchSpacer();
	discSizer->Add(m_btnDisconnect, 0, wxALL | wxALIGN_CENTER, 0);

	// Add the three sizers to the window's sizer
	mainPanelSizer->Add(listSizer, 1, wxALL | wxEXPAND | wxALIGN_CENTER, 8);
	mainPanelSizer->Add(chanBoxSizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER, 8);
	mainPanelSizer->Add(discSizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER, 8);

	// Compute layout
	mainPanelSizer->SetMinSize(CW_MAIN_PANEL_MIN_SIZE);
	mainPanelSizer->Layout();
	// Fit window to sizer
	mainPanelSizer->SetSizeHints(this);
	Fit();

	// Load info from profile
	Profile profile = m_appInterface->GetSettingsManager()->GetSelectedProfile();
	m_txtChannel->SetValue(profile.GetChannelName());
	m_txtPassword->SetValue(profile.GetChannelPassword());

	m_txtChannel->SetFocus();
}

/** Update Channels list control.
*
*/
void ChannelWindow::UpdateChannelList()
{
	const int numColumns = 3;
	//Delete all items
	m_channelList->ClearAll();

	// Create columns
	wxListItem itemCol;

	//Divide headers equally
	int columnWidth = m_channelList->GetSize().GetWidth() / numColumns;
	
	//Column Channels
	itemCol.SetText(CW_LST_COL_CHANNEL);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_channelList->InsertColumn(0, itemCol);
	m_channelList->SetColumnWidth(0, columnWidth);
	
	//Column Users
	itemCol.SetText(CW_LST_COL_USERS);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_channelList->InsertColumn(1, itemCol);
	m_channelList->SetColumnWidth(1, columnWidth);
	
	//Column Topic
	itemCol.SetText(CW_LST_COL_TOPIC);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_channelList->InsertColumn(2, itemCol);
	m_channelList->SetColumnWidth(2, columnWidth);

	// Fill the channels list.
	ChannelList &list = m_appInterface->GetChannelList();
	ChannelIterator it;
	long i = 0;
	for(it = list.GetBeginIterator(); it != list.GetEndIterator(); it++)
	{
		itemCol.SetText(it->GetName());
		itemCol.SetData(i);
		itemCol.SetColumn(0);
		m_channelList->InsertItem(itemCol);

		wxString ucnt;
		ucnt << it->GetUserCount();
		itemCol.SetText(ucnt);
		itemCol.SetData(i);
		itemCol.SetColumn(1);
		m_channelList->SetItem(itemCol);

		itemCol.SetText(it->GetTopic());
		itemCol.SetData(i);
		itemCol.SetColumn(2);
		m_channelList->SetItem(itemCol);
		i++;
	}
	m_btnRefresh->Enable();
}

// Event handlers
void ChannelWindow::OnClose(wxCloseEvent &event)
{
	// Using the wxCloseEvent::CanVeto() / wxCloseEvent::Veto() pair is the right way to do closing confirmation
	// dialogs as it allows to differentiate between program-generated close events and user-generated close events.
	if(event.CanVeto())
	{
		MessageDialog messageDlg(this, CW_DLG_DISCONNECT_CAPTION, CW_DLG_DISCONNECT_LABEL);
		//MessageDialog messageDlg(this, wxID_ANY, _("TEST Disconnect Dialog"));
		int answer = messageDlg.ShowModal();
		if (answer != ID_MESSAGEDIALOG_BTN_YES)
		{
			event.Veto();
			return;
		}
	}
	if (m_timer.IsRunning())
		m_timer.Stop();

	m_appInterface->CloseConnection();

	GetParent()->Show();

	Destroy();
}

void ChannelWindow::OnListItemSelect(wxListEvent &event)
{
	m_txtChannel->SetValue(event.GetText());
}

void ChannelWindow::OnListItemDblClick(wxListEvent &event)
{
	DoJoinChannel(event.GetText(), m_txtPassword->GetValue());
}

void ChannelWindow::OnRefreshBtnClick(wxCommandEvent &event)
{
	m_appInterface->RequestChannelList();
	m_btnRefresh->Disable();
}

void ChannelWindow::OnJoinChannelBtnClick(wxCommandEvent &event)
{
	wxString channel = m_txtChannel->GetValue();
	wxString password = m_txtPassword->GetValue();
	DoJoinChannel(channel, password);
}

void ChannelWindow::OnDisconnectBtnClick(wxCommandEvent &event)
{
	Close();
}

void ChannelWindow::OnChannelTextChange(wxCommandEvent &event)
{
	wxString str = m_txtChannel->GetValue();

	if (str.Len())
		m_btnJoinChannel->Enable(true);
	else
		m_btnJoinChannel->Enable(false);
}

/** Disables all input controls
*
*/
void ChannelWindow::DisableAllControls()
{
	m_btnJoinChannel->Disable();
	m_btnDisconnect->Disable();
	m_btnRefresh->Disable();
	m_channelList->Disable();
	m_txtChannel->Disable();
	m_txtPassword->Disable();
}

/** Enables all input controls
*
*/
void ChannelWindow::EnableAllControls()
{
	m_btnJoinChannel->Disable();
	m_btnDisconnect->Disable();
	m_btnRefresh->Disable();
	m_channelList->Disable();
	m_txtChannel->Disable();
	m_txtPassword->Disable();
}

/**Channel join routine (private method)
* @param[in] Channel.
* @param[in] Password.
*/
void ChannelWindow::DoJoinChannel(const wxString &channel, const wxString &password)
{
	if(channel != wxEmptyString)
	{
		Profile lastProfile = m_appInterface->GetSettingsManager()->GetSelectedProfile();

		// Create a profile with the new values and the old chann
		Profile profile(wxT("Latest"), lastProfile.GetUserName(), lastProfile.GetUserPassword(),
						lastProfile.GetServerName(), lastProfile.GetServerPassword(),
						m_txtChannel->GetValue(), m_txtPassword->GetValue());

		// Modify the Latest profile with the new information stored in profile
		m_appInterface->GetSettingsManager()->ModifyProfile(wxT("Latest"), profile);

		// Set the last profile used as Latest profile
		m_appInterface->GetSettingsManager()->SetProfile(wxT("Latest"));

		DisableAllControls();

		SkinLite *skin = (SkinLite *)GetParent();
		skin->CheckConferenceRecording(channel);

		// join channel
		m_appInterface->JoinChannel(channel, password);
	}
}

void ChannelWindow::OnTimer(wxTimerEvent &event)
{
	const AppConfig &cfg = m_appInterface->GetAppConfig();
	if(m_appInterface->GetCurrentUser())
	{
		wxString msg;
		// Review this line for readability
		msg << wxString::Format(LABEL_FRM_CHANNELS_LIST_STATUSTEXT_CH_NO, m_appInterface->GetCurrentUser()->GetNickName().c_str()) << wxT("     ") << _("Connected on server: ") << cfg.GetConfServer();
		SetStatusText(msg);
	}
	else
		SetStatusText(_("Not connected"));
}
