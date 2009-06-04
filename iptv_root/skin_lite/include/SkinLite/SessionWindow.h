/////////////////////////////////////////////////////////////////////////////
// Name:        SessionWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     19/02/2008 15:33:55
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _SESSIONWINDOW_H_
#define _SESSIONWINDOW_H_


/*!
 * Includes
 */
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/splitter.h>

#include "Constants.h"
#include "ChatFrame.h"
#include "StatusChat.h"
#include "AppInterface.h"
#include "VideoDisplayControl.h"
#include "TransmissionDisplayControl.h"
#include "TransmissionDisplayControlList.h"
#include "SophisticatedListControl.h"
#include "SendFilesFrame.h"
#include "PollStatsDialog.h"
#include "PollAnswerDialog.h"
#include "WhiteboardManager.h"

/*!
 * Forward declarations
 */
class wxBoxSizer;
class StatusChat;

enum{
	ID_USERSLISTFRM_MAIN_PANEL = 10002,
	ID_USERSLISTFRM_MENU_PANEL,
	ID_USERSLISTFRM_PANEL1,
	ID_USERSLISTFRM_MENU_BTN_BOUNCING_SHAPE,
	ID_USERSLIST_FRM_LST_USERS,
	ID_USERSLISTFRM_PANEL3,
	ID_USERSLISTFRM_PANEL2,
	ID_USERSLIST_FRM_BTN_SHOW_MENU,
	ID_USERSLIST_FRM_BTN_PUBLIC_CHAT,
	ID_USERSLIST_FRM_BTN_USERLIST,
	ID_USERSLIST_FRM_BTN_CHANGE_VIEW,
	ID_USERSLIST_FRM_BTN_SHOWHIDE_SPLIT,
	ID_USERSLIST_FRM_BTN_FULLSCRREN,
	ID_USERSLIST_FRM_BTN_BACK,
	ID_USERSLIST_FRM_SESSION_TIME_TIMER,
	ID_USERSLISTFRM_BTN_PLAYPAUSEFILEPLAYBACK,
	ID_USERSLISTFRM_BTN_STOPFILEPLAYBACK,
	ID_USERSLISTFRM_BTN_PLAYSENDVIDEOCAMERA,
	ID_USERSLISTFRM_BTN_STOPSENDVIDEOCAMERA,
	ID_USERSLISTFRM_BTN_PLAY_STOP_CAPTURECAMERA,
	ID_USERSLISTFRM_BTN_MENU_SENDFILES,
	ID_USERSLISTFRM_BTN_MENU_POLL,
	ID_USERSLISTFRM_BTN_CHANNEL_MODES,
	ID_USERSLISTFRM_BTN_MENU_CONFIGURE,
	ID_USERSLISTFRM_BTN_MENU_WBOARD,
	ID_USERSLISTFRM_BTN_MENU_PLAYLIST,
	ID_USERSLISTFRM_BTN_CMD_REQUESTVOICE
};

enum menuItemID
{
	ID_CMD_MENU_FIRST = 12000,
	ID_CMD_MENU_SEND,
	ID_CMD_MENU_SEND_PRIVATE_CHAT,
	ID_CMD_MENU_GRANT_VOICE,
	ID_CMD_MENU_REMOVE_VOICE,
	ID_CMD_MENU_GRANT_SHARING,
	ID_CMD_MENU_REMOVE_SHARING,
	ID_CMD_MENU_REMOVE_ALL_SHARING,
	ID_CMD_MENU_START_SERIES_COLLABORATION,
	ID_CMD_MENU_STOP_SERIES_COLLABORATION,
	ID_CMD_MENU_GRANT_OP_STATUS,
	ID_CMD_MENU_REMOVE_OP_STATUS,
	ID_CMD_MENU_TITLE,
	ID_CMD_MENU_LAST
};

typedef enum
{
	SW_VIEW_MODE_NULL = 0,
	SW_VIEW_MODE_EQUAL_SPACING,
	SW_VIEW_MODE_ONE_LARGE_WINDOW
} ViewMode;

#define SW_VIEW_MODE_CONFERENCE_DEFAULT		SW_VIEW_MODE_ONE_LARGE_WINDOW
#define SW_VIEW_MODE_DEFAULT				SW_VIEW_MODE_EQUAL_SPACING
#define SW_MINORBITRATE_33K 33
#define SW_MINORBITRATE_64K 64
#define SW_MINORBITRATE_128K 128
#define SW_MINORBITRATE_256K 256
#define SW_MINORBITRATE_384K 384
#define SW_MINORBITRATE_512K 512
#define SW_MINORBITRATE_1M 1024

WX_DECLARE_OBJARRAY(StatusChat, ArrayOfStatusChat);

/*!
 * Control identifiers
 */
#define ID_USERSLISTFRM 10000
#define SYMBOL_USERSLISTFRAME_STYLE wxDEFAULT_FRAME_STYLE
#define SYMBOL_USERSLISTFRAME_TITLE _("IP.TV - Viewer")
#define SYMBOL_USERSLISTFRAME_IDNAME 10000
#define SYMBOL_USERSLISTFRAME_SIZE wxSize(650, 400)
#define SYMBOL_USERSLISTFRAME_POSITION wxDefaultPosition

class WhiteboardManager;

/** @brief Class thats implements main conference window.
*
* The user enter in this window after choose the channel.
*/
class SessionWindow: public wxFrame
{
private:
	WhiteboardManager *m_whiteboardManager;
	// TODO: Dead code? Check this out
	ChatFrame *m_publicChatFrame;
	bool m_publicChatFrameExists;

	ViewMode m_viewMode;

	wxPanel *m_mainPanel;
	wxBoxSizer *m_mainPanelSizer;
	// View mode 1 (equally spaced)
	wxPanel *m_equalModePanel;
	wxGridSizer *m_gridSizer;
	// View mode 2 (one large window)
	wxPanel *m_largeModePanel;
	wxPanel *m_largeWindowPanel, 
	        *m_smallWindowPanel;
	wxSplitterWindow *m_splitter;

	unsigned m_sashPosition;
	bool m_splitModeIsActive;
	bool m_hasMultipleColaborations;

	wxBoxSizer *m_largeWindowSizer,
	           *m_smallWindowSizer,
               *m_largeModeSizer;

	TransmissionDisplayControl *m_tdcLargeVideo;	// This will just point to the VDC currently on the large pane

	TransmissionDisplayControlList m_tdcList;

	AppInterface *m_appInterface;

	wxArrayLong m_itemBlink;

	wxTimer m_timer;

	wxString m_strUserSelected;//String selected of lstUsers
	int m_indexUserSelected;//Index selected of lstUsers

	wxPanel *m_menuPanel,
	        *m_usersListPanel,
	        *m_publicChatPanel,
			*m_statusTextPanel;
    wxBoxSizer *m_mainVertPanelSizer,
	           *m_mainHorPanelSizer,
			   *m_subVertSizer,
	           *m_menuPanelHorSizer,
	           *m_menuPanelVertSizer,
	           *m_userListPanelSizer,
			   *m_publicChatSizer,
			   *m_publicChatTextSizer,
			   *m_publicChatInputSizer,
			   *m_ctrlPanelSizer,
			   *m_ctrlSizer,
			   *m_captureCtrlSizer,
			   *m_userCmdSizer;

	wxStaticBoxSizer *m_videoCaptureSizer;

	wxBitmapButton *m_btnShowMenu,
	               *m_btnShowPublicChat,
                   *m_btnShowUsersList,
                   *m_btnChangeViewMode,
				   *m_btnShowHideSplit,
                   *m_btnFullScreen,
                   *m_btnBack,
				   *m_btnPlayPauseFilePlayback,
				   *m_btnStopFilePlayback,
				   *m_btnPlaySendVideoCamera,
				   *m_btnStopSendVideoCamera,
				   *m_btnPlayStopCaptureCamera,
				   *m_btnRequestVoice;
	wxTextCtrl *m_chatTextBox,
		       *m_chatInputBox;
	wxSlider *m_fileProgressSlider;
	wxButton *m_chatSendBtn;
	//wxListCtrl *m_lstUsers;
	SophisticatedListControl *m_lstUsers;
	wxStaticText *m_lblStatusText;
	wxStaticBitmap *m_staticBmpVideoCaptureScreen;

	VideoDisplayControl *m_vdcNoVideos,			// Shows static image
                        *m_vdcCaptureView;		// Shows capture
	
	TransmissionDisplayControl *m_tdcMirror,			// Video capture mirror
                               *m_appSharingDisplay;	// Application Sharing display - TODO: Use this instead of AppSharingViewFrame

	//Members to open the chat frames
	ArrayOfStatusChat m_arrStatusChat;
	//END Members to open the chat frames

	//Menu buttons
	//wxButton *m_menuBtnBouncingShape;

	bool m_showUsersList;

	wxString m_userName;
	wxString m_channel;
	StatusChat m_statusChat;

	bool m_paused;
	bool m_stopped;

	bool m_isRequestingVoice;

	wxBitmap m_bmpStopCaptureCamera;
	wxBitmap m_bmpPlayCaptureCamera;

	wxBitmap m_bmpHandOff;
	wxBitmap m_bmpHandOn;

	wxButton *m_btnMenuSendFile;
	wxButton *m_btnMenuPoll;
	wxButton *m_btnChannelModes;
	wxButton *m_btnMenuConfigure;
	wxButton *m_btnMenuWhiteboard;
	wxButton *m_btnMenuPlaylist;

	wxMenu *m_menuCommands;

	PollStatsDialog *m_pollStatsDialog;
	PollAnswerDialog *m_pollAnswerDialog;

	long m_userBitrate;

	void UpdateCommandButtonStates();
	void UpdateControlBarButtonStates();
	void StartCapture();
	void StopCapture();
	void CreateMirror();
	void DestroyMirror();
	long SmallestBitrate(const long &bitrate1, const long &bitrate2);

	void SetViewMode(ViewMode mode);
	void TogglePanels();

public:
    /// Constructors
    SessionWindow();
    SessionWindow( wxWindow* parent, wxWindowID id = SYMBOL_USERSLISTFRAME_IDNAME, const wxString& caption = SYMBOL_USERSLISTFRAME_TITLE, const wxPoint& pos = SYMBOL_USERSLISTFRAME_POSITION, const wxSize& size = SYMBOL_USERSLISTFRAME_SIZE, long style = SYMBOL_USERSLISTFRAME_STYLE, AppInterface* iface = NULL);

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_USERSLISTFRAME_IDNAME, const wxString& caption = SYMBOL_USERSLISTFRAME_TITLE, const wxPoint& pos = SYMBOL_USERSLISTFRAME_POSITION, const wxSize& size = SYMBOL_USERSLISTFRAME_SIZE, long style = SYMBOL_USERSLISTFRAME_STYLE );

    /// Destructor
    ~SessionWindow();

    /// Initialises member variables
    void Init();

	// Used to pass the App reference between windows
	void SetAppInterface(AppInterface *iface);

    /// Creates the controls and sizers
    void CreateControls();

	void UpdateUserList();
	void ShowPrivateChat(const wxString &user);	//Opens the private ChatFrame for 'user'
	void ShowPublicChat();	// Will change
	ChatFrame *GetPublicChatFrame();	// Deprecated, but kept here for reference

	void SetLabelStatusText(wxString userName, wxString channel);

	StatusChat FindStatusChat(const wxString& userName, int &index, bool &found);

	bool FindItemBlink(const long &item);

	StatusChat GetStatusChat(){return m_statusChat;}
	void SetStatusChat(StatusChat statusChat, const int &index);
	//ChatFrame *FindChatFrame(const wxString& userName, bool &found);

	void BlinkUserReceiveMessage(const wxString &username);
	void ContinueBlinkingUsername();

	// VideoDisplayControl management functions
	void AddTransmissionDisplayControl(TransmissionDisplayControl *tdc);
	void RemoveTransmissionDisplayControl(TransmissionDisplayControl *tdc);
	void RefreshVideoDisplayArea();

	void CollaborationAutoPlay();
	void CollaborationStop();
	
	// Application Sharing
	void RegisterAppSharingDisplayControl();
	void UnregisterAppSharingDisplayControl();

	// Whiteboard
	void OnWhiteboardMessage(const wxString &channel, const wxString &user, const wxString &data);
	void StartWhiteboardFileTransmission(const wxString &filePath);

	// New embedded public chat
	void SetPublicChatMessage(const wxString &sender, const wxString &msg);

	// Update file progress slider position
	void UpdateFileProgressSlider(float percent);

	void UpdateActivePollStats();
	void ShowPollStats(const PollInfo &info);
	void ShowPollAnswerDialog();

	void FileTransferSendUserLostVoice();

	void EnableSendFileTransfer();
	void DisableSendFileTransfer();
	void EnableSendPlaylist();
	void DisableSendPlaylist();

////@begin SessionWindow event handler declarations
	void OnClose(wxCloseEvent &event);
	void OnShowMenuBtnClick(wxCommandEvent &event);
	void OnBackBtnClick(wxCommandEvent &event);
	void OnFullScreenBtnClick(wxCommandEvent &event);
	void OnChangeViewBtnClick(wxCommandEvent &event);
	void OnShowHideSplitBtnClick(wxCommandEvent &event);
	void OnShowUsersListBtnClick(wxCommandEvent &event);
	void OnShowPublicChatBtnClick(wxCommandEvent &event);	// Will be changed
	void OnUsersLstSelected(wxListEvent &event);
	void OnUsersLstDClick(wxListEvent &event);
	void OnShowContextMenu(wxContextMenuEvent &event);
	void OnUsersLstDeselected(wxListEvent& event);
	void OnContextMenuClick(wxCommandEvent &event);
	void OnFileProgressSliderChange(wxScrollEvent &event);

	void OnPublicChatSendBtnClick(wxCommandEvent &event);
	void OnPublicChatInputTextChange(wxCommandEvent &event);

	void OnPublicChatClose(wxCloseEvent &event);	// Will be changed/declared deprecated

	void OnPollStatsDialogClose(wxCloseEvent &event);
	void OnPollAnswerDialogClose(wxCloseEvent &event);

	void OnVDCDblClick(wxCommandEvent &event);

	void OnTimer(wxTimerEvent &event);

	//void OnBouncingShapeMenuBtnClick(wxCommandEvent &event);

	void OnPlayPauseFilePlayback(wxCommandEvent &event);
	void OnStopFilePlayback(wxCommandEvent &event);

	void OnPlayStopCaptureCameraBtnClick(wxCommandEvent &event);
	void OnPlaySendVideoCameraBtnClick(wxCommandEvent &event);
	void OnStopSendVideoCameraBtnClick(wxCommandEvent &event);	

	void OnMenuSendFileBtnClick(wxCommandEvent &event);
	void OnMenuPollBtnClick(wxCommandEvent &event);
	void OnChannelModesBtnClick(wxCommandEvent &event);
	void OnMenuConfigureBtnClick(wxCommandEvent &event);
	void OnMenuWhiteboardBtnClick(wxCommandEvent &event);
	void OnMenuPlaylistBtnClick(wxCommandEvent &event);
	
	void OnRequestVoiceBtnClick(wxCommandEvent &event);

	// Events for view mode 2
//	void OnSashDrag(wxSashEvent &event);			// Sash dragged
//	void OnLargeModePanelSize(wxSizeEvent &event);	// Panel resized

////@end SessionWindow event handler declarations

    DECLARE_EVENT_TABLE()

    /// Should we show tooltips?
    static bool ShowToolTips();
};

#endif
    // _USERSLISTFRAME_H_
