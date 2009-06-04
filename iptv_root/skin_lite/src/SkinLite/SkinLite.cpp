// SkinLite.cpp : Defines the entry point for the DLL application.
//

#include <wx/wx.h>
#ifdef MAX
#undef MAX
#endif
#define MAX(a, b)	(a > b ? a : b)

// #include <cmath>

#include <list>

#include "wx/menu.h"

#include "SkinLite.h"
#include "ChannelWindow.h"
#include "ChatFrame.h"
#include "UnicastConfigDialog.h"
#include "SkinLiteConstants.h"
#include "AppInterface.h"
#include "AppConfig.h"
#include "SettingsManagerDialog.h"
#include "MulticastConfigDialog.h"
#include "TransmissionConfigDialog.h"
#include "MediaDeviceItem.h"
#include "RecordingDialog.h"

#include "ChannelUser.h"
#include "VideoSink.h"

#include "SendFilesFrame.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

#ifdef WIN32

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#endif

#ifdef _MANAGED
#pragma managed(pop)
#endif

extern "C"
{

/** Instantiates the skin and returns it's pointer.
*
*/
SKINLITE_API Skin* AllocSkin()
{
	return new SkinLite();
}

/** Returns the name of the skin.
*
*/
SKINLITE_API const char* GetSkinName()
{
#ifdef _DEBUG
	return SKIN_NAME_DEBUG;
#else
	return SKIN_NAME;
#endif
}

} // extern "C"

// Event table
BEGIN_EVENT_TABLE(SkinLite, wxFrame)
#ifdef __WXGTK__
	EVT_WINDOW_CREATE(SkinLite::OnWindowCreate)
#endif
END_EVENT_TABLE()

/** SkinLite default constructor.
*
*/
SkinLite::SkinLite() : Skin()
{
	m_sessionWindowExists = false;
	m_sessionWindow = NULL;
	m_activeReceiveFilesFrame = NULL;

	wxIcon icon = wxIcon(S_LITE_ICON_FILENAME, wxBITMAP_TYPE_ICO);

	wxFrame::Create(NULL, wxID_ANY, wxString(SKIN_CAPTION) + wxString(_("Skin")) + wxString(SKIN_NAME, wxConvUTF8), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));

	SetLabel(wxString(SKIN_CAPTION) + wxString(_("Skin")) + wxString(SKIN_NAME, wxConvUTF8));

	SetIcon(icon);
	Centre();

	SetStatusBar(CreateStatusBar());

	wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(itemBoxSizer2);

    wxBoxSizer* itemBoxSizer3 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer2->Add(itemBoxSizer3, 1, wxGROW, 5);

    m_panel = new SkinLitePanel(this);
    itemBoxSizer3->Add(m_panel, 1, wxGROW, 5);
	
	m_BoxSizerPanel = new wxBoxSizer(wxVERTICAL);
	m_panel->SetSizer(m_BoxSizerPanel);

	m_cmdSizer = new wxBoxSizer(wxHORIZONTAL);
	m_BoxSizerPanel->Add(m_cmdSizer, 1, wxALL | wxEXPAND, 8);

	m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
	m_BoxSizerPanel->Add(m_ctrlSizer, 0, wxGROW|wxALL, 5);

	SetStatusText(S_LITE_STATUSTEXT_WELCOME);
}

/** SkinLite destructor.
*
*/
SkinLite::~SkinLite()
{
}

void SkinLite::Initialize()
{
	InitializeBase();

	//m_ctrlSizer->AddSpacer(8);
	//CreateChangeSkinBtn();
	//CreateChangeLanguageBtn();
	//CreateProfileBtn();
	//CreateConfigureBtn();
	//CreateAboutBtn();
	//m_ctrlSizer->AddSpacer(8);

	CreateUnicastBtn();
	CreateMulticastBtn();
	CreateFileBtn();

	CreateMenuBar();

	// hide button in release version because feature is still in testing
//#ifndef _DEVELOPER
//	m_btnProfile->Hide();
//#endif

	m_cmdSizer->Layout();
	m_ctrlSizer->Layout();
	m_BoxSizerPanel->Layout();
	m_BoxSizerPanel->SetSizeHints(this);

	m_sendFilesExists = false;
	m_receiveFilesExists = false;
	m_playlistExists = false;
}

/** Creates the menu bar.
*
*/
void SkinLite::CreateMenuBar()
{
	wxMenuBar *menuBar = new wxMenuBar;
	// Settings
	m_menuSettings = new wxMenu;
	m_menuSettings->Append(SL_MENU_ID_SETTINGS_CHANGE_SKIN, _("Change &Skin"));
	m_menuSettings->Append(SL_MENU_ID_SETTINGS_CHANGE_LANGUAGE, _("Change &Language"));
	m_menuSettings->AppendSeparator();
	m_menuSettings->Append(SL_MENU_ID_SETTINGS_PROFILE, _("Connection &profile"));
	m_menuSettings->Append(SL_MENU_ID_SETTINGS_TRANSMISSION_CONFIG, _("&Transmission"));
	m_menuSettings->Append(SL_MENU_ID_SETTINGS_RECORDING_CONFIG, _("&Recording"));


	Connect(SL_MENU_ID_SETTINGS_CHANGE_SKIN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnChangeSkin));
	Connect(SL_MENU_ID_SETTINGS_CHANGE_LANGUAGE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnChangeLanguage));
	Connect(SL_MENU_ID_SETTINGS_PROFILE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnProfileBtnClick));
	Connect(SL_MENU_ID_SETTINGS_TRANSMISSION_CONFIG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnConfigureBtnClick));
	Connect(SL_MENU_ID_SETTINGS_RECORDING_CONFIG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnRecordingSettingsClick));

	// Help
	m_menuHelp = new wxMenu;
	m_menuHelp->Append(SL_MENU_ID_HELP_ABOUT, _("&About..."));
	Connect(SL_MENU_ID_HELP_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SkinLite::OnAboutBtnClick));
	// Add the menus to the menu-bar
	menuBar->Append(m_menuSettings, _("&Settings"));
	menuBar->Append(m_menuHelp, _("&Help"));

	// Set the menu bar on top of window
	SetMenuBar(menuBar);
}

/** Creates the change Skin button.
*
*/
void SkinLite::CreateChangeSkinBtn()
{
	m_btnChangeSkin = new wxButton(m_panel, ID_CHOOSE_SKIN, LABEL_CHANGE_SKIN, wxDefaultPosition, wxDefaultSize);
	m_btnChangeSkin->SetMaxSize(wxSize(-1, m_btnChangeSkin->GetSize().GetHeight()));
	Connect(ID_CHOOSE_SKIN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnChangeSkin));

	m_ctrlSizer->Add(m_btnChangeSkin, 1,  wxRIGHT | wxEXPAND, 8 );
}

/** Creates the change language button.
*
*/
void SkinLite::CreateChangeLanguageBtn()
{
	m_btnChangeLanguage = new wxButton(m_panel, ID_CHOOSE_LANGUAGE, LABEL_CHANGE_LANGUAGE, wxDefaultPosition, wxDefaultSize);
	m_btnChangeLanguage->SetMaxSize(wxSize(-1, m_btnChangeLanguage->GetSize().GetHeight()));
	Connect(ID_CHOOSE_LANGUAGE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnChangeLanguage));

	m_ctrlSizer->Add(m_btnChangeLanguage, 1, wxRIGHT | wxEXPAND, 8);
}

/** Creates an About button.
*
*/
void SkinLite::CreateAboutBtn()
{
	// TODO: Revise the button ID
	m_btnAbout = new wxButton(m_panel, ID_BTN_KERNEL_TEST, _("About..."), wxDefaultPosition, wxDefaultSize);
	m_btnAbout->SetMaxSize(wxSize(-1, m_btnAbout->GetSize().GetHeight()));
	Connect(ID_BTN_KERNEL_TEST, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnAboutBtnClick));

	m_ctrlSizer->Add(m_btnAbout, 1, wxEXPAND, 0);
}

/** Creates an Profile button.
*
*/
void SkinLite::CreateProfileBtn()
{
	m_btnProfile = new wxButton(m_panel, ID_BTN_PROFILE, _("Profile"), wxDefaultPosition, wxDefaultSize);
	m_btnProfile->SetMaxSize(wxSize(-1, m_btnProfile->GetSize().GetHeight()));
	Connect(ID_BTN_PROFILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnProfileBtnClick));

	m_ctrlSizer->Add(m_btnProfile, 1, wxRIGHT|wxEXPAND, 8);
}

/** Creates an Configure button.
*
*/
void SkinLite::CreateConfigureBtn()
{
	m_btnConfigure = new wxButton(m_panel, ID_BTN_CONFIGURE, _("Configuration"), wxDefaultPosition, wxDefaultSize);
	m_btnConfigure->SetMaxSize(wxSize(-1, m_btnConfigure->GetSize().GetHeight()));
	Connect(ID_BTN_CONFIGURE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnConfigureBtnClick));

	m_ctrlSizer->Add(m_btnConfigure, 1, wxRIGHT|wxEXPAND, 8);
}

/** Creates the Unicast button.
*
*/
void SkinLite::CreateUnicastBtn()
{
	m_btnUnicast = new wxButton(m_panel, ID_BTN_UNICAST, LABEL_BTN_UNICAST, wxDefaultPosition, wxDefaultSize);
	Connect(ID_BTN_UNICAST, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnUnicastBtnClick));
	wxSize sz = m_btnUnicast->GetSize();
	int side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnUnicast->SetMinSize(wxSize(side, side));
	m_btnUnicast->SetInitialSize(wxSize(side, side));

	m_cmdSizer->Add(m_btnUnicast, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxSHAPED, 8);
	m_cmdSizer->Layout();
}

/** Creates the Multicast button.
*
*/
void SkinLite::CreateMulticastBtn()
{
	m_btnMulticast = new wxButton(m_panel, ID_BTN_MULTICAST, LABEL_BTN_MULTICAST, wxDefaultPosition, wxDefaultSize);
	Connect(ID_BTN_MULTICAST, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnMulticastBtnClick));
	wxSize sz = m_btnMulticast->GetSize();
	int side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnMulticast->SetMinSize(wxSize(side, side));
	m_btnMulticast->SetInitialSize(wxSize(side, side));

	m_cmdSizer->Add(m_btnMulticast, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxSHAPED, 8);
	m_cmdSizer->Layout();
}

/** Creates the File button.
*
*/
void SkinLite::CreateFileBtn()
{
	m_btnFile = new wxButton(m_panel, ID_BTN_FILE, LABEL_BTN_FILE, wxDefaultPosition, wxDefaultSize);
	Connect(ID_BTN_FILE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SkinLite::OnFileBtnClick));
	wxSize sz = m_btnFile->GetSize();
	int side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnFile->SetMinSize(wxSize(side, side));
	m_btnFile->SetInitialSize(wxSize(side, side));

	m_cmdSizer->Add(m_btnFile, 1, wxALL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND | wxSHAPED, 8);
	m_cmdSizer->Layout();
}

/** Updates the controls captions when the language change.
*
*/
void SkinLite::UpdateText()
{
	wxSize sz;
	int side;
	SetLabel(wxString(SKIN_CAPTION) + wxString(_("Skin")) + wxString(SKIN_NAME, wxConvUTF8));

	// m_btnChangeSkin->SetLabel(LABEL_CHANGE_SKIN);
	// m_btnChangeSkin->SetSize(m_btnChangeSkin->GetBestSize());
	

	// m_btnChangeLanguage->SetLabel(LABEL_CHANGE_LANGUAGE);
	// m_btnChangeLanguage->SetSize(m_btnChangeLanguage->GetBestSize());

	// Update the menu-bar
	wxMenuBar *menuBar = GetMenuBar();
	if(menuBar)
	{
#ifndef __WXGTK__
		menuBar->SetMenuLabel(0, _("&Settings"));
		menuBar->SetMenuLabel(1, _("&Help"));
#else
		menuBar->SetLabel(0, _("&Settings"));
		menuBar->SetLabel(1, _("&Help"));
#endif
	}

	m_menuSettings->SetLabel(SL_MENU_ID_SETTINGS_CHANGE_SKIN, _("Change &Skin"));
	m_menuSettings->SetLabel(SL_MENU_ID_SETTINGS_CHANGE_LANGUAGE, _("Change &Language"));
	m_menuSettings->SetLabel(SL_MENU_ID_SETTINGS_PROFILE, _("Connection &profile"));	
	m_menuSettings->SetLabel(SL_MENU_ID_SETTINGS_TRANSMISSION_CONFIG, _("&Transmission"));

	m_menuHelp->SetLabel(SL_MENU_ID_HELP_ABOUT, _("&About..."));
	
	m_btnUnicast->SetLabel(LABEL_BTN_UNICAST);
	sz = m_btnUnicast->GetBestSize();
	side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnUnicast->SetMinSize(wxSize(side, side));
	m_btnUnicast->SetInitialSize(wxSize(side, side));
	m_cmdSizer->Layout();

	m_btnMulticast->SetLabel(LABEL_BTN_MULTICAST);
	sz = m_btnMulticast->GetBestSize();
	side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnMulticast->SetMinSize(wxSize(side, side));
	m_btnMulticast->SetInitialSize(wxSize(side, side));
	m_cmdSizer->Layout();

	m_btnFile->SetLabel(LABEL_BTN_FILE);
	sz = m_btnFile->GetBestSize();
	side = MAX(sz.GetWidth(), sz.GetHeight());
	m_btnFile->SetMinSize(wxSize(side, side));
	m_btnFile->SetInitialSize(wxSize(side, side));
	m_cmdSizer->Layout();
	m_BoxSizerPanel->SetSizeHints(this);

	// m_btnProfile->SetLabel(_("Profile"));
	// m_btnAbout->SetLabel(_("About..."));

	Fit();
	Center();

	SetStatusText(S_LITE_STATUSTEXT_WELCOME);
}


/** About text information.
*
*/
wxString SkinLite::GetAboutInfo()
{
	return wxT("SkinLite version 0.3 rev 388 (Feb 6 2009)\nBy the MultiTV Team");
}

// Events
void SkinLite::OnUnicastBtnClick(wxCommandEvent &event)
{
	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));
	UnicastConfigDialog ucDlg(this, m_appInterface, SYMBOL_UNICASTDIALOG_IDNAME, SYMBOL_UNICASTDIALOG_TITLE, this->GetPosition() + S_LITE_POSITION_RELATIONAL_PARENT);
	if(ucDlg.ShowModal() == wxID_OK)
	{
		AppConfig cfg = ucDlg.GetConfig();
		m_appInterface->SetAppConfig(cfg);
		m_appInterface->RequestConnection(cfg.GetConfServer(), cfg.GetConfServerPass(), cfg.GetConfUser(), cfg.GetConfPass());
		DisableAllButtons();
		wxString msg;
		msg.Printf(S_LITE_STATUSTEXT_CONNECTING, cfg.GetConfServer().c_str());
		SetStatusText(msg);
	}
}

void SkinLite::OnFileBtnClick(wxCommandEvent &event)
{
	// We rely on m_appInterface not being NULL here!
	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));

	wxFileDialog fdlg(	this, _("Open file"), wxEmptyString, wxEmptyString,
						CFG_DLG_FILE_TYPES, wxFD_OPEN | wxFD_FILE_MUST_EXIST,
						this->GetPosition() + S_LITE_POSITION_RELATIONAL_PARENT);

	if(fdlg.ShowModal() == wxID_OK)
	{
		/*wxString fnStr = fdlg.GetPath();
		m_appInterface.m_config.SetFileName(fnStr);
		StartViewer();*/
		AppConfig cfg;
		cfg.SetMode(APP_MODE_FILE);
		cfg.SetFileName(fdlg.GetPath());
		m_appInterface->SetAppConfig(cfg);
		// Create session window unconditionally here as there is 
		// no such thing as joining channels in file or multicast mode
		CreateSessionWindow();
		Hide();
		// Initialize only after it's been created
		m_appInterface->InitFilePlayback(cfg.GetFileName());
	}
}

void SkinLite::OnMulticastBtnClick(wxCommandEvent &event)
{
	// We rely on m_appInterface not being NULL here!
	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));

	MulticastConfigDialog mcDlg(this);
	mcDlg.SetPosition(this->GetPosition() + S_LITE_POSITION_RELATIONAL_PARENT);
	mcDlg.SetAppInterface(m_appInterface);
	if(mcDlg.ShowModal() == wxID_OK)
	{
		AppConfig cfg;
		//m_appInterface.m_config = mcDlg.GetConfig();
		//StartViewer();

		// wxMessageBox(_("To be implemented Multicast"), wxT("Information"), wxCENTRE | wxICON_INFORMATION, this);
		cfg = mcDlg.GetConfig();
		m_appInterface->SetAppConfig(cfg);
		// Create session window unconditionally here as there is 
		// no such thing as joining channels in file or multicast mode
		CreateSessionWindow();
		Hide();
		// Initialize only after the Session Window has been created
		m_appInterface->InitMulticast(cfg.GetMcastAddress(), cfg.GetMcastPort());
		wxString msg;
		msg << _("Listening on ") << cfg.GetMcastAddress() << wxT(":") << cfg.GetMcastPort();
		SetStatusText(msg);
	}
}

void SkinLite::OnProfileBtnClick(wxCommandEvent &event)
{
	SettingsManagerDialog *settingsManagerDialog = new SettingsManagerDialog(this, m_appInterface);
	settingsManagerDialog->ShowModal();
}

void SkinLite::OnConfigureBtnClick(wxCommandEvent &event)
{
	ConfigureBtnClick();
}

/** Opens the transmission configuration dialog. 
*
*/
void SkinLite::ConfigureBtnClick()
{
	if(!m_appInterface)
		return;

	TransmissionConfigDialog *cfgDlg = new TransmissionConfigDialog(this);
	std::list<VideoDeviceItem> videoDeviceList;
	std::list<AudioDeviceItem> audioDeviceList;
	std::list<wxString> videoCodecList;
	std::list<wxString> videoBitrateList;
	std::list<wxString> totalAudioCodecList;
	std::list<wxString> audioCodecList;

	int videoDevice, audioDevice;
	wxString videoCodec, audioCodec, transmissionProfile;

	m_appInterface->GetVideoDeviceList(videoDeviceList);
	m_appInterface->GetAudioDeviceList(audioDeviceList);
	m_appInterface->GetVideoCodecList(videoCodecList, videoBitrateList);

//Show only free codecs on Release version
#ifndef _DEVELOPER
	m_appInterface->GetAudioCodecList(totalAudioCodecList);
	std::list<wxString>::iterator audioCodecIt;
	for(audioCodecIt = totalAudioCodecList.begin(); audioCodecIt != totalAudioCodecList.end(); audioCodecIt++)
	{
		if ((*audioCodecIt == wxT("speex_hi")) || (*audioCodecIt == wxT("g711_alaw")) || (*audioCodecIt == wxT("g711_ulaw")) || (*audioCodecIt == wxT("g728_160")))
		{
			audioCodecList.push_back(*audioCodecIt);
		}
			
	}
#else
	m_appInterface->GetAudioCodecList(audioCodecList);
#endif
//END Show only free codecs on Release version

	//Filling the dropDowns
	//cfgDlg->DefaultConnectionTypes();
	wxString defaultVideoDevice = wxEmptyString;
	wxString defaultAudioDevice = wxEmptyString;

	cfgDlg->AddTransmissionProfiles(videoBitrateList, m_appInterface->GetSettingsManager()->GetTransmissionProfile());

	std::list<VideoDeviceItem>::iterator videoDeviceIt;
	for(videoDeviceIt = videoDeviceList.begin(); videoDeviceIt != videoDeviceList.end(); videoDeviceIt++)
	{
		long defaultVideoDeviceIndex;
		m_appInterface->GetSettingsManager()->GetVideoDevice().ToLong(&defaultVideoDeviceIndex);

		if (videoDeviceIt->index == defaultVideoDeviceIndex)
			defaultVideoDevice = videoDeviceIt->name;
	}
	cfgDlg->AddVideoDevices(videoDeviceList, defaultVideoDevice);

	std::list<AudioDeviceItem>::iterator audioDeviceIt;
	for(audioDeviceIt = audioDeviceList.begin(); audioDeviceIt != audioDeviceList.end(); audioDeviceIt++)
	{
		long defaultAudioDeviceIndex;
		m_appInterface->GetSettingsManager()->GetAudioDevice().ToLong(&defaultAudioDeviceIndex);

		if (audioDeviceIt->index == defaultAudioDeviceIndex)
			defaultAudioDevice = audioDeviceIt->name;
	}
	cfgDlg->AddConferAudioDevices(audioDeviceList, defaultAudioDevice);

	cfgDlg->AddVideoCodecs(videoCodecList, m_appInterface->GetSettingsManager()->GetVideoCodec());
	cfgDlg->AddAudioCodecs(audioCodecList, m_appInterface->GetSettingsManager()->GetAudioCodec());
	
	cfgDlg->Fit();

	//Action when dialog OK button is pressed
	if (cfgDlg->ShowModal() == ID_TRANSMISSIONCONFIGDIALOG_BTN_OK)
	{	
		audioDevice = cfgDlg->GetConferenceAudioDevice().index;
		videoDevice = cfgDlg->GetVideoDevice().index;

		videoCodec = cfgDlg->GetVideoCodec().itemName;
		audioCodec = cfgDlg->GetAudioCodec().itemName;
		transmissionProfile = cfgDlg->GetTransmissionProfile().itemName;

		m_appInterface->GetSettingsManager()->SetAudioDevice(wxString::Format(wxT("%d"), audioDevice));
		m_appInterface->GetSettingsManager()->SetVideoDevice(wxString::Format(wxT("%d"), videoDevice));
		m_appInterface->GetSettingsManager()->SetAudioCodec(audioCodec);
		m_appInterface->GetSettingsManager()->SetVideoCodec(videoCodec);
		m_appInterface->GetSettingsManager()->SetTransmissionProfile(transmissionProfile);

	}
}

void SkinLite::OnRecordingSettingsClick(wxCommandEvent &event)
{
	bool recordingStatus;
	long intRecStatus;
	wxString recordingPath;
	RecordingDialog recDlg(this);

	m_appInterface->GetSettingsManager()->GetRecordingStatus().ToLong(&intRecStatus);
	
	recordingStatus = LongToBool(intRecStatus);
	recordingPath = m_appInterface->GetSettingsManager()->GetRecordingPath();
	
	recDlg.SetRecordingEnable(recordingStatus);
	recDlg.SetRecordingFilePath(recordingPath);

	int returnValue = recDlg.ShowModal();
	if (returnValue == ID_RECORDINGDIALOG_BTN_CONFIRM)
	{
		bool recordingStatus = recDlg.GetRecordingStatus();

		wxString strRecStatus = wxString::Format(wxT("%d"), BoolToLong(recordingStatus));
		m_appInterface->GetSettingsManager()->SetRecordingStatus(strRecStatus);

		if (recordingStatus)			
			m_appInterface->GetSettingsManager()->SetRecordingPath(recDlg.GetRecordingFilePath());

	}
}

// App->Skin communication methods

void SkinLite::OnHostConnect()
{
}

/** Shows the channels list.
*
*/
void SkinLite::ShowChannelList()
{
	EnableAllButtons();
	m_channelWindow = new ChannelWindow(m_appInterface, this);
	m_channelWindow->Show();
	// This window will only show again when user disconnects.
	SetStatusText(S_LITE_STATUSTEXT_DISCONNECTED);
	Hide();
}

/** Shows a message on the connecton fail.
* @param[in] message. The message thats showed.
*/
void SkinLite::OnConnectionFailure(wxString message)
{
	AppConfig cfg = m_appInterface->GetAppConfig();
	wxString msg;
	msg.Printf(	S_LITE_STATUSTEXT_FAILURE,
				cfg.GetConfServer().c_str(),
				message.c_str());
	EnableAllButtons();
	SetStatusText(msg);
	SetFocus();
	Raise();
}

/** Shows a message when the server disconnect.
* @param[in] message. The message thats showed.
*/
void SkinLite::OnServerDisconnect(wxString message)
{
	AppConfig cfg = m_appInterface->GetAppConfig();
	// Terminate session if it is active
	if(m_sessionWindowExists)
	{
		m_sessionWindow->Close(true);
	}
	if(cfg.GetMode() == APP_MODE_CONFERENCE)
		m_channelWindow->Close(true);

	Show();
	EnableAllButtons();
	wxString msg;
	msg.Printf(	S_LITE_STATUSTEXT_DISCONNECTED_FROM,
				cfg.GetConfServer().c_str(),
				message.c_str());
	SetStatusText(msg);
	SetFocus();
	Raise();
}

/** Refreshs the channels list.
*
*/
void SkinLite::RefreshChannelList()
{
	if(m_channelWindow->IsShown())
		m_channelWindow->UpdateChannelList();
}

/** Refreshs the users list.
*
*/
void SkinLite::RefreshUserList()
{
	if(!m_sessionWindowExists)
		return;
	if(m_sessionWindow && m_sessionWindow->IsShown())
	{
		m_sessionWindow->UpdateUserList();
		m_sessionWindow->RefreshVideoDisplayArea();
	}
}

/** Triggered when user joins a channel
*
*/
void SkinLite::OnChannelJoin()
{
	m_channelWindow->EnableAllControls();
	m_channelWindow->Hide();
	CreateSessionWindow();
	m_sessionWindow->UpdateUserList();
}

/** Checks if recording is enable or disable
* @param[in] channel. The channel name to compose the record filename.
*/
void SkinLite::CheckConferenceRecording(const wxString& channel)
{
	long longRecordingEnable;
	m_appInterface->GetSettingsManager()->GetRecordingStatus().ToLong(&longRecordingEnable);

	bool isRecordingEnable = LongToBool(longRecordingEnable);

	if (isRecordingEnable)
	{
		wxString completePath;
		completePath = m_appInterface->GetSettingsManager()->GetRecordingPath();

	// Getting path separator.
	#ifdef WIN32
		char pathSeparator = '\\';
	#endif
	#ifdef _LINUX
		char pathSeparator = '/';
	#endif

		completePath += pathSeparator;
		completePath += GetConferenceRecordingFilename(channel);
		
		//Calls routine to enable Recording on kernel
		m_appInterface->EnableConferenceRecording(completePath);
		//wxMessageBox(wxString::Format(wxT("Recording on file: %s"), completePath.c_str()));
	}
	else
		//Calls routine to disable Recording on kernel
		m_appInterface->DisableConferenceRecording();
		//wxMessageBox(wxT("Disables Recording"));
}

/** Triggered when joining a channel fails due to bad password.
* @param[in] failureMessage. Message showed when this event occurs.
*/
void SkinLite::OnChannelBadPassword(const wxString &failureMessage)
{
	wxMessageBox(_("Unable to join channel: Wrong password."), _("Error"), wxCENTRE | wxICON_ERROR, m_channelWindow);
	m_channelWindow->EnableAllControls();
}

/** Triggered when a private message comes up.
* @param[in] sender. user thats send the message.
* @param[in] msg. The message.
*/
void SkinLite::OnPrivateMessage(const wxString &sender, const wxString &msg)
{
	// Only allow chat if in channel
	if(!m_appInterface->GetCurrentChannel())
		return;
	// Get a reference to the current chat window list
	PrivateChatWindowList &pcwList = GetPrivateChatWindowList();
	// Is there a chat window already open for this sender?
	PrivateChatWindowBase *win = pcwList.GetWindowByUserName(sender);
	if(!win)	// If not, create it now and add it to the list
		win = CreatePrivateChatWindow(sender);
	//win->OpenWindow();
	win->SetMessage(sender, msg);
	
	ChatFrame *chatFrameWin = (ChatFrame *)win;
	if ((chatFrameWin->IsIconized() == true) || (chatFrameWin->IsShown() == false))
		m_sessionWindow->BlinkUserReceiveMessage(sender);
}

/** Triggered by App when the number of active transmissions changes.
* @param[in] newCount. New number of active transmissions.
*/
void SkinLite::OnActiveTransmissionCountChange(int newCount)
{
	if(!m_sessionWindowExists)
		return;
	m_appInterface->SetDebugMessage(wxString(wxT("<SkinLite> OnActiveTransmissionChange: newCount = ")) << newCount);
	m_sessionWindow->RefreshVideoDisplayArea();
}

// Triggered when a private chat window is closed
void SkinLite::OnPrivateChatWindowClose(wxCloseEvent &event)
{
	// Get window that is being closed
	ChatFrame *win = static_cast<ChatFrame *>(event.GetEventObject());
	// Get reference to list
	PrivateChatWindowList &pcwList = GetPrivateChatWindowList();
	// Remove window from list
	pcwList.RemoveWindow(win->GetUserName());
	// Destroy window
	win->Destroy();
}

// Triggered when the session window is closed
void SkinLite::OnSessionWindowClose(wxCloseEvent &event)
{
	AppConfig cfg = m_appInterface->GetAppConfig();
	// Terminate SendFilesFrame if this exists
	OnSendFilesClose();
	// Terminate ReceiveFilesFrame if this exists
	OnReceiveFilesClose();
	// Do SessionWindow-specific closing procedure
	m_sessionWindow->OnClose(event);
	// Terminate media session
	m_appInterface->TerminateAllConferences();
	// Leave the channel (send 'PART' message). 
	// Only makes sense in CONFERENCE mode. Show Channel Window afterwards.
	if(cfg.GetMode() == APP_MODE_CONFERENCE)
	{
		m_appInterface->LeaveChannel(m_appInterface->GetCurrentChannel()->GetName());
		ShowChannelList();
	}
	else
		Show();

	((wxWindow *)event.GetEventObject())->Destroy();
	// Reset session window existence flag
	m_sessionWindowExists = false;
}

/** Triggered when a channel message comes up.
* @param[in] sender. User thats send the message.
* @param[in] msg. The message.
* @param[in] channel. The channel name who receives the message.
*/
void SkinLite::OnChannelMessage(const wxString &sender, const wxString &msg, const wxString &channel)
{
	// Only allow chat if in channel
	if(!m_appInterface->GetCurrentChannel())
		return;
#if 0	// Old code
	ChatFrame *publicChatFrame = m_sessionWindow->GetPublicChatFrame();
	if(publicChatFrame->IsIconized())
	{
		publicChatFrame->Iconize(false);
		publicChatFrame->RequestUserAttention();
	}
	publicChatFrame->SetMessage(sender, msg);
#endif
	m_sessionWindow->SetPublicChatMessage(sender, msg);
}

/** Add a VideoDisplayControl.
* @param[in] tdc. The trasmission display control.
*/
void SkinLite::AddTransmissionDisplayControl(TransmissionDisplayControl *tdc)
{
	// TODO: Review this behavior.
	if(!m_sessionWindowExists)
		return;
	m_sessionWindow->AddTransmissionDisplayControl(tdc);
}

/** Remove a VideoDisplayControl.
* @param[in] tdc. The trasmission display control.
*/
void SkinLite::RemoveTransmissionDisplayControl(TransmissionDisplayControl *tdc)
{
	if(!m_sessionWindowExists)
		return;
	m_sessionWindow->RemoveTransmissionDisplayControl(tdc);
}

/** VideoDisplayControl handling method, version 4 - Add a TransmissionDisplayControl.
*
*/
VideoSink *SkinLite::CreateVideoSink(void *extraData)
{
	const AppConfig &cfg = m_appInterface->GetAppConfig();
	TransmissionDisplayControl *tdc = new TransmissionDisplayControl;
	switch(cfg.GetMode())
	{
	case APP_MODE_CONFERENCE:
		tdc->SetChannelUser((ChannelUser *)extraData);
		break;
	case APP_MODE_MULTICAST:
		tdc->SetCaption(*((wxString *)extraData));
		break;
	default:
		break;
	}
	
	tdc->SetParentSkin(this);
	m_sessionWindow->AddTransmissionDisplayControl(tdc);
	return tdc;
}


/** Create a private chat window.
* @param[in] userName. The user name thats create the chat.
*/
PrivateChatWindowBase *SkinLite::CreatePrivateChatWindow(const wxString &userName)
{
	// Get a reference to the current chat window list
	PrivateChatWindowList &pcwList = GetPrivateChatWindowList();
	// Create the window, and set some initial values for it
	ChatFrame *win = new ChatFrame(m_sessionWindow);
	win->SetAppInterface(m_appInterface);
	win->SetUserName(userName);
	// Set title bar. TODO: test under Unix/GTK+
	wxString title;
	title << userName << wxT(" - ") << _("Private Chat");
	win->SetTitle(title);
	// win->SetLabel(title);
	// NOTE: The Close event for the private chat windows is treated HERE instead of in m_sessionWindow,
	// even though m_sessionWindow is the parent for all of them. This makes it easier to take note of
	// when private chat windows are closed, removing them from the list.
	win->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SkinLite::OnPrivateChatWindowClose), NULL, this);
	pcwList.AddWindow(win);

	return win;
}

/** Update the file progress bar.
*
*/
void SkinLite::UpdateFileProgressIndicator(float percent)
{
	// Can't update position on a non-existing slider
	if(m_sessionWindowExists)
	{
		m_sessionWindow->UpdateFileProgressSlider(percent);
	}
}

void SkinLite::OnFileTransferReceptionBegin(long mediaId,
											const wxString &fileName,
											const wxString &tempFilePath,
											const wxString &senderNick,
											unsigned long fileSize,
											unsigned packetSize,
											bool renameFile)
{
	// Can't receive file if a SessionWindow don't exist
	if(m_sessionWindowExists)
	{
		// We don't need to destroy the last ReceiveFilesFrame, the user will do it on close
		// or it will be killed when the SessionWindow closes
		m_activeReceiveFilesFrame = new ReceiveFilesFrame(m_sessionWindow, m_appInterface);

		// Send the information to m_activeReceiveFilesFrame
		m_activeReceiveFilesFrame->OnFileTransferReceptionBegin(mediaId, fileName, tempFilePath,
																senderNick, fileSize, packetSize,
																renameFile);

		// Show m_activeReceiveFilesFrame
		m_activeReceiveFilesFrame->Show();
	}
}

void SkinLite::OnFileTransferReceptionProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	if(m_receiveFilesExists)
	{
		m_activeReceiveFilesFrame->OnFileTransferReceptionProgress(currentPacketIndex, lastPacketIndex);
	}
}

void SkinLite::OnFileTransferReceptionLostPacket(unsigned long lostPackets)
{
	if(m_receiveFilesExists)
	{
		m_activeReceiveFilesFrame->OnFileTransferReceptionLostPacket(lostPackets);
	}
}

void SkinLite::OnFileTransferReceptionSuccess()
{
	// We don't need to destroy the ReceiveFilesFrame, the user will do it on close
	// or it will be killed when the SessionWindow closes
	if(m_receiveFilesExists)
	{
		m_activeReceiveFilesFrame->OnFileTransferReceptionSuccess();
		//m_activeReceiveFilesFrame = NULL; // no more notifications will come to this frame
	}
}

void SkinLite::OnFileTransferReceptionError()
{
	// We don't need to destroy the ReceiveFilesFrame, the user will do it on close
	// or it will be killed when the SessionWindow closes
	if(m_receiveFilesExists)
	{
		if(m_sessionWindowExists)
			m_activeReceiveFilesFrame->OnFileTransferReceptionError();
		//m_activeReceiveFilesFrame = NULL; // no more notifications will come to this frame
	}
}

void SkinLite::OnFileTranferSendBegin(long mediaId)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTranferSendBegin(mediaId);
	}
}

void SkinLite::OnFileTransferSendProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendProgress(currentPacketIndex, lastPacketIndex);
	}
}

void SkinLite::OnFileTransferSendLostPacket(unsigned long lostPackets, unsigned long lostPacketsTotal)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendLostPacket(lostPackets, lostPacketsTotal);
	}
}

void SkinLite::OnFileTransferSendEof()
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendEof();
	}
}

void SkinLite::OnFileTransferSendSuccess()
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendSuccess();
	}
}

void SkinLite::OnFileTransferSendError()
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendError();
	}
}

void SkinLite::OnFileTransferSendReceiverNew(long mediaId)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendReceiverNew(mediaId);
	}
}

void SkinLite::OnFileTransferSendReceiverCancel(long mediaId)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendReceiverCancel(mediaId);
	}
}

void SkinLite::OnFileTransferSendReceiverEnd(long mediaId)
{
	if (m_sendFilesExists)
	{
		m_sendFilesFrame->OnFileTransferSendReceiverEnd(mediaId);
	}
}

//Movie playlist events
//-----------------------
/** Movie started.
*
*/
void SkinLite::OnMovieSendBegin(long mediaId, const wxString &channelName)
{
	if (m_playlistExists)
		m_playlistFrame->OnMovieSendBegin(mediaId, channelName);

}

/** Movie finished.
*
*/
void SkinLite::OnMovieSendEof(long mediaId, const wxString &channelName)
{
	if (m_playlistExists)
		m_playlistFrame->OnMovieSendEof(mediaId, channelName);
}

// Poll methods
// ------------
/** Received a new poll.
*
*/
void SkinLite::OnPoll()
{
	if(m_sessionWindowExists)
	{
		m_sessionWindow->ShowPollAnswerDialog();
	}
}

/** Active poll received a vote.
*
*/
void SkinLite::OnPollVote()
{
	if(m_sessionWindowExists)
		m_sessionWindow->UpdateActivePollStats();
}

/** Received poll statistics.
*
*/
void SkinLite::OnPollStats(const PollInfo &info)
{
	if(m_sessionWindowExists)
	{
		m_sessionWindow->ShowPollStats(info);
	}
}

/** Application Sharing receiving began.
*
*/
void SkinLite::OnAppSharingBegin()
{
	if(m_sessionWindowExists)
	{
//		m_sessionWindow->ShowAppSharingWindow();
		m_sessionWindow->RegisterAppSharingDisplayControl();
	}
}

/** Application Sharing receiving ended.
*
*/
void SkinLite::OnAppSharingEnd()
{
	if(m_sessionWindowExists)
	{
		m_sessionWindow->UnregisterAppSharingDisplayControl();
	}
}

/** Whiteboard message received.
*
*/
void SkinLite::OnWhiteboardMessage(const wxString &channel, const wxString &user, const wxString &data)
{
	m_appInterface->SetDebugMessage(wxString(wxT("<Skin> Whiteboard message, user = ")) << user);
	m_sessionWindow->OnWhiteboardMessage(channel, user, data);
}

/**Auto play Collaboration
*
*/
void SkinLite::OnCollaborationAutoPlay()
{
	if (m_sessionWindowExists)
		m_sessionWindow->CollaborationAutoPlay();
}

/** Stop Collaboration.
*
*/
void SkinLite::OnCollaborationStop()
{
	if (m_sessionWindowExists)
		m_sessionWindow->CollaborationStop();
}

// Private methods
/** Disables all buttons.
* Private method.
*/
void SkinLite::DisableAllButtons()
{
	m_btnUnicast->Disable();
	m_btnMulticast->Disable();
	m_btnFile->Disable();
	//m_btnChangeLanguage->Disable();
	//m_btnChangeSkin->Disable();
	//m_btnAbout->Disable();
	//m_btnProfile->Disable();
	//m_btnConfigure->Disable();
}

/** Enables all buttons.
* Private method.
*/
void SkinLite::EnableAllButtons()
{
	m_btnUnicast->Enable();
	m_btnMulticast->Enable();
	m_btnFile->Enable();
	//m_btnChangeLanguage->Enable();
	//m_btnChangeSkin->Enable();
	//m_btnAbout->Enable();
	//m_btnProfile->Enable();
	//m_btnConfigure->Enable();
}

/** Creates the Session window.
* Private method.
*/
void SkinLite::CreateSessionWindow()
{
	m_sessionWindow = new SessionWindow(this, wxID_ANY, SYMBOL_USERSLISTFRAME_CAPTION, SYMBOL_USERSLISTFRAME_POSITION, SYMBOL_USERSLISTFRAME_SIZE, SYMBOL_USERSLISTFRAME_STYLE, m_appInterface);
	m_sessionWindow->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SkinLite::OnSessionWindowClose), NULL, this);
	//m_sessionWindow->SetAppInterface(m_appInterface);
	m_sessionWindow->Show();
	m_sessionWindowExists = true;
}

/** Creates the send files window.
* Private method.
*/
void SkinLite::CreateSendFiles()
{
	// Search the sendFilesFrame
	wxWindow* window = wxWindow::FindWindowById(ID_SENDFILESFRAME, this);

	// If it doesn't exist then create one
	if(window == NULL)
	{
		if (m_sessionWindowExists)
		{
			m_sendFilesFrame = new SendFilesFrame(m_sessionWindow, m_appInterface);
			m_sendFilesFrame->Show();
		}
	}
	// If it exists then set focus to it
	else
	{
		m_sendFilesFrame = (SendFilesFrame *)window;
		m_sendFilesFrame->Raise();
	}	
}

/** Closes the send files window.
* Private method.
*/
void SkinLite::OnSendFilesClose()
{
	if (m_sendFilesExists)
		m_sendFilesFrame->Close(true);
}

/** Closes the receive files window.
* Private method.
*/
void SkinLite::OnReceiveFilesClose()
{
	if (m_receiveFilesExists)
		m_activeReceiveFilesFrame->Close(true);
	m_activeReceiveFilesFrame = NULL;
}

/** Change the receive files flag when this window is created or destroyed.
* Private method.
*/
void SkinLite::ChangeReceiveFilesExistsStatus(const bool &receiveFilesExists)
{
	m_receiveFilesExists = receiveFilesExists;
}

/** Change the send files flag when this window is created or destroyed.
* Private method.
*/
void SkinLite::ChangeSendFilesExistsStatus(const bool &sendFilesExists)
{
	m_sendFilesExists = sendFilesExists;
}

/** Change the playlist flag when this window is created or destroyed.
* Private method.
*/
void SkinLite::ChangePlaylistExistsStatus(const bool &playlistExists)
{
	m_playlistExists = playlistExists;
}

/** Checks the session window existance.
* 
*/
bool SkinLite::CheckSessionWindowExists()
{
	return m_sessionWindowExists;
}

/** Creates the playlist window.
* Private method.
*/
void SkinLite::CreatePlaylist()
{
	// Search the sendFilesFrame
	wxWindow* window = wxWindow::FindWindowById(ID_PLAYLISTFRAME, this);

	// If it doesn't exist then create one
	if(window == NULL)
	{
		if (m_sessionWindowExists)
		{
			m_playlistFrame = new PlaylistFrame(m_sessionWindow, m_appInterface);
			m_playlistFrame->Show();
		}
	}
	// If it exists then set focus to it
	else
	{
		m_playlistFrame = (PlaylistFrame *)window;
		m_playlistFrame->Raise();
	}
	m_playlistExists = true;
}

/** Creates the whiteboard.
* Private method.
*/
void SkinLite::StartWhiteboardFileTransmission(const wxString &filePath)
{
	CreateSendFiles();
	m_sendFilesFrame->StartWhiteboardFileTransmission(filePath);
}

// Auxiliary Methods
long SkinLite::BoolToLong(const bool &boolVariable)
{
	if (boolVariable)
		return 1;
	else
		return 0;
}

bool SkinLite::LongToBool(const long &longVariable)
{
	if (longVariable == 0)
		return false;
	else
		return true;
}

/** Return the recording file name.
*
*/
wxString SkinLite::GetConferenceRecordingFilename(const wxString& channel)
{
	wxDateTime currentTime = wxDateTime::Now();
	wxString recordingFilename;
	wxString channelName = channel;
	wxString userName = m_appInterface->GetCurrentUser()->GetNickName();
	int day = currentTime.GetDay();
	int month = currentTime.GetMonth();
	int year = currentTime.GetYear();
	int hour = currentTime.GetHour();
	int minute = currentTime.GetMinute();
	int second = currentTime.GetSecond();

	wxString strDay = wxString::Format(wxT("%d"), day);
	wxString strMonth = wxString::Format(wxT("%d"), month);
	wxString strYear = wxString::Format(wxT("%d"), year);
	wxString strHour = wxString::Format(wxT("%d"), hour);
	wxString strMinute = wxString::Format(wxT("%d"), minute);
	wxString strSecond = wxString::Format(wxT("%d"), second);

	recordingFilename = wxT("IPTVConf.");
	recordingFilename += channelName + wxT(".");
	recordingFilename += userName + wxT(".");
	
	if (day < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strDay + wxT("-");

	if (month < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strMonth + wxT("-");

	if (year < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strYear + wxT(".");

	if (hour < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strHour + wxT("_");

	if (minute < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strMinute + wxT("_");

	if (second < 10)
		recordingFilename += wxT("0"); 
	recordingFilename += strSecond + wxT(".imc");

	return recordingFilename;
}

// Compatibility with GTK+ 2
#ifdef __WXGTK__
void SkinLite::OnWindowCreate(wxWindowCreateEvent &WXUNUSED(event))
{
	// Set label
	SetLabel(wxString(SKIN_CAPTION) + wxString(_("Skin")) + wxString(SKIN_NAME, wxConvUTF8));
}
#endif // __WXGTK__
