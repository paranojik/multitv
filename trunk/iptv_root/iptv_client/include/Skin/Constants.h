#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <wx/wx.h>

#define DEFAULT_PASSWORD	wxT("-")
#define DEFAULT_REAL_NAME	wxT("Not supplied")
#define DEFAULT_EMAIL		wxT("Not supplied")

#define DEFAULT_SKIN_FILE_NAME		wxT("Skin/DefaultSkin.ini")
#define LOCALE_FILE_NAME 			wxT("iptv")
#define LOCALE_DIRECTORY_NAME		wxT("Lang")

#define NO_LANGUAGE_SELECTED		-1

// User messages
#define NO_SKIN_FOUND				_("No skin found")
#define DEFAULT_SKIN_NOT_FOUND		_("Default skin not found")
#define DEFAULT_SKIN_FILE_NOT_FOUND _("Default skin file not found")
#define CANNOT_INITIALIZE_CONFIG	_("Cannot initialize configuration file")

// Resource files
#define VIDEO_BITMAP_FILE_NAME		wxT("Graphics/Logo.png")

// Labels
#define LABEL_CHANGE_SKIN			_("Change Skin")
#define LABEL_CHANGE_LANGUAGE		_("Change Language")
#define LABEL_LANGUAGE				_("Language")
#define LABEL_EXIT					_("Exit")
#define LABEL_BTN_UNICAST			_("Connect to server")
#define LABEL_BTN_MULTICAST			_("Join multicast")
#define LABEL_BTN_FILE				_("File playback")
#define LABEL_DLG_UNICAST_SERVER_BOX		_("Server")
#define LABEL_DLG_UNICAST_SERVER_NAME		_("Name:")
#define LABEL_DLG_UNICAST_SERVER_PASSWORD	_("Password:")
#define LABEL_DLG_UNICAST_USER_BOX			_("User")
#define LABEL_DLG_UNICAST_USER_NAME			_("Name:")
#define LABEL_DLG_UNICAST_USER_PASSWORD		_("Password:")
#define LABEL_DLG_UNICAST_BTN_OK			_("OK")
#define LABEL_DLG_UNICAST_BTN_CANCEL			_("Cancel")
#define LABEL_FRM_CHANNELS_LIST_COL_CHANNELS	_("Public Channels")
#define LABEL_FRM_CHANNELS_LIST_COL_USERS		_("Users")
#define LABEL_FRM_CHANNELS_LIST_COL_TOPIC		_("Topic")
#define LABEL_FRM_CHANNELS_LIST_BTN_UPDATE		_("Update List")
#define LABEL_FRM_CHANNELS_LIST_BTN_JOIN		_("Join Channel")
#define LABEL_FRM_CHANNELS_LIST_BTN_LEAVE		_("Leave Channel")
#define LABEL_FRM_CHANNELS_LIST_BTN_BACK		_("Back")
#define LABEL_FRM_USERS_LIST_COL_USERS				_("Users")
#define LABEL_FRM_CHANNELS_LIST_STATUSTEXT_CH_NO	_("%s is out of channel")
#define LABEL_FRM_CHANNELS_LIST_STATUSTEXT_CH_OK	_("%s is on channel %s")
#define LABEL_FRM_USERS_LIST_STATUSTEXT				_("%s is on channel %s")
#define LABEL_FRM_CHAT_BTN_SEND						_("Send")
#define LABEL_FRM_CHAT_USER_MESSAGE_SAY				_("%s says: ")

// Exception messages
#define VBEXCEPTION				wxT("VBException")
#define VBEXCEPTION_CROSSDLL	wxT("VBException CrossDLL")

// Captions
#define CHOOSE_SKIN_CAPTION			_("Select Skin")
#define CHOOSE_LANGUAGE_CAPTION		_("Select Language")
#define SKIN_CAPTION				_("IP.TV - ")
#define UNICAST_DIALOG_CAPTION		_("Unicast Settings")
#define MESSAGE_ERROR_CAPTION				_("Error")
#define SYMBOL_CHANNELSLISTFRAME_CAPTION	_("Channels List Frame")
#define SYMBOL_USERSLISTFRAME_CAPTION		_("IP.TV - Users Viewer")
#define FRM_CHAT_PRIVATE_CHAT_CAPTION		_("%s - Private Chat")
#define FRM_CHAT_PUBLIC_CHAT_CAPTION		_("%s - Public Chat")

// Dialog messages
#define CHOOSE_SKIN		_("Select Skin Dialog")

// Dialog label Buttons
#define MULTICAST_CFG_DLG_OK_LABEL _("&OK")
#define MULTICAST_CFG_DLG_CANCEL_LABEL _("&Cancel")

//Configurations
#ifdef WIN32	// "All files" means "*.*" in Windows, "*" in other systems
	#define CFG_DLG_FILE_TYPES	_("Conference files (*.imc)|*.imc|All files|*.*")
#else
	#define CFG_DLG_FILE_TYPES	_("Conference files (*.imc)|*.imc|All files|*")
#endif // WIN32

// Series Collaboration default interval
#define SC_DEFAULT_SWITCH_INTERVAL		wxTimeSpan::Seconds(20)

//Error messages
#define DLG_UNICAST_SERVER_NAME_EMPTY	_("Server name must not be blank.")
#define DLG_UNICAST_USER_NAME_EMPTY		_("User name must not be blank.")
#define DLG_UNICAST_USER_PASSWORD_EMPTY	_("User password must not be blank.")
#define FRM_CHANNEL_LIST_NO_CHANNEL		_("No channel selected")

// Profiles directories
#ifdef _DEBUG
#define SETTINGS_FILENAME wxT("settings_d.xml")
#else
#define SETTINGS_FILENAME wxT("settings.xml")
#endif

#endif	// CONSTANTS_H
