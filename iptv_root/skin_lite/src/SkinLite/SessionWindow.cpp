/////////////////////////////////////////////////////////////////////////////
// Name:        SessionWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     19/02/2008 15:33:55
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/bitmap.h>
#include <wx/imaglist.h>

////@begin includes
#define SKIN_API
#include "Skin.h"
#include "SkinLite.h"

#include "AppInterface.h"
#include "UserList.h"
#include "SessionWindow.h"
#include "SkinLiteConstants.h"
#include "VideoDisplayControl.h"
#include "SendPollFrame.h"
#include "PollStatsDialog.h"
#include "PollAnswerDialog.h"
#include "ChannelModesDialog.h"
#include "TransmissionDisplayControl.h"
#include "TransmissionConfigDialog.h"
#include "SeriesCollaborationDialog.h"

////@end includes


////@begin XPM images
////@end XPM images

// Old code
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfStatusChat);

/*!
 * SessionWindow event table definition
 */

BEGIN_EVENT_TABLE( SessionWindow, wxFrame )
	// EVT_CLOSE(SessionWindow::OnClose)
	//EVT_BUTTON(ID_USERSLISTFRM_MENU_BTN_BOUNCING_SHAPE,SessionWindow::OnBouncingShapeMenuBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_SHOW_MENU,SessionWindow::OnShowMenuBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_BACK,SessionWindow::OnBackBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_FULLSCRREN,SessionWindow::OnFullScreenBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_CHANGE_VIEW,SessionWindow::OnChangeViewBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_SHOWHIDE_SPLIT,SessionWindow::OnShowHideSplitBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_USERLIST,SessionWindow::OnShowUsersListBtnClick)
	EVT_BUTTON(ID_USERSLIST_FRM_BTN_PUBLIC_CHAT,SessionWindow::OnShowPublicChatBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_PLAYPAUSEFILEPLAYBACK,SessionWindow::OnPlayPauseFilePlayback)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_STOPFILEPLAYBACK,SessionWindow::OnStopFilePlayback)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_PLAY_STOP_CAPTURECAMERA, SessionWindow::OnPlayStopCaptureCameraBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_PLAYSENDVIDEOCAMERA, SessionWindow::OnPlaySendVideoCameraBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_STOPSENDVIDEOCAMERA, SessionWindow::OnStopSendVideoCameraBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_MENU_SENDFILES, SessionWindow::OnMenuSendFileBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_MENU_POLL, SessionWindow::OnMenuPollBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_MENU_CONFIGURE, SessionWindow::OnMenuConfigureBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_MENU_WBOARD, SessionWindow::OnMenuWhiteboardBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_MENU_PLAYLIST, SessionWindow::OnMenuPlaylistBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_CHANNEL_MODES, SessionWindow::OnChannelModesBtnClick)
	EVT_BUTTON(ID_USERSLISTFRM_BTN_CMD_REQUESTVOICE, SessionWindow::OnRequestVoiceBtnClick)
	EVT_LIST_ITEM_SELECTED(ID_USERSLIST_FRM_LST_USERS, SessionWindow::OnUsersLstSelected)
	EVT_LIST_ITEM_DESELECTED(ID_USERSLIST_FRM_LST_USERS, SessionWindow::OnUsersLstDeselected)
	EVT_LIST_ITEM_ACTIVATED(ID_USERSLIST_FRM_LST_USERS, SessionWindow::OnUsersLstDClick)
	// EVT_MENU(ID_USERSLIST_FRM_MENUITEM_PRIVATE_CHAT, SessionWindow::OnCtxMenuPrvChatClick)
	EVT_MENU(wxID_ANY, SessionWindow::OnContextMenuClick)
	EVT_COMMAND(wxID_ANY, iptvEVT_VDC_DBLCLICK, SessionWindow::OnVDCDblClick)
	EVT_TIMER(ID_USERSLIST_FRM_SESSION_TIME_TIMER, SessionWindow::OnTimer)
END_EVENT_TABLE()


/** SessionWindow default constructor.
 * 
 */
SessionWindow::SessionWindow()
{
    Init();
}

/** SessionWindow constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_USERSLISTFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_USERSLISTFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_USERSLISTFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_USERSLISTFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_USERSLISTFRAME_STYLE.
* @param[in] iface. Interface with application. Default NULL.
*/
SessionWindow::SessionWindow(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style, AppInterface* iface)
{
    Init();

	m_appInterface = iface;

	Create( parent, id, caption, pos, size, style );

	m_paused = false;
	m_stopped = false;

	if(iface != NULL)
	{
		const Channel *channel = m_appInterface->GetCurrentChannel();
		if(channel)
			m_appInterface->InitializeConference(channel->GetName());
		else
			m_appInterface->InitializeConference(wxEmptyString);

		const AppConfig &appCfg = m_appInterface->GetAppConfig();
	}
}


/** SessionWindow creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_USERSLISTFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_USERSLISTFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_USERSLISTFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_USERSLISTFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_USERSLISTFRAME_STYLE.
* @param[in] iface. Interface with application. Default NULL.
*/
bool SessionWindow::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	const AppConfig &appCfg = m_appInterface->GetAppConfig();
////@begin SessionWindow creation
	SetMinSize(SYMBOL_USERSLISTFRAME_SIZE);
    wxFrame::Create( parent, id, caption, pos, size, style );
	
	// Set the initial view mode. Must be done before CreateControls() comes in
	if(appCfg.GetMode() == APP_MODE_CONFERENCE)
		m_viewMode = SW_VIEW_MODE_CONFERENCE_DEFAULT;
	else
		m_viewMode = SW_VIEW_MODE_DEFAULT;

	wxIcon icon = wxIcon(S_LITE_ICON_FILENAME, wxBITMAP_TYPE_ICO);
	SetIcon(icon);
	CreateControls();
	Centre();
	Show();
	Maximize();
	// Forced minimum size to work around problem with sizers. Numbers are arbitrary.
	// TODO: try to find a decent fix
	SetMinSize(wxSize(600,450));
	wxString title = wxT("IP.TV");

	switch(appCfg.GetMode())
	{
	case APP_MODE_CONFERENCE:
		{
			const Channel *channel = m_appInterface->GetCurrentChannel();
			wxString topic = channel->GetTopic();
			title << wxT(" - #") << channel->GetName();
			if(topic != wxEmptyString)
				title << wxT(": ") << topic;
		}
		break;
	case APP_MODE_MULTICAST:
		title << wxT(" - Viewer");
		break;
	case APP_MODE_FILE:
		title << wxT(" - ") << appCfg.GetFileName();
		break;
	}
	SetTitle(title);

	// The "no-video" VDC is instantiated, but not effectively created in CreateControls().
	// This is to ensure the window is visible when its GL texture is created.
	m_vdcNoVideos->Create(this);
	m_subVertSizer->Prepend(m_vdcNoVideos, 2, wxALL | wxEXPAND, 0);
	m_mainPanel->Hide();
	m_subVertSizer->Layout();

	m_timer.SetOwner(this, ID_USERSLIST_FRM_SESSION_TIME_TIMER);
	m_timer.Start(USERLIST_FRM_SESSIONTIME_TIMER_INTERVAL);

	m_menuPanel->Hide();
	if(appCfg.GetMode() == APP_MODE_CONFERENCE)
		m_usersListPanel->Hide();
	m_mainHorPanelSizer->Layout();
	m_mainVertPanelSizer->Layout();

	if(appCfg.GetMode() == APP_MODE_CONFERENCE)
	{
		m_btnShowUsersList->Show();
		m_btnShowPublicChat->Show();
	}
	else	// Neither multicast nor file have user list
	{
		m_btnShowUsersList->Hide();
		m_btnShowPublicChat->Hide();
	}
	m_ctrlPanelSizer->Layout();


////@end SessionWindow creation
    return true;
}


/** SessionWindow destructor.
 * 
 */
SessionWindow::~SessionWindow()
{
	if(m_whiteboardManager)
		delete m_whiteboardManager;
}

// Triggered when closing window.
// This function is NOT in the event table, and is called from 
// within the real OnClose handler (Praise be to wxEvtHandler::Connect()).
// Now used to do stuff specific to this window before the more 
// general stuff done outside this function.
void SessionWindow::OnClose(wxCloseEvent &event)
{
	if(m_whiteboardManager)
	{
		delete m_whiteboardManager;
		m_whiteboardManager = NULL;
	}

	Skin *skin = (Skin *)GetParent();
	const AppConfig &cfg = m_appInterface->GetAppConfig();

	m_appInterface->StopMediaColaboration();

	m_timer.Stop();
	if(cfg.GetMode() == APP_MODE_CONFERENCE)
	{
		m_lblStatusText->SetLabel(_("Leaving channel..."));
		m_appInterface->UnregisterAllCaptureVideoSinks();
		m_appInterface->UnregisterAllAppSharingVideoSinks();
		StopCapture();
	}
	else
		m_lblStatusText->SetLabel(_("Terminating..."));
	// Get rid of the "no-video" VDC
	if(!m_vdcNoVideos->IsShown())
		m_vdcNoVideos->Show();
	m_vdcNoVideos->FreeTexture();

	if(m_menuCommands)
	{
		delete m_menuCommands;
		m_menuCommands = NULL;
	}
}

/*!
 * Member initialisation
 */

void SessionWindow::Init()
{
////@begin SessionWindow member initialisation
	m_whiteboardManager = NULL;
	m_appInterface = NULL;
    m_mainVertPanelSizer = NULL;
    m_userListPanelSizer = NULL;
    m_lstUsers = NULL;
    m_ctrlPanelSizer = NULL;
    m_ctrlSizer = NULL;
    m_btnShowUsersList = NULL;
    m_btnFullScreen = NULL;
    m_btnBack = NULL;
	m_btnChannelModes = NULL;
	m_tdcMirror = NULL;
	m_appSharingDisplay = NULL;
	m_pollStatsDialog = NULL;
	m_pollAnswerDialog = NULL;
	m_showUsersList = false;
	m_publicChatFrameExists = false;
	m_viewMode = SW_VIEW_MODE_DEFAULT;
	m_tdcLargeVideo = NULL;	// this must be NULL for view mode 1
	m_strUserSelected = wxEmptyString;
	m_indexUserSelected = USERLIST_FRM_ITEM_DESELECTED;
	m_sashPosition = 0;
	m_splitModeIsActive = true;
	m_hasMultipleColaborations = false;
	m_userBitrate = 0;

	//defines on first position the public chat - TODO: check this code
	StatusChat publicChat;

	publicChat.SetNameChatFrame(USERLIST_FRM_NAME_PUBLIC_CHAT);
	publicChat.SetOpenedChatFrame(false);
	m_arrStatusChat.Add(publicChat);
	//END defines on first position the public chat

	//Bouncing Shape
//	m_bouncingShapeExists = false;
	//END Bouncing Shape

	
////@end SessionWindow member initialisation
}


/*!
 * Control creation for SessionWindow
 */

void SessionWindow::CreateControls()
{
	m_whiteboardManager = new WhiteboardManager(m_appInterface, this);
	m_whiteboardManager->CreateWhiteboardFrame(this);

	const AppConfig &appCfg = m_appInterface->GetAppConfig();
////@begin SessionWindow content construction
    SessionWindow* itemFrame1 = this;

	// Load label bitmaps for the buttons. See SkinLiteConstants.h for the values used here.
	// TODO: Clean this up.
	wxBitmap bmpShowMenu = wxBitmap(USERLIST_FRM_BMP_SHOW_MENU, wxBITMAP_TYPE_PNG);
	wxBitmap bmpPublicChat = wxBitmap(USERLIST_FRM_BMP_PUBLIC_CHAT, wxBITMAP_TYPE_PNG);
	wxBitmap bmpUsersList = wxBitmap(USERLIST_FRM_BMP_USERLIST, wxBITMAP_TYPE_PNG);
	wxBitmap bmpFullScreen = wxBitmap(USERLIST_FRM_BMP_FULLSCREEN, wxBITMAP_TYPE_PNG);
	wxBitmap bmpBack = wxBitmap(USERLIST_FRM_BMP_BACK, wxBITMAP_TYPE_PNG);
	wxBitmap bmpMagnify = wxBitmap(USERLIST_FRM_BMP_MAGNIFY, wxBITMAP_TYPE_PNG);
	wxBitmap bmpChangeViewMode = wxBitmap(USERLIST_FRM_BMP_CHANGE_VIEW, wxBITMAP_TYPE_PNG);
	wxBitmap bmpShowHideSplit = wxBitmap(USERLIST_FRM_BMP_SHOWHIDE_SPLIT, wxBITMAP_TYPE_PNG);
	wxBitmap bmpPauseFilePlayback = wxBitmap(USERLIST_FRM_BMP_PAUSEFILEPLAYBACK, wxBITMAP_TYPE_PNG);
	wxBitmap bmpStopFilePlayback = wxBitmap(USERLIST_FRM_BMP_STOPFILEPLAYBACK, wxBITMAP_TYPE_PNG);
	
	wxBitmap bmpStopSendVideoCamera = wxBitmap(USERLIST_FRM_BMP_STOPSENDVIDEOCAMERA, wxBITMAP_TYPE_PNG);
	wxBitmap bmpPlaySendVideoCamera = wxBitmap(USERLIST_FRM_BMP_PLAYSENDVIDEOCAMERA, wxBITMAP_TYPE_PNG);
	wxBitmap bmpVideoCaptureScreen = wxBitmap(USERLIST_FRM_BMP_VIDEOCAPTURESCREEN, wxBITMAP_TYPE_PNG);

	m_bmpStopCaptureCamera = wxBitmap(USERLIST_FRM_BMP_STOPCAPTURECAMERA, wxBITMAP_TYPE_PNG);
	m_bmpPlayCaptureCamera = wxBitmap(USERLIST_FRM_BMP_PLAYCAPTURECAMERA, wxBITMAP_TYPE_PNG);

	m_bmpHandOff = wxBitmap(USERLIST_FRM_BMP_REQUESTVOICEOFF, wxBITMAP_TYPE_PNG);
	m_bmpHandOn = wxBitmap(USERLIST_FRM_BMP_REQUESTVOICEON, wxBITMAP_TYPE_PNG);
	
	// Create the main sizers.
    m_mainVertPanelSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertPanelSizer);
	
	m_mainHorPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainVertPanelSizer->Add(m_mainHorPanelSizer, 2, wxGROW, 5);

// -------------------------------------- Command pop-up menu ---------------------------------------------------- //
	m_menuCommands = new wxMenu;
	wxMenu *sendSubMenu = new wxMenu;
#ifndef WIN32
	m_menuCommands->Append(ID_CMD_MENU_TITLE, wxT(""));
	m_menuCommands->AppendSeparator();
#endif
	sendSubMenu->Append(ID_CMD_MENU_SEND_PRIVATE_CHAT, _("Private Chat"));
	m_menuCommands->AppendSubMenu(sendSubMenu, _("Send..."));
	m_menuCommands->AppendSeparator();
	m_menuCommands->Append(ID_CMD_MENU_GRANT_VOICE, _("Grant Voice"));
	m_menuCommands->Append(ID_CMD_MENU_REMOVE_VOICE, _("Remove Voice"));
	m_menuCommands->AppendSeparator();
	m_menuCommands->Append(ID_CMD_MENU_GRANT_SHARING, _("Grant Sharing"));
	m_menuCommands->Append(ID_CMD_MENU_REMOVE_SHARING, _("Remove Sharing"));
	m_menuCommands->Append(ID_CMD_MENU_REMOVE_ALL_SHARING, _("Remove all collaborations"));
	m_menuCommands->AppendSeparator();
	m_menuCommands->Append(ID_CMD_MENU_START_SERIES_COLLABORATION, _("Start series collaboration..."));
	m_menuCommands->Append(ID_CMD_MENU_STOP_SERIES_COLLABORATION, _("Stop series collaboration"));
	m_menuCommands->AppendSeparator();
	m_menuCommands->Append(ID_CMD_MENU_GRANT_OP_STATUS, _("Assign mediation"));
	m_menuCommands->Append(ID_CMD_MENU_REMOVE_OP_STATUS, _("Remove mediation"));

// --------------------------------- End command pop-up menu ------------------------------------------------------ //

/*-------------------------------------------Menu Panel------------------------------------------------------------*/
	m_menuPanel = new wxPanel( itemFrame1, ID_USERSLISTFRM_MENU_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertPanelSizer->Prepend(m_menuPanel, 0, wxGROW, 5);
	
	m_menuPanelVertSizer = new wxBoxSizer(wxVERTICAL);
    m_menuPanel->SetSizer(m_menuPanelVertSizer);

	m_menuPanelHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_menuPanelVertSizer->Add(m_menuPanelHorSizer, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
	/*itemBoxSizer5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);*/

/*----------------------------BUTTONS OF MENU-----------------------------------------------------*/
	if(appCfg.GetMode() == APP_MODE_CONFERENCE)
	{
		m_btnChannelModes = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_CHANNEL_MODES, USERLIST_FRM_BTN_CHANNELMODES_LABEL);
		m_menuPanelHorSizer->Add(m_btnChannelModes, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnMenuConfigure = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_MENU_CONFIGURE, USERLIST_FRM_BTN_MENUCONFIGURE_LABEL);
		m_menuPanelHorSizer->Add(m_btnMenuConfigure, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnMenuSendFile = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_MENU_SENDFILES, USERLIST_FRM_BTN_MENUSENDFILE_LABEL);
		m_menuPanelHorSizer->Add(m_btnMenuSendFile, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
		//m_btnMenuSendFile->Hide();
	
		m_btnMenuPoll = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_MENU_POLL, USERLIST_FRM_BTN_MENUPOLL_LABEL);
		m_menuPanelHorSizer->Add(m_btnMenuPoll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnMenuWhiteboard = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_MENU_WBOARD, USERLIST_FRM_BTN_MENUWBOARD_LABEL);
		m_menuPanelHorSizer->Add(m_btnMenuWhiteboard, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		m_btnMenuPlaylist = new wxButton(m_menuPanel, ID_USERSLISTFRM_BTN_MENU_PLAYLIST, USERLIST_FRM_BTN_MENUPLAYLIST_LABEL);
		m_menuPanelHorSizer->Add(m_btnMenuPlaylist, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	}
/*------------------------END BUTTONS OF MENU-----------------------------------------------------*/

/*-----------------------------------------END Menu Panel----------------------------------------------------------*/

/*-------------------------------------------Main panel------------------------------------------------------------*/
	m_mainPanel = new wxPanel(itemFrame1, ID_USERSLISTFRM_MAIN_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	// a horizontal sizer for the main panel, which will contain only 1 element (m_equalModePanel) in view mode 1
	// and more (the sash window + big window panel and the small window panel) in view mode 2
	m_mainPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainPanel->SetSizer(m_mainPanelSizer);
	// a "sub" vertical sizer, which will contain the video and chat panels and be contained in the "main" horizontal sizer
	m_subVertSizer = new wxBoxSizer(wxVERTICAL);
	// the video panel has proportion "2" and the chat panel (created below) has proportion "1" (sum 3), that is,
	// when shown, the chat panel takes 1/3 of the main sizer area while the other 2/3 are reserved for video
	m_subVertSizer->Add(m_mainPanel, 2, wxGROW, 5);

	// View mode 1 (equally spaced windows) controls
	// ---------------------------------------------
	m_equalModePanel = new wxPanel(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	// GridSizer which will contain the VideoDisplayControls
	m_gridSizer = new wxGridSizer(1, 1, 4, 4);
	m_equalModePanel->SetSizer(m_gridSizer);
//	m_mainPanelSizer->Add(m_equalModePanel, 1, wxALL | wxEXPAND);
//	m_mainPanel->SetSizer(m_gridSizer);	
	// View mode 2 (one large window) controls
	// ---------------------------------------

	m_largeModePanel = new wxPanel(m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_largeModePanel->SetMinSize(wxSize(512,384));
//	m_largeModePanel->Connect(wxEVT_SIZE, wxSizeEventHandler(SessionWindow::OnLargeModePanelSize), NULL, this);
//	m_mainPanelSizer->Add(m_largeModePanel, 1, wxALL | wxEXPAND);
	m_largeModeSizer = new wxBoxSizer(wxHORIZONTAL);
	m_largeModePanel->SetSizer(m_largeModeSizer);
	// Add the splitter
	m_splitter = new wxSplitterWindow(m_largeModePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_BORDER | wxSP_LIVE_UPDATE);
	m_splitter->SetMinSize(wxSize(512,384));
	m_splitter->SetSashGravity(1.0);	// this causes the left pane to be resized as the main window is resized
	m_largeModeSizer->Add(m_splitter, 1, wxALL | wxEXPAND);
	// Create the panels:
	// Large window panel...
	// m_largeWindowPanel = new wxPanel(m_largeModePanel, wxID_ANY, wxPoint(0,0), wxSize(320, m_largeModePanel->GetSize().GetHeight()));
	m_largeWindowPanel = new wxPanel(m_splitter, wxID_ANY);
	m_largeWindowSizer = new wxBoxSizer(wxHORIZONTAL);
	m_largeWindowPanel->SetSizer(m_largeWindowSizer);
	m_largeWindowPanel->SetMinSize(wxSize(320,240));
//	m_largeModeSizer->Add(m_largeWindowPanel, 1, wxEXPAND);
//	m_largeWindowPanel->SetBackgroundColour(wxColour(64,64,64));
	// and small window panel
	m_smallWindowPanel = new wxPanel(m_splitter, wxID_ANY);
	m_smallWindowPanel->SetMinSize(wxSize(160,120));
	m_smallWindowSizer = new wxBoxSizer(wxVERTICAL);
	m_smallWindowPanel->SetSizer(m_smallWindowSizer);

	m_largeModeSizer->Layout();
	// Then, split 'em up
	m_splitter->SplitVertically(m_largeWindowPanel, m_smallWindowPanel, -160);
	m_splitter->SetMinimumPaneSize(USERLIST_FRM_SASH_MIN_PANE_SIZE);
	m_splitter->SetSashSize(6);

//	m_mainPanelSizer->Add(m_largeModePanel, 1, wxALL | wxEXPAND);
//	m_mainPanelSizer->Layout();
	
	// NOTE: These controls are created for CONFERENCE mode only.
	if (appCfg.GetMode() == APP_MODE_CONFERENCE)
	{
		// Sizers inside the new public chat panel
		m_publicChatSizer = new wxBoxSizer(wxVERTICAL);
		m_publicChatTextSizer = new wxBoxSizer(wxVERTICAL);
		m_publicChatInputSizer = new wxBoxSizer(wxHORIZONTAL);
		// Create the public chat panel
		m_publicChatPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
		m_chatTextBox = new wxTextCtrl(m_publicChatPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
			wxTE_MULTILINE| wxTE_READONLY | wxTE_RICH);
		// Create the controls inside it
		m_chatInputBox = new wxTextCtrl(m_publicChatPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		m_chatInputBox->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SessionWindow::OnPublicChatInputTextChange), NULL, this);
		m_chatInputBox->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SessionWindow::OnPublicChatSendBtnClick), NULL, this);
		m_chatSendBtn = new wxButton(m_publicChatPanel, wxID_ANY, _("Send"));
		m_chatSendBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SessionWindow::OnPublicChatSendBtnClick), NULL, this);
		m_chatSendBtn->Disable();

		m_publicChatPanel->SetSizer(m_publicChatSizer);
		
		m_publicChatTextSizer->Add(m_chatTextBox, 1, wxALL | wxEXPAND);
		m_publicChatInputSizer->Add(m_chatInputBox, 1, wxALL | wxEXPAND | wxALIGN_LEFT);
		m_publicChatInputSizer->Add(m_chatSendBtn, 0, wxLEFT | wxEXPAND | wxALIGN_RIGHT, 8);

		wxStaticText *label = new wxStaticText(m_publicChatPanel, wxID_ANY, wxString(wxT(" ")) + _("Public Chat"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
		label->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
		label->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
		label->SetMinSize(wxSize(-1, 16));
		m_publicChatSizer->Add(label, 0, wxBOTTOM | wxEXPAND | wxALIGN_CENTER_VERTICAL);
		m_publicChatSizer->Add(m_publicChatTextSizer, 1, wxALL | wxEXPAND, 8);
		m_publicChatSizer->Add(m_publicChatInputSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 8);
		m_publicChatSizer->AddSpacer(8);
		// Add the panel to the "sub" vertical sizer
		m_subVertSizer->Add(m_publicChatPanel, 1, wxALL | wxEXPAND);
		// Initially hidden
		m_publicChatPanel->Hide();
	}
	// Add the "sub" sizer to the "main" sizer
	m_mainHorPanelSizer->Add(m_subVertSizer, 1, wxALL | wxEXPAND);

/*-----------------------------------------END Main panel----------------------------------------------------------*/

/*----------------------------------- "No-video" VideoDisplayControl  ---------------------------------------------*/

	m_vdcNoVideos = new VideoDisplayControl;

/*-------------------------------- END "No-video" VideoDisplayControl  --------------------------------------------*/

/*------------------------------------------Users List Panel-------------------------------------------------------*/
	// NOTE: These controls are created for CONFERENCE mode only.
	if (appCfg.GetMode() == APP_MODE_CONFERENCE)
	{

		m_usersListPanel = new wxPanel( itemFrame1, ID_USERSLISTFRM_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
		m_mainHorPanelSizer->Add(m_usersListPanel, 0, wxGROW, 5);

		m_userListPanelSizer = new wxBoxSizer(wxVERTICAL);
		m_usersListPanel->SetSizer(m_userListPanelSizer);

		m_lstUsers = new SophisticatedListControl(m_usersListPanel, ID_USERSLIST_FRM_LST_USERS, true, USERLIST_FRM_USERS_LIST_BLINK_INTERVAL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL );
		m_lstUsers->SetBlinkColor(USERLIST_FRM_USERS_LIST_BLINK_FILL_COLOR, USERLIST_FRM_USERS_LIST_BLINK_ERASED_COLOR);

		wxStaticBoxSizer *userListBoxSizer = new wxStaticBoxSizer(wxVERTICAL, m_usersListPanel, _("User List"));
		userListBoxSizer->Add(m_lstUsers, 1, wxEXPAND | wxALL, 5);

		m_userListPanelSizer->Add(userListBoxSizer, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

		// Connect event for ContextMenu
		m_lstUsers->Connect(ID_USERSLIST_FRM_LST_USERS, wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(SessionWindow::OnShowContextMenu), NULL, this);

		// Image list (icons) for the list control
		wxImageList *imgList = new wxImageList(USERLIST_FRM_BMP_WIDTH, USERLIST_FRM_BMP_HEIGHT);
		wxBitmap mediaVoiceIcon(USERLIST_FRM_BMP_ICON_MEDIAVOICE, wxBITMAP_TYPE_ANY);
		wxBitmap mediaVoiceMuteIcon(USERLIST_FRM_BMP_ICON_MEDIAVOICEMUTE, wxBITMAP_TYPE_ANY);
		wxBitmap mediaCollabIcon(USERLIST_FRM_BMP_ICON_MEDIACOLLAB, wxBITMAP_TYPE_ANY);
		wxBitmap mediaCollabMuteIcon(USERLIST_FRM_BMP_ICON_MEDIACOLLABMUTE, wxBITMAP_TYPE_ANY);
		wxBitmap muteIcon(USERLIST_FRM_BMP_ICON_MUTE, wxBITMAP_TYPE_ANY);
		wxBitmap camIcon(USERLIST_FRM_BMP_ICON_CAMERA, wxBITMAP_TYPE_ANY);
		wxBitmap handIcon(USERLIST_FRM_BMP_ICON_REQUESTVOICE, wxBITMAP_TYPE_ANY);
		imgList->Add(mediaVoiceIcon);
		imgList->Add(mediaVoiceMuteIcon);
		imgList->Add(mediaCollabIcon);
		imgList->Add(mediaCollabMuteIcon);
		imgList->Add(muteIcon);
		imgList->Add(camIcon);
		imgList->Add(handIcon);
		m_lstUsers->AssignImageList(imgList, wxIMAGE_LIST_SMALL);
 
	/*---------------------------------User commands Area--------------------------------------------*/
		m_userCmdSizer = new wxStaticBoxSizer(wxVERTICAL, m_usersListPanel, _("User Commands"));
		m_userListPanelSizer->Add(m_userCmdSizer, 0, wxEXPAND | wxALL, 5);

		m_btnRequestVoice = new wxBitmapButton( m_usersListPanel, ID_USERSLISTFRM_BTN_CMD_REQUESTVOICE, m_bmpHandOff, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
		
		wxBoxSizer *cmdBtnSizer = new wxBoxSizer(wxHORIZONTAL);
		cmdBtnSizer->Add(m_btnRequestVoice, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

		m_userCmdSizer->Add(cmdBtnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
		m_btnRequestVoice->SetToolTip(USERLIST_FRM_TOOLTIP_REQUESTVOICE_BTN);
		m_userCmdSizer->Layout();
	/*-------------------------------END User commands Area------------------------------------------*/

	/*------------------------------------Video Capture Area-----------------------------------*/
		m_videoCaptureSizer = new wxStaticBoxSizer(wxVERTICAL, m_usersListPanel, USERLIST_FRM_LBL_SIZER_VIDEOCAPTURE_CAPTION);
		m_userListPanelSizer->Add(m_videoCaptureSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
		
		m_vdcCaptureView = new VideoDisplayControl(m_usersListPanel);
		m_vdcCaptureView->SetMinSize(wxSize(160, 120));
		// Force 4:3 aspect for now
		m_vdcCaptureView->SetAspectRatio(4.0f/3.0f);
		m_videoCaptureSizer->Add(m_vdcCaptureView, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
		
		m_captureCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
		m_videoCaptureSizer->Add(m_captureCtrlSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5);

	  /*-------------------Capture Ctrl Area-----------------*/
		m_btnPlayStopCaptureCamera = new wxBitmapButton( m_usersListPanel, ID_USERSLISTFRM_BTN_PLAY_STOP_CAPTURECAMERA, m_bmpStopCaptureCamera, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
		m_captureCtrlSizer->Add(m_btnPlayStopCaptureCamera, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
		m_btnPlayStopCaptureCamera->SetToolTip(USERLIST_FRM_TOOLTIP_PLAY_STOP_CAPTURECAMERA_BTN);

		m_btnPlaySendVideoCamera = new wxBitmapButton( m_usersListPanel, ID_USERSLISTFRM_BTN_PLAYSENDVIDEOCAMERA, bmpPlaySendVideoCamera, wxDefaultPosition, m_btnPlayStopCaptureCamera->GetSize(), wxBU_AUTODRAW );
		m_captureCtrlSizer->Add(m_btnPlaySendVideoCamera, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
		m_btnPlaySendVideoCamera->SetToolTip(USERLIST_FRM_TOOLTIP_PLAYSENDVIDEOCAMERA_BTN);
		
		m_btnStopSendVideoCamera = new wxBitmapButton( m_usersListPanel, ID_USERSLISTFRM_BTN_STOPSENDVIDEOCAMERA, bmpStopSendVideoCamera, wxDefaultPosition, m_btnPlayStopCaptureCamera->GetSize(), wxBU_AUTODRAW );
		m_captureCtrlSizer->Add(m_btnStopSendVideoCamera, 0, wxALIGN_CENTER_VERTICAL|wxALL, 2);
		m_btnStopSendVideoCamera->SetToolTip(USERLIST_FRM_TOOLTIP_STOPSENDVIDEOCAMERA_BTN);
	  /*---------------END Capture Ctrl Area-----------------*/

	/*--------------------------------END Video Capture Area-----------------------------------*/


	}
/*---------------------------------------END Users List Panel------------------------------------------------------*/

/*-------------------------------------- File playback controls -----------------------------------------------------*/
	if (appCfg.GetMode() == APP_MODE_FILE)
	{
		wxPanel *sliderPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
		wxBoxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);
		sliderPanel->SetSizer(sliderSizer);

		m_btnPlayPauseFilePlayback = new wxBitmapButton( sliderPanel, ID_USERSLISTFRM_BTN_PLAYPAUSEFILEPLAYBACK, bmpPauseFilePlayback, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
		sliderSizer->Add(m_btnPlayPauseFilePlayback, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 8);
		m_btnStopFilePlayback = new wxBitmapButton( sliderPanel, ID_USERSLISTFRM_BTN_STOPFILEPLAYBACK, bmpStopFilePlayback, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
		sliderSizer->Add(m_btnStopFilePlayback, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);

		m_fileProgressSlider = new wxSlider(sliderPanel, wxID_ANY, 0, 0, SLIDER_MAX);
		m_fileProgressSlider->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(SessionWindow::OnFileProgressSliderChange), NULL, this);
		sliderSizer->Add(m_fileProgressSlider, 1, wxLEFT | wxRIGHT | wxEXPAND, 8);
		m_mainVertPanelSizer->Add(sliderPanel, 0, wxEXPAND);
		sliderPanel->Show();
	}
/*---------------------------------- END File playback controls -----------------------------------------------------*/
	
/*------------------------------------------StatusBar--------------------------------------------------------------*/
    m_ctrlPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainVertPanelSizer->Add(m_ctrlPanelSizer, 0, wxGROW, 5);
	
  /*--------------------------------------Status Text panel-----------------------------------------------------*/
	m_statusTextPanel = new wxPanel( itemFrame1, ID_USERSLISTFRM_PANEL3, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	m_ctrlPanelSizer->Add(m_statusTextPanel, 1, wxGROW, 5);

	wxBoxSizer* statusTextSizer = new wxBoxSizer(wxHORIZONTAL);
	m_statusTextPanel->SetSizer(statusTextSizer);

	m_lblStatusText = new wxStaticText(m_statusTextPanel, wxID_ANY, wxEmptyString);
	statusTextSizer->Add(m_lblStatusText,0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
  /*-----------------------------------END Status Text panel----------------------------------------------------*/

  /*----------------------------------Ctrl panel (where buttons shows)------------------------------------------*/
    wxPanel* itemPanel7 = new wxPanel( itemFrame1, ID_USERSLISTFRM_PANEL2, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
	m_ctrlPanelSizer->Add(itemPanel7, 0, wxALIGN_CENTER_VERTICAL, 5);

    m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
    itemPanel7->SetSizer(m_ctrlSizer);
	m_ctrlSizer->AddSpacer(4);
	
	m_btnShowMenu = new wxBitmapButton( itemPanel7, ID_USERSLIST_FRM_BTN_SHOW_MENU, bmpShowMenu, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_ctrlSizer->Add(m_btnShowMenu, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALL, 5);
	m_btnShowMenu->SetToolTip(USERLIST_FRM_TOOLTIP_SHOW_MENU);

	m_btnShowUsersList = new wxBitmapButton( itemPanel7, ID_USERSLIST_FRM_BTN_USERLIST, bmpUsersList, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_ctrlSizer->Add(m_btnShowUsersList, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | (wxALL & ~wxLEFT), 5);
	m_btnShowUsersList->SetToolTip(USERLIST_FRM_TOOLTIP_SHOW_USERSLIST);

	m_btnShowPublicChat = new wxBitmapButton( itemPanel7, ID_USERSLIST_FRM_BTN_PUBLIC_CHAT, bmpPublicChat, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_ctrlSizer->Add(m_btnShowPublicChat, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL |(wxALL & ~wxLEFT), 5);
	m_btnShowPublicChat->SetToolTip(USERLIST_FRM_TOOLTIP_SHOW_PUBCHAT);

	m_btnFullScreen = new wxBitmapButton( itemPanel7, ID_USERSLIST_FRM_BTN_FULLSCRREN, bmpFullScreen, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_ctrlSizer->Add(m_btnFullScreen, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | (wxALL & ~wxLEFT), 5);
	m_btnFullScreen->SetToolTip(USERLIST_FRM_TOOLTIP_SHOW_FULLSCREEN);

	m_btnShowHideSplit = new wxBitmapButton(itemPanel7, ID_USERSLIST_FRM_BTN_SHOWHIDE_SPLIT, bmpShowHideSplit,
	wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_ctrlSizer->Add(m_btnShowHideSplit, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | (wxALL & ~wxLEFT), 5);
	m_btnShowHideSplit->SetToolTip(_("Show secondary window"));

	m_btnChangeViewMode = new wxBitmapButton(itemPanel7, ID_USERSLIST_FRM_BTN_CHANGE_VIEW, bmpChangeViewMode,
	wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
	m_ctrlSizer->Add(m_btnChangeViewMode, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | (wxALL & ~wxLEFT), 5);
	m_btnChangeViewMode->SetToolTip(_("Equal videos size"));

    m_btnBack = new wxBitmapButton( itemPanel7, ID_USERSLIST_FRM_BTN_BACK, bmpBack, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_ctrlSizer->Add(m_btnBack, 0, wxEXPAND | wxALIGN_CENTER_VERTICAL | (wxALL & ~wxLEFT), 5);
	m_btnBack->SetToolTip(USERLIST_FRM_TOOLTIP_BACK_BTN);
  /*-------------------------------END Ctrl panel (where buttons shows)-----------------------------------------*/


	if(appCfg.GetMode() != APP_MODE_CONFERENCE)
		m_btnShowMenu->Hide();

	m_btnChangeViewMode->Disable();
	m_btnShowHideSplit->Disable();


/*------------------------------------------END StatusBar----------------------------------------------------------*/
	// Set up the window for the initial view mode
	SetViewMode(m_viewMode);

	m_mainVertPanelSizer->SetSizeHints(this);

//@end SessionWindow content construction
}

/** Sets linkage with application.
* @param[in] iface. Application interface.
*/
void SessionWindow::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
}

/** When the state is file play, updates the progress slider
* @param[in] percent. Percentage of file.
*/
void SessionWindow::UpdateFileProgressSlider(float percent)
{
	int sldMin, sldMax;
	sldMin = m_fileProgressSlider->GetMin();
	sldMax = m_fileProgressSlider->GetMax();
	m_fileProgressSlider->SetValue((int)((percent / 100) * (sldMax - sldMin)) + sldMin);
}

/** Updates the list of users in channel.
*
*/
void SessionWindow::UpdateUserList()
{
	wxCHECK_RET(m_appInterface, wxT("Aaaaah!!!! m_appInterface is NULL!!!"));
	StatusChat statusChat;
	wxString nameUser;
	UserList *userList = m_appInterface->GetUserList();
	int usersCount = userList->GetUserCount();

	wxListItem itemCol;

	//Delete all items if list users is not empty
	m_lstUsers->ClearAll();
	
	// Camera column
	itemCol.SetColumn(USERLIST_FRM_LST_COL_CAMERA_INDEX);
	// itemCol.SetText(LABEL_FRM_USERS_LIST_COL_USERS);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_lstUsers->InsertColumn(USERLIST_FRM_LST_COL_CAMERA_INDEX, itemCol);
	m_lstUsers->SetColumnWidth(USERLIST_FRM_LST_COL_CAMERA_INDEX, 20);
	// Voice column
	itemCol.SetColumn(USERLIST_FRM_LST_COL_USERS_INDEX);
	// itemCol.SetText(LABEL_FRM_USERS_LIST_COL_USERS);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_lstUsers->InsertColumn(USERLIST_FRM_LST_COL_VOICE_INDEX, itemCol);
	m_lstUsers->SetColumnWidth(USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_WIDTH);
	
	// User name column
	itemCol.SetColumn(USERLIST_FRM_LST_COL_USERS_INDEX);
	// itemCol.SetText(LABEL_FRM_USERS_LIST_COL_USERS);
	itemCol.SetAlign(wxLIST_FORMAT_LEFT);
	m_lstUsers->InsertColumn(USERLIST_FRM_LST_COL_USERS_INDEX, itemCol);
	m_lstUsers->SetColumnWidth(USERLIST_FRM_LST_COL_USERS_INDEX, m_lstUsers->GetClientSize().GetWidth() - (2 * USERLIST_FRM_BMP_WIDTH));

	int i = 0;
	UserIterator it;
	for(it = userList->GetBeginIterator(); it != userList->GetEndIterator(); it++)
	{
		nameUser = it->GetNickName();

		// Only show user if (s)he is not invisible
		if(!it->HasMode(IRM_MODE_USER_INVISIBLE))
		{
			m_lstUsers->InsertItem(i, nameUser);

			m_lstUsers->SetItemData(i,i);

			m_lstUsers->SetItem(i, USERLIST_FRM_LST_COL_CAMERA_INDEX, wxEmptyString);
			m_lstUsers->SetItem(i, USERLIST_FRM_LST_COL_VOICE_INDEX, wxEmptyString);
			m_lstUsers->SetItem(i, USERLIST_FRM_LST_COL_USERS_INDEX, nameUser);
		
			// Set Color
			if(it->IsChannelOperator())
				m_lstUsers->SetItemTextColour(i, USERLIST_FRM_LST_USERS_OPERATOR_COLOR);
			else if(it->HasMode(IRM_MODE_USER_REGISTERED))
				m_lstUsers->SetItemTextColour(i, USERLIST_FRM_LST_USERS_REGISTERED_COLOR);
			else
				m_lstUsers->SetItemTextColour(i, USERLIST_FRM_LST_USERS_DEFAULT_COLOR);
			// Set Icons
			// Media Voice
			if(it->HasMediaVoice())
			{
				if(it->HasMode(IRM_MODE_USER_MIC_MUTE))
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_MEDIAVOICEMUTE);
				else
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_MEDIAVOICE);
			}
			// Collaboration
			else if(it->IsChannelCollaborator())
			{
				if(it->HasMode(IRM_MODE_USER_MIC_MUTE))
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_MEDIACOLLABMUTE);
				else
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_MEDIACOLLAB);
			}
			else
			{
				if(it->IsRequestingVoice())
				{
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_REQUESTVOICE);
				}
				else if(it->HasMode(IRM_MODE_USER_MIC_MUTE))
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, USERLIST_FRM_BMP_ICON_INDEX_MUTE);
				else
					m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_VOICE_INDEX, -1);	// No icon
			}
			// Camera
			if(it->HasMode(IRM_MODE_USER_CAMERA_ON))
				m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_CAMERA_INDEX, USERLIST_FRM_BMP_ICON_INDEX_CAMERA);
			else
				m_lstUsers->SetItemColumnImage(i, USERLIST_FRM_LST_COL_CAMERA_INDEX, -1);
		} // if (!invisible)

		i++;
	}

	UpdateCommandButtonStates();
	UpdateControlBarButtonStates();
	//If other users enters or leaves the channel, don't affects other blinking users
	ContinueBlinkingUsername();
}

/** Create action of Back Button.
*
*/
void SessionWindow::OnBackBtnClick(wxCommandEvent &event)
{
	Close();
}

/*Create action of FullScreen Button*/
void SessionWindow::OnFullScreenBtnClick(wxCommandEvent &event)
{
	if (this->IsFullScreen() == true)
		this->ShowFullScreen(false);
	else
		this->ShowFullScreen(true);
}

// ChangeViewMode button functionality.
void SessionWindow::OnChangeViewBtnClick(wxCommandEvent &event)
{
	switch(m_viewMode)
	{
		case SW_VIEW_MODE_EQUAL_SPACING:
			SetViewMode(SW_VIEW_MODE_ONE_LARGE_WINDOW);
			break;
		case SW_VIEW_MODE_ONE_LARGE_WINDOW:
			SetViewMode(SW_VIEW_MODE_EQUAL_SPACING);
			break;
		default:
			return;
	}
	UpdateControlBarButtonStates();
}

// ShowHideSplit button functionality.
void SessionWindow::OnShowHideSplitBtnClick(wxCommandEvent &event)
{
	if (m_splitter->IsSplit())
	{
		//m_smallWindowPanel->Show(false);
		m_sashPosition = m_splitter->GetSashPosition();
        m_splitter->Unsplit();
		m_splitModeIsActive = false;
	}
	else
	{
		/*m_smallWindowPanel->Show(true);*/
		m_splitter->SplitVertically(m_largeWindowPanel, m_smallWindowPanel);
		m_splitter->SetSashPosition(m_sashPosition);
		m_splitter->SetMinimumPaneSize(USERLIST_FRM_SASH_MIN_PANE_SIZE);
		m_splitter->SetSashSize(6);
		m_splitModeIsActive = true;
	}

	UpdateControlBarButtonStates();

}

void SessionWindow::OnShowMenuBtnClick(wxCommandEvent &event)
{
	if (m_menuPanel->IsShown() == true)
		m_menuPanel->Show(false);
	else
		m_menuPanel->Show(true);
	m_mainVertPanelSizer->Layout();
}

/*Create action of ShowUsersList Button*/
void SessionWindow::OnShowUsersListBtnClick(wxCommandEvent &event)
{
	if (m_usersListPanel->IsShown() == true)
		m_usersListPanel->Show(false);
	else
		m_usersListPanel->Show(true);
	m_mainHorPanelSizer->Layout();
}

//Open the public Chat
void SessionWindow::OnShowPublicChatBtnClick(wxCommandEvent &event)
{
	m_publicChatPanel->Show((!m_publicChatPanel->IsShown()));
	m_chatInputBox->SetFocus();
	m_subVertSizer->Layout();
}

#if 0	// Not used right now, but kept for reference
/*Updates users list*/
void SessionWindow::UpdateUsersList()
{
	StatusChat statusChat;
	wxString nameUser;
	int usersCount = 0;

	wxListItem itemCol;

	//Delete all items if list users is not empty
	m_lstUsers->ClearAll();
	
	//Column Users
	itemCol.SetText(LABEL_FRM_USERS_LIST_COL_USERS);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	m_lstUsers->InsertColumn(FRM_USERS_LIST_NUM_COL_USERS, itemCol);
	m_lstUsers->SetColumnWidth(FRM_USERS_LIST_NUM_COL_USERS, (m_lstUsers->GetSize().GetWidth() - 22));//-22 used to eliminate scrollBar
	for(int i = 0;i<usersCount;i++)
	{
//		nameUser = m_usersList.Item(i).GetNameUser();

		if (nameUser != m_userName)
		{
			m_lstUsers->InsertItem(i, nameUser);

			//Prepare for private chats
			statusChat.SetNameChatFrame(nameUser);
			statusChat.SetOpenedChatFrame(false);
			m_arrStatusChat.Add(statusChat);
			//END Prepare for private chats
		}
	}
}
#endif

/**Show the public chat window.
*
*/
void SessionWindow::ShowPublicChat()
{
#if 0	// Old code, kept for reference
	if(!m_publicChatFrameExists)
	{

		m_publicChatFrame = new ChatFrame(this, ID_CHATFRAME, wxString::Format(FRM_CHAT_PUBLIC_CHAT_CAPTION, m_userName.c_str()), USERLIST_FRM_NAME_PUBLIC_CHAT);
		m_publicChatFrame->SetAppInterface(m_appInterface);
		m_publicChatFrame->SetPublicChatFlag(true);
		m_publicChatFrame->SetUserNameStatus(m_appInterface->GetCurrentChannel());
		m_publicChatFrame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SessionWindow::OnPublicChatClose), NULL, this);
		m_publicChatFrameExists = true;
	}
	else
	{
		if (m_publicChatFrame->IsIconized())
		{
			m_publicChatFrame->Iconize(false);
			//m_btnChatToolTip = USERLIST_FRM_TOOLTIP_HIDE_PUBCHAT;
		}
		else
		{
			m_publicChatFrame->Iconize(true);
			//m_btnChatToolTip = USERLIST_FRM_TOOLTIP_SHOW_PUBCHAT;
		}
	}
	m_publicChatFrame->Show();
#endif
}

/** Shows the private Chat window.
* @param[in] user. Nickname of the user thats sent message.
*/
void SessionWindow::ShowPrivateChat(const wxString &user)
{
	wxCHECK_RET(m_appInterface, wxT("Where's m_appInterface?!"));

	if(user == m_appInterface->GetCurrentUser()->GetNickName())
		return;

	// Ugly, but that's the way it is. 
	// TODO: Revise architecture, to avoid things like this throughout the code
	Skin *skin = (Skin *)GetParent();
	// Get a reference to the current chat window list
	PrivateChatWindowList &pcwList = skin->GetPrivateChatWindowList();
	// Is there a chat window already open for this sender?
	PrivateChatWindowBase *chatWin = pcwList.GetWindowByUserName(m_strUserSelected);
	if(!chatWin)	// If not, create it now and add it to the list
		chatWin = skin->CreatePrivateChatWindow(m_strUserSelected);

	chatWin->OpenWindow();

	if (FindItemBlink(m_indexUserSelected))
	{
		m_itemBlink.Remove(m_indexUserSelected);
		m_lstUsers->BlinkItem(m_indexUserSelected, false);
	}
}

/** Find if a item is blinking.
* @param[in] item. Index of the user list thats will be searched.
* @return Returns true if the item is blinking or false for the other case.
*/
bool SessionWindow::FindItemBlink(const long &item)
{
	bool found = false;

	for(unsigned i=0; i < m_itemBlink.GetCount(); i++)
	{
		if (m_itemBlink[i] == item)
			found = true;
	}

	return found;
}


// Returns a pointer to the public chat window. (DEPRECATED)
ChatFrame *SessionWindow::GetPublicChatFrame()
{
	if(!m_publicChatFrameExists)
		ShowPublicChat();
	return m_publicChatFrame;
}

/** Add message in public chat text box.
* @param[in] sender. User name of who sent the message.
* @param[in] msg. The message.
*/
void SessionWindow::SetPublicChatMessage(const wxString &sender, const wxString &msg)
{
	if(!m_publicChatPanel->IsShown())
	{
		m_publicChatPanel->Show();
		m_chatInputBox->SetFocus();
		m_subVertSizer->Layout();
	}
	//UserName Identification
	m_chatTextBox->SetDefaultStyle(wxTextAttr(*wxRED));
	m_chatTextBox->AppendText(wxT("<") + sender + wxT("> "));

	//UserName Message
	m_chatTextBox->SetDefaultStyle(wxTextAttr(*wxBLACK));
	m_chatTextBox->AppendText(msg + wxT("\n"));
}


/*Get name and index of users list when a item is selected*/
void SessionWindow::OnUsersLstSelected(wxListEvent& event)
{
	// Using event.GetItem() doesn't work for GTK+. Must use event.GetIndex() here
	// then set the item structure accordingly. This is due to wxListCtrl using
	// native implementations in report mode, and some implementation difference between
	// Windows and GTK+ causes this problem.
#ifdef WIN32
	wxListItem item = event.GetItem();
#else
	wxListItem item;
	item.SetId(event.GetIndex());
#endif
	item.SetColumn(USERLIST_FRM_LST_COL_USERS_INDEX);
	wxString debugMsg = wxT("<SkinLite> List item selected, index = ");
	debugMsg << item.GetId() << wxT(", text = ");
	m_lstUsers->GetItem(item);
	m_indexUserSelected = item.GetId();
	m_strUserSelected = item.GetText();

	debugMsg << m_strUserSelected;
	m_appInterface->SetDebugMessage(debugMsg);
}

/*Non item is selected*/
void SessionWindow::OnUsersLstDeselected(wxListEvent& event)
{
	m_indexUserSelected = USERLIST_FRM_ITEM_DESELECTED;
}

/** Set labels on status bar.
* @param[in] userName. The user name.
* @param[in] channel. The current channel name.
*/
void SessionWindow::SetLabelStatusText(wxString userName, wxString channel)
{
	m_userName = userName;
	m_channel = channel;

	m_lblStatusText->SetLabel(wxString::Format(LABEL_FRM_USERS_LIST_STATUSTEXT, m_userName.c_str(), m_channel.c_str()));
}

//event for lstUsers double-click
void SessionWindow::OnUsersLstDClick(wxListEvent &event)
{
	ShowPrivateChat(m_strUserSelected);
}

// Defines events for the context menu.
void SessionWindow::OnContextMenuClick(wxCommandEvent &event)
{
	wxString currentChannel = m_appInterface->GetCurrentChannel()->GetName();
	wxString targetUser = m_strUserSelected;

	switch(event.GetId())
	{
	case ID_CMD_MENU_SEND_PRIVATE_CHAT:
		ShowPrivateChat(m_strUserSelected);
		break;
	case ID_CMD_MENU_GRANT_VOICE:
		m_appInterface->RequestSetMediaVoice(targetUser);
		break;
	case ID_CMD_MENU_REMOVE_VOICE:
		m_appInterface->RequestUnsetMediaVoice();
		break;
	case ID_CMD_MENU_GRANT_SHARING:
		m_appInterface->RequestSetChannelMediaCollaborator(currentChannel, targetUser);
		break;
	case ID_CMD_MENU_REMOVE_SHARING:
		m_appInterface->RequestUnsetChannelMediaCollaborator(currentChannel, targetUser);
		break;
	case ID_CMD_MENU_REMOVE_ALL_SHARING:
		{
			const Channel *channel = m_appInterface->GetCurrentChannel();
			if(channel)
			{
				const UserList &userList = ((Channel *)channel)->GetUserList();
				UserIterator it, begin, end;
				begin = userList.GetBeginIterator();
				end = userList.GetEndIterator();
				for(it = begin; it != end; it++)
				{
					if(it->IsChannelCollaborator())
						m_appInterface->RequestUnsetChannelMediaCollaborator(currentChannel, it->GetNickName());
				}
			}
		}
		break;
	case ID_CMD_MENU_START_SERIES_COLLABORATION:
		{
			UserList *userList = m_appInterface->GetUserList();
			SeriesCollaborationDialog scDlg(this);
			scDlg.SetDisplayedUserList(userList->GetSubList(SLC_SERIES_COLLAB_ENABLED));
			scDlg.ShowModal();
		}
		break;
	case ID_CMD_MENU_GRANT_OP_STATUS:
		m_appInterface->RequestSetChannelOperator(currentChannel, targetUser);
		break;
	case ID_CMD_MENU_REMOVE_OP_STATUS:
		m_appInterface->RequestUnsetChannelOperator(currentChannel, targetUser);
		break;
	default:
		break;
	}
}

void SessionWindow::OnShowContextMenu(wxContextMenuEvent &event)
{
	//if (m_lstUsers->GetItemCount() > 0)
	if (m_indexUserSelected != USERLIST_FRM_ITEM_DESELECTED)
	{
		// Title for pop-up menus seems to be supported only in Windows
		// so it's "emulated" with a "dummy" menu item elsewhere
#ifdef WIN32
		m_menuCommands->SetTitle(m_strUserSelected);
#else
		m_menuCommands->SetLabel(ID_CMD_MENU_TITLE, m_strUserSelected);
#endif
		// Enable or disable certain items based on current user's flags
		// or on the current channel's state
		const Channel *channel = m_appInterface->GetCurrentChannel();
		if(!channel)
			return;	// Bail out if channel == NULL
		const ChannelUser *user = m_appInterface->GetCurrentChannelUser();
		if(!user)
			return;	// Bail out if user == NULL

		UserList &list = ((Channel *)m_appInterface->GetCurrentChannel())->GetUserList();
		ChannelUser *selectedUser = list.FindUserByName(m_strUserSelected);

		bool hasVoice = user->HasMediaVoice();
		bool isOperator = user->IsChannelOperator();
		bool mediaVoiceActive = channel->IsMediaVoiceActive();
		bool selectedUserHasVoice = selectedUser->HasMediaVoice();
		bool selectedUserHasSharing = selectedUser->IsChannelCollaborator();
		bool seriesCollabIsRunning = m_appInterface->IsSeriesCollaborationStarted();
		// Enable or disable commands based on current user's status
		m_menuCommands->Enable(ID_CMD_MENU_GRANT_VOICE, isOperator);
		m_menuCommands->Enable(ID_CMD_MENU_GRANT_SHARING, (hasVoice || (mediaVoiceActive && isOperator)) && !selectedUserHasVoice && !selectedUserHasSharing);
		m_menuCommands->Enable(ID_CMD_MENU_REMOVE_SHARING, (hasVoice || (mediaVoiceActive && isOperator)) && !selectedUserHasVoice && selectedUserHasSharing);
		m_menuCommands->Enable(ID_CMD_MENU_REMOVE_ALL_SHARING, hasVoice || (mediaVoiceActive && isOperator));
		m_menuCommands->Enable(ID_CMD_MENU_START_SERIES_COLLABORATION, hasVoice && !seriesCollabIsRunning);
		m_menuCommands->Enable(ID_CMD_MENU_STOP_SERIES_COLLABORATION, hasVoice && seriesCollabIsRunning);
		m_menuCommands->Enable(ID_CMD_MENU_GRANT_OP_STATUS, isOperator);
		m_menuCommands->Enable(ID_CMD_MENU_REMOVE_OP_STATUS, isOperator);

		// Disable the "remove voice" command if the selected user doesn't have voice;
		// and disable the "grant sharing" command if the selected user HAS voice.
		if(user)
		{
			m_menuCommands->Enable(ID_CMD_MENU_REMOVE_VOICE, (isOperator && selectedUser->HasMediaVoice()));
		}
		
		// Show the menu
		PopupMenu(m_menuCommands, ScreenToClient(event.GetPosition()));
	}

}

//Find the userName status ChatFrame on statusChat array
StatusChat SessionWindow::FindStatusChat(const wxString& userName, int &index, bool &found)
{
	StatusChat statusChat;
	int statusCount;
	
	found = false;
	index = -1;
	statusCount = (int)(m_arrStatusChat.GetCount());

	for(int i=0;i<statusCount;i++)
	{
		statusChat = m_arrStatusChat.Item(i);
		if (statusChat.GetNameChatFrame() == userName)
		{
			found = true;
			index = i;
			return statusChat;
		}
	}
	return statusChat;
}

// Triggered when the public chat window is closed (DEPRECATED)
void SessionWindow::OnPublicChatClose(wxCloseEvent &event)
{
	m_publicChatFrameExists = false;
	((wxWindow *)(event.GetEventObject()))->Destroy();
}

//When a chatFrame is closed this method change StatusChat.OpenedChatFrame to false 
void SessionWindow::SetStatusChat(StatusChat statusChat, const int &index)
{
	m_arrStatusChat.Item(index).SetOpenedChatFrame(statusChat.GetOpenedChatFrame());
	m_arrStatusChat.Item(index).SetNameChatFrame(statusChat.GetNameChatFrame());
}

/** Start Blink the user if received a private message.
* @param[in] username. The user thats sent the message.
*/
void SessionWindow::BlinkUserReceiveMessage(const wxString &username)
{
	/*for(int i=0;i<m_lstUsers->GetItemCount();i++)
		wxMessageBox(m_lstUsers->GetItemText(i));*/

	// long userItem = m_lstUsers->FindItem(-1, username);
	int i = 0;
	wxListItem item;
	bool found = false;
	
	item.SetId(0);
	item.SetColumn(USERLIST_FRM_LST_COL_USERS_INDEX);
	
	while(m_lstUsers->GetItem(item))
	{
		if (item.GetText() == username)
		{
			m_itemBlink.Add(i);
			m_lstUsers->BlinkItem(i, true);
			found = true;
			break;
		}
		item.SetId(++i);
	}
	if(!found)
		wxMessageBox(_("List Item not found"));
}

/**Continue Blinking a user if other enter or leave channel
*
*/
void SessionWindow::ContinueBlinkingUsername()
{
	if (!m_itemBlink.IsEmpty())
	{
		for(int i=0;i<m_itemBlink.GetCount();i++)
			m_lstUsers->BlinkItem(m_itemBlink.Item(i), true);
	}
}

// View mode management
// Private member function - sets current view mode. TODO: make it work.
void SessionWindow::SetViewMode(ViewMode mode)
{	
	TDCListIterator begin = m_tdcList.GetBeginIterator();
	TDCListIterator end = m_tdcList.GetEndIterator();
	TDCListIterator it;

	m_viewMode = mode;

	switch(m_viewMode)
	{
	case SW_VIEW_MODE_EQUAL_SPACING:
		m_mainPanelSizer->Detach(m_largeModePanel);
		m_equalModePanel->Show();
		m_largeModePanel->Hide();
		m_mainPanelSizer->Add(m_equalModePanel, 1, wxALL | wxEXPAND);
		m_mainPanelSizer->Layout();
		// Invalidate the selected large window
		m_tdcLargeVideo = NULL;
		// Relocation for the VDCs
		for(it = begin;it != end;it++)
			(*it)->SetNewParentWindow(m_equalModePanel);

		break;
	case SW_VIEW_MODE_ONE_LARGE_WINDOW:
		m_mainPanelSizer->Detach(m_equalModePanel);
		m_equalModePanel->Hide();
		m_largeModePanel->Show();
		m_mainPanelSizer->Add(m_largeModePanel, 1, wxALL | wxEXPAND);
		m_mainPanelSizer->Layout();
		// Relocation for the VDCs
		for(it = begin;it != end;it++)
			(*it)->SetNewParentWindow(m_largeModePanel);

		break;
	default:
		break;
	}
	// call this so all the VDC relocations take effect
	RefreshVideoDisplayArea();
	m_mainPanelSizer->Layout();
}

// ------------------------------- VideoDisplayControl management functions --------------------------------------------
/** Adds a transmission display control.
* @param[in] tdc. The transmission display control.
*/
void SessionWindow::AddTransmissionDisplayControl(TransmissionDisplayControl *tdc)
{
	const AppConfig &cfg = m_appInterface->GetAppConfig();
	
	VideoDisplayControl *vdc = tdc->GetVideoDisplayControl();
#if 0
	if(!m_mainPanel->IsShown())
	{
		m_vdcNoVideos->Hide();
		m_mainPanel->Show();
		m_subVertSizer->Layout();
	}
#endif
	if(!vdc->GetParent())
	{
		switch(m_viewMode)
		{
		case SW_VIEW_MODE_EQUAL_SPACING:
			tdc->Create(m_equalModePanel);
			break;
		case SW_VIEW_MODE_ONE_LARGE_WINDOW:
#if 0
			if(!m_vdcLargeVideo)	// If there is no large video, this is the one
			{
				tdc->Create(m_largeWindowPanel);
				m_largeWindowSizer->Add(tdc, 1, wxALL | wxEXPAND);
				m_largeWindowSizer->Layout();
				m_vdcLargeVideo = tdc;
			}
			else
			{
				tdc->Create(m_smallWindowPanel);
				m_smallWindowSizer->Add(tdc, 1, wxALL | wxEXPAND | wxSHAPED);
				m_smallWindowSizer->Layout();
			}
#endif
			// Layout calculations will be made in RefreshVideoDisplayArea() below
			tdc->Create(m_largeModePanel);
			break;
		default:
			return;		// Should never happen
		}
	}

	if(cfg.GetMode() != APP_MODE_FILE)
	{
		const ChannelUser *user = tdc->GetChannelUser();
		if(user)
			tdc->SetCaption(user->GetNickName());
	}

	if(!vdc->IsShown())
		vdc->Show();
	// Force 4/3 aspect ratio.
	// TODO: Move this to "advanced config" as someday one might want to use it with a wide 16/9 camera...
	tdc->SetAspectRatio(4.0f/3.0f);
	
	// Add newly-created control to the list, and update accordingly
	tdc->SetVDCEventHandler(this);
	m_tdcList.Add(tdc);
	RefreshVideoDisplayArea();

//	tdc->Play();
}

/** Removes a transmission display control.
* @param[in] tdc. The transmission display control.
*/
void SessionWindow::RemoveTransmissionDisplayControl(TransmissionDisplayControl *tdc)
{
	if(tdc)
	{
		int count = m_appInterface->GetTransmissionCount() - 1;

#if 0	// Not yet functional, kept for reference
		// "Unmagnify" if the currently magnified video is to be destroyed
		if(m_magnified)
		{
			if(m_frameSizer->GetItem(vdc))
			{
				m_frameSizer->Detach(vdc);
				m_frameSizer->Insert(0, m_gridSizer, 1, wxEXPAND | wxALL);
				m_frameSizer->Show(m_gridSizer);
				m_frameSizer->Layout();
				vdcList.StartAllControls();
				m_magnified = false;
			}
		}
#endif

		// Remove it from the internal list, stop it, and refresh the display area
		// If we're in mode 2, we have to check if the current "large" VDC is the one being removed
		if(m_viewMode == SW_VIEW_MODE_ONE_LARGE_WINDOW)
		{
			// additional check, the equality operator may return true with both being NULL
			if(m_tdcLargeVideo == tdc)
			{
				m_tdcLargeVideo = NULL;		// Reset the large mode selection logic
			}
		}
		m_tdcList.Remove(tdc);
		RefreshVideoDisplayArea();
	}
		// Refresh grid layout if not magnified
#if 0
	if(!m_magnified) 

	if(count == 0)	// last TDC removed
	{
		m_mainPanel->Hide();
		m_vdcNoVideos->Show();
		m_subVertSizer->Layout();
	}
#endif
}

/** This function restores order where there was chaos, i.e. it rearranges the TDC's in order.
*
*/
// TODO: Make this function sensitive to view mode (currently it only supports view mode 1).
void SessionWindow::RefreshVideoDisplayArea()
{
	int count = m_appInterface->GetTransmissionCount();
	TDCListIterator begin = m_tdcList.GetBeginIterator();
	TDCListIterator end = m_tdcList.GetEndIterator();
	TDCListIterator it;

/*------------------Update the bitrate-----------------------------------------------------------*/
	// (TODO: Check the code over to see if this really should be here.)
	const AppConfig &cfg = m_appInterface->GetAppConfig();
	if(cfg.GetMode() == APP_MODE_CONFERENCE)	// Bit rate display is for CONFERENCE mode only.
	{
		long transmissionProfile;
		m_appInterface->GetSettingsManager()->GetTransmissionProfile().ToLong(&transmissionProfile);

		Mode result;
		long channelBitrate;
		if(m_appInterface->GetCurrentChannel()->GetModeList().FindMode(IRM_MODE_CHANNEL_BITRATE, result))
				result.GetParameter().ToLong(&channelBitrate);
			else
				channelBitrate = 128;
		if (count == 0)
			m_userBitrate = SmallestBitrate(transmissionProfile, channelBitrate);
		else
			m_userBitrate = SmallestBitrate(transmissionProfile, channelBitrate/count);
	}
/*------------------END Update the bitrate-----------------------------------------------------------*/

	// The mirror counts as an active transmission, but is not counted by AppInterface.
	// TODO: Review this behavior.
	if(m_tdcMirror)	
		count++;
	// Same as above, for App Sharing display
	if(m_appSharingDisplay)
		count++;

	// If there are no transmissions, no point in doing this
	if(count > 0)
	{
		// Show the VDCs when coming back from count zero.
		if(!m_mainPanel->IsShown())
		{
			m_vdcNoVideos->Hide();
			m_mainPanel->Show();
			m_subVertSizer->Layout();
		}
		// This implements (or at least *should* implement) specific rules for each view mode.
		switch(m_viewMode)
		{
			case SW_VIEW_MODE_EQUAL_SPACING:
			{
				// Detach all from the sizer.
				m_gridSizer->Clear();
				m_gridSizer->Layout();
				// Reset the sizer's row/column count.
				m_gridSizer->SetRows(1);
				m_gridSizer->SetCols(1);
				
		
				// Add them back, in order. (TODO: Optimize this algorithm.)
				int i = 0;
				int cols = m_gridSizer->GetCols();
				int rows = m_gridSizer->GetRows();
				for(it = begin; it != end; it++)
				{
					i++;
					if(i > (rows * cols))
					{
						if(cols == rows)
							m_gridSizer->SetCols(++cols);
						else
							m_gridSizer->SetRows(++rows);
					}
					m_gridSizer->Add((*it)->GetVideoDisplayControl(), 0, wxEXPAND|wxALL);
					// (*it)->ShowStaticImage();
				}
				m_gridSizer->Layout();
			}
			break;
			case SW_VIEW_MODE_ONE_LARGE_WINDOW:
			{
				// Remove everything from sizers.
				m_smallWindowSizer->Clear();
				m_largeWindowSizer->Clear();
				// If there is no window selected to be in the large pane,
				// select the first in list
				if(!m_tdcLargeVideo)
				{
					if(!m_tdcList.IsEmpty())
						m_tdcLargeVideo = *begin;
				}
				// SetNewParentWindow and add to sizers in order
				for(it = begin; it != end; it++)
				{
					// Place the currently selected window in the "large" pane
					if(*it == m_tdcLargeVideo)
					{
						(*it)->SetNewParentWindow(m_largeWindowPanel);
						m_largeWindowSizer->Add((*it)->GetVideoDisplayControl(), 1, wxALL | wxEXPAND);
						m_largeWindowPanel->Layout();
					}
					// and all others in the "small" (it's actually resizeable) pane
					else
					{
						(*it)->SetNewParentWindow(m_smallWindowPanel);
						m_smallWindowSizer->Add((*it)->GetVideoDisplayControl(), 1, wxTOP | wxBOTTOM | wxEXPAND, 4);
						m_smallWindowPanel->Layout();
					}
				}
				m_smallWindowSizer->Layout();
				m_largeWindowSizer->Layout();
			}
			break;
			default:
				break;
		}
	}
	else
	{
		m_mainPanel->Hide();
		m_vdcNoVideos->Show();
		m_subVertSizer->Layout();
		if(m_viewMode == SW_VIEW_MODE_ONE_LARGE_WINDOW)
			m_tdcLargeVideo = NULL;
	}


	//Test if m_btnChangeViewMode and m_btnShowHideSplit should be disable or enable, according to number of transmissions
	if (count < 2)
	{
		m_btnChangeViewMode->Disable();
		m_btnShowHideSplit->Disable();
		// Hide/show splitter only makes sense for view mode 2
		if(m_viewMode == SW_VIEW_MODE_ONE_LARGE_WINDOW)
		{
			// On this condition, the right-hand pane should go away
			if(m_splitter->IsSplit())
				m_splitter->Unsplit();
		}
		m_hasMultipleColaborations = false;
	}
	else
	{
		m_btnChangeViewMode->Enable();
		m_btnShowHideSplit->Enable();
		// Hide/show splitter only makes sense for view mode 2
		if(m_viewMode == SW_VIEW_MODE_ONE_LARGE_WINDOW)
		{
			// On this condition, the right-hand pane should come back
			if(!m_splitter->IsSplit())
				m_splitter->SplitVertically(m_largeWindowPanel, m_smallWindowPanel, -160);
		}
		m_hasMultipleColaborations = true;
	}

	UpdateControlBarButtonStates();

}
// ------------------------------------ END VDC management functions ---------------------------------------------

/** Updates the poll statistics.
*
*/
void SessionWindow::UpdateActivePollStats()
{
	if(!m_appInterface)
		return;

	const PollInfo *info = m_appInterface->GetActivePoll();
	if(!info)
		return;

	// find the poll window
	wxWindow* sendPollWindow = wxWindow::FindWindowById( ID_SENDPOLLFRAME );
    if (sendPollWindow)
    {
		((SendPollFrame *)sendPollWindow)->UpdateNumberOfVotes();
	}
}

/** Shows the poll statistics dialog.
* @param[in] info. Information of the statistics.
*/
void SessionWindow::ShowPollStats(const PollInfo &info)
{
	// Kill the "answer window" when receiving the stats (last report).
	if(m_pollAnswerDialog)
	{
		m_pollAnswerDialog->Destroy();
		m_pollAnswerDialog = NULL;
	}

	if(!m_pollStatsDialog)
	{
		m_pollStatsDialog = new PollStatsDialog(info, this);
		m_pollStatsDialog->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SessionWindow::OnPollStatsDialogClose), NULL, this);
		m_pollStatsDialog->Show();
	}
	// If dialog exists, update info only
	else
		m_pollStatsDialog->SetPollInfo(info);
}

/** Shows the poll answer dialog.
*
*/
void SessionWindow::ShowPollAnswerDialog()
{
	if(m_pollAnswerDialog)
		m_pollAnswerDialog->Destroy();

	m_pollAnswerDialog = new PollAnswerDialog;
	m_pollAnswerDialog->SetAppInterface(m_appInterface);
	m_pollAnswerDialog->Create(this);
	m_pollAnswerDialog->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SessionWindow::OnPollAnswerDialogClose), NULL, this);
	m_pollAnswerDialog->Show();
}

void SessionWindow::OnPollStatsDialogClose(wxCloseEvent &event)
{
	m_pollStatsDialog->Destroy();
	m_pollStatsDialog = NULL;
}

void SessionWindow::OnPollAnswerDialogClose(wxCloseEvent &event)
{
	m_pollAnswerDialog->Destroy();
	m_pollAnswerDialog = NULL;
}

void SessionWindow::OnPublicChatSendBtnClick(wxCommandEvent &event)
{
	wxString strMessage = m_chatInputBox->GetValue();
	if (strMessage != wxEmptyString)
	{
		// Can't proceed without m_appInterface
		wxCHECK_RET(m_appInterface, wxT("Aaaaah!! m_appInterface is NULL!! Now what do I do?!"));
		
		const Channel *channel = m_appInterface->GetCurrentChannel();
		wxString userName = m_appInterface->GetCurrentUser()->GetNickName();
		// Send message to channel		
		m_appInterface->SendChannelMessage(channel->GetName(), strMessage);
		// Echo the message on this window
		SetPublicChatMessage(userName, strMessage);
		
		//Clear txtSendMessage and disable send button
		m_chatInputBox->Clear();
		m_chatInputBox->SetFocus();
		m_chatSendBtn->Disable();
	}
}
void SessionWindow::OnPublicChatInputTextChange(wxCommandEvent &event)
{
	m_chatSendBtn->Enable((m_chatInputBox->GetValue() != wxEmptyString));
}

void SessionWindow::OnFileProgressSliderChange(wxScrollEvent &event)
{
// Seek in file playback. Currently disabled due to a bug somewhere else.
// Compile with ENABLE_FILE_SEEK defined to enable.
#ifdef ENABLE_FILE_SEEK
	int sldMin = m_fileProgressSlider->GetMin();
	int sldMax = m_fileProgressSlider->GetMax();
	float percent = (float)event.GetPosition() / (sldMax - sldMin);
	m_appInterface->FileSeek(percent);
#endif
}

void SessionWindow::OnPlayPauseFilePlayback(wxCommandEvent &event)
{
	if(m_stopped)
	{
		m_stopped = false;
		AppConfig cfg = m_appInterface->GetAppConfig();
		m_appInterface->InitFilePlayback(cfg.GetFileName());
		wxBitmap bmpPauseFilePlayback = wxBitmap(USERLIST_FRM_BMP_PAUSEFILEPLAYBACK, wxBITMAP_TYPE_PNG);
		m_btnPlayPauseFilePlayback->SetBitmapLabel(bmpPauseFilePlayback);

		// enable file seek slider
		m_fileProgressSlider->Enable(true);
	}
	else
	{
		if(m_paused)
		{
			m_paused = false;
			m_appInterface->FilePlay();
			wxBitmap bmpPauseFilePlayback = wxBitmap(USERLIST_FRM_BMP_PAUSEFILEPLAYBACK, wxBITMAP_TYPE_PNG);
			m_btnPlayPauseFilePlayback->SetBitmapLabel(bmpPauseFilePlayback);

			// enable file seek slider
			m_fileProgressSlider->Enable(true);
		}
		else
		{
			m_paused = true;
			m_appInterface->FilePause();
			wxBitmap bmpPlayFilePlayback = wxBitmap(USERLIST_FRM_BMP_PLAYFILEPLAYBACK, wxBITMAP_TYPE_PNG);
			m_btnPlayPauseFilePlayback->SetBitmapLabel(bmpPlayFilePlayback);

			// disable file seek slider
			m_fileProgressSlider->Disable();
		}
	}
}

void SessionWindow::OnStopFilePlayback(wxCommandEvent &event)
{
	if(!m_stopped)
	{
		m_stopped = true;
		m_appInterface->TerminateAllConferences();
		m_tdcList.Clear();
		RefreshVideoDisplayArea();
		wxBitmap bmpPlayFilePlayback = wxBitmap(USERLIST_FRM_BMP_PLAYFILEPLAYBACK, wxBITMAP_TYPE_PNG);
		m_btnPlayPauseFilePlayback->SetBitmapLabel(bmpPlayFilePlayback);

		// rewind and disable file seek slider
		m_fileProgressSlider->SetValue(m_fileProgressSlider->GetMin());
		m_fileProgressSlider->Disable();
	}
}

/** @brief This is the function that handles the double-click event for all VDCs contained here.
 */
void SessionWindow::OnVDCDblClick(wxCommandEvent &event)
{
	if(m_viewMode == SW_VIEW_MODE_ONE_LARGE_WINDOW)
	{
		// double-clicked video is the "large" one
		if(m_tdcLargeVideo && (m_tdcLargeVideo == (TransmissionDisplayControl *)(event.GetClientData())))
			TogglePanels();
		m_tdcLargeVideo = (TransmissionDisplayControl *)(event.GetClientData());
		RefreshVideoDisplayArea();
	}
	else	// we're in display mode 1
	{
		// do the same procedure, for any VDC double-clicked
		TogglePanels();
	}
}

void SessionWindow::OnTimer(wxTimerEvent &event)
{
	if(!m_appInterface)
		return;

	const AppConfig &cfg = m_appInterface->GetAppConfig();
	const Channel *channel = m_appInterface->GetCurrentChannel();
	const User *user = m_appInterface->GetCurrentUser();
	Mode mode;
	wxString msg;
	switch(cfg.GetMode())
	{
	case APP_MODE_CONFERENCE:
		{
			msg << wxString::Format(LABEL_FRM_CHANNELS_LIST_STATUSTEXT_CH_OK, user->GetNickName().c_str(), 
				channel->GetName().c_str())
				<< wxT(" | ") 
				<< _("Connected on server:") << wxT(" ") << cfg.GetConfServer() << wxT(" | ") 
				<< _("Session time:") << wxT(" ") << m_appInterface->GetSessionTime().Format(wxT("%H:%M:%S"));
			
			//Old
			/*wxString strChannelBitrate;
			long channelBitrate;
			wxString strTransmissionProfile = m_appInterface->GetSettingsManager()->GetTransmissionProfile();
			long transmissionProfile;
			strTransmissionProfile.ToLong(&transmissionProfile);
			wxString strSwBitrate;

			if(channel->GetModeList().FindMode(IRM_MODE_CHANNEL_BITRATE, mode))
			{
				strChannelBitrate = mode.GetParameter();
				strChannelBitrate.ToLong(&channelBitrate);
				
				strSwBitrate.Clear();
				if (channelBitrate <= transmissionProfile)
					strSwBitrate << wxString::Format(wxT("%d"), channelBitrate);
				else
					strSwBitrate << wxString::Format(wxT("%d"), transmissionProfile);
				
				msg << wxT(" | ") << _("Bitrate:") << wxT(" ") << strSwBitrate << wxT(" kbps");
			}*/

			//Show the bitrate on statusbar
			wxString strSwBitrate = wxString::Format(wxT("%d"), m_userBitrate);
			msg << wxT(" | ") << _("Bitrate:") << wxT(" ") << strSwBitrate << wxT(" kbps");
		}
		break;
	case APP_MODE_MULTICAST:
		{
			msg << _("Listening on ") << cfg.GetMcastAddress() << wxT(":") << cfg.GetMcastPort();
		}
		break;
	case APP_MODE_FILE:
		{
			msg << _("Playing from ") << cfg.GetFileName();
		}
		break;
	default:
		break;
	}
	m_lblStatusText->SetLabel(msg);
}

// Private functions to help handling capture

/** Starts the capture.
* Private method.
*/
void SessionWindow::StartCapture()
{
	if(m_appInterface->InitializeCapture())
	{
		m_appInterface->RegisterCaptureVideoSink(m_vdcCaptureView);
		m_appInterface->RequestSetUserCameraOn();
		m_btnPlayStopCaptureCamera->SetBitmapLabel(m_bmpPlayCaptureCamera);
		if(m_tdcMirror)
		{
			m_appInterface->RegisterCaptureVideoSink(m_tdcMirror);
			m_tdcMirror->NotifyStart();
		}
	}
	else
		wxMessageBox(_("Error initializing capture."), _("Error"), wxCENTRE | wxICON_ERROR, this);
}

/** Stops the capture.
* Private method.
*/
void SessionWindow::StopCapture()
{
	m_appInterface->ReleaseCapture();
	m_appInterface->RequestUnsetUserCameraOn();
	m_appInterface->UnregisterCaptureVideoSink(m_vdcCaptureView);
	m_btnPlayStopCaptureCamera->SetBitmapLabel(m_bmpStopCaptureCamera);
	if(m_tdcMirror)
		m_appInterface->UnregisterCaptureVideoSink(m_tdcMirror);
}

/**  Auto play the collaboration of the capture.
*
*/
void SessionWindow::CollaborationAutoPlay()
{
	const ChannelUser *user = m_appInterface->GetCurrentChannelUser();

	//Future implementation
	/*if(((user->IsChannelCollaborator() || user->HasMediaVoice()) && (m_appInterface->IsCaptureEnabled())) && (!m_appInterface->IsTransmittingMedia()))*/
	if(((user->IsChannelCollaborator() || user->HasMediaVoice()) && (!m_appInterface->IsTransmittingMedia())))
	{
		m_appInterface->InitializeMediaColaboration(m_userBitrate);
		CreateMirror();
		//// Update button states
		UpdateCommandButtonStates();
		UpdateControlBarButtonStates();
	}
}

/** Stops the capture collaboration.
*
*/
void SessionWindow::CollaborationStop()
{
	const ChannelUser *user = m_appInterface->GetCurrentChannelUser();

	//Future Implementation
	/*if(((user->IsChannelCollaborator() || user->HasMediaVoice()) && (m_appInterface->IsCaptureEnabled())) && (m_appInterface->IsTransmittingMedia()))*/
	if(((user->IsChannelCollaborator() || user->HasMediaVoice()) && (m_appInterface->IsTransmittingMedia())))
	{
		m_appInterface->StopMediaColaboration();
		DestroyMirror();
		//// Update button states
		UpdateCommandButtonStates();
		UpdateControlBarButtonStates();
	}
}

/** Register application sharing display control.
*
*/
void SessionWindow::RegisterAppSharingDisplayControl()
{
#if 0
	if(!m_appInterface || !m_asViewFrame)
		return;
	VideoDisplayControl *vdc = m_asViewFrame->GetVideoDisplayControl();
	if(vdc)
		m_appInterface->RegisterAppSharingVideoSink(vdc);
#endif
	m_appSharingDisplay = new TransmissionDisplayControl;
	m_appSharingDisplay->SetSpecialFlag(true);
	// Reset the "large video" pointer
	m_tdcLargeVideo = NULL;
	// and then let RefreshVideoDisplayArea() do its thing
	AddTransmissionDisplayControl(m_appSharingDisplay);
	m_appInterface->RegisterAppSharingVideoSink(m_appSharingDisplay);
}

/** Unregister application sharing display control.
*
*/
void SessionWindow::UnregisterAppSharingDisplayControl()
{
	RemoveTransmissionDisplayControl(m_appSharingDisplay);
	if(m_tdcLargeVideo == m_appSharingDisplay)
		m_tdcLargeVideo = NULL;
	RefreshVideoDisplayArea();
	m_appInterface->UnregisterAppSharingVideoSink(m_appSharingDisplay);
	m_appSharingDisplay->Destroy();
	m_appSharingDisplay = NULL;
}

/** Send the whiteboard message.
* @param[in] channel. The channel name.
* @param[in] user. The user name.
* @param[in] data. The data to be send.
*/
void SessionWindow::OnWhiteboardMessage(const wxString &channel, const wxString &user, const wxString &data)
{
	if(m_whiteboardManager)
		m_whiteboardManager->OnWhiteboardChannelMessage(channel, user, data);
}

/** Start the whiteboard transmission.
* @param[in] filePath. The file path.
*/
void SessionWindow::StartWhiteboardFileTransmission(const wxString &filePath)
{
	SkinLite *skinLite = (SkinLite *)GetParent();
	skinLite->StartWhiteboardFileTransmission(filePath);
}

/** Creates the mirror.
* Private method.
*/
void SessionWindow::CreateMirror()
{
	// Create mirror
	if(!m_tdcMirror)
	{
		m_tdcMirror = new TransmissionDisplayControl;
		m_tdcMirror->Create(m_mainPanel);
		m_tdcMirror->SetChannelUser((ChannelUser *)m_appInterface->GetCurrentChannelUser());
		// Register it as a capture sink if capture is enabled
		if(m_appInterface->IsCaptureEnabled())
		{	
			m_appInterface->RegisterCaptureVideoSink(m_tdcMirror);
		}
	}
	// Add it to the display area
	m_tdcMirror->NotifyStart();
	AddTransmissionDisplayControl(m_tdcMirror);
}

/** Destroys the mirror.
* Private method.
*/
void SessionWindow::DestroyMirror()
{
	if(m_tdcMirror)
	{
		m_appInterface->UnregisterCaptureVideoSink(m_tdcMirror);
		RemoveTransmissionDisplayControl(m_tdcMirror);
		m_tdcMirror->Destroy();
		m_tdcMirror = NULL;
		RefreshVideoDisplayArea();
	}
}

// Event for init send or stop camera capture
void SessionWindow::OnPlayStopCaptureCameraBtnClick(wxCommandEvent &event)
{
	wxCHECK_RET(m_appInterface, wxT("SessionWindow::OnPlayStopCaptureCameraBtnClick(): No AppInterface!!"));
	if(!m_appInterface->IsCaptureEnabled())
		StartCapture();
	else
		StopCapture();

	UpdateCommandButtonStates();
}

//Event for init send video conference
void SessionWindow::OnPlaySendVideoCameraBtnClick(wxCommandEvent &event)
{
	//m_appInterface->InitializeMediaColaboration();
	//CreateMirror();
	// Update button states
	//UpdateCommandButtonStates();
	CollaborationAutoPlay();
}

//Event for init stop video conference
void SessionWindow::OnStopSendVideoCameraBtnClick(wxCommandEvent &event)
{
	//m_appInterface->StopMediaColaboration();
	//DestroyMirror();
	// Update button states
	//UpdateCommandButtonStates();
	CollaborationStop();
}

/*--------------------------------Command buttons events area---------------------------------------------*/

//Event for Request voice button
void SessionWindow::OnRequestVoiceBtnClick(wxCommandEvent &event)
{
	wxCHECK_RET(m_appInterface, wxT("SessionWindow::OnRequestVoiceBtnClick(): I need AppInterface to work!!"));
	const ChannelUser *user = m_appInterface->GetCurrentChannelUser();
	if(user->IsRequestingVoice())
		m_appInterface->SendMediaVoiceRequestCancel();
	else
		m_appInterface->SendMediaVoiceRequest();
	UpdateCommandButtonStates();
}


// Private function that updates the command button states based on the
/** User flags for the currently active user.
* Private method.
*/
void SessionWindow::UpdateCommandButtonStates()
{
	wxCHECK_RET(m_appInterface, wxT("SessionWindow::UpdateCommandButtonStates(): I need AppInterface to work!!"));
	const ChannelUser *user = m_appInterface->GetCurrentChannelUser();
	// This function may now get called when not in CONFERENCE mode, 
	// so we have to take that into account.
	const AppConfig &cfg = m_appInterface->GetAppConfig();
	if(cfg.GetMode() == APP_MODE_CONFERENCE)
	{
		// -------------------- Request Voice button -------------------- //
		// Disable the button if we have the voice or collaboration
		m_btnRequestVoice->Enable(!(user->HasMediaVoice() || user->IsChannelCollaborator()));
		
		// Enable the buttons if we have the voice
		m_btnMenuSendFile->Enable(user->HasMediaVoice());
		m_btnMenuPoll->Enable(user->HasMediaVoice());
		m_btnMenuPlaylist->Enable(user->HasMediaVoice());
		
		if(!(user->IsRequestingVoice()))
		{
			m_btnRequestVoice->SetBitmapLabel(m_bmpHandOff);
			m_btnRequestVoice->SetToolTip(_("Request voice"));
		}
		else
		{
			m_btnRequestVoice->SetBitmapLabel(m_bmpHandOn);
			m_btnRequestVoice->SetToolTip(_("Cancel voice request"));
		}

		//If user lost voice, cancel send file
		if (!user->HasMediaVoice())
			FileTransferSendUserLostVoice();

		// Whiteboard stuff.
		if (user->HasMediaVoice())
		{
			m_whiteboardManager->Enable();
			m_whiteboardManager->EnableImageControl();
		}

		else
		{
			if(user->IsChannelCollaborator())
			{
				m_whiteboardManager->Enable();
				m_whiteboardManager->DisableImageControl();
			}

			else
			{
				m_whiteboardManager->Disable();
			}
		}
			
		// ------------- Start transmission ("play") button ------------ //
		// ---------------- and Stop transmission button --------------- //
		
		//Future implementation
		/*if((user->IsChannelCollaborator() || user->HasMediaVoice()) && (m_appInterface->IsCaptureEnabled()))*/
		if((user->IsChannelCollaborator() || user->HasMediaVoice()))
		{
			if(m_appInterface->IsTransmittingMedia())
			{
				m_btnPlaySendVideoCamera->Disable();
				m_btnStopSendVideoCamera->Enable();
				m_btnStopSendVideoCamera->SetToolTip(_("Stop media transmission"));
			}
			else
			{
				m_btnStopSendVideoCamera->Disable();
				m_btnPlaySendVideoCamera->Enable();
				m_btnPlaySendVideoCamera->SetToolTip(_("Start media transmission"));
			}
		}
		else
		{
			m_btnPlaySendVideoCamera->Disable();
			m_btnStopSendVideoCamera->Disable();
			// FIXME: should this really be here?
			if(m_tdcMirror)
				DestroyMirror();
		}
	
		//Configuration menu button
		if ((!m_appInterface->IsTransmittingMedia()) && (!m_appInterface->IsCaptureEnabled()))
			m_btnMenuConfigure->Enable(true);
		else
			m_btnMenuConfigure->Enable(false);
	}	// if (APP_MODE_CONFERENCE)
	

	//ShowHideSplit menu button
	/*if (m_splitModeIsActive) && (m_viewMode == SW_VIEW_MODE_EQUAL_SPACING)
		m_btnChangeViewMode->Enable();
	else
		m_btnChangeViewMode->Disable();*/

}

/** Updates the buttons states in control bar.
* Private method.
*/
void SessionWindow::UpdateControlBarButtonStates()
{
	//ChangeViewMode menu button
	if (m_hasMultipleColaborations) //If have more than one collaborations
	{
		switch(m_viewMode)
		{
			case SW_VIEW_MODE_EQUAL_SPACING:
				m_btnChangeViewMode->Enable();
				m_btnShowHideSplit->Disable();
				break;
			case SW_VIEW_MODE_ONE_LARGE_WINDOW:
				m_btnShowHideSplit->Enable();
				if (m_splitModeIsActive)
					m_btnChangeViewMode->Enable();
				else
					m_btnChangeViewMode->Disable();
				break;
			default:
				return;
		}
	}
}

/** Handles visibility of the top/bottom panels when double-clicking VDCs.
 */
void SessionWindow::TogglePanels()
{
	// Find the panel with the bitmap buttons
	wxWindow *bottomBarBtnPanel = FindWindowById(ID_USERSLISTFRM_PANEL2, this);
	// go into full-screen (TODO: Check this for coherence with business rules), AND...
	if(!IsFullScreen())
		ShowFullScreen(true);
	// toggle the lower panel
	if(m_statusTextPanel->IsShown())
	{
		m_statusTextPanel->Hide();
		bottomBarBtnPanel->Hide();
		m_mainVertPanelSizer->Layout();
	}
	else
	{
		m_statusTextPanel->Show();
		bottomBarBtnPanel->Show();
		m_mainVertPanelSizer->Layout();
	}
	// and hide the upper panel if it is shown
	if(m_menuPanel && m_menuPanel->IsShown())
		m_menuPanel->Hide();			
}

/** Returns the smallest bitrate between bitrate1 and bitrate2
* @param[in] bitrate1. The first bitrate.
* @param[in] bitrate2. The second bitrate.
* @return Returns the smallest bitrate.
* Private method.
*/
long SessionWindow::SmallestBitrate(const long &bitrate1, const long &bitrate2)
{
	long minorBitrate;

	//Select the smaller bitrate
	if (bitrate1 < bitrate2)
		minorBitrate = bitrate1;
	else
		minorBitrate = bitrate2;

	////Bitrate until 33k
	//if (minorBitrate <= SW_MINORBITRATE_33K)
	//	return SW_MINORBITRATE_33K;
	
	//Bitrate between until value smaller than 64k, return 33k
	if (minorBitrate < SW_MINORBITRATE_64K)
		return SW_MINORBITRATE_33K;

	//Bitrate between 64k and smaller than 128k, return 64k
	if (minorBitrate < SW_MINORBITRATE_128K)
		return SW_MINORBITRATE_64K;

	//Bitrate between 128k and smaller than 256k, return 128k
	if (minorBitrate < SW_MINORBITRATE_256K)
		return SW_MINORBITRATE_128K;

	//Bitrate between 256k and smaller than 384k, return 256k
	if (minorBitrate < SW_MINORBITRATE_384K)
		return SW_MINORBITRATE_256K;

	//Bitrate between 384k and smaller than 512k, return 384k
	if (minorBitrate < SW_MINORBITRATE_512K)
		return SW_MINORBITRATE_384K;

	//Bitrate between 512k and smaller than 1024k, return 512k
	if (minorBitrate < SW_MINORBITRATE_1M)
		return SW_MINORBITRATE_512K;

	//Bitrate equal 1024k
	return SW_MINORBITRATE_1M;
}

/** Enables send file transfer menu button.
*
*/
void SessionWindow::EnableSendFileTransfer()
{
	m_btnMenuSendFile->Enable();
}

/** Disables send file transfer menu button.
*
*/
void SessionWindow::DisableSendFileTransfer()
{
	m_btnMenuSendFile->Disable();
}

/** Enables send playlist menu button.
*
*/
void SessionWindow::EnableSendPlaylist()
{
	m_btnMenuPlaylist->Enable();
}

/** Disables send playlist menu button.
*
*/
void SessionWindow::DisableSendPlaylist()
{
	m_btnMenuPlaylist->Disable();
}

/*------------------------------END Command buttons events area-------------------------------------------*/

/** Closes the SendfilesFrame if user lost voice.
*
*/
void SessionWindow::FileTransferSendUserLostVoice()
{
	SkinLite *skinLite = (SkinLite *)GetParent();
	skinLite->OnFileTransferSendError();
}


/*!
 * Should we show tooltips?
 */

bool SessionWindow::ShowToolTips()
{
    return true;
}

// ----- View mode 2 events -----
#if 0
// Respond to a sash drag.
void SessionWindow::OnSashDrag(wxSashEvent &event)
{
	if(event.GetDragStatus() == wxSASH_STATUS_OK)
	{
		wxSize panelSize = m_largeModePanel->GetSize();
		wxSize newSmallWindowSize = wxSize(event.GetDragRect().GetSize().GetWidth(), panelSize.GetHeight());
		wxSize newLargeWindowSize(panelSize.GetWidth() - newSmallWindowSize.GetWidth(), panelSize.GetHeight());
		m_largeWindowPanel->SetSize(newLargeWindowSize);
		wxRect newSmallWindowRect(m_largeWindowPanel->GetRect().GetTopRight(), newSmallWindowSize);
		m_sashWindow->SetSize(newSmallWindowRect);
		
	}
	wxPoint pos = m_sashWindow->GetPosition();
	wxSize size = m_sashWindow->GetSize();
	m_appInterface->SetDebugMessage(wxString::Format(wxT("<SkinLite> SessionWindow::OnSashDrag(): Sash window @ (%d, %d), size %d x %d"), pos.x, pos.y, size.GetWidth(), size.GetHeight()));
}

// Recalculate sizes when panel is resized.
void SessionWindow::OnLargeModePanelSize(wxSizeEvent &event)
{
	m_sashWindow->SetSize(wxSize(m_smallWindowPanel->GetMinSize().GetWidth(), event.GetSize().GetHeight()));
	m_largeWindowPanel->SetSize(m_largeModePanel->GetSize().GetWidth() - m_sashWindow->GetSize().GetWidth(), event.GetSize().GetHeight());
	wxRect sashRect = m_sashWindow->GetRect();
	wxRect largeRect = m_largeWindowPanel->GetRect();
	sashRect.SetX(largeRect.GetRight());
	sashRect.SetY(largeRect.GetTop());
	
	m_sashWindow->SetSize(sashRect);
	m_sashWindow->SetMaximumSizeX(GetSize().GetWidth() - m_largeWindowPanel->GetMinSize().GetWidth());
	m_sashWindow->SetMinimumSizeY(m_largeModePanel->GetSize().GetWidth());
	m_sashWindow->SetMaximumSizeY(m_sashWindow->GetMinimumSizeY());
	
	// debug
	wxPoint pos = m_sashWindow->GetPosition();
	wxSize size = m_sashWindow->GetSize();
	m_appInterface->SetDebugMessage(wxString::Format(wxT("<SkinLite> SessionWindow::OnLargeModePanelSize(): Sash window @ (%d, %d), size %d x %d"), pos.x, pos.y, size.GetWidth(), size.GetHeight()));
	
}
#endif
/*----------------------------------------Menu Buttons--------------------------------------------------*/
/*They creates and show yours respectives frames only for tests.
 *They should be modified later
*/
void SessionWindow::OnMenuSendFileBtnClick(wxCommandEvent &event)
{
	SkinLite *skinLite = (SkinLite *)GetParent();
	skinLite->CreateSendFiles();
}

void SessionWindow::OnMenuPollBtnClick(wxCommandEvent &event)
{
	wxWindow* oldSendPollWindow = wxWindow::FindWindowById( ID_SENDPOLLFRAME );
    if (oldSendPollWindow)
    {
		// oldSendPollWindow->SetFocus();
		oldSendPollWindow->Raise();		// this brings it to the top of the z-order!
    }
	else
	{
		SendPollFrame *sendPollFrame = new SendPollFrame(this);
		sendPollFrame->SetAppInterface(m_appInterface);
		sendPollFrame->Show();
		sendPollFrame->Raise();
	}
}

void SessionWindow::OnChannelModesBtnClick(wxCommandEvent &event)
{
	ChannelModesDialog *channelModesDialog = new ChannelModesDialog(this, m_appInterface);
	channelModesDialog->ShowModal();
}

void SessionWindow::OnMenuConfigureBtnClick(wxCommandEvent &event)
{
	SkinLite *skinLite = (SkinLite *)GetParent();
	skinLite->ConfigureBtnClick();
}

void SessionWindow::OnMenuWhiteboardBtnClick(wxCommandEvent &event)
{
	if(m_whiteboardManager)
		m_whiteboardManager->ShowWhiteboardFrame();
}

void SessionWindow::OnMenuPlaylistBtnClick(wxCommandEvent &event)
{
	SkinLite *skinLite = (SkinLite *)GetParent();
	skinLite->CreatePlaylist();
}

/*------------------------------------END Menu Buttons--------------------------------------------------*/
