#ifndef SKINLITE_CONSTANTS_H
#define SKINLITE_CONSTANTS_H

#include <wx/wx.h>

// General
#define SLIDER_MAX	100000

//SkinLite Constants
#define S_LITE_INITIAL_WIDTH 400
#define S_LITE_INITIAL_HEIGHT 240
#define S_LITE_POSITION_RELATIONAL_PARENT wxSize(30,30)
#define SKIN_NAME	"Lite"
#define SKIN_NAME_DEBUG	"Lite (Debug)"
#define S_LITE_ICON_FILENAME _("Graphics/SkinLite/iptv.ico")
#define S_LITE_STATUSTEXT_WELCOME _("Welcome to IP.TV!")
#define S_LITE_STATUSTEXT_CONNECTING _("Connecting to %s...")
#define S_LITE_STATUSTEXT_DISCONNECTED _("Disconnected")
#define S_LITE_STATUSTEXT_FAILURE _("Couldn't connect to %s. %s")
#define S_LITE_STATUSTEXT_DISCONNECTED_FROM _("Disconnected from %s. %s")

enum
{
	SL_MENU_ID_FIRST = wxID_HIGHEST + 5000,
	SL_MENU_ID_SETTINGS_CHANGE_SKIN,
	SL_MENU_ID_SETTINGS_CHANGE_LANGUAGE,
	SL_MENU_ID_SETTINGS_PROFILE,
	SL_MENU_ID_SETTINGS_TRANSMISSION_CONFIG,
	SL_MENU_ID_SETTINGS_RECORDING_CONFIG,
	SL_MENU_ID_HELP_ABOUT,
	SL_MENU_ID_LAST
};

//ChannelWindow Constants
#define CW_WINDOW_SIZE	wxSize(550, 300)
#define CW_DLG_DISCONNECT_LABEL _("Disconnect from IRM server?")
#define CW_DLG_DISCONNECT_CAPTION _("Disconnect")
#define CW_BTN_REFRESH_LABEL _("Refresh")
#define CW_LBL_CHANNEL_LABEL _("Channel:")
#define CW_BTN_JOINCHANNEL_LABEL _("Join")
#define CW_BTN_DISCONNECT_LABEL _("Disconnect")
#define CW_LST_COL_CHANNEL _("Channel")
#define CW_LST_COL_USERS _("Users")
#define CW_LST_COL_TOPIC _("Topic")
#define CW_SESSIONTIME_TIMER_INTERVAL 500

//UsersListFrame Constants
#define USERLIST_FRM_BMP_SHOW_MENU wxT("Graphics/SkinLite/bmpBtnShowMenu.png")
#define USERLIST_FRM_BMP_BACK wxT("Graphics/SkinLite/bmpBtnBack.png")
#define USERLIST_FRM_BMP_FULLSCREEN wxT("Graphics/SkinLite/bmpBtnFullScreen.png")
#define USERLIST_FRM_BMP_USERLIST wxT("Graphics/SkinLite/bmpBtnUsersList.png")
#define USERLIST_FRM_BMP_PUBLIC_CHAT wxT("Graphics/SkinLite/bmpBtnPublicChat.png")
#define USERLIST_FRM_BMP_MAGNIFY wxT("Graphics/SkinLite/bmpBtnMagnify.png")
#define USERLIST_FRM_BMP_CHANGE_VIEW wxT("Graphics/SkinLite/bmpBtnChangeViewMode.png")
#define USERLIST_FRM_BMP_SHOWHIDE_SPLIT wxT("Graphics/SkinLite/bmpBtnPip.png")
#define USERLIST_FRM_BMP_PLAYFILEPLAYBACK wxT("Graphics/SkinLite/bmpBtnPlayFilePlayback.png")
#define USERLIST_FRM_BMP_PAUSEFILEPLAYBACK wxT("Graphics/SkinLite/bmpBtnPauseFilePlayback.png")
#define USERLIST_FRM_BMP_STOPFILEPLAYBACK wxT("Graphics/SkinLite/bmpBtnStopFilePlayback.png")
#define USERLIST_FRM_BMP_STOPSENDVIDEOCAMERA wxT("Graphics/SkinLite/bmpBtnStopSendVideoCamera.png")
#define USERLIST_FRM_BMP_PLAYSENDVIDEOCAMERA wxT("Graphics/SkinLite/bmpBtnPlaySendVideoCamera.png")
#define USERLIST_FRM_BMP_STOPCAPTURECAMERA wxT("Graphics/SkinLite/bmpBtnStopCaptureCamera.png")
#define USERLIST_FRM_BMP_PLAYCAPTURECAMERA wxT("Graphics/SkinLite/bmpBtnPlayCaptureCamera.png")
#define USERLIST_FRM_BMP_VIDEOCAPTURESCREEN wxT("Graphics/SkinLite/bmpVideoCaptureScreen.png")

#define USERLIST_FRM_BMP_REQUESTVOICEOFF wxT("Graphics/SkinLite/bmpBtnHandOff.png")
#define USERLIST_FRM_BMP_REQUESTVOICEON wxT("Graphics/SkinLite/bmpBtnHandOn.png")
// Icons in user list. These should be 16x16 images.
// Rationale: the ListCtrl uses a wxImageList, which requires all images
// it contains to be the same size.
#define USERLIST_FRM_BMP_WIDTH		16
#define USERLIST_FRM_BMP_HEIGHT		16

#define USERLIST_FRM_BMP_ICON_MEDIAVOICE			wxT("Graphics/SkinLite/bmpIcoMicVoice.png")
#define USERLIST_FRM_BMP_ICON_MEDIAVOICEMUTE		wxT("Graphics/SkinLite/bmpIcoMicVoiceMute.png")
#define USERLIST_FRM_BMP_ICON_MEDIACOLLAB			wxT("Graphics/SkinLite/bmpIcoMicCollaboration.png")
#define USERLIST_FRM_BMP_ICON_MEDIACOLLABMUTE		wxT("Graphics/SkinLite/bmpIcoMicCollaborationMute.png")
#define USERLIST_FRM_BMP_ICON_MUTE					wxT("Graphics/SkinLite/bmpIcoMicMute.png")
#define USERLIST_FRM_BMP_ICON_CAMERA				wxT("Graphics/SkinLite/bmpIcoCam.png")
#define USERLIST_FRM_BMP_ICON_REQUESTVOICE			wxT("Graphics/SkinLite/bmpIcoHand.png")

#define USERLIST_FRM_BMP_ICON_INDEX_MEDIAVOICE			0
#define USERLIST_FRM_BMP_ICON_INDEX_MEDIAVOICEMUTE		1
#define USERLIST_FRM_BMP_ICON_INDEX_MEDIACOLLAB			2
#define USERLIST_FRM_BMP_ICON_INDEX_MEDIACOLLABMUTE		3
#define USERLIST_FRM_BMP_ICON_INDEX_MUTE				4
#define USERLIST_FRM_BMP_ICON_INDEX_CAMERA				5
#define USERLIST_FRM_BMP_ICON_INDEX_REQUESTVOICE		6

// User List control column order definition. Starting from 0.

#define USERLIST_FRM_LST_COL_CAMERA_INDEX	0
#define USERLIST_FRM_LST_COL_VOICE_INDEX	1
#define USERLIST_FRM_LST_COL_USERS_INDEX	2

// User List Colors. TODO: Clean up this whole mess!!
#define USERLIST_FRM_LST_USERS_OPERATOR_COLOR	wxColour(192,112,48)
#define USERLIST_FRM_LST_USERS_REGISTERED_COLOR	wxColour(0, 160, 0)
#define USERLIST_FRM_LST_USERS_DEFAULT_COLOR	wxColour(0, 0, 0)

#define USERLIST_FRM_LBL_STATUSTEXT _("StatusText UserName")
#define USERLIST_FRM_NAME_PUBLIC_CHAT _("Public")
#define USERLIST_FRM_ITEM_DESELECTED -1
#define USERLIST_FRM_LBL_MENUITEM_SEND _("Send...")
#define USERLIST_FRM_LBL_MENUITEM_PRVCHAT _("Private Chat")
#define USERLIST_FRM_TOOLTIP_SHOW_MENU _("Hide/Show Menu")
#define USERLIST_FRM_TOOLTIP_SHOW_PUBCHAT _("Public Messages")
#define USERLIST_FRM_TOOLTIP_SHOW_FULLSCREEN _("Enlarge screen")
#define USERLIST_FRM_TOOLTIP_SHOW_USERSLIST _("Userslist")
#define USERLIST_FRM_TOOLTIP_BACK_BTN _("Back")

#define USERLIST_FRM_TOOLTIP_STOPSENDVIDEOCAMERA_BTN _("Stop Share video capture")
#define USERLIST_FRM_TOOLTIP_PLAYSENDVIDEOCAMERA_BTN _("Share video capture")
#define USERLIST_FRM_TOOLTIP_PLAY_STOP_CAPTURECAMERA_BTN _("Play/Stop video capture")

#define USERLIST_FRM_TOOLTIP_REQUESTVOICE_BTN _("Request Voice")

#define USERLIST_FRM_SESSIONTIME_TIMER_INTERVAL 500
#define USERLIST_FRM_USERS_LIST_BLINK_INTERVAL 500
#define USERLIST_FRM_USERS_LIST_BLINK_FILL_COLOR wxColour(0,255,0)
#define USERLIST_FRM_USERS_LIST_BLINK_ERASED_COLOR wxColour(255,255,255)
#define USERLIST_FRM_LBL_SIZER_VIDEOCAPTURE_CAPTION _("Video Capture")

//Menu buttons labels
#define USERLIST_FRM_BTN_MENUSENDFILE_LABEL _("Send File")
#define USERLIST_FRM_BTN_MENUPOLL_LABEL _("Poll")
#define USERLIST_FRM_BTN_CHANNELMODES_LABEL _("Channel Modes")
#define USERLIST_FRM_BTN_MENUCONFIGURE_LABEL _("Configuration")
#define USERLIST_FRM_BTN_MENUWBOARD_LABEL _("Whiteboard")
#define USERLIST_FRM_BTN_MENUPLAYLIST_LABEL _("Executive TV - Movie")
//END Menu buttons labels

#define USERLIST_FRM_SASH_MIN_PANE_SIZE 160

#define USERLIST_FRM_MENU_BTN_BOUNCING_SHAPE_LABEL _("Bouncing Shape")
//END UsersListFrame Constants

// Buttons
#define BUTTON_YES_LABEL _("&Yes")
#define BUTTON_NO_LABEL _("&No")
#define BUTTON_OK_LABEL _("&Ok")
#define BUTTON_CANCEL_LABEL _("&Cancel")

//SettingsManager Constants
#define SYMBOL_SETTINGSMANAGERFRAME_TITLE _("Profile Manager")
#define SYMBOL_SETTINGSMANAGERFRAME_TITLE_NEW _("Profile Manager - Creating New Profile")
#define SYMBOL_SETTINGSMANAGERFRAME_TITLE_EDITING _("Profile Manager - Editing Profile")
#define SETMANFRM_LBL_PROFILENAME_LABEL _("Profile Name:")
#define SETMANFRM_LBL_USER_LABEL _("User:")
#define SETMANFRM_LBL_USERPWD_LABEL _("User Password:")
#define SETMANFRM_LBL_SERVER_LABEL _("Server:")
#define SETMANFRM_LBL_SERVERPWD_LABEL _("Server Password:")
#define SETMANFRM_LBL_CHANNEL_LABEL _("Channel:")
#define SETMANFRM_LBL_CHANNELPWD_LABEL _("Channel Password:")
#define SETMANFRM_BTN_SELECT_LABEL _("Select")
#define SETMANFRM_BTN_CANCEL_LABEL _("Cancel")
#define SETMANFRM_BTN_CREATE_LABEL _("Create")
#define SETMANFRM_BTN_DELETE_LABEL _("Delete")
#define SETMANFRM_BTN_SAVE_LABEL _("Save")
#define SETMANFRM_BTN_IMPORT_LABEL _("Import")
#define SETMANFRM_BTN_EXPORT_LABEL _("Export")
//END SettingsManager Constants

//SendFilesFrame Constants
#define SENDFILESFRAME_ANIMATEDSENDGIF wxT("Graphics/SkinLite/gifFileSend.gif")
#define SENDFILESFRAME_LBL_FILENAME_LABEL _("File Name:")
#define SENDFILESFRAME_BTN_SEARCH_LABEL _("Search")
#define SENDFILESFRAME_LBL_FILESIZE_CAPTION _("Size:")
#define SENDFILESFRAME_LBL_FILESENT_CAPTION _("Sent:")
#define SENDFILESFRAME_LBL_FILESENTBYTES_CAPTION _("Sent Bytes:")
#define SENDFILESFRAME_LBL_REMAININGTIME_CAPTION _("Remaining Time:")
#define SENDFILESFRAME_LBL_BITRATE_CAPTION _("Bitrate:")
#define SENDFILESFRAME_LBL_RECOVERED_CAPTION _("Recovered:")
#define SENDFILESFRAME_LBL_REMAININGTIMERECOVERED_CAPTION _("Remaining Time:")
#define SENDFILESFRAME_BTN_SEND_LABEL _("Send")
#define SENDFILESFRAME_BTN_CANCEL_LABEL _("Cancel")
#define SENDFILESFRAME_LBL_STATISTICSENT_CAPTION _("Sent:")
#define SENDFILESFRAME_LBL_STATISTICCANCELLED_CAPTION _("Cancelled:")
#define SENDFILESFRAME_LBL_STATISTICRECEIVED_CAPTION _("Received:")
#define SENDFILESFRAME_LBL_STATISTICTORECEIVE_CAPTION _("To Receive:")
#define SENDFILESFRAME_DLG_CHOOSEFILE_LABEL _("Choose a file")
//END SendFilesFrame Constants

//ReceiveFilesFrame Constants
#define RECEIVEFILESFRAME_ANIMATEDRECEIVEGIF wxT("Graphics/SkinLite/gifFileReceive.gif")
#define RECEIVEFILESFRAME_LBL_SIZE_CAPTION _("Size:")
#define RECEIVEFILESFRAME_LBL_RECEIVED_CAPTION _("Received:")
#define RECEIVEFILESFRAME_LBL_BYTESRECEIVED_CAPTION _("Bytes received:")
#define RECEIVEFILESFRAME_LBL_REMAININGTIME_CAPTION _("Remaining time:")
#define RECEIVEFILESFRAME_LBL_BITRATE_CAPTION _("Bitrate:")
#define RECEIVEFILESFRAME_LBL_PACKAGESTORECOVER_CAPTION _("Packages to Recover:")
#define RECEIVEFILESFRAME_BTN_SAVE_LABEL _("Save")
#define RECEIVEFILESFRAME_BTN_CANCEL_LABEL _("Cancel")
#define RECEIVEFILESFRAME_DLG_SAVE_LABEL _("Save")
//END ReceiveFilesFrame Constants

//SendPollFrame Constants
#define SENDPOLLFRAME_LBL_ANSWERSTOTAL_CAPTION _("Answers:")
#define SENDPOLLFRAME_LBL_QUESTION_CAPTION _("Question:")
#define SENDPOLLFRAME_BTN_ERASEPOLL_LABEL _("Erase")
#define SENDPOLLFRAME_BTN_SENDPOLL_LABEL _("Send")
#define SENDPOLLFRAME_LBL_SENTQUESTIONS_CAPTION _("Sent Questions:")
#define SENDPOLLFRAME_LBL_RECEIVEDANSWERS_CAPTION _("Received Answers:")
#define SENDPOLLFRAME_BTN_VISUALIZESTATISTIC_LABEL _("Visualize")
#define SENDPOLLFRAME_BTN_SENDSTATISTIC_LABEL _("Send")
//END SendPollFrame Constants

//PollQuestionCtrl Constants
#define POLLQUESTIONCTRL_LBL_NUMBEROFANSWERS_CAPTION _("Number of options:")
#define POLLQUESTIONCTRL_LBL_QUESTION_CAPTION _("Question:")
//END PollQuestionCtrl Constants

//RecordingDialog Constants
#define RECORDINGDIALOG_LBL_BROWSEFORFOLDER_CAPTION _("Browse for folder")
#define POLLQUESTIONCTRL_LBL_CANCEL_CAPTION _("Cancel:")
//END RecordingDialog Constants

#endif
