#include <list>
#include <deque>

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filesys.h>
#include <wx/textfile.h>

#include "ChatMessage.h"
#include "AboutDialog.h"
#include "IPTVClientApp.h"
#include "AppConstants.h"
#include "Mode.h"
#include "User.h"
#include "Channel.h"
#include "ChannelUser.h"
#include "UserList.h"
#include "MediaVideoSource.h"
#include "CamVideoSource.h"
#include "MediaDeviceItem.h"
#include "FileTransferParameters.h"
#include "WhiteboardParameters.h"
#include "PollInfo.h"
#include "iptv_client_buildid.h"

#ifdef IPTV_DEBUG
#include <iostream>
#include "DebugWindow.h"
#endif

using namespace std;

// A set of wxCommandEvents to be used by App::OnKernelNotify()
// Done to avoid GUI calls from threads other than the main (app) thread

// IRC events
DEFINE_EVENT_TYPE(iptvEVT_CONNECTED_HOST)
DEFINE_EVENT_TYPE(iptvEVT_CONNECTED_CHAT)
DEFINE_EVENT_TYPE(iptvEVT_DISCONNECTED)
DEFINE_EVENT_TYPE(iptvEVT_AUTHENTICATED)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_QUERY_ITEM)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_QUERY_END)
DEFINE_EVENT_TYPE(iptvEVT_USER_QUERY_ITEM)
DEFINE_EVENT_TYPE(iptvEVT_USER_QUERY_END)
DEFINE_EVENT_TYPE(iptvEVT_USER_MODE_ON)
DEFINE_EVENT_TYPE(iptvEVT_USER_MODE_OFF)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_JOIN)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_PART)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_MODE_ON)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_MODE_OFF)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_USER_FLAGS_ON)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_USER_FLAGS_OFF)
DEFINE_EVENT_TYPE(iptvEVT_PRIV_MSG)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_MSG)
DEFINE_EVENT_TYPE(iptvEVT_USER_QUIT)
// Media events
DEFINE_EVENT_TYPE(iptvEVT_NEW_TRANSMISSION)
DEFINE_EVENT_TYPE(iptvEVT_MEDIA_INFO)
DEFINE_EVENT_TYPE(iptvEVT_AUDIO_ALIVE)
DEFINE_EVENT_TYPE(iptvEVT_VIDEO_ALIVE)
DEFINE_EVENT_TYPE(iptvEVT_MEDIA_PROGRESS)
// Capture event
DEFINE_EVENT_TYPE(iptvEVT_CAPTURE_NEW_FRAME)
// File Transfer events
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_WB_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_PROGRESS)
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_LOST_PACKET)
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_SUCCESS)
DEFINE_EVENT_TYPE(iptvEVT_FT_RECEIVE_ERROR)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_PROGRESS)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_LOST_PACKET)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_EOF)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_SUCCESS)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_ERROR)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_NEW)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_CANCEL)
DEFINE_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_END)

// Movie Playlist events
DEFINE_EVENT_TYPE(iptvEVT_MOVIE_SEND_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_MOVIE_SEND_EOF)

// Poll events
DEFINE_EVENT_TYPE(iptvEVT_POLL_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_POLL_QUESTION)
DEFINE_EVENT_TYPE(iptvEVT_POLL_OPTION)
DEFINE_EVENT_TYPE(iptvEVT_POLL_END)
DEFINE_EVENT_TYPE(iptvEVT_POLL_ANSWER)
DEFINE_EVENT_TYPE(iptvEVT_POLL_STATS_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_POLL_STATS_QUESTION)
DEFINE_EVENT_TYPE(iptvEVT_POLL_STATS_OPTION)
DEFINE_EVENT_TYPE(iptvEVT_POLL_STATS_END)
// Application Sharing
DEFINE_EVENT_TYPE(iptvEVT_AS_RECEIVE_BEGIN)
DEFINE_EVENT_TYPE(iptvEVT_AS_RECEIVE_NEW_FRAME)
DEFINE_EVENT_TYPE(iptvEVT_AS_RECEIVE_END)
// Whiteboard
DEFINE_EVENT_TYPE(iptvEVT_WB_WBOARD_CHANNEL)

// Timeout
// DEFINE_EVENT_TYPE(iptvEVT_TIMEOUT)
// Error events
DEFINE_EVENT_TYPE(iptvEVT_AUTHENTICATION_ERROR)
DEFINE_EVENT_TYPE(iptvEVT_CHANNEL_BAD_PASSWORD)
#ifdef IPTV_DEBUG
DEFINE_EVENT_TYPE(iptvEVT_DEBUG)
#endif

BEGIN_EVENT_TABLE(IPTVClientApp, wxApp)
	EVT_COMMAND(wxID_ANY, iptvEVT_CONNECTED_HOST, IPTVClientApp::OnHostConnect)
	EVT_COMMAND(wxID_ANY, iptvEVT_CONNECTED_CHAT, IPTVClientApp::OnChatConnect)
	EVT_COMMAND(wxID_ANY, iptvEVT_DISCONNECTED, IPTVClientApp::OnDisconnect)
	EVT_COMMAND(wxID_ANY, iptvEVT_AUTHENTICATED, IPTVClientApp::OnAuthenticationSuccess)
	EVT_COMMAND(wxID_ANY, iptvEVT_AUTHENTICATION_ERROR, IPTVClientApp::OnAuthenticationFailure)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_QUERY_ITEM, IPTVClientApp::OnChannelQueryItem)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_QUERY_END, IPTVClientApp::OnChannelQueryEnd)
	EVT_COMMAND(wxID_ANY, iptvEVT_USER_QUERY_ITEM, IPTVClientApp::OnUserQueryItem)
	EVT_COMMAND(wxID_ANY, iptvEVT_USER_QUERY_END, IPTVClientApp::OnUserQueryEnd)
	EVT_COMMAND(wxID_ANY, iptvEVT_USER_MODE_ON, IPTVClientApp::OnUserMode)
	EVT_COMMAND(wxID_ANY, iptvEVT_USER_MODE_OFF, IPTVClientApp::OnUserMode)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_JOIN, IPTVClientApp::OnChannelJoin)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_PART, IPTVClientApp::OnChannelPart)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_MODE_ON, IPTVClientApp::OnChannelMode)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_MODE_OFF, IPTVClientApp::OnChannelMode)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_USER_FLAGS_ON, IPTVClientApp::OnChannelUserFlags)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_USER_FLAGS_OFF, IPTVClientApp::OnChannelUserFlags)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_BAD_PASSWORD, IPTVClientApp::OnChannelJoinFailure)
	EVT_COMMAND(wxID_ANY, iptvEVT_PRIV_MSG, IPTVClientApp::OnPrivateMessage)
	EVT_COMMAND(wxID_ANY, iptvEVT_CHANNEL_MSG, IPTVClientApp::OnChannelMessage)
	EVT_COMMAND(wxID_ANY, iptvEVT_USER_QUIT, IPTVClientApp::OnUserQuit)
	// Media events
	EVT_COMMAND(wxID_ANY, iptvEVT_NEW_TRANSMISSION, IPTVClientApp::OnNewTransmission)
	EVT_COMMAND(wxID_ANY, iptvEVT_MEDIA_INFO, IPTVClientApp::OnMediaInfo)
	EVT_COMMAND(wxID_ANY, iptvEVT_AUDIO_ALIVE, IPTVClientApp::OnMediaAudioAlive)
	EVT_COMMAND(wxID_ANY, iptvEVT_VIDEO_ALIVE, IPTVClientApp::OnMediaVideoAlive)
	EVT_COMMAND(wxID_ANY, iptvEVT_MEDIA_PROGRESS, IPTVClientApp::OnMediaProgressNotify)
	// Capture event
	EVT_COMMAND(wxID_ANY, iptvEVT_CAPTURE_NEW_FRAME, IPTVClientApp::OnCaptureNewFrame)
	// File Transfer events
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_BEGIN, IPTVClientApp::OnFileTransferReceptionBegin)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_WB_BEGIN, IPTVClientApp::OnFileTransferReceptionWBBegin)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_PROGRESS, IPTVClientApp::OnFileTransferReceptionProgress)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_LOST_PACKET, IPTVClientApp::OnFileTransferReceptionLostPacket)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_SUCCESS, IPTVClientApp::OnFileTransferReceptionSuccess)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_RECEIVE_ERROR, IPTVClientApp::OnFileTransferReceptionError)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_BEGIN, IPTVClientApp::OnFileTransferSendBegin)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_PROGRESS, IPTVClientApp::OnFileTransferSendProgress)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_LOST_PACKET, IPTVClientApp::OnFileTransferSendLostPacket)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_EOF, IPTVClientApp::OnFileTransferSendEof)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_SUCCESS, IPTVClientApp::OnFileTransferSendSuccess)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_ERROR, IPTVClientApp::OnFileTransferSendError)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_RECEIVER_NEW, IPTVClientApp::OnFileTransferSendReceiverNew)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_RECEIVER_CANCEL, IPTVClientApp::OnFileTransferSendReceiverCancel)
	EVT_COMMAND(wxID_ANY, iptvEVT_FT_SEND_RECEIVER_END, IPTVClientApp::OnFileTransferSendReceiverEnd)
	//Movie playlist events
	EVT_COMMAND(wxID_ANY, iptvEVT_MOVIE_SEND_BEGIN, IPTVClientApp::OnMovieSendBegin)
	EVT_COMMAND(wxID_ANY, iptvEVT_MOVIE_SEND_EOF, IPTVClientApp::OnMovieSendEof)
	// Poll events
	EVT_COMMAND(wxID_ANY, iptvEVT_POLL_END, IPTVClientApp::OnPoll)
	EVT_COMMAND(wxID_ANY, iptvEVT_POLL_ANSWER, IPTVClientApp::OnPollAnswer)
	EVT_COMMAND(wxID_ANY, iptvEVT_POLL_STATS_END, IPTVClientApp::OnPollStats)
	// Application Sharing events
	EVT_COMMAND(wxID_ANY, iptvEVT_AS_RECEIVE_BEGIN, IPTVClientApp::OnAppSharingBegin)
	EVT_COMMAND(wxID_ANY, iptvEVT_AS_RECEIVE_END, IPTVClientApp::OnAppSharingEnd)
	EVT_COMMAND(wxID_ANY, iptvEVT_AS_RECEIVE_NEW_FRAME, IPTVClientApp::OnAppSharingNewFrame)
	// Whiteboard events
	EVT_COMMAND(wxID_ANY, iptvEVT_WB_WBOARD_CHANNEL, IPTVClientApp::OnWhiteboardChannel)
	// Time management
	EVT_COMMAND(TR_MEDIA_AUDIO_TIMEOUT, iptvEVT_TIMEOUT, IPTVClientApp::OnMediaAudioTimeout)
	EVT_COMMAND(TR_MEDIA_VIDEO_TIMEOUT, iptvEVT_TIMEOUT, IPTVClientApp::OnMediaVideoTimeout)
	EVT_TIMER(ID_APP_TIMER, IPTVClientApp::OnTimer)
#ifdef IPTV_DEBUG
	EVT_COMMAND(wxID_ANY, iptvEVT_DEBUG, IPTVClientApp::OnDebugEvent)
#endif
END_EVENT_TABLE()

IMPLEMENT_APP(IPTVClientApp)

/** Default constructor.
*
*/
IPTVClientApp::IPTVClientApp() : m_selectedLanguageIndex(0)
{
	m_skin = NULL;
	m_locale = NULL;
	m_currentChannel = NULL;
	m_currentUser = NULL;
	m_settingsManager = NULL;
	m_activePoll = NULL;
	m_enableTransmissions = false;
	m_FTReceiveNotificationsEnable = true;
	m_isTransmitting = false;
	m_seriesCollabStarted = false;
	m_scSelectAllUsers = false;
	m_scSwitchInterval = SC_DEFAULT_SWITCH_INTERVAL;

#ifdef IPTV_DEBUG
	m_debugWindow = NULL;
#endif

	//Session time initialization
	m_appGlobalTimer.SetOwner(this, ID_APP_TIMER);
	m_timeStarted = wxDateTime::UNow();
	m_sessionTime = wxTimeSpan(0, 0, 0, 0);
	m_appGlobalTimer.Start(APP_TIMER_INTERVAL);
	//END Session time initialization
}

/** Destructor.
*
*/
IPTVClientApp::~IPTVClientApp()
{
	if(m_locale != NULL)
	{
		delete m_locale;
	}

	if(m_settingsManager != NULL)
	{
		delete m_settingsManager;
	}
}

/** App initialization.
*
*/
bool IPTVClientApp::OnInit()
{

	// Set working directory to executable dir
	wxSetWorkingDirectory(GetAppPath());
	//wxMessageBox(wxGetCwd());

#ifdef IPTV_IE_PLUGIN
	wxString path;
	if (wxGetEnv(wxT("iptv_client"), &path))
	{
		wxMessageBox(wxT("GetEnv"));
		wxMessageBox(path);
	}
	else
	{
		wxMessageBox(wxT("Can't get iptv_client envinroment value"));
		return false;
	}

	
	wxSetWorkingDirectory(wxString::Format(wxT("%s/bin"), path.c_str()));
	wxMessageBox(wxGetCwd());
#endif
	// initialize all image handlers
	wxInitAllImageHandlers();
	// Initialize timeout register
	m_timeoutRegister.SetEventHandler(this);
	// initialize kernel notifications
	m_kernel.SetNotify(this);
	m_lastConnectionState = m_kernel.GetConnectionState();
	m_lastJoinState = m_kernel.GetChannelJoinState();
#ifdef IPTV_DEBUG
	// Initialize the debug window.
	m_debugWindow = new DebugWindow(NULL);
	m_debugWindow->Show();
#endif

	// load application configurations
	m_settingsManager = new SettingsManager();
	if(!m_settingsManager->Initialize())
	{
		wxMessageBox(CANNOT_INITIALIZE_CONFIG);
		return false;
	}

	// Enable Media Conference
	m_kernel.EnableMediaConference();
	// Enable file transfer
	EnableFileTransfer();
	// Enable poll module
	m_kernel.EnablePoll();
	// Initialize poll variables
	ResetActivePoll();
	// Enable whiteboard module
	m_kernel.EnableWhiteboard();

	// load language
	long languageIndex;
	wxString language = m_settingsManager->GetLanguage();
	language.ToLong(&languageIndex);
	SelectLanguage(languageIndex);


	// load skin

	bool loadSkinSucessful = LoadDefaultSkin();
	if(loadSkinSucessful)
	{
		//StringHash x;
		//x = GetAvailableSkins();
		//wxString y;

		//StringHash::iterator i = x.begin();

		//while(i != x.end())
		//{
		//	y += i->first;
		//	y += _("-");
		//	y += i->second;
		//	y += _("\n");

		//	i++;
		//}
		//
		//wxMessageBox(y);

		if(m_skin != NULL)
		{
			SetTopWindow(m_skin);
			m_skin->SetAppInterface(this);
			m_skin->Show();
		}

		else
		{
			return false;
		}
	}

	return loadSkinSucessful;
}

/** App termination procedures.
 *
 */
int IPTVClientApp::OnExit()
{
	m_appGlobalTimer.Stop();
	m_camVideoSource.Close();	// makes sure the device is released on close
	if(m_currentUser) delete m_currentUser;
	ClearUserList();
	ClearChannelList();
	return wxApp::OnExit();
}

/** Loads the default skin.
*
* This procedure loads the default skin, as defined in the configuration file (settings.xml).
* @return true if skin load succeeded, false otherwise.
*/
bool IPTVClientApp::LoadDefaultSkin()
{
	try
	{
		// Gets the name of the default skin.
		wxString defaultSkin = m_settingsManager->GetSkin();
		
		m_skinSelected = defaultSkin;

		if(defaultSkin == DEFAULT_SKIN_FILE_NOT_FOUND)
		{
			wxMessageBox(DEFAULT_SKIN_FILE_NOT_FOUND);
			return false;
		}

		StringHash skinHash = GetAvailableSkins();
		int n_dll = (int)skinHash.size();

		if(n_dll == 0)
		{
			wxMessageBox(NO_SKIN_FOUND);
			return false;
		}

		StringHash::iterator i = skinHash.begin();

		wxString skinName = wxEmptyString;
		wxString skinLibrary = wxEmptyString;

		while(i != skinHash.end())
		{
			skinName = i->first;

			if(defaultSkin.Lower() == skinName.Lower())
			{
				skinLibrary = i->second;
				break;
			}

			i++;
		}

		if(skinLibrary == wxEmptyString)
		{
			wxMessageBox(DEFAULT_SKIN_NOT_FOUND);
			return false;
		}

		return LoadSkin(skinLibrary);
	}

	catch(VBException_crossDLL & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION_CROSSDLL);
		return false;
	}

	catch(VBException & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION);
		return false;
	}

	return false;
}

/** Gets a list of available skins.
 *
 * This method gets a list of available Skin shared objects present in the skins directory.
 * All Skin shared objects must export the AllocSkin and GetSkinName functions. (Their prototypes
 * are documented elsewhere).
 * @return a StringHash containing pairs of skin name/file name.
 */
StringHash IPTVClientApp::GetAvailableSkins()
{
	VBCrossDLLContainer dllContainer;

	const char *exportedNamesDllMustHave[] = {"AllocSkin", "GetSkinName"};
	unsigned s = sizeof(exportedNamesDllMustHave)/sizeof(const char *);

	typedef Skin *(*AllocSkinPtrType)();
	typedef const char *(*GetSkinNamePtrType)();

//	AllocSkinPtrType allocSkinPtr;
	GetSkinNamePtrType getSkinNamePtr;

	StringHash skinHash;

	try
	{
		// Loads dynamic libraries
		dllContainer.loadDllsWithAllExportedNames
			(exportedNamesDllMustHave, s, DYNAMIC_LIBRARY_FOLDER, DYNAMIC_LIBRARY_EXTENSION);

		// Gets the number of dynamic libraries loaded
		unsigned n_dll = dllContainer.getNumberOfDlls();

		// Fill the hash table
		for(unsigned i=0 ; i < n_dll ; i++)
		{
			getSkinNamePtr = (GetSkinNamePtrType) dllContainer[i].getAddress("GetSkinName");

			wxString skinName = wxString(getSkinNamePtr(), wxConvUTF8);
			wxString skinLibrary = wxString(dllContainer[i].getName(), wxConvUTF8);

			skinHash[skinName] = skinLibrary;
		}

		return skinHash;
	}

	catch(VBException_crossDLL & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION_CROSSDLL);
		StringHash emptyHash;
		return emptyHash;
	}

	catch(VBException & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION);
		StringHash emptyHash;
		return emptyHash;
	}

	StringHash emptyHash;
	return emptyHash;
}

/** Loads a Skin shared object, and shows it.
 * @param[in] skinLibrary	Shared object file name.
 * @return true if load succeeded, false otherwise.
 */
bool IPTVClientApp::LoadSkin(wxString skinLibrary)
{
	typedef Skin *(*AllocSkinPtrType)();
	AllocSkinPtrType allocSkinPtr;

	try
	{
		Skin *skin;
		VBCrossDLL newSkinLibrary;
							
		newSkinLibrary.load(skinLibrary.mb_str());

		// Gets the function pointer of the skin
		allocSkinPtr = (AllocSkinPtrType) newSkinLibrary.getAddress("AllocSkin");

		// Late bind here, new Skin instantiation
		skin = allocSkinPtr();

		if(m_skin != NULL)
		{
			SetTopWindow(skin);
			skin->SetAppInterface(this);
			m_skin->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(IPTVClientApp::OnSkinMainWindowClose), NULL, this);
			m_skin->Close(true);
		}

		m_skin = skin;

		m_vbCrossDll.unLoad();
		m_vbCrossDll = newSkinLibrary;

		m_skin->SetAppInterface(this);
		m_skin->Initialize();
		m_skin->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(IPTVClientApp::OnSkinMainWindowClose), NULL, this);
		skin->Show();
		return true;
	}

	catch(VBException_crossDLL & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION_CROSSDLL);
		return false;
	}

	catch(VBException & e)
	{
		wxMessageBox(wxString(e.getExplanation(), wxConvUTF8), VBEXCEPTION);
		return false;
	}

	return false;
}

/** Selects the application language.
* @param[in] language		The language canonical name, e.g. "pt_BR"
* @param[in] savePrevious	Whether or not the previous language set should be saved (default: false)
*/
void IPTVClientApp::SelectLanguage(int language, bool savePrevious)
{
	if( m_locale != NULL )
	{
		delete m_locale;
	}

	wxString localePath;
	localePath << GetAppPath() << wxT("/") << LOCALE_DIRECTORY_NAME;
	m_locale = new wxLocale(language);
	m_locale->AddCatalogLookupPathPrefix(localePath);
	m_locale->AddCatalog(LOCALE_FILE_NAME);

	//Puts the default language
	m_languageSelected = GetLanguageName(language);

	if(!savePrevious)
		m_selectedLanguageIndex = language;
}

/** Gets the list of the canonical names of the supported languages.
* @return An array of strings containing the canonical names of all supported languages.
*/
wxArrayString IPTVClientApp::GetCanonicalNamesList()
{
	wxArrayString languageList;

	languageList.Add(wxT(""));     
	languageList.Add(wxT(""));     
	languageList.Add(wxT("ab"));   
	languageList.Add(wxT("aa"));   
	languageList.Add(wxT("af_ZA"));
	languageList.Add(wxT("sq_AL"));
	languageList.Add(wxT("am"));   
	languageList.Add(wxT("ar"));   
	languageList.Add(wxT("ar_DZ"));
	languageList.Add(wxT("ar_BH"));
	languageList.Add(wxT("ar_EG"));
	languageList.Add(wxT("ar_IQ"));
	languageList.Add(wxT("ar_JO"));
	languageList.Add(wxT("ar_KW"));
	languageList.Add(wxT("ar_LB"));
	languageList.Add(wxT("ar_LY"));
	languageList.Add(wxT("ar_MA"));
	languageList.Add(wxT("ar_OM"));
	languageList.Add(wxT("ar_QA"));
	languageList.Add(wxT("ar_SA"));
	languageList.Add(wxT("ar_SD"));
	languageList.Add(wxT("ar_SY"));
	languageList.Add(wxT("ar_TN"));
	languageList.Add(wxT(""));     
	languageList.Add(wxT("ar_YE"));
	languageList.Add(wxT("hy"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("ay"));   
	languageList.Add(wxT("az"));   
	languageList.Add(wxT("az"));   
	languageList.Add(wxT("az"));   
	languageList.Add(wxT("ba"));   
	languageList.Add(wxT("eu_ES"));
	languageList.Add(wxT("be_BY"));
	languageList.Add(wxT("bn"));   
	languageList.Add(wxT("dz"));   
	languageList.Add(wxT("bh"));   
	languageList.Add(wxT("bi"));   
	languageList.Add(wxT("br"));   
	languageList.Add(wxT("bg_BG"));
	languageList.Add(wxT("my"));   
	languageList.Add(wxT("km"));   
	languageList.Add(wxT("ca_ES"));
	languageList.Add(wxT("zh_TW"));
	languageList.Add(wxT("zh_CN"));
	languageList.Add(wxT("zh_TW"));
	languageList.Add(wxT(""));     
	languageList.Add(wxT(""));     
	languageList.Add(wxT("zh_SG"));
	languageList.Add(wxT("zh_TW"));
	languageList.Add(wxT("co"));   
	languageList.Add(wxT("hr_HR"));
	languageList.Add(wxT("cs_CZ"));
	languageList.Add(wxT("da_DK"));
	languageList.Add(wxT("nl_NL"));
	languageList.Add(wxT("nl_BE"));
	languageList.Add(wxT("en_GB"));
	languageList.Add(wxT("en_GB"));
	languageList.Add(wxT("en_US"));
	languageList.Add(wxT("en_AU"));
	languageList.Add(wxT("en_BZ"));
	languageList.Add(wxT("en_BW"));
	languageList.Add(wxT("en_CA"));
	languageList.Add(wxT("en_CB"));
	languageList.Add(wxT("en_DK"));
	languageList.Add(wxT("en_IE"));
	languageList.Add(wxT("en_JM"));
	languageList.Add(wxT("en_NZ"));
	languageList.Add(wxT("en_PH"));
	languageList.Add(wxT("en_ZA"));
	languageList.Add(wxT("en_TT"));
	languageList.Add(wxT("en_ZW"));
	languageList.Add(wxT("eo"));   
	languageList.Add(wxT("et_EE"));
	languageList.Add(wxT("fo_FO"));
	languageList.Add(wxT("fa_IR"));
	languageList.Add(wxT("fj"));   
	languageList.Add(wxT("fi_FI"));
	languageList.Add(wxT("fr_FR"));
	languageList.Add(wxT("fr_BE"));
	languageList.Add(wxT("fr_CA"));
	languageList.Add(wxT("fr_LU"));
	languageList.Add(wxT("fr_MC"));
	languageList.Add(wxT("fr_CH"));
	languageList.Add(wxT("fy"));   
	languageList.Add(wxT("gl_ES"));
	languageList.Add(wxT("ka"));   
	languageList.Add(wxT("de_DE"));
	languageList.Add(wxT("de_AT"));
	languageList.Add(wxT("de_BE"));
	languageList.Add(wxT("de_LI"));
	languageList.Add(wxT("de_LU"));
	languageList.Add(wxT("de_CH"));
	languageList.Add(wxT("el_GR"));
	languageList.Add(wxT("kl_GL"));
	languageList.Add(wxT("gn"));   
	languageList.Add(wxT("gu"));   
	languageList.Add(wxT("ha"));   
	languageList.Add(wxT("he_IL"));
	languageList.Add(wxT("hi_IN"));
	languageList.Add(wxT("hu_HU"));
	languageList.Add(wxT("is_IS"));
	languageList.Add(wxT("id_ID"));
	languageList.Add(wxT("ia"));   
	languageList.Add(wxT("ie"));   
	languageList.Add(wxT("iu"));   
	languageList.Add(wxT("ik"));   
	languageList.Add(wxT("ga_IE"));
	languageList.Add(wxT("it_IT"));
	languageList.Add(wxT("it_CH"));
	languageList.Add(wxT("ja_JP"));
	languageList.Add(wxT("jw"));   
	languageList.Add(wxT("kn"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT(""));     
	languageList.Add(wxT("kk"));   
	languageList.Add(wxT("kw_GB"));
	languageList.Add(wxT("rw"));   
	languageList.Add(wxT("ky"));   
	languageList.Add(wxT("rn"));   
	languageList.Add(wxT("c"));    
	languageList.Add(wxT("ko_KR"));
	languageList.Add(wxT("ku"));   
	languageList.Add(wxT("lo"));   
	languageList.Add(wxT("la"));   
	languageList.Add(wxT("lv_LV"));
	languageList.Add(wxT("ln"));   
	languageList.Add(wxT("lt_LT"));
	languageList.Add(wxT("mk_MK"));
	languageList.Add(wxT("mg"));   
	languageList.Add(wxT("ms_MY"));
	languageList.Add(wxT("ml"));   
	languageList.Add(wxT("ms_BN"));
	languageList.Add(wxT("ms_MY"));
	languageList.Add(wxT("mt_MT"));
	languageList.Add(wxT(""));     
	languageList.Add(wxT("mi"));   
	languageList.Add(wxT("mr_IN"));
	languageList.Add(wxT("mo"));   
	languageList.Add(wxT("mn"));   
	languageList.Add(wxT("na"));   
	languageList.Add(wxT("ne"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("nb_NO"));
	languageList.Add(wxT("nn_NO"));
	languageList.Add(wxT("oc"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("om"));   
	languageList.Add(wxT("ps"));   
	languageList.Add(wxT("pl_PL"));
	languageList.Add(wxT("pt_PT"));
	languageList.Add(wxT("pt_BR"));
	languageList.Add(wxT("pa"));   
	languageList.Add(wxT("qu"));   
	languageList.Add(wxT("rm"));   
	languageList.Add(wxT("ro_RO"));
	languageList.Add(wxT("ru_RU"));
	languageList.Add(wxT("ru_UA"));
	languageList.Add(wxT("sm"));   
	languageList.Add(wxT("sg"));   
	languageList.Add(wxT("sa"));   
	languageList.Add(wxT("gd"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("sr_YU"));
	languageList.Add(wxT("sr_YU"));
	languageList.Add(wxT("sh"));   
	languageList.Add(wxT("st"));   
	languageList.Add(wxT("tn"));   
	languageList.Add(wxT("sn"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("si"));   
	languageList.Add(wxT("ss"));   
	languageList.Add(wxT("sk_SK"));
	languageList.Add(wxT("sl_SI"));
	languageList.Add(wxT("so"));   
	languageList.Add(wxT("es_ES"));
	languageList.Add(wxT("es_AR"));
	languageList.Add(wxT("es_BO"));
	languageList.Add(wxT("es_CL"));
	languageList.Add(wxT("es_CO"));
	languageList.Add(wxT("es_CR"));
	languageList.Add(wxT("es_DO"));
	languageList.Add(wxT("es_EC"));
	languageList.Add(wxT("es_SV"));
	languageList.Add(wxT("es_GT"));
	languageList.Add(wxT("es_HN"));
	languageList.Add(wxT("es_MX"));
	languageList.Add(wxT("es_ES"));
	languageList.Add(wxT("es_NI"));
	languageList.Add(wxT("es_PA"));
	languageList.Add(wxT("es_PY"));
	languageList.Add(wxT("es_PE"));
	languageList.Add(wxT("es_PR"));
	languageList.Add(wxT("es_UY"));
	languageList.Add(wxT("es_US"));
	languageList.Add(wxT("es_VE"));
	languageList.Add(wxT("su"));   
	languageList.Add(wxT("sw_KE"));
	languageList.Add(wxT("sv_SE"));
	languageList.Add(wxT("sv_FI"));
	languageList.Add(wxT("tl_PH"));
	languageList.Add(wxT("tg"));   
	languageList.Add(wxT("ta"));   
	languageList.Add(wxT("tt"));   
	languageList.Add(wxT("te"));   
	languageList.Add(wxT("th_TH"));
	languageList.Add(wxT("bo"));   
	languageList.Add(wxT("ti"));   
	languageList.Add(wxT("to"));   
	languageList.Add(wxT("ts"));   
	languageList.Add(wxT("tr_TR"));
	languageList.Add(wxT("tk"));   
	languageList.Add(wxT("tw"));   
	languageList.Add(wxT("ug"));   
	languageList.Add(wxT("uk_UA"));
	languageList.Add(wxT("ur"));   
	languageList.Add(wxT(""));     
	languageList.Add(wxT("ur_PK"));
	languageList.Add(wxT("uz"));   
	languageList.Add(wxT("uz"));   
	languageList.Add(wxT("uz"));   
	languageList.Add(wxT("vi_VN"));
	languageList.Add(wxT("vo"));   
	languageList.Add(wxT("cy"));   
	languageList.Add(wxT("wo"));   
	languageList.Add(wxT("xh"));   
	languageList.Add(wxT("yi"));   
	languageList.Add(wxT("yo"));   
	languageList.Add(wxT("za"));   
	languageList.Add(wxT("zu"));   

	return languageList;
}

/** Gets the languages for which there is a catalog available.
* @return An array of integers, which are indices to the array of supported language names.
*/
wxArrayInt IPTVClientApp::GetAvailableLanguageCodes()
{
	wxArrayString languageList = GetCanonicalNamesList();
	wxArrayInt availableLanguages;

	wxFileSystem fs;

	for(unsigned int i = 0; i < languageList.GetCount(); i++)
	{
		wxString searchPath;
		searchPath << GetAppPath() << wxT("/") << LOCALE_DIRECTORY_NAME << wxT("/") << languageList[i];

		wxString path;

		wxString fileName;
		fileName << LOCALE_FILE_NAME << wxT(".mo");

		if(fs.FindFileInPath(&path, searchPath, fileName))
		{
			availableLanguages.Add(i);
		}
	}

	return availableLanguages;
}

/** Gets the current application language code.
*
*/
int IPTVClientApp::GetLanguageCode()
{	
	if(m_locale != NULL)
		return m_locale->GetLanguage();
	else
		return NO_LANGUAGE_SELECTED;
}

/** Gets the name of the specified language. 
* This method returns the full name of the specified language (e.g. "English").
* @param[in] language	The language index. Negative values mean the current language. (default: -1) 
* @return The specified language's canonical name. Returns the empty string on error.
*/
wxString IPTVClientApp::GetLanguageName(int language)
{
	if(m_locale != NULL)
	{
		if(language < 0)
			return m_locale->GetLanguageName(GetLanguageCode());
		
		return m_locale->GetLanguageName(language);
	}
	else
		return wxEmptyString;
}

/** Gets the current application language canonical name.
* @return The current application language's canonical name (e.g. "pt_BR"). Returns the empty string on error.
*/
wxString IPTVClientApp::GetLanguageCanonicalName()
{
	if(m_locale != NULL)
		return m_locale->GetCanonicalName();
	else
		return wxEmptyString;
}

/** Returns the path of the application executable file.
* @return The path of the executable file.
*/
wxString IPTVClientApp::GetAppPath()
{
	wxStandardPaths stdpaths;
	wxFileName fn(stdpaths.GetExecutablePath());
	return fn.GetPath();
}

/** Returns the name of the application executable file.
* @return The executable's file name.
*/
wxString IPTVClientApp::GetAppFileName()
{
	wxStandardPaths stdpaths;
	wxFileName fn(stdpaths.GetExecutablePath());
	return fn.GetFullName();
}

/** Sets the skin used.
 * @param[in] skinSelected	The selected skin.
 */
void IPTVClientApp::SetSkinSelected(wxString skinSelected)
{
	m_skinSelected = skinSelected;
}

/** Gets the skin used.
 * @return The selected skin.
 */
wxString IPTVClientApp::GetSkinSelected(){return m_skinSelected;}

/** Sets the language used.
 * @param[in] The selected language.
 */
void IPTVClientApp::SetLanguageSelected(wxString languageSelected)
{
	m_languageSelected = languageSelected;
}

/** Return current app configuration.
 *
 * This method returns a const-reference to the application's
 * "running configuration", i.e. all the parameters it is currently using.
 * More details in the documentation for AppConfig.
 *
 * @return The current AppConfig.
 */
const AppConfig &IPTVClientApp::GetAppConfig() const
{
	return m_appConfig;
}

/** Set app configuration.
 *
 * Sets the app's current running config.
 * @param[in] config	The config. to set.
 */
void IPTVClientApp::SetAppConfig(const AppConfig &config)
{
	m_appConfig = config;
}

/** Returns the current user.
 *
 * @return A pointer to the current User.
 */
User *IPTVClientApp::GetCurrentUser()
{
	return m_currentUser;
}

/** Returns the current user as found in the user list for the current channel.
 *
 * @return A pointer to the current User. Will be NULL if not in CONFERENCE mode.
 */
const ChannelUser *IPTVClientApp::GetCurrentChannelUser() const
{
	if(!m_currentChannel)
		return NULL;
	return m_currentChannel->GetUserList().FindUserByName(m_currentUser->GetNickName());
}

/** Returns the current channel.
 * @return A pointer to the current Channel, or NULL if not in CONFERENCE mode.
 */
const Channel *IPTVClientApp::GetCurrentChannel() const
{
	return m_currentChannel;
}

/** Returns the current channel list.
 * @return A reference to the current channel list.
 */
ChannelList &IPTVClientApp::GetChannelList()
{
	// TODO: Give a proper implementation.
	return m_channelList;
}

/** Returns the session time.
 * @return current session time as a wxTimeSpan.
 */
const wxTimeSpan &IPTVClientApp::GetSessionTime() const
{
	return m_sessionTime;
}

/** Returns the users list for the current channel.
 * @return A pointer to the users list for the current channel.
 */
UserList *IPTVClientApp::GetUserList()
{
	// TODO: Give a proper implementation.
	if(!m_currentChannel)
		return NULL;
	return &(m_currentChannel->GetUserList());
}

/** Gets the language used.
 * @return The selected language.
 */
wxString IPTVClientApp::GetLanguageSelected()
{
	return m_languageSelected;
}

/** Returns the current transmission count.
 *
 * @return The active transmission count.
 * @remarks Please note that this method only takes into account the currently active incoming
 * media transmissions, and does NOT count outgoing transmission, nor Application Sharing.
 */
unsigned IPTVClientApp::GetTransmissionCount()
{
	return m_transmissionManager.GetActiveTransmissionCount();
}

/** Kernel notifications
 *
 * This implements Kernel's OnKernelNotify method. This is where messages from Kernel are handled.
 * @return Currently always true.
 * @remarks WATCH OUT!! This function may (and does) get called from a different thread!!
 * Avoid using GUI functions directly here. Instead, post wxCommandEvents to m_skin
 * and let it handle them (it does it in its own thread).
 */
bool IPTVClientApp::OnKernelNotify(KernelParam &param)
{
	switch(param.m_code)
	{
	// Received upon contacting host. Parameter class: KernelParam
	case KNC_IRM_CONNECTED_HOST:	
		SendCommandEvent(iptvEVT_CONNECTED_HOST);
		break;
	// Received upon server connect. Parameter class: KernelParam
	case KNC_IRM_CONNECTED_CHAT:				
		SendCommandEvent(iptvEVT_CONNECTED_CHAT);
		break;
	// Received upon server disconnect. Parameter class: KernelParam
	case KNC_IRM_DISCONNECTED:
	{
		SendCommandEvent(iptvEVT_DISCONNECTED, WX_FROM_CSTR(param.m_kernelMessage.c_str()));
	}
	break;
	// Received upon successful registered user login. Parameter class: KernelParam
	case KNC_IRM_AUTHENTICATED:				
		SendCommandEvent(iptvEVT_AUTHENTICATED);
		break;
	// Received upon failed registered user login. Parameter class: KernelParam
	case KNC_IRM_AUTHENTICATION_ERROR:
		SendCommandEvent(iptvEVT_AUTHENTICATION_ERROR);
		break;
	// Received once per item in a channel listing. Parameter class: KernelChannelParam
	case KNC_IRM_CHANNEL_QUERY_ITEM:			 
	// Use the void * argument of SendCommandEvent to post the event carrying the data with it
	{
		KernelChannelParam &msgParam = (KernelChannelParam &)param;
		// Allocate a new ChannelInfo structure and fill it up with item data
		Channel *channel = new Channel;
		channel->SetName(WX_FROM_CSTR(msgParam.m_channelName));
		channel->SetTopic(WX_FROM_CSTR(msgParam.m_channelTopic));
		channel->SetUserCount(msgParam.m_userCount);
		// Add it to the channel list.
		SendCommandEvent(iptvEVT_CHANNEL_QUERY_ITEM, wxEmptyString, (void *)channel);
	}
	break;
	// Received on end of channel listing. Parameter class: KernelParam 
	case KNC_IRM_CHANNEL_QUERY_END:			
		SendCommandEvent(iptvEVT_CHANNEL_QUERY_END);
		break;
	// Received once per item in an user listing. Parameter class: KernelUserParam
	case KNC_IRM_USER_QUERY_ITEM:
	{
		KernelUserParam &msgParam = (KernelUserParam &)param;
		// Allocate a new User structure and fill it up with item data.
		ChannelUser *user = new ChannelUser;
		user->SetNickName(wxString(msgParam.m_userName, wxConvUTF8));
		// Send event
		SendCommandEvent(iptvEVT_USER_QUERY_ITEM, wxString(msgParam.m_channelName, wxConvUTF8), (void *)user);
	}
	break;
	// Received on end of user listing. Parameter class: KernelUserParam
	case KNC_IRM_USER_QUERY_END:
	{
		KernelUserParam &msgParam = (KernelUserParam &)param;
		ChannelUser *user = new ChannelUser;
		user->SetNickName(wxString(msgParam.m_userName, wxConvUTF8));
		// Add it to the channel list.
		SendCommandEvent(iptvEVT_USER_QUERY_END, wxString(msgParam.m_channelName, wxConvUTF8), (void *)user, m_kernel.GetChannelJoinState());
	}
	break;
	// Received when an user joins the current channel. Parameter class: KernelUserParam
	case KNC_IRM_CHANNEL_JOIN:
	{
		KernelUserParam &msgParam = (KernelUserParam &)param;
		// Allocate a new User structure and fill it up with data
		ChannelUser *user = new ChannelUser;
		user->SetNickName(wxString(msgParam.m_userName, wxConvUTF8));
		// Send out the event
		SendCommandEvent(iptvEVT_CHANNEL_JOIN, wxString(msgParam.m_channelName, wxConvUTF8), (void *)user, m_kernel.GetChannelJoinState());
	}
	break;
	// Received when an user leaves the current channel. Parameter class: KernelUserParam
	case KNC_IRM_CHANNEL_PART:
	{
		KernelUserParam &msgParam = (KernelUserParam &)param;
		// Allocate a new ChannelUser structure and fill it up with data
		ChannelUser *user = new ChannelUser;
		user->SetNickName(wxString(msgParam.m_userName, wxConvUTF8));
		SendDebugEvent(wxString(wxT("Sent channel part event, user = ")) + user->GetNickName());
		// Send out the event
		SendCommandEvent(iptvEVT_CHANNEL_PART, wxString(msgParam.m_channelName, wxConvUTF8), (void *)user, m_kernel.GetChannelPartState());
	}
	break;
	// These are received on user mode changes. Parameter class: KernelModeParam.
	// ---------------------------------------------------------------------------------------------------------------------------------------
	case KNC_IRM_USER_MODE_INVISIBLE:
		SendIRMUserModeEvent(IRM_MODE_USER_INVISIBLE, (KernelModeParam &)param);
		break;
	case KNC_IRM_USER_MODE_REGISTERED:
		SendIRMUserModeEvent(IRM_MODE_USER_REGISTERED, (KernelModeParam &)param);
		break;
	case KNC_IRM_USER_MODE_CAMERA_ON:
		SendIRMUserModeEvent(IRM_MODE_USER_CAMERA_ON, (KernelModeParam &)param);
		break;
	case KNC_IRM_USER_MODE_MIC_MUTE:
		SendIRMUserModeEvent(IRM_MODE_USER_MIC_MUTE, (KernelModeParam &)param);
		break;
	case KNC_IRM_USER_MODE_AWAY:
		SendIRMUserModeEvent(IRM_MODE_USER_AWAY, (KernelModeParam &)param);
		break;
	//------------------------------ (end user mode notifications) ------------------------------------------------------------------

	// These are channel modes that toggle user flags.
	case KNC_IRM_CHANNEL_MODE_OPERATOR:
		SendIRMChannelUserFlagsEvent(IRM_MODE_CHANNEL_OPERATOR, (KernelModeParam &)param);
		break;
	case KNC_IRM_CHANNEL_MODE_MEDIA_COLLABORATOR:
		SendIRMChannelUserFlagsEvent(IRM_MODE_CHANNEL_MEDIA_COLLABORATOR, (KernelModeParam &)param);
		break;
	// These are not modes at all, but these messages toggle user flags as well. Go figure.
	case KNC_IRM_CHANNEL_VOICE:
	{
		KernelUserParam &userParam = (KernelUserParam &)param;
		KernelModeParam modeParam;
		modeParam.m_code = userParam.m_code;
		modeParam.m_kernelMessage = userParam.m_kernelMessage;
		modeParam.m_target = userParam.m_channelName;
		modeParam.m_parameter = userParam.m_userName;
		modeParam.m_value = true;

		SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE, modeParam);
	}
	break;

	// Voice request messages
	case KNC_IRM_VOICE_REQUEST:
	{
		KernelUserParam &userParam = (KernelUserParam &)param;
		KernelModeParam modeParam;
		modeParam.m_code = userParam.m_code;
		modeParam.m_kernelMessage = userParam.m_kernelMessage;
		modeParam.m_target = userParam.m_channelName;
		modeParam.m_parameter = userParam.m_userName;
		modeParam.m_value = true;

		SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE_REQUEST, modeParam);
	}
	break;
	case KNC_IRM_VOICE_REQUEST_CANCEL:
	{
		KernelUserParam &userParam = (KernelUserParam &)param;
		KernelModeParam modeParam;
		modeParam.m_code = userParam.m_code;
		modeParam.m_kernelMessage = userParam.m_kernelMessage;
		modeParam.m_target = userParam.m_channelName;
		modeParam.m_parameter = userParam.m_userName;
		modeParam.m_value = false;

		SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE_REQUEST, modeParam);
	}
	break;
	case KNC_IRM_VOICE_REQUEST_REMOVE_ALL:
	{
		KernelUserParam &userParam = (KernelUserParam &)param;
		m_kernel.SendMediaVoiceRequestCancel(WX_TO_CSTR(m_currentChannel->GetName()));
		
		// Disable the voice request icon for myself.
		KernelModeParam modeParam;
		modeParam.m_code = userParam.m_code;
		modeParam.m_kernelMessage = userParam.m_kernelMessage;
		modeParam.m_target = userParam.m_channelName;
		modeParam.m_parameter = WX_TO_CSTR(GetCurrentChannelUser()->GetNickName());
		modeParam.m_value = false;
		SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE_REQUEST, modeParam);
	}
	break;

	// Channel mode notifications. These are received when channel modes change. Parameter class: KernelModeParam.
	// ------------------------------------------------------------------------------------------------------------------------------
	case KNC_IRM_CHANNEL_MODE_PRIVATE:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_PRIVATE, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_SECRET:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_SECRET, (KernelModeParam &)param);
		break;
	case KNC_IRM_CHANNEL_MODE_INVITE_ONLY:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_INVITE_ONLY, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_OP_CHANGE_TOPIC:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_OP_ONLY_TOPIC, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_NO_EXT_MSGS, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_MODERATED:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_MODERATED, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_USER_LIMIT:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_USER_LIMIT, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_BAN:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_BAN, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_KEY:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_PASSWORD_PROTECTED, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_BIT_RATE:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_BITRATE, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_AUTO_BITRATE, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_MULTIPLE_TRANSMISSION, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_ONE_AUDIO:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_ONE_AUDIO, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_AUDIO_MUTE:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_AUDIO_MUTE, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_BAND_SHARE_LIMIT:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_BAND_SHARE_LIMIT, (KernelModeParam &)param);
	break;
	case KNC_IRM_CHANNEL_MODE_TRANSMISSION_LIMIT:
		SendIRMChannelModeEvent(IRM_MODE_CHANNEL_TRANSMISSION_LIMIT, (KernelModeParam &)param);
	break;
	// ---------------------------------------- (end channel mode notifications) -----------------------------------------------------

	// Received when the channel user is trying to join is password-protected and user hasn't supplied the correct password
	case KNC_IRM_CHANNEL_PASSWD_INVALID:
	{
		SendCommandEvent(iptvEVT_CHANNEL_BAD_PASSWORD, WX_FROM_CSTR((const char *)param.m_kernelMessage));
	}
	break;
	// Received when an user disconnects from the server while on channel. Parameter class: KernelUserParam
	case KNC_IRM_USER_QUIT:
	{
		KernelUserParam &msgParam = (KernelUserParam &)param;
		// Allocate a new User structure and fill it up with data
		ChannelUser *user = new ChannelUser;
		user->SetNickName(wxString(msgParam.m_userName, wxConvUTF8));
		// Send out the event
		SendCommandEvent(iptvEVT_USER_QUIT, wxString(msgParam.m_channelName, wxConvUTF8), (void *)user);
	}
	break;
	// Received when user receives a private message (remote user speaking directly). Parameter class: KernelMessageParam
	case KNC_IRM_PRIV_MSG:
	{
		KernelMessageParam &msgParam = (KernelMessageParam &)param;
		// allocate a new ChatMessage structure and fill it up with data
		ChatMessage *data = new ChatMessage(WX_FROM_CSTR(msgParam.m_channelName), WX_FROM_CSTR(msgParam.m_sender), WX_FROM_CSTR(msgParam.m_message));
		// Send event
		SendCommandEvent(iptvEVT_PRIV_MSG, wxEmptyString, (void *)data);
	}
	break;
	// Received when user receives a message from the channel ("public chat"). Parameter class: KernelMessageParam
	case KNC_IRM_CHANNEL_PRIV_MSG:
	{
		KernelMessageParam &msgParam = (KernelMessageParam &)param;
		// allocate a new ChatMessage structure and fill it up with data
		ChatMessage *data = new ChatMessage;
		data->SetChannel(WX_FROM_CSTR(msgParam.m_channelName));
		data->SetSender(WX_FROM_CSTR(msgParam.m_sender));
		data->SetMessage(WX_FROM_CSTR(msgParam.m_message));
		// Send event
		SendCommandEvent(iptvEVT_CHANNEL_MSG, wxEmptyString, (void *)data);
	}
	break;
	// Received when a new media transmission starts in CONFERENCE mode. Parameter class: KernelMediaIDParam
	case KNC_NEW_CONFERENCE_TRANSMISSION:
	{
		KernelMediaIDParam &msgParam = (KernelMediaIDParam &)param;
//			SendCommandEvent(iptvEVT_NEW_TRANSMISSION, msgParam.m_mediaID, WX_FROM_CSTR(msgParam.m_userNickName));
		SendCommandEvent(iptvEVT_MEDIA_INFO, msgParam.m_mediaID, WX_FROM_CSTR(msgParam.m_userNickName));
	}
	break;
	// Received when a new media transmission starts. Parameter class: KernelMediaIDParam.
	// NOTE: Treatment of this notification is disabled, because the transmission may only be initiated
	// when we have user name information, and this event may precede the arrival of said information
	// when the application is in MULTICAST mode.
	case KNC_TRCV_NEW_TRANSMISSION:
#if 0
	{
		KernelMediaIDParam msgParam = *(KernelMediaIDParam *)&param;
		SendCommandEvent(iptvEVT_NEW_TRANSMISSION, msgParam.m_mediaID);
	}
#endif
	break;
	// Received periodically, to inform that a video transmission is "alive". Parameter class: KernelMediaIDParam
	case KNC_PRCS_VIDEO_ALIVE:
	{
		KernelMediaIDParam &msgParam = (KernelMediaIDParam &)param;
		Transmission *transm = m_transmissionManager.GetTransmissionByID(msgParam.m_mediaID);
		if(transm)
		{
			SendDebugEvent(wxString::Format(wxT("Video alive, media ID = %ld"), msgParam.m_mediaID));
			SendCommandEvent(iptvEVT_VIDEO_ALIVE, WX_FROM_CSTR(msgParam.m_kernelMessage), (void *)transm);
		}
		else
		{
			SendCommandEvent(iptvEVT_NEW_TRANSMISSION, msgParam.m_mediaID);
		}
		m_timeoutRegister.Register(TR_MEDIA_VIDEO_TIMEOUT, msgParam.m_mediaID, MEDIA_TIMEOUT_INTERVAL);
	}
	break;
	// Received periodically, to inform that an audio transmission is "alive". Parameter class: KernelMediaIDParam
	case KNC_PRCS_AUDIO_ALIVE:
	{
		KernelMediaIDParam &msgParam = (KernelMediaIDParam &)param;
		Transmission *transm = m_transmissionManager.GetTransmissionByID(msgParam.m_mediaID);
		if(transm)
		{
			SendDebugEvent(wxString::Format(wxT("Audio alive, media ID = %ld"), msgParam.m_mediaID));
			SendCommandEvent(iptvEVT_AUDIO_ALIVE, WX_FROM_CSTR(msgParam.m_kernelMessage), (void *)transm);
		}
		else
		{
			SendCommandEvent(iptvEVT_NEW_TRANSMISSION, msgParam.m_mediaID);
		}
		m_timeoutRegister.Register(TR_MEDIA_AUDIO_TIMEOUT, msgParam.m_mediaID, MEDIA_TIMEOUT_INTERVAL);
	}
	break;
	case KNC_TRCV_NOTIFY_PROGRESS:
	{
		KernelMediaProgressParam &msgParam = (KernelMediaProgressParam &)param;
		// m_percent is multiplied by 1000 to improve precision in conversion to integer 
		// (it comes from iptv_kernel as floating-point).
		SendCommandEvent(iptvEVT_MEDIA_PROGRESS, (long)(msgParam.m_percent * 1000));
	}
	break;
	case KNC_FT_RECEIVE_WB_BEGIN:
	{
		KernelFTReceiveBeginParam &beginParam = (KernelFTReceiveBeginParam &)param;

		FTReceptionBeginParameter *data = new FTReceptionBeginParameter();
		data->m_fileName = WX_FROM_CSTR(beginParam.m_fileName);
		data->m_fileSize = beginParam.m_fileSize;
		data->m_mediaId = beginParam.m_mediaId;
		data->m_packetSize = beginParam.m_packetSize;
		data->m_senderNick = WX_FROM_CSTR(beginParam.m_senderNick);
		data->m_tempFilePath = WX_FROM_CSTR(beginParam.m_tempFilePath);

		SendCommandEvent(iptvEVT_FT_RECEIVE_WB_BEGIN, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_RECEIVE_BEGIN:
	{
		KernelFTReceiveBeginParam &beginParam = (KernelFTReceiveBeginParam &)param;

		FTReceptionBeginParameter *data = new FTReceptionBeginParameter();
		data->m_fileName = WX_FROM_CSTR(beginParam.m_fileName);
		data->m_fileSize = beginParam.m_fileSize;
		data->m_mediaId = beginParam.m_mediaId;
		data->m_packetSize = beginParam.m_packetSize;
		data->m_senderNick = WX_FROM_CSTR(beginParam.m_senderNick);
		data->m_tempFilePath = WX_FROM_CSTR(beginParam.m_tempFilePath);

		SendCommandEvent(iptvEVT_FT_RECEIVE_BEGIN, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_RECEIVE_PROGRESS:
	{
		KernelFTProgressParam &progressParam = (KernelFTProgressParam &)param;

		FTProgressParameter *data = new FTProgressParameter();
		data->m_currentPacketIndex = progressParam.m_currentPacketIndex;
		data->m_lastPacketIndex = progressParam.m_lastPacketIndex;

		SendCommandEvent(iptvEVT_FT_RECEIVE_PROGRESS, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_RECEIVE_LOST_PACKET:
	{
		KernelFTLostPacketParam &lostPacketParam = (KernelFTLostPacketParam &)param;

		FTLostPacketParameter *data = new FTLostPacketParameter();
		data->m_lostPackets = lostPacketParam.m_lostPackets;
		
		SendCommandEvent(iptvEVT_FT_RECEIVE_LOST_PACKET, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_RECEIVE_SUCCESS:
	{
		SendCommandEvent(iptvEVT_FT_RECEIVE_SUCCESS);
	}
	break;
	case KNC_FT_RECEIVE_ERROR:
	{
		SendCommandEvent(iptvEVT_FT_RECEIVE_ERROR);
	}
	break;
	case KNC_FT_SEND_BEGIN:
	{
		KernelMediaIDParam &sendBeginParam = (KernelMediaIDParam &)param;

		FTSendBeginParameter *data = new FTSendBeginParameter();
		data->m_mediaId = sendBeginParam.m_mediaID;

		SendCommandEvent(iptvEVT_FT_SEND_BEGIN, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_SEND_PROGRESS:
	{
		KernelFTProgressParam &progressParam = (KernelFTProgressParam &)param;

		FTProgressParameter *data = new FTProgressParameter();
		data->m_currentPacketIndex = progressParam.m_currentPacketIndex;
		data->m_lastPacketIndex = progressParam.m_lastPacketIndex;

		SendCommandEvent(iptvEVT_FT_SEND_PROGRESS, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_SEND_LOST_PACKET:
	{
		KernelFTLostPacketParam &lostPacketParam = (KernelFTLostPacketParam &)param;

		FTLostPacketParameter *data = new FTLostPacketParameter();
		data->m_lostPackets = lostPacketParam.m_lostPackets;
		data->m_lostPacketsTotal = lostPacketParam.m_lostPacketsTotal;
		
		SendCommandEvent(iptvEVT_FT_SEND_LOST_PACKET, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_SEND_EOF:
	{
		SendCommandEvent(iptvEVT_FT_SEND_EOF);
	}
	break;
	case KNC_FT_SEND_SUCCESS:
	{
		SendCommandEvent(iptvEVT_FT_SEND_SUCCESS);
	}
	break;
	case KNC_FT_SEND_ERROR:
	{
		SendCommandEvent(iptvEVT_FT_SEND_ERROR);
	}
	break;
	case KNC_FT_SEND_RECEIVER_NEW:
	{
		KernelMediaIDParam &kParam = (KernelMediaIDParam &)param;

		FTSendMediaIdParameter *data = new FTSendMediaIdParameter();
		data->m_mediaId = kParam.m_mediaID;

		SendCommandEvent(iptvEVT_FT_SEND_RECEIVER_NEW, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_SEND_RECEIVER_CANCEL:
	{
		KernelMediaIDParam &kParam = (KernelMediaIDParam &)param;

		FTSendMediaIdParameter *data = new FTSendMediaIdParameter();
		data->m_mediaId = kParam.m_mediaID;

		SendCommandEvent(iptvEVT_FT_SEND_RECEIVER_CANCEL, wxEmptyString, (void *)data);
	}
	break;
	case KNC_FT_SEND_RECEIVER_END:
	{
		KernelMediaIDParam &kParam = (KernelMediaIDParam &)param;

		FTSendMediaIdParameter *data = new FTSendMediaIdParameter();
		data->m_mediaId = kParam.m_mediaID;

		SendCommandEvent(iptvEVT_FT_SEND_RECEIVER_END, wxEmptyString, (void *)data);
	}
	break;
	//Movie playlist notifications
	case KNC_MOVIE_SEND_BEGIN:
	{
		KernelMediaIDParam &kParam = (KernelMediaIDParam &)param;
		SendCommandEvent(iptvEVT_MOVIE_SEND_BEGIN, kParam.m_mediaID, WX_FROM_CSTR(kParam.m_channelName));
	}
	break;
	case KNC_MOVIE_SEND_EOF:
	{
		KernelMediaIDParam &kParam = (KernelMediaIDParam &)param;
		SendCommandEvent(iptvEVT_MOVIE_SEND_EOF, kParam.m_mediaID, WX_FROM_CSTR(kParam.m_channelName));
	}
	break;
	// Poll notifications. All of them take KernelPollParam as parameter.
	// ------------------------------------------------------------------
	case KNC_POLL_BEGIN:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			ResetActivePoll();
			m_pollData.channel = pollParam.m_channelName;
			m_pollData.pollOwner = pollParam.m_userName;
		}
	}
	break;
	case KNC_POLL_QUESTION:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			if(m_pollData.pollOwner == pollParam.m_userName)
			{
				m_pollData.question = pollParam.m_text;
			}
		}
	}
	break;
	case KNC_POLL_OPTION:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			if(m_pollData.pollOwner == pollParam.m_userName)
			{
				PollAnswer pollAnswer;
				pollAnswer.text = pollParam.m_text;
				pollAnswer.index = pollParam.m_index;
				pollAnswer.answerCount = 0;
				m_pollData.answerList.push_back(pollAnswer);
			}
		}
	}
	break;
	case KNC_POLL_END:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			m_pollData.answerList.sort();
			SendCommandEvent(iptvEVT_POLL_END, WX_FROM_CSTR(pollParam.m_userName));
		}
	}
	break;
	case KNC_POLL_ANSWER:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		// NOTE: Commented out because Kernel currently doesn't return channel name.
		// TODO: Verify if this is correct behavior, and wipe out this code in affirmative case
		// wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		// if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		// {
		// 	SendCommandEvent(iptvEVT_POLL_ANSWER, pollParam.m_index, WX_FROM_CSTR(pollParam.m_userName));
		// }
		SendCommandEvent(iptvEVT_POLL_ANSWER, pollParam.m_index, WX_FROM_CSTR(pollParam.m_userName));
	}
	break;
	case KNC_POLL_STATS_BEGIN:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			wxString msg = wxString(wxT("Poll stats: from ")) << WX_FROM_CSTR(pollParam.m_userName) << wxT(" for channel ") << WX_FROM_CSTR(pollParam.m_channelName);
			SendDebugEvent(msg);
			ResetPollIntermediateData();
			m_pollData.channel = pollParam.m_channelName;
			m_pollData.pollOwner = pollParam.m_userName;
		}
	}
	break;
	case KNC_POLL_STATS_QUESTION:	// duplicated from KNC_POLL_QUESTION, left explicitly here
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			if(m_pollData.pollOwner == pollParam.m_userName)
			{
				wxString msg = wxString(wxT("Poll stats, question: ")) << WX_FROM_CSTR(pollParam.m_text);
				SendDebugEvent(msg);
				m_pollData.question = pollParam.m_text;
			}
		}
	}
	break;
	case KNC_POLL_STATS_OPTION:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			if(m_pollData.pollOwner == pollParam.m_userName)
			{
				PollAnswer pollAnswer;
				pollAnswer.text = pollParam.m_text;
				pollAnswer.index = pollParam.m_index;
				pollAnswer.answerCount = pollParam.m_answerCount;
				wxString msg = wxString(wxT("Poll stats option: ")) << pollAnswer.index << wxT(") ") << WX_FROM_CSTR(pollAnswer.text) 
					<< wxT(" (") << pollAnswer.answerCount << wxT(" votes)");
				SendDebugEvent(msg);
				m_pollData.answerList.push_back(pollAnswer);
			}
		}
	}
	break;
	case KNC_POLL_STATS_END:
	{
		KernelPollParam &pollParam = (KernelPollParam &)param;
		wxString channelName = WX_FROM_CSTR(pollParam.m_channelName);
		if(m_currentChannel && (m_currentChannel->GetName() == channelName))
		{
			SendDebugEvent(wxT("Poll stats end."));
			m_pollData.answerList.sort();
			SendCommandEvent(iptvEVT_POLL_STATS_END, WX_FROM_CSTR(pollParam.m_userName));
		}
	}
	break;
	// ------------------------------------------------------------------------------------------ //
	// ------------------------------ Application Sharing notifications ------------------------------//
	// A.S. reception initiated. Parameter class: KernelMediaIDParam
	case KNC_AS_RECEIVE_BEGIN:
	{
		KernelMediaIDParam &kernelParam = (KernelMediaIDParam &)param;
		SendDebugEvent(wxString(wxT("Application Sharing reception begin, media ID = ")) << kernelParam.m_mediaID);
		SendCommandEvent(iptvEVT_AS_RECEIVE_BEGIN, kernelParam.m_mediaID, WX_FROM_CSTR(kernelParam.m_userNickName));
	}
	break;
	// A.S. reception ended. Parameter class: KernelMediaIDParam
	case KNC_AS_RECEIVE_END:
	{
		KernelMediaIDParam &kernelParam = (KernelMediaIDParam &)param;
		SendDebugEvent(wxString(wxT("Application Sharing reception end, media ID = ")) << kernelParam.m_mediaID);
		SendCommandEvent(iptvEVT_AS_RECEIVE_END, kernelParam.m_mediaID, WX_FROM_CSTR(kernelParam.m_userNickName));	
	}
	break;
	// A.S. got a new frame. Parameter class: KernelASFrameParam
	case KNC_AS_RECEIVE_NEW_FRAME:
	{
		KernelASFrameParam *dataParam = new KernelASFrameParam((KernelASFrameParam &)param);
		SendCommandEvent(iptvEVT_AS_RECEIVE_NEW_FRAME, wxEmptyString, dataParam);	
	}
	break;
	// White Board message. Parameter class: KernelWBMessageParam
	case KNC_WB_MESSAGE:
	{
		KernelWBMessageParam &kernelParam = (KernelWBMessageParam &)param;
		SendDebugEvent(wxString(wxT("Whiteboard message, user = ")) << WX_FROM_CSTR(kernelParam.m_userName.c_str()));

		WBMessage *data = new WBMessage();
		data->channel = WX_FROM_CSTR(kernelParam.m_channelName.c_str());
		data->user = WX_FROM_CSTR(kernelParam.m_userName.c_str());
		data->data = WX_FROM_CSTR(kernelParam.m_data.c_str());

		SendCommandEvent(iptvEVT_WB_WBOARD_CHANNEL, wxEmptyString, (void *)data);
	}
	break;
	// ---------------------------- END Application Sharing notifications ------------------------------ //
	case KNC_DEBUG:
	{
		SendDebugEvent(WX_FROM_CSTR(param.m_kernelMessage));
	}
	break;
	case KNC_CAPTURE_VIDEO_DATA:
	{
		KernelVideoCaptureDataParam *dataParam = new KernelVideoCaptureDataParam((KernelVideoCaptureDataParam &)param);
		SendCommandEvent(iptvEVT_CAPTURE_NEW_FRAME, wxEmptyString, dataParam);
	}
	break;
	case KNC_LOCALE_SET_ENGLISH:
	{
		//SelectLanguage(wxLANGUAGE_ENGLISH, true);
		// Wait
	}
	break;
	case KNC_LOCALE_RESTORE:
	{
		//SelectLanguage(m_selectedLanguageIndex);
	}
	break;
	default:
	break;		// should throw here?!
	}
	return true;
}

// Custom App event methods
/** Sends a wxCommandEvent to App.
 * @param[in] type		Event type.
 * @param[in] kernelMessage	Kernel message string.
 * @param[in] clientData	Pointer to a block of data defined by caller.
 * @param[in] kernelState	Intended to carry the current state of the Kernel state machine upon calling.
 */
void IPTVClientApp::SendCommandEvent(const wxEventType &type, wxString kernelMessage, void *clientData, int kernelState)
{
	wxCommandEvent event(type, wxID_ANY);
	event.SetEventObject(this);
	if(kernelMessage != wxEmptyString)
		event.SetString(kernelMessage);
	if(clientData)
		event.SetClientData(clientData);
	if(kernelState >= 0)
		event.SetInt(kernelState);
	wxPostEvent(this, event);
}

/** Sends a wxCommandEvent to App.
 * @param[in] type		Event type.
 * @param[in] extraLong		A numeric variable to carry along with the event.
 * @param[in] kernelMessage	Kernel message string.
 */
void IPTVClientApp::SendCommandEvent(const wxEventType &type, long extraLong, wxString kernelMessage)
{
	wxCommandEvent event(type, wxID_ANY);
	event.SetEventObject(this);
	event.SetExtraLong(extraLong);
	event.SetString(kernelMessage);
	wxPostEvent(this, event);
}

/** Dispatches IRM user mode events.
 *
 * This method sends to App wxCommandEvents with type iptvEVT_USER_MODE_ON or iptvEVT_USER_MODE_OFF
 * depending on the IRM mode message coming from Kernel.
 *
 * @param[in] modeID	Mode ID.
 * @param[in] param	Kernel mode parameters.
 */
void IPTVClientApp::SendIRMUserModeEvent(int modeID, const KernelModeParam &param)
{
	Mode *mode = new Mode(modeID, WX_FROM_CSTR(param.m_parameter));
	if(param.m_value)
		SendCommandEvent(iptvEVT_USER_MODE_ON, WX_FROM_CSTR(param.m_target), (void *)mode);
	else
		SendCommandEvent(iptvEVT_USER_MODE_OFF, WX_FROM_CSTR(param.m_target), (void *)mode);
}

/** Dispatches IRM channel mode events.
 *
 * This method sends to App wxCommandEvents with type iptvEVT_CHANNEL_MODE_ON or iptvEVT_CHANNEL_MODE_OFF
 * depending on the IRM mode message coming from Kernel.
 *
 * @param[in] modeID	Mode ID.
 * @param[in] param	Kernel mode parameters.
 */
void IPTVClientApp::SendIRMChannelModeEvent(int modeID, const KernelModeParam &param)
{
	Mode *mode = new Mode(modeID, WX_FROM_CSTR(param.m_parameter));
	if(param.m_value)
		SendCommandEvent(iptvEVT_CHANNEL_MODE_ON, WX_FROM_CSTR(param.m_target), (void *)mode);
	else
		SendCommandEvent(iptvEVT_CHANNEL_MODE_OFF, WX_FROM_CSTR(param.m_target), (void *)mode);
}

/** Dispatches IRM user flag events.
 *
 * This method sends to App wxCommandEvents with type iptvEVT_USER_FLAGS_ON or iptvEVT_CHANNEL_USER_FLAGS_OFF
 * depending on the IRM user flags message coming from Kernel. "User flags" events are dispatched when
 * channel modes are activated meaning to set certain flags to users, such as media voice.
 *
 * @param[in] modeID	Mode ID.
 * @param[in] param	Kernel mode parameters.
 */
void IPTVClientApp::SendIRMChannelUserFlagsEvent(int modeID, const KernelModeParam &param)
{
	Mode *mode = new Mode(modeID, WX_FROM_CSTR(param.m_parameter));
	if(param.m_value)
		SendCommandEvent(iptvEVT_CHANNEL_USER_FLAGS_ON, WX_FROM_CSTR(param.m_target), (void *)mode);
	else
		SendCommandEvent(iptvEVT_CHANNEL_USER_FLAGS_OFF, WX_FROM_CSTR(param.m_target), (void *)mode);
}

// App->kernel methods
//--------------------

/** Gets the temp directory.
 * @return The temp directory.
 * @remarks in Linux/GTK+ builds, it defaults to "/var/tmp". On Win32 it's the value of the TEMP environment variable.
 */
wxString IPTVClientApp::GetTempDirectory()
{
	wxString tempDirectory;
#ifdef WIN32
			char *winTempDirectory;
			winTempDirectory = getenv("TEMP");
			if(winTempDirectory != NULL)
			{
				tempDirectory = WX_FROM_CSTR(winTempDirectory);
				tempDirectory += wxT("\\");
			}
#endif
#ifdef _LINUX
			tempDirectory = wxT("/var/tmp/");
#endif

	return tempDirectory;
}

/** Initialize multicast.
 *
 * This starts listening on a multicast transmission.
 * @param[in] address	IPv4 multicast address.
 * @param[in] port	Number of the port to listen on.
 */
void IPTVClientApp::InitMulticast(const wxString &address, unsigned int port)
{
	EnableTransmissions();
	m_kernel.InitializeMulticast((const char *)address.mb_str(), port);
}

/** Start file playback.
 *
 * Starts playing back a media conference file.
 * @param[in] filename	File name.
 */
void IPTVClientApp::InitFilePlayback(const wxString &filename)
{
	EnableTransmissions();
	m_kernel.InitializeFilePlayback(WX_TO_CSTR(filename));
}

/** Seek while in file playback.
 *
 * Seek to a position in file playback mode.
 * @param[in] percent	The percentage of the file to seek to.
 */
void IPTVClientApp::FileSeek(float percent)
{
	if(m_appConfig.GetMode() == APP_MODE_FILE)
		m_kernel.SeekFilePlayback(percent);
}

/** Play while in file playback.
 */
void IPTVClientApp::FilePlay()
{
	if(m_appConfig.GetMode() == APP_MODE_FILE)
		m_kernel.PlayFilePlayback();
}

/** Pause while in file playback.
 */
void IPTVClientApp::FilePause()
{
	if(m_appConfig.GetMode() == APP_MODE_FILE)
		m_kernel.PauseFilePlayback();
}

/** Request connection to server.
 *
 * This requests a connection to the IRM server.
 * @param[in] server		Server name. May be a DNS name or IPv4 address.
 * @param[in] serverPass	Server password, for servers that require one.
 * @param[in] user		User nickname.
 * @param[in] pass		User password.
 * @param[in] realName		User real name.
 * @param[in] email		User email address.
 */
void IPTVClientApp::RequestConnection(const wxString &server, const wxString &serverPass, 
											 const wxString &user, const wxString &pass,
											 const wxString &realName, const wxString &email)
{
	m_kernel.RequestConnection(WX_TO_CSTR(server), UNICAST_PORT,
		WX_TO_CSTR(user), WX_TO_CSTR(pass), WX_TO_CSTR(realName), WX_TO_CSTR(email),WX_TO_CSTR(serverPass));
	if(m_currentUser) delete m_currentUser;
	m_currentUser = new User(user);
}

/** Close connection to server.
 *
 * This closes the connection to the IRM server, leaving the CONFERENCE.
 */
void IPTVClientApp::CloseConnection()
{
	// Only attempt to close connection if we're not already disconnected
	if(m_kernel.GetConnectionState() != IRMCS_DISCONNECTED)
		m_kernel.Disconnect();
	// User is no longer connected, so clear it
	if(m_currentUser) delete m_currentUser;
	m_currentUser = NULL;
}

/** Ask for the channel list.
 */
void IPTVClientApp::RequestChannelList()
{
	SendDebugEvent(wxT("Requesting channel list."));
	ClearChannelList();
	m_kernel.QueryChannel();
}

/** Attempt to join a channel.
 * @param[in] channel	Channel name.
 * @param[in] password	Channel password, for channels that require one.
 * @remarks This is for use in CONFERENCE mode, after connection to the server.
 */
void IPTVClientApp::JoinChannel(const wxString &channel, const wxString &password)
{
	SendDebugEvent(wxString(wxT("Attempting to join channel ")) << channel << wxT("..."));
	m_kernel.RequestChannelJoin(WX_TO_CSTR(channel), WX_TO_CSTR(password));
}

/** Leave channel.
 *
 * Call this function to leave the current channel.
 *
 * @param[in] channel	Channel name.
 */
void IPTVClientApp::LeaveChannel(const wxString &channel)
{
	SendDebugEvent(wxString(wxT("Leaving channel ")) << channel << wxT("..."));
	m_kernel.RequestChannelPart(WX_TO_CSTR(channel));
}

/** Send message to an user in channel.
 * @param[in] user	User to send message to.
 * @param[in] message	Message text.
 */
void IPTVClientApp::SendPrivateMessage(const wxString &user, const wxString &message)
{
	m_kernel.SendPrivateMessage(WX_TO_CSTR(user), WX_TO_CSTR(message));
}

/** Send message to all users in channel.
 * @param[in] channel	Channel to send message to.
 * @param[in] message	Message text.
 */
void IPTVClientApp::SendChannelMessage(const wxString &channel, const wxString &message)
{
	m_kernel.SendChannelPrivateMessage(WX_TO_CSTR(channel), WX_TO_CSTR(message));
}

/** Gets a video frame from incoming media transmission.
 *
 * Ask for a video frame of the media identified by 'id'. Image data is stored in 
 * location pointed to by *data, and its dimensions in *width and *height.
 *
 * @param[in]  id	Media ID
 * @param[out] data	Frame data
 * @param[out] width	Image width (pixels)
 * @param[out] height	Image height (pixels)
 * @return true on success, false on failure
 */
bool IPTVClientApp::GetVideoFrame(unsigned long id, unsigned char **data, unsigned long *width, unsigned long *height)
{
	return m_kernel.GetColaborationVideoFrame(id, data, width, height);
}

/** Initializes conference transmission for the given channel.
 * @param[in] channel	Conference channel.
 * @return true on success, false on failure
 */
bool IPTVClientApp::InitializeConference(wxString channel)
{
	return m_kernel.InitializeConference(WX_TO_CSTR(channel));
}

/**  De-initializes the conference transmission.
 */
void IPTVClientApp::TerminateAllConferences()
{
	DisableTransmissions();	// This disables treatment of "alive" notifications
	m_transmissionManager.TerminateAllTransmissions();
	m_kernel.TerminateAllMediaTransmissions();
}

/** Register a VideoSink to receive capture frames.
 * @param[in] sink	VideoSink to register.
 */
void IPTVClientApp::RegisterCaptureVideoSink(VideoSink *sink)
{
	m_camVideoSource.RegisterVideoSink(sink);
}

/** Unregister a VideoSink registered for capture.
 * @param[in] sink	VideoSink to unregister.
 */
void IPTVClientApp::UnregisterCaptureVideoSink(VideoSink *sink)
{
	m_camVideoSource.UnregisterVideoSink(sink);
}

/** Unregister all capture video sinks.
 */
void IPTVClientApp::UnregisterAllCaptureVideoSinks()
{
	m_camVideoSource.UnregisterAllVideoSinks();
}

/** This initializes the whole video capture structure, and is exposed to Skin through AppInterface.
 * @return true on success, false otherwise
 */
bool IPTVClientApp::InitializeCapture()
{
	// Initializes capture through CamVideoSource.
	if(!m_camVideoSource.IsOk())
	{
		// get device ID from configuration
		long index = 0;
		wxString videoDevice = m_settingsManager->GetVideoDevice();
		if(videoDevice != wxEmptyString)
			videoDevice.ToLong(&index);

		m_camVideoSource.SetCamID(index);
		return m_camVideoSource.Init();
	}
	return true;
}

/** Initialize the capture device.
 * @param[in] deviceId	Device ID.
 * @return true on success, false on failure
 */
bool IPTVClientApp::InitializeCaptureDevice(int deviceId)
{
	return m_kernel.InitializeVideoCapture(deviceId);
}

/** Stops the whole capture structure. This method is exposed to Skin through AppInterface.
 */
void IPTVClientApp::ReleaseCapture()
{
	m_camVideoSource.Close();
}

/** Releases the capture device.
 */
void IPTVClientApp::ReleaseCaptureDevice()
{
	m_kernel.ReleaseVideoCapture();
}

/** Checks whether or not capture is running.
 * @return true if capture is running, false otherwise
 */
bool IPTVClientApp::IsCaptureEnabled() const
{
	return m_camVideoSource.IsRunning();
}

//Recording control
//-----------------

/** Enable recording.
 *
 * This method enables recording of the transmissions in CONFERENCE mode.
 * @param[in] path	Path where conference files are stored.
 */
void IPTVClientApp::EnableConferenceRecording(const wxString &path)
{
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Recording was enabled\n");
	debugMsg += wxT("Recording file: \"") + path + wxT("\"\n");
	SendDebugEvent(debugMsg);
#endif
	m_kernel.EnableConferenceRecording(WX_TO_FILE_CSTR(path));
}

/** Disable recording.
 *
 * This method disables recording in CONFERENCE mode.
 */
void IPTVClientApp::DisableConferenceRecording()
{
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Recording was disabled\n");
	SendDebugEvent(debugMsg);
#endif
	m_kernel.DisableConferenceRecording();
}

/** Starts outgoing media transmission.
 *
 * This method starts outgoing media transmission. If capture is enabled, will send both 
 * audio and video; if not, will send audio only.
 */
void IPTVClientApp::InitializeMediaColaboration(const long &bitrate)
{
	if(!m_isTransmitting)
	{
		// Old code. Reading device from file.
		/*
		int audioDevice = 0;
		if(wxFile::Exists(wxT("audio.config")))
		{
			wxTextFile file(wxT("audio.config"));
			if(file.Open())
			{
				wxString text = file.GetFirstLine();
				long number;
				text.ToLong(&number);
				audioDevice = (int)number;
			}
		}
		*/

		// Getting audio device ID from settings manager.
		long audioDeviceIndex = 0;
		wxString audioDevice = m_settingsManager->GetAudioDevice();
		if(audioDevice != wxEmptyString)
			audioDevice.ToLong(&audioDeviceIndex);

		// Getting audio codec from settings manager.
		wxString audioCodec;
		audioCodec = m_settingsManager->GetAudioCodec();
		if(audioCodec == wxEmptyString)
			audioCodec = wxT("speex_hi");

		// Getting video codec from settings manager.
		wxString videoCodec;
		videoCodec = m_settingsManager->GetVideoCodec();
		if(videoCodec == wxEmptyString)
			videoCodec = wxT("h264");
		
		//Old version of bitrate
		
		//Mode result;
		//wxString bitRateString;	// string in the format the lower-level libraries require
		//long bitRate;			// numeric value, in kbps

		//wxString transmissionProfileString = m_settingsManager->GetTransmissionProfile(); // string of transmission profile
		//long transmissionProfile;			  //transmission profile numeric value, in kbps

		//if(m_currentChannel->GetModeList().FindMode(IRM_MODE_CHANNEL_BITRATE, result))
		//	bitRateString = result.GetParameter();
		//else
		//	bitRateString = wxT("128k");	// TODO: Implement bit-rate control rules

		//bitRateString.ToLong(&bitRate);
		//transmissionProfileString.ToLong(&transmissionProfile);
		//// Workaround for encoding bug found while implementing this: if the picture size requested by codec
		//// is smaller than the capture size (320x240 on our tests) it corrupts the heap, causing all sorts of
		//// pseudo-random problems. For the currently hard-coded Speex/H.264 combination, the bitrate that
		//// avoids this problem is >= 128k so we force it here until the problem is solved.
		///*if(bitRate < 128)
		//{
		//	bitRate = 128;
		//	bitRateString.Clear();
		//	bitRateString << bitRate;
		//}
		//if(bitRateString.Right(1) != wxT("k"))
		//	bitRateString << wxT("k");*/

		////Solved up workaround
		//if(bitRate > transmissionProfile)
		//{
		//	bitRateString.Clear();
		//	bitRateString << transmissionProfile;
		//	bitRateString.ToLong(&bitRate);
		//}
		//if(bitRateString.Right(1) != wxT("k"))
		//	bitRateString << wxT("k");

		////wxString info;
		////info.Printf(wxT("Audio device = %d\nAudio Codec = %s\nVideo Codec = %s\nBit rate string = %s"), audioDeviceIndex, audioCodec, videoCodec, bitRateString);
		////wxMessageBox(info, wxT("Transmission Info"));
		wxString bitrateString = wxString::Format(wxT("%dk"), bitrate);
		m_kernel.InitializeMediaColaboration( WX_TO_CSTR(m_currentChannel->GetName()), (int)audioDeviceIndex, WX_TO_CSTR(audioCodec), WX_TO_CSTR(bitrateString), WX_TO_CSTR(videoCodec), WX_TO_CSTR(bitrateString), (unsigned)bitrate);
		m_isTransmitting = true;
	}
}

/** Stops the outgoing media transmission. 
 * @remarks Use in CONFERENCE mode only.
 */
void IPTVClientApp::StopMediaColaboration()
{
	if(m_isTransmitting)
	{
		m_kernel.StopMediaColaboration();
		m_isTransmitting = false;
	}
}

/** Checks for outgoing media transmission.
 * @return true if this client is transmitting media, false otherwise
 */
bool IPTVClientApp::IsTransmittingMedia() const
{
	return m_isTransmitting;
}

// Device/codec listing functions
//-------------------------------
/** Returns a list of video capture devices.
 * @param[out] The video capture device list.
 */
void IPTVClientApp::GetVideoDeviceList(list<VideoDeviceItem> &deviceList)
{
	deque<VideoDeviceInfo> kernelList;
	deque<VideoDeviceInfo>::iterator it;
	VideoDeviceItem item;
	m_kernel.GetVideoDeviceList(kernelList);
	if(!kernelList.empty())
	{
		for(it = kernelList.begin(); it!= kernelList.end(); it++)
		{
			item.index = it->index;
			item.name = WX_FROM_CSTR(it->name.c_str());
			deviceList.push_back(item);
		}
	}
}

/** Fills a list of audio input devices.
 * @param[out] deviceList	The audio input device list.
 */
void IPTVClientApp::GetAudioDeviceList(list<AudioDeviceItem> &deviceList)
{
	deque<string> kernelList;
	deque<string>::iterator it;
	AudioDeviceItem item;
	unsigned index = 0;
	m_kernel.GetAudioDeviceList(kernelList);
	if(!kernelList.empty())
	{
		for(it = kernelList.begin(); it!= kernelList.end(); it++)
		{
			item.index = index;
			item.name = WX_FROM_CSTR(it->c_str());
			deviceList.push_back(item);
			index++;
		}
	}
}

/** Fills a list of video codecs and supported bitrates,
 * @param[out] codecList	The list of codecs.
 * @param[out] videoBitrateList	The list of video bit rates.
 */
void IPTVClientApp::GetVideoCodecList(std::list<wxString> &codecList, std::list<wxString> &videoBitrateList)
{
	deque<string> kernelList;
	deque<string> kernelBitrateList;	// Not used right now
	deque<string>::iterator it;
	m_kernel.GetVideoEncoders(kernelList, kernelBitrateList);
	if(!kernelList.empty())
	{
		for(it = kernelList.begin(); it!= kernelList.end(); it++)
			codecList.push_back(WX_FROM_CSTR(it->c_str()));
		
		for(it = kernelBitrateList.begin(); it!= kernelBitrateList.end(); it++)
			videoBitrateList.push_back(WX_FROM_CSTR(it->c_str()));
	}
}

/** Fills a list of audio codecs.
 * @param[out] codecList	The list of codecs.
 */
void IPTVClientApp::GetAudioCodecList(std::list<wxString> &codecList)
{
	deque<string> kernelList;
	deque<string>::iterator it;
	m_kernel.GetAudioEncoders(kernelList);
	if(!kernelList.empty())
	{
		for(it = kernelList.begin(); it!= kernelList.end(); it++)
			codecList.push_back(WX_FROM_CSTR(it->c_str()));
	}
}

// Version strings
//----------------
/** Returns client version.
 * @return The client version string.
 */
const wxString IPTVClientApp::GetClientVersion() const
{
	return wxString(wxT(BUILD_USER_VERSION));
}

/** Returns the client build ID.
 * @return The client build ID.
 */
const wxString IPTVClientApp::GetClientBuildID() const
{
	return wxString(wxT(BUILD_ID));
}

/** Returns the client build date.
 * @return The client build date.
 */
const wxString IPTVClientApp::GetClientBuildDateStr() const
{
	return wxString(wxT(BUILD_DATETIME));
}

/** Returns the application core version.
 * @return The core version.
 */
const wxString IPTVClientApp::GetCoreVersion()
{
	return WX_FROM_CSTR(m_kernel.GetCoreVersion());
}

// ------------------------------------------------------------
// About dialog
// ------------------------------------------------------------
/** Show the About dialog.
 */
void IPTVClientApp::ShowAboutDialog()
{
	AboutDialog aboutDlg(m_skin);
	aboutDlg.SetTitle(_("About IP.TV"));
	aboutDlg.SetSkinInfo(m_skin->GetAboutInfo());
	aboutDlg.ShowModal();
}

// ------------------------------------------------------------

// iptv_kernel notification event handlers
// -------------------------------------------
void IPTVClientApp::OnHostConnect(wxCommandEvent &event)
{
	m_skin->OnHostConnect();
}

/** This event is triggered when connection to the IRM server succeeds.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChatConnect(wxCommandEvent &event)
{
	m_skin->ShowChannelList();
	m_lastConnectionState = m_kernel.GetConnectionState();
}

/** Fires when connection fails, either by being unable to reach the server or connection being broken.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnDisconnect(wxCommandEvent &event)
{
	SendDebugEvent(wxString(wxT("Disconnect event received.")));
	switch(m_lastConnectionState)
	{
	case IRMCS_DISCONNECTED:
	case IRMCS_CONNECTING:
		SendDebugEvent(wxString(wxT("Failed to connect. ")) << event.GetString());
		m_skin->OnConnectionFailure(event.GetString());
		break;
	case IRMCS_CONNECTED:
		SendDebugEvent(wxString(wxT("Connection lost. ")) << event.GetString());
		m_skin->OnServerDisconnect(event.GetString());
		break;
	default:
		break;
	}
	
	if(m_currentChannel)
	{
		delete m_currentChannel;
		m_currentChannel = NULL;
	}

	if(m_currentUser)
	{
		delete m_currentUser;
		m_currentUser = NULL;
	}

	if(m_activePoll)
		ResetActivePoll();

	m_lastConnectionState = m_kernel.GetConnectionState();
	m_appConfig.SetMode(APP_MODE_NONE);
}

void IPTVClientApp::OnAuthenticationSuccess(wxCommandEvent &event)
{
}

void IPTVClientApp::OnAuthenticationFailure(wxCommandEvent &event)
{
}

/** Fires when Kernel notifies App of an item in the channels list (in response to a query command).
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChannelQueryItem(wxCommandEvent &event)
{
	Channel *channel = (Channel *)(event.GetClientData());
	Channel newChannel = *channel;
	m_channelList.AddChannel(newChannel);
	wxString msg;
	msg << wxT("Channel: ") << channel->GetName() << wxT(" Users: ") << channel->GetUserCount();
	SendDebugEvent(msg);
	delete channel;
}

/** Fires when Kernel notifies App of the end of channel list. Tells Skin to update its channel list control.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChannelQueryEnd(wxCommandEvent &event)
{
	SendDebugEvent(wxT("End of channel listing."));
	m_skin->RefreshChannelList();
}

/** This is triggered when App receives a new user list item from Kernel.
 *
 * This is triggered when App receives a new user list item from Kernel. 
 * wxCommandEvent's ClientData carries an User structure filled in the Kernel notification handler's code.
 *
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnUserQueryItem(wxCommandEvent &event)
{
	// Assert: we require m_currentChannel is not NULL here
	wxCHECK_RET(m_currentChannel, wxT("Can't carry out user queries for no channel!!"));

	wxString channelName = event.GetString();
	ChannelUser *user = (ChannelUser *)(event.GetClientData());

	if(channelName == m_currentChannel->GetName())	// only add users from current channel
	{
		SendDebugEvent(wxString(wxT("User: ")) << user->GetNickName());
		if(*user == *m_currentUser)
			m_currentChannel->AddUser(*m_currentUser);
		else
		{
			ChannelUser newUser = *user;
			m_currentChannel->AddUser(newUser);
		}
	}
	// ignore other list items
	delete user;
}

/** This is triggered when Kernel informs App that the user listing has ended.
 *
 * This is triggered when Kernel informs App that the user listing has ended.
 * wxCommandEvent's integer member (retrieved by GetInt()) carries Kernel's state
 * when the operation was in progress, so that App may know whether the list was 
 * received upon joining a channel or by user's request and take appropriate action.
 *
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnUserQueryEnd(wxCommandEvent &event)
{
	// Pretty self-documenting variables, for the sake of code readability
	ChannelUser *user = (ChannelUser *)(event.GetClientData());
	int kernelJoinState = event.GetInt();
	wxString channelName = event.GetString();

	SendDebugEvent(wxString(wxT("End of user listing for channel ")) + channelName + wxT("."));

	if(kernelJoinState == IRMCJS_JOINING && channelName == m_currentChannel->GetName())	// This marks end of channel join procedure
	{
		m_skin->OnChannelJoin();
		m_timeConnected = wxDateTime::UNow();
		m_sessionTime = wxTimeSpan(0, 0, 0 ,0);
		ResetActivePoll();
	}
	else
		m_skin->RefreshUserList();

	delete user;
}

/** This is triggered on user-mode changes.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnUserMode(wxCommandEvent &event)
{
	wxString target = event.GetString();
	Mode *ptrMode = (Mode *)event.GetClientData();
	Mode mode = *ptrMode;
	User *user;	// May not necessarily be in channel

	wxString debugMsg;
	debugMsg << wxT("User mode: ")	<< target;
	if(event.GetEventType() == iptvEVT_USER_MODE_ON)
		debugMsg << wxT(" Mode On: ");
	if(event.GetEventType() == iptvEVT_USER_MODE_OFF)
		debugMsg << wxT(" Mode Off: ");
	debugMsg << mode.GetID() << wxT(", parameter = ") << mode.GetParameter();
	SendDebugEvent(debugMsg);

	if(m_currentUser)
	{
		if(target == m_currentUser->GetNickName())
		{
			if(event.GetEventType() == iptvEVT_USER_MODE_ON)
			{
				m_currentUser->AddMode(mode);
				// TODO: do other things here, such as refreshing user list etc.
			}
			if(event.GetEventType() == iptvEVT_USER_MODE_OFF)
			{
				m_currentUser->RemoveMode(mode);
				// TODO: do other things here, such as refreshing user list etc.
			}
		}
	}

	if(m_currentChannel)
	{
		UserList &userList = m_currentChannel->GetUserList();
		user = userList.FindUserByName(target);
		if(user)
		{
			if(event.GetEventType() == iptvEVT_USER_MODE_ON)
			{
				user->AddMode(mode);
				// TODO: do other things here, such as refreshing user list etc.
			}
			if(event.GetEventType() == iptvEVT_USER_MODE_OFF)
			{
				user->RemoveMode(mode);
				// TODO: do other things here, such as refreshing user list etc.
			}
		}
	}
	m_skin->RefreshUserList();
	delete ptrMode;
}

/** This is triggered when the current user joins a channel.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChannelJoin(wxCommandEvent &event)
{
	// Pretty self-documenting variables, for the sake of code readability
	ChannelUser *user = (ChannelUser *)(event.GetClientData());
	wxString channelName = event.GetString();	// Channel names are stored in the list without '#'
	int kernelJoinState = event.GetInt();

	SendDebugEvent(wxString(wxT("Received channel join event, channel = ")) << channelName << wxT(", user = ") << user->GetNickName());

	if((kernelJoinState == IRMCJS_JOINING) && (*user == *m_currentUser))
	{
		Channel *channel = new Channel;
		if(!m_channelList.FindChannelByName(channelName, *channel))
		{
			// If channel is not in channels list, create a new Channel object which will
			// not be present in list (it may be flagged as secret).
			channel->SetName(channelName);
		}
		m_currentChannel = channel;
		ClearUserList();
		EnableTransmissions(); // This enables treatment of "alive" notifications (initially disabled)
		SendDebugEvent(wxString(wxT("Joined channel ")) << m_currentChannel->GetName() << wxT(" as ") << m_currentUser->GetNickName());
	}
	else
	{
		SendDebugEvent(user->GetNickName() + wxT(" has joined channel ") + m_currentChannel->GetName());
		ChannelUser newUser = *user;
		m_currentChannel->AddUser(newUser);
		m_skin->RefreshUserList();
	}
	delete user;
}

/** This is triggered when the join channel operation fails.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChannelJoinFailure(wxCommandEvent &event)
{
	if(event.GetEventType() == iptvEVT_CHANNEL_BAD_PASSWORD)
	{
		m_skin->OnChannelBadPassword(event.GetString());
	}
}

/** This is triggered when any user leaves the current channel.
 * @param[in] event	Event object
 * @remarks There is a differentiation in behavior for when the current
 * user is leaving or any user other than this is leaving.
 */
void IPTVClientApp::OnChannelPart(wxCommandEvent &event)
{
	ChannelUser *user = (ChannelUser *)(event.GetClientData());
	SendDebugEvent(wxString(wxT("Received channel part event, user = ")) + user->GetNickName());

	// If this client is the one parting, clear everything
	if(event.GetInt() == IRMCPS_PARTING && *user == *m_currentUser)
	{
		if(m_currentChannel) 
		{
			ResetActivePoll();	// Clear the Poll when leaving channel, to free up memory
			SendDebugEvent(wxString(wxT("Left channel ")) << m_currentChannel->GetName());
			delete m_currentChannel;
		}
		m_currentChannel = NULL;
	}
	else // we've received a message telling us another user has left
	{
		ChannelUser *userFromList = m_currentChannel->GetUserList().FindUserByName(user->GetNickName());
		if(userFromList)
		{
			// If the leaving user has a Transmission object associated to him/her, remove it
			Transmission *transm = m_transmissionManager.GetTransmissionByName(userFromList->GetNickName());
			if(transm)
				m_transmissionManager.RemoveTransmission(transm->GetMediaID());
			// Remove the leaving user from the users list
			m_currentChannel->RemoveUser(*userFromList);
			// Also, if this user was in the Series Collaboration waiting line, remove him/her
			if(!m_seriesCollabQueue.empty())
			{
				for(UserQueue::iterator it = m_seriesCollabQueue.begin(); it != m_seriesCollabQueue.end(); it++)
				{
					if(*it == userFromList->GetNickName())
					{
						m_seriesCollabQueue.erase(it);
						break;
					}
				}
			}
		}
		m_skin->RefreshUserList();			
	}
	// Another pointer usage check point
	delete user;
}

/** Handles channel mode events.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnChannelMode(wxCommandEvent &event)
{
	Mode *ptrMode = (Mode *)event.GetClientData();
	Mode mode = *ptrMode;
	wxString target = event.GetString();

	if(!m_currentChannel)		// Doesn't make sense without a channel
	{
		delete ptrMode;
		return;
	}

	wxString debugMsg;
	debugMsg << wxT("Channel mode: ") << target;
	if(event.GetEventType() == iptvEVT_CHANNEL_MODE_ON)
		debugMsg << wxT(" Mode On: ");
	if(event.GetEventType() == iptvEVT_CHANNEL_MODE_OFF)
		debugMsg << wxT(" Mode Off: ");
	debugMsg << mode.GetID() << wxT(", parameter = ") << mode.GetParameter();
	SendDebugEvent(debugMsg);

	if(target == m_currentChannel->GetName())
	{
		if(event.GetEventType() == iptvEVT_CHANNEL_MODE_ON)
			m_currentChannel->AddMode(mode);

		if(event.GetEventType() == iptvEVT_CHANNEL_MODE_OFF)
			m_currentChannel->RemoveMode(mode);

		m_currentChannel->GetUserList().Sort();
		m_skin->RefreshUserList();
	}

	delete ptrMode;
}

/** Channel user flags changed.
 * @param[in] event	Event object
 */
void IPTVClientApp::OnChannelUserFlags(wxCommandEvent &event)
{
	Mode *ptrMode = (Mode *)event.GetClientData();
	Mode mode = *ptrMode;
	wxString target = event.GetString();

	if(!m_currentChannel)		// Doesn't make sense without a channel
	{
		delete ptrMode;
		return;
	}

	wxString debugMsg;
	debugMsg << wxT("Channel User Flags: ") << mode.GetParameter();
	if(event.GetEventType() == iptvEVT_CHANNEL_USER_FLAGS_ON)
		debugMsg << wxT(" Flag On: ");
	if(event.GetEventType() == iptvEVT_CHANNEL_USER_FLAGS_OFF)
		debugMsg << wxT(" Flag Off: ");
	debugMsg << mode.GetID() << wxT(", parameter = ") << mode.GetParameter();
	SendDebugEvent(debugMsg);

	if(target == m_currentChannel->GetName())
	{
		bool valueToSet;
		// I know there are only two possibilities, but no 'else' clause anyway
		// This is intentional, to make event types explicit.
		if(event.GetEventType() == iptvEVT_CHANNEL_USER_FLAGS_ON)
			valueToSet = true;
		if(event.GetEventType() == iptvEVT_CHANNEL_USER_FLAGS_OFF)
			valueToSet = false;

		wxString userName = mode.GetParameter();
		UserList &userList = m_currentChannel->GetUserList();
		ChannelUser *user = userList.FindUserByName(userName);
		const ChannelUser *myself = GetCurrentChannelUser();
		switch(mode.GetID())
		{
		case IRM_MODE_CHANNEL_OPERATOR:
			if(user)
				user->SetChannelOperator(valueToSet);
			break;
		case IRM_MODE_CHANNEL_MEDIA_COLLABORATOR:
			if(user)
			{
				user->SetChannelCollaborator(valueToSet);
				// If user got collaboration, she got what she wanted, so clear her voice request flag!
				// This is for all users (including myself), so this is now unconditional (issue #180).
				if(user->IsRequestingVoice())
					user->SetVoiceRequest(false);

				// User lost collaboration.
				if(!valueToSet)
				{
					if(user && myself)	// To avoid nasty stuff (such as segfaults)
					{
						if(user != myself)	// If this is not the current user for this client...
						{
							// ... deactivate the Transmission object associated to him/her (instead of
							// waiting for TimeoutRegister to do so after the timeout expires)
							Transmission *transm = m_transmissionManager.GetTransmissionByName(user->GetNickName());
							if(transm)
							{
								m_timeoutRegister.Clear(TR_MEDIA_AUDIO_TIMEOUT, transm->GetMediaID());
								m_timeoutRegister.Clear(TR_MEDIA_AUDIO_TIMEOUT, transm->GetMediaID());
								transm->Deactivate();
							}
						}
					}
				}

				// If we received collaboration, we aren't requesting voice anymore 
				//// (since we got what we wanted!)
				//if(myself)
				//{
				//	if(*user == *myself)
				//	{
				//		if(user->IsRequestingVoice())
				//			user->SetVoiceRequest(false);
				//	}
				//}
				m_skin->OnCollaborationAutoPlay();
			}			
			break;
		case IRM_MODE_MEDIA_VOICE:
			{
				// Media voice is treated as a channel-dependent user flag, but is
				// given a special treatment. Only one user has voice. Notification is
				// always of type FLAGS_ON. If parameter is blank, no one has voice.
				// Else, user name given by parameter gets voice.
				
				// First off, find the user who is currently with media voice
				const ChannelUser *userWithVoice = userList.FindUserWithMediaVoice();
				if(userWithVoice)	// If there is an user with media voice...
				{
					// and that user is not myself,
					if(userWithVoice != myself)
					{
						// deactivate the Transmission object associated to him/her (instead of
						// waiting for TimeoutRegister to do so after the timeout expires)
						Transmission *transm = m_transmissionManager.GetTransmissionByName(userWithVoice->GetNickName());
						if(transm)
						{
							m_timeoutRegister.Clear(TR_MEDIA_AUDIO_TIMEOUT, transm->GetMediaID());
							m_timeoutRegister.Clear(TR_MEDIA_AUDIO_TIMEOUT, transm->GetMediaID());
							transm->Deactivate();
						}
					}
				}
				// Now, clear the voice flag (for everyone).
				userList.ResetMediaVoice();
				if(user)	// if user is found in list, she gained voice
				{
					user->SetMediaVoice(valueToSet);
					// If user was requesting voice, she got what she wanted, so clear the flag.
					if(user->IsRequestingVoice())
						user->SetVoiceRequest(false);
	
					m_skin->OnCollaborationAutoPlay();
				}
			}
			break;
		case IRM_MODE_MEDIA_VOICE_REQUEST:
			if(user)
				user->SetVoiceRequest(valueToSet);
			break;
		default:
			break;
		}
		// If after this operation, the current user has neither voice or collaboration, 
		// tell Kernel to stop the transmission engine, so that we don't waste bandwidth.
		if(myself)
		{
			if(!(myself->HasMediaVoice() || myself->IsChannelCollaborator()))
			{
				m_skin->OnCollaborationStop();
				StopMediaColaboration();
			}
		}

		userList.Sort();
		m_skin->RefreshUserList();
	}	
	delete ptrMode;
}

/** Triggers when a private message is received.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnPrivateMessage(wxCommandEvent &event)
{
	ChatMessage *msg = (ChatMessage *)event.GetClientData();
	m_skin->OnPrivateMessage(msg->GetSender(), msg->GetMessage());
	delete msg;
}

/** Triggered when a public (channel) message was received
 * @param[in] event	Event object
 */
void IPTVClientApp::OnChannelMessage(wxCommandEvent &event)
{
	ChatMessage *msg = (ChatMessage *)event.GetClientData();
	m_skin->OnChannelMessage(msg->GetSender(), msg->GetMessage(), msg->GetChannel());
	delete msg;
}

/** Triggered when an user has disconnected
 * @param[in] event	Event object
 */
void IPTVClientApp::OnUserQuit(wxCommandEvent &event)
{
	ChannelUser *user = (ChannelUser *)(event.GetClientData());
	wxString channelName = event.GetString();

	// This is never too much.
	if(!user)
		return;

	ChannelUser *userFromList = m_currentChannel->GetUserList().FindUserByName(user->GetNickName());
	if(userFromList)
	{
		// If the leaving user has a Transmission object associated to him/her, remove it
		Transmission *transm = m_transmissionManager.GetTransmissionByName(userFromList->GetNickName());
		if(transm)
			m_transmissionManager.RemoveTransmission(transm->GetMediaID());
		// Also, if this user was in the Series Collaboration waiting line, remove him/her
		if(!m_seriesCollabQueue.empty())
		{
			for(UserQueue::iterator it = m_seriesCollabQueue.begin(); it != m_seriesCollabQueue.end(); it++)
			{
				if(*it == userFromList->GetNickName())
				{
					m_seriesCollabQueue.erase(it);
					break;
				}
			}
		}
		// Remove the leaving user from the users list
		m_currentChannel->RemoveUser(*userFromList);
	}

	m_skin->RefreshUserList();
	// Potential problem with pointer usage - checkpoint here
	delete user;
}

// Media control events
// ====================

/** Private method to attach a VideoSink (coming from Skin) to a Transmission. Done only to avoid excessive code replication.
 *
 * @param[in] transm	Transmission to attach the VideoSink to.
 * @param[in] user	ChannelUser to whom the Transmission belongs to.
 * @param[in] userName	User name, use only in MULTICAST mode.
 */
void IPTVClientApp::AttachTransmissionVideoSink(Transmission *transm, ChannelUser *user, const wxString &userName)
{
	if(!transm)
		return;

	VideoSink *sink;
	wxString name = userName;
	if(m_appConfig.GetMode() != APP_MODE_MULTICAST)
		sink = m_skin->CreateVideoSink(user);
	else
		sink = m_skin->CreateVideoSink(&name);

	transm->RegisterVideoSink(sink);
}

/** Triggered when a new media transmission begins.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnNewTransmission(wxCommandEvent &event)
{
	if(TransmissionsEnabled())
	{
		long mediaID = event.GetExtraLong();
		wxString userName = event.GetString();

		SendDebugEvent(wxString::Format(wxT("New transmission, media ID = %ld, user = %s"), mediaID, userName.c_str()));

		ChannelUser *user;
		Transmission *transm = m_transmissionManager.GetTransmissionByID(mediaID);

		if(!transm)	// transmission does not exist, add it.
		{
			transm = m_transmissionManager.AddNewTransmission(mediaID, userName);
			if((m_appConfig.GetMode() == APP_MODE_CONFERENCE) && m_currentChannel)
			{
				UserList &userList = m_currentChannel->GetUserList();
				user = userList.FindUserByName(userName);
			}
			else
				user = NULL;

			AttachTransmissionVideoSink(transm, user, userName);
		}
		else
		{
			// If the transmission is not active, activate it. But first,
			// check if it has a VideoSink attached to it, and if it doesn't,
			// ask Skin for one.
			if(!transm->HasVideoSink())
			{
				ChannelUser *user;
				if((m_appConfig.GetMode() == APP_MODE_CONFERENCE) && m_currentChannel)
				{	
					wxString userName = transm->GetUserName();
					UserList &userList = m_currentChannel->GetUserList();
					user = userList.FindUserByName(userName);	
				}
				else
					user = NULL;
				
				AttachTransmissionVideoSink(transm, user, userName);
			}	
			// Then, activate it
			if(!transm->IsActive())
			{
				transm->Activate();
				m_skin->OnActiveTransmissionCountChange(GetTransmissionCount());
			}
		}
	}
}

/** Media information update event (deals with media/username association).
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaInfo(wxCommandEvent &event)
{
	if(TransmissionsEnabled())
	{
		long mediaID = event.GetExtraLong();
		wxString userName = event.GetString();

		SendDebugEvent(wxString::Format(wxT("Media info event, media ID = %ld, user = %s"), mediaID, userName.c_str()));
		
		Transmission *transm = m_transmissionManager.GetTransmissionByID(mediaID);
		ChannelUser *user;

		if((m_appConfig.GetMode() == APP_MODE_CONFERENCE) && m_currentChannel)
		{	
			UserList &userList = m_currentChannel->GetUserList();
			user = userList.FindUserByName(userName);	
		}
		else
			user = NULL;

		if(!transm)
		{
			transm = m_transmissionManager.AddNewTransmission(mediaID, userName);
			// Reminder: AttachTransmissionVideoSink must be called only when
			// the transmission is to be activated!!
			// AttachTransmissionVideoSink(transm, user, userName);
		}
		else	// Transmission exists, but may not have a VideoSink associated to it.
		{
			if((transm->IsActive()) && (!transm->HasVideoSink()))
			AttachTransmissionVideoSink(transm, user, userName);
		}
	}
}

/** "Audio alive" notification treatment.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaAudioAlive(wxCommandEvent &event)
{
	if(TransmissionsEnabled())
	{
		long mediaID = event.GetExtraLong();
		Transmission *transm = (Transmission *)event.GetClientData();
		transm->SetAudioAlive(true);
		// Check if it has a VideoSink attached to it, and if it doesn't,
		// ask Skin for one.
		if(!transm->HasVideoSink())
		{
			ChannelUser *user;
			if((m_appConfig.GetMode() == APP_MODE_CONFERENCE) && m_currentChannel)
			{	
				wxString userName = transm->GetUserName();
				UserList &userList = m_currentChannel->GetUserList();
				user = userList.FindUserByName(userName);	
			}
			else
				user = NULL;

			AttachTransmissionVideoSink(transm, user, transm->GetUserName());
		}
		// Is transmission active?
		if(!transm->IsActive())
		{
			// if it isn't, activate it
			transm->Activate();
			// and let Skin know we did that
			m_skin->OnActiveTransmissionCountChange(GetTransmissionCount());
		}
	}
}

/** "Video alive" notification treatment.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaVideoAlive(wxCommandEvent &event)
{
	if(TransmissionsEnabled())
	{
		Transmission *transm = (Transmission *)event.GetClientData();
		transm->SetVideoAlive(true);
		// Check if it has a VideoSink attached to it, and if it doesn't,
		// ask Skin for one.
		if(!transm->HasVideoSink())
		{
			ChannelUser *user;
			if((m_appConfig.GetMode() == APP_MODE_CONFERENCE) && m_currentChannel)
			{	
				wxString userName = transm->GetUserName();
				UserList &userList = m_currentChannel->GetUserList();
				user = userList.FindUserByName(userName);	
			}
			else
				user = NULL;
		
			AttachTransmissionVideoSink(transm, user, transm->GetUserName());
		}
		// Is transmission active?
		if(!transm->IsActive())
		{
			// If it isn't, activate it
			transm->Activate();
			// and let Skin know we did that
			m_skin->OnActiveTransmissionCountChange(GetTransmissionCount());
		}
	}
}

/** File playback progress notification treatment.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaProgressNotify(wxCommandEvent &event)
{
	if(m_appConfig.GetMode() == APP_MODE_FILE)
		m_skin->UpdateFileProgressIndicator((float)(event.GetExtraLong() / 1000.0f));
}

/** Gives the translation of 'message' in the currently selected locale
 * @param[in] message	Message to translate.
 */
wxString IPTVClientApp::Translate(const wxString &message)
{
	return m_locale->GetString(message);
}

// List handling functions (private)

/** Private function: Reset channel list. All items are allocated with operator new()
 */
void IPTVClientApp::ClearChannelList()
{
	m_channelList.Clear();
}

/** Triggered on media audio timeout.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaAudioTimeout(wxCommandEvent &event)
{
	SendDebugEvent(wxString(wxT("Audio timeout, media ID =")) << event.GetExtraLong());

	unsigned long mediaID = (unsigned long)event.GetExtraLong();
	Transmission *transm = m_transmissionManager.GetTransmissionByID(mediaID);
	
	if(!transm)		// No such transmission!
		return;

	// If we have no video already, kill the transmission...
	if(!transm->IsVideoAlive())
	{
		transm->Deactivate();
		// and let Skin know we did that
		m_skin->OnActiveTransmissionCountChange(GetTransmissionCount());
	}
	else	// This transmission has just become video-only
		transm->SetAudioAlive(false);
}

/** Triggered on media video timeout.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMediaVideoTimeout(wxCommandEvent &event)
{
	SendDebugEvent(wxString(wxT("Video timeout, media ID =")) << event.GetExtraLong());

	unsigned long mediaID = (unsigned long)event.GetExtraLong();
	Transmission *transm = m_transmissionManager.GetTransmissionByID(mediaID);
	if(!transm)	// No such transmission! 
		return;

	// If we have no audio already, kill the transmission...
	if(!transm->IsAudioAlive())
	{
		transm->Deactivate();
		// and let Skin know we did that
		m_skin->OnActiveTransmissionCountChange(GetTransmissionCount());
	}
	else	// This transmission has just become audio-only
	{
		transm->SetVideoAlive(false);
	}
}

// Application Sharing events & functions
// ======================================
/** Triggered when an Application Sharing transmission begins.
 * @param[in] event	Event object
 */
void IPTVClientApp::OnAppSharingBegin(wxCommandEvent &event)
{
	// TODO: Notify Skin and make it handle the event appropriately.
	// NOTE: The VideoSink will be created on Skin's initiative --
	// TODO: Modify AppInterface to allow this.
	SendDebugEvent(wxT("Application Sharing begin"));
	if(!m_asVideoSource.IsOk())
		m_asVideoSource.Init();
	
	if(m_skin)
		m_skin->OnAppSharingBegin();	
}

/** Triggered when an Application Sharing transmission ends.
 * @param[in] event	Event object
 */
void IPTVClientApp::OnAppSharingEnd(wxCommandEvent &event)
{
	SendDebugEvent(wxT("Application Sharing end"));
	// TODO: Notify Skin and make it handle the event appropriately
	if(m_skin)
		m_skin->OnAppSharingEnd();
		
	if(m_asVideoSource.IsRunning())
		m_asVideoSource.Stop();

	m_asVideoSource.Close();
}

/** Triggered when Application Sharing gets a new frame.
 * @param[in] event	Event object
 */
void IPTVClientApp::OnAppSharingNewFrame(wxCommandEvent &event)
{
	// NOTE: This is handled internally and doesn't have to be notified to Skin
	KernelASFrameParam *param = (KernelASFrameParam *)event.GetClientData();

	if(!m_asVideoSource.IsRunning())
		m_asVideoSource.Start();
	m_asVideoSource.NewFrame(param->m_buffer, param->m_width, param->m_height);

	delete [] param->m_buffer;
	delete param;	// param came in a void * and was allocated with operator new.
}

/** Triggered when a White Board message is received.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnWhiteboardChannel(wxCommandEvent &event)
{
	WBMessage *p = (WBMessage *)event.GetClientData();

	if(m_skin)
		m_skin->OnWhiteboardMessage(p->channel, p->user, p->data);

	delete p;
}

/** Register a VideoSink to receive Application Sharing frames.
 * @param[in] sink	VideoSink to be registered.
 */
void IPTVClientApp::RegisterAppSharingVideoSink(VideoSink *sink)
{
	if(sink)
		m_asVideoSource.RegisterVideoSink(sink);
}

/** Unregister a VideoSink from Application Sharing.
 * @param[in] sink	VideoSink to unregister.
 */
void IPTVClientApp::UnregisterAppSharingVideoSink(VideoSink *sink)
{
	if(sink)
		m_asVideoSource.UnregisterVideoSink(sink);
}

/** Unregister all VideoSinks from Application Sharing.
 */
void IPTVClientApp::UnregisterAllAppSharingVideoSinks()
{
	m_asVideoSource.UnregisterAllVideoSinks();
}

// Video capture event
// ===================
/** Triggered when the Capture module gets a new frame.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnCaptureNewFrame(wxCommandEvent &event)
{
	KernelVideoCaptureDataParam *param = (KernelVideoCaptureDataParam *)event.GetClientData();
	m_camVideoSource.NewFrame(param->m_data, param->m_width, param->m_height);
	delete param;
}

// Other functions
// ===============
/** Application timer routine.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnTimer(wxTimerEvent &event)
{
	// Updates connection time info
	if((m_kernel.GetConnectionState() == IRMCS_CONNECTED) && m_timeConnected.IsValid())
		m_sessionTime = wxDateTime::UNow() - m_timeConnected;

	if(m_seriesCollabStarted)
	{
		// Run a Series Collaboration pass if it's time for it
		// or the last run time isn't valid (which indicates the algorithm has not run yet)
		if(!m_scLastSwitchTime.IsValid() || (wxDateTime::UNow() > (m_scLastSwitchTime + m_scSwitchInterval)))
			SeriesCollaborationPass();
	}
	// Skin-dependent routine triggered on this timer event
	if(m_skin)
		m_skin->OnAppTimer();
}

/** Returns the time when the application was started.
 * @return The date/time of launch.
 */
const wxDateTime &IPTVClientApp::GetAppStartTime() const
{
	return m_timeStarted;
}

/** Clears user list.
 */
void IPTVClientApp::ClearUserList()
{
	if(!m_currentChannel)
		return;
	m_currentChannel->GetUserList().Clear();
}

// Transmission handling toggle methods
/** Disable transmission handling.
 */
void IPTVClientApp::DisableTransmissions()
{
	m_enableTransmissions = false;
}

/** Enable transmission handling.
 */
void IPTVClientApp::EnableTransmissions()
{
	m_enableTransmissions = true;
}

/** Checks if transmission handling is enabled.
 * @return true if handling is enabled, false otherwise
 */
bool IPTVClientApp::TransmissionsEnabled() const
{
	return m_enableTransmissions;
}

/** Returns a pointer to the instance of SettingsManager.
 * @return A pointer to the instance of SettingsManager.
 */
SettingsManager *IPTVClientApp::GetSettingsManager()
{
	return m_settingsManager;
}

/** Sends text to the Debug Window.
 *
 * This is a debug function. It implements AppInterface::SetDebugMessage.
 * Use it to send text from your Skin to the Debug Window.
 * @param[in] message	The text to send to the Debug Window.
 */
void IPTVClientApp::SetDebugMessage(const wxString &message)
{
	SendDebugEvent(message);
}

/** Sends out a debug event.
 *
 * Sends out a Debug event. Enabled for debug builds only.
 * This event is used because one may want to send debug text from
 * functions that are called from different threads.
 * @param[in] message		Message to be sent.
 * @param[in] clientData	Client data. Caller is responsible for managing it.
 */
void IPTVClientApp::SendDebugEvent(wxString message, void *clientData)
{
#ifdef IPTV_DEBUG
	wxCommandEvent event;
	event.SetEventType(iptvEVT_DEBUG);
	event.SetString(message);
	event.SetClientData(clientData);
	wxPostEvent(this, event);
#endif
}

// Debug event handler.
#ifdef IPTV_DEBUG
/** Handles the debug event.
 * @param[in] event	Event handler.
 */
void IPTVClientApp::OnDebugEvent(wxCommandEvent &event)
{
	std::cout << WX_TO_CSTR(event.GetString()) << std::endl;
	m_debugWindow->SetDebugMessage(event.GetString());
}
#endif

/** Handles Skin's Close event.
 *
 * This handles Skin's Close event. It calls Skin's OnMainWindowClose,
 * the function that handles Skin-dependent closing behavior (such as
 * asking for confirmation, etc.) and evaluates its return value.
 * Kills everything (ending the program) if the function returns true.
 *
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnSkinMainWindowClose(wxCloseEvent &event)
{
	wxCHECK_RET(m_skin, wxT("Can't handle events from a NULL skin!"));
	bool close = m_skin->OnMainWindowClose(!(event.CanVeto()));
	if(!close)
		event.Veto();
	else
	{
		m_skin->Destroy();
		m_skin = NULL;
#ifdef IPTV_DEBUG
		m_debugWindow->Destroy();
#endif
	}
}

//BEGIN Series Collaboration functions
// -----------------------------------

/** Adds an user to the Series Collaboration list.
 * @param[in] userName	Nickname of the user to be added.
 */
void IPTVClientApp::AddToSeriesCollaboration(const wxString &userName)
{
	wxCHECK_RET(m_currentChannel, wxT("Series Collaboration only makes sense while in channel!"));
	UserList &list = m_currentChannel->GetUserList();
	ChannelUser *user = list.FindUserByName(userName);
	if(user)
	{
		SendDebugEvent(wxString(wxT("IPTVClientApp::AddToSeriesCollaboration(): Adding user ")) << user->GetNickName() << wxT(" to SC list"));
		user->SetSeriesCollaboration(true);
	}
}

/** Initializes the Series Collaboration algorithm.
 *
 * Initializes the SC algorithm's control variables.
 * The list of users to participate is determined BEFORE this function 
 * is called, by either auto-selecting all users -- in which case the
 * argument to it is true -- or individually selecting among the enabled users
 * -- in which case the argument to this function is false. The latter is the
 * default behavior.)
 *
 * @param[in] interval			Switch interval
 * @param[in] selectAllUsers	Auto-select all enabled users to Series Collaboration.
 */
void IPTVClientApp::StartSeriesCollaboration(const wxTimeSpan &interval, bool selectAllUsers)
{
	if(!m_seriesCollabStarted)	// Don't restart if already started
	{
		SendDebugEvent(wxT("IPTVClientApp::StartSeriesCollaboration(): Starting series collaboration"));
		m_scSelectAllUsers = selectAllUsers;
		m_scSwitchInterval = interval;
		SendDebugEvent(wxString(wxT("IPTVClientApp::StartSeriesCollaboration(): Switch interval = ")) << m_scSwitchInterval.GetSeconds().ToString() << wxT("seconds"));
		if(m_scSelectAllUsers)
			SendDebugEvent(wxT("IPTVClientApp::StartSeriesCollaboration(): Using auto-select"));
		else
			SendDebugEvent(wxT("IPTVClientApp::StartSeriesCollaboration(): Using manual selection"));

		m_seriesCollabStarted = true;
	}
}

/** Resets the SC algorithm's control variables, stopping it.
 *
 * This method stops the Series Collaboration, and resets the control variables.
 */
void IPTVClientApp::StopSeriesCollaboration()
{
	if(m_seriesCollabStarted)
	{
		// Clear flags and FIFO
		m_seriesCollabQueue.clear();
		if(m_currentChannel)
		{
			UserList &list = m_currentChannel->GetUserList();
			list.ClearSeriesCollaboration();
		}
		m_seriesCollabStarted = false;
	}
}

/** Private function: Runs a single pass of the SC algorithm.
 */
void IPTVClientApp::SeriesCollaborationPass()
{
	wxCHECK_RET(m_currentChannel, wxT("IPTVClientApp::SeriesCollaborationPass(): Series Collaboration only makes sense in channel!"));

	int totalCollabSlots, availableCollabSlots;
	UserIterator it;
	UserQueue::iterator qit;
	
	// Calculates the total number of collaboration slots
	const ModeList &channelModes = m_currentChannel->GetModeList();
	Mode maxTransmissions;
	if(!channelModes.FindMode(IRM_MODE_CHANNEL_TRANSMISSION_LIMIT, maxTransmissions))
	{
		// Transmission limit is unset - assume 'infinity'
		totalCollabSlots = -1;
	}
	else
	{
		// Transmission limit is set - 1 is reserved for the 'media voice' user so account for that
		long lMaxTransm;
		maxTransmissions.GetParameter().ToLong(&lMaxTransm);
		totalCollabSlots = (int)lMaxTransm - 1;
	}
	
	// Save a reference to the users list
	UserList &userList = m_currentChannel->GetUserList();
	
	// If we have more than zero slots, then allocate them according to the rules
	if(totalCollabSlots > 0)
	{
		// At each pass, we have to check if our waiting line is sane.
		// It could become inconsistent if an user enters the conference while Series Collaboration is running.
		// This is especially true in the "auto select everyone" case.
		const UserList *scSelectedList;
		if(m_scSelectAllUsers)
		{
			// If "auto select everyone" is enabled, use the list of all available "series collaborators"
			SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass(): Using auto-select"));
			scSelectedList = userList.GetSubList(SLC_SERIES_COLLAB_ENABLED);
		}
		else
		{
			// else, use only those selected by the user to participate
			SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass(): Using manual selection"));
			scSelectedList = userList.GetSubList(SLC_SERIES_COLLAB_SELECTED);
			for(it = scSelectedList->GetBeginIterator(); it != scSelectedList->GetEndIterator(); it++)
			{
				SendDebugEvent(wxString(wxT("IPTVClientApp::SeriesCollaborationPass(): User ")) << it->GetNickName() << wxT(" is selected"));
			}
		}
		
		// Then, here we check if there are any users in the selected list (above) that neither have
		// collaboration nor are in the waiting line. 
		// NOTE: In the 1st pass of the algorithm this will populate the waiting line with the entire list 
		// (as no slots will be occupied and the waiting line will be empty before this runs).
		for(it = scSelectedList->GetBeginIterator(); it != scSelectedList->GetEndIterator(); it++)
		{
			if(!(it->IsChannelCollaborator()))	// user does not have collaboration
			{
				SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + it->GetNickName() + wxT(" does not have collaboration"));
				// Look in the waiting line
				bool found = false;
				for(qit = m_seriesCollabQueue.begin(); qit != m_seriesCollabQueue.end(); qit++)
				{
					found = (*qit == it->GetNickName());
					if(found)
					{
						SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + *qit + wxT(" found in queue"));
						break;
					}
				}
				if(!found)	// User is also not in the waiting line...
				{
					SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + it->GetNickName() + wxT(" not found in queue"));
					m_seriesCollabQueue.push_back(it->GetNickName());	// so put him at the back of the line
				}
			}
			else
				SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + it->GetNickName() + wxT(" has collaboration"));
		}
		
		// If the "waiting line" is empty, do nothing
		if(!m_seriesCollabQueue.empty())
		{
			// otherwise, give collaboration to the first in line. But first,
			// we have to check if there are any slots available.
			// Get current list of collaborating users
			const UserList *collabList = userList.GetSubList(SLC_HAS_COLLABORATION);
			if(collabList)
			{
				for(qit = m_seriesCollabQueue.begin(); qit != m_seriesCollabQueue.end(); qit++)
				{
					SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + *qit + wxT(" is in queue"));
				}
				// Calculate number of available collaboration slots
				availableCollabSlots = totalCollabSlots - collabList->GetUserCount();
				// If this number equals zero,
				if(availableCollabSlots == 0)
				{
					// remove collaboration from last user in collaboration list, to free up a slot
					UserReverseIterator rit = collabList->GetReverseBeginIterator();
					SendDebugEvent(wxString(wxT("IPTVClientApp::SeriesCollaborationPass(): Removing collaboration from ")) << rit->GetNickName());
					RequestUnsetChannelMediaCollaborator(m_currentChannel->GetName(), rit->GetNickName());
					// then send that user to the back of the line
					m_seriesCollabQueue.push_back(rit->GetNickName());
				}
				// Now, take the first user from the line and grant him/her collaboration
				wxString &firstInLine = m_seriesCollabQueue.front();
				SendDebugEvent(wxString(wxT("IPTVClientApp::SeriesCollaborationPass(): Granting collaboration to ")) << firstInLine);
				RequestSetChannelMediaCollaborator(m_currentChannel->GetName(), firstInLine);
				// and remove that from the "waiting line"
				m_seriesCollabQueue.pop_front();
				for(qit = m_seriesCollabQueue.begin(); qit != m_seriesCollabQueue.end(); qit++)
				{
					SendDebugEvent(wxT("IPTVClientApp::SeriesCollaborationPass():") + *qit + wxT(" is in queue"));
				}
			}
		}
	}
	// Set last run time to the current time
	m_scLastSwitchTime = wxDateTime::UNow();
}

/** Checks if Series Collaboration is running.
 * @return true if SC is running, false otherwise.
 */
bool IPTVClientApp::IsSeriesCollaborationStarted() const
{
	return m_seriesCollabStarted;
}

//END Series Collaboration functions

/** Set user as channel operator.
 * @param[in] channel	Channel on which to operate.
 * @param[in] user		User to make operator.
 */
void IPTVClientApp::RequestSetChannelOperator(wxString channel, wxString user)
{
	m_kernel.RequestSetChannelOperator(WX_TO_CSTR(channel), WX_TO_CSTR(user));
}

/** Unset channel operator flag for user.
 * @param[in] channel	Channel on which to operate.
 * @param[in] user		User whose operator flag is to be unset.
 */
void IPTVClientApp::RequestUnsetChannelOperator(wxString channel, wxString user)
{
	m_kernel.RequestUnsetChannelOperator(WX_TO_CSTR(channel), WX_TO_CSTR(user));
}

/** Set the private channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelPrivate(wxString channel)
{
	m_kernel.RequestSetChannelPrivate(WX_TO_CSTR(channel));
}

/** Unset the private channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelPrivate(wxString channel)
{
	m_kernel.RequestUnsetChannelPrivate(WX_TO_CSTR(channel));
}

/** Set the secret channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelSecret(wxString channel)
{
	m_kernel.RequestSetChannelSecret(WX_TO_CSTR(channel));
}

/** Unset the secret channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelSecret(wxString channel)
{
	m_kernel.RequestUnsetChannelSecret(WX_TO_CSTR(channel));
}

/** Set the invite-only channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelInviteOnly(wxString channel)
{
	m_kernel.RequestSetChannelInviteOnly(WX_TO_CSTR(channel));
}

/** Unset the invite-only channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelInviteOnly(wxString channel)
{
	m_kernel.RequestUnsetChannelInviteOnly(WX_TO_CSTR(channel));
}

/** Set the "operator-only topic change" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelOpChangeTopic(wxString channel)
{
	m_kernel.RequestSetChannelOpChangeTopic(WX_TO_CSTR(channel));
}

/** Unset the "operator-only topic change" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelOpChangeTopic(wxString channel)
{
	m_kernel.RequestUnsetChannelOpChangeTopic(WX_TO_CSTR(channel));
}

/** Set the "no external messages" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelNoExternalMessages(wxString channel)
{
	m_kernel.RequestSetChannelNoExternalMessages(WX_TO_CSTR(channel));
}

/** Unset the "no external messages" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelNoExternalMessages(wxString channel)
{
	m_kernel.RequestUnsetChannelNoExternalMessages(WX_TO_CSTR(channel));
}

/** Set the "moderated" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelModerated(wxString channel)
{
	m_kernel.RequestSetChannelModerated(WX_TO_CSTR(channel));
}

/** Unset the "moderated" channel flag.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelModerated(wxString channel)
{
	m_kernel.RequestUnsetChannelModerated(WX_TO_CSTR(channel));
}

/** Set the user number limit for a channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] limit		User limit to set.
 */
void IPTVClientApp::RequestSetChannelUserLimit(wxString channel, int limit)
{
	m_kernel.RequestSetChannelUserLimit(WX_TO_CSTR(channel), limit);
}

/** Unset the user limit for a channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelUserLimit(wxString channel)
{
	m_kernel.RequestUnsetChannelUserLimit(WX_TO_CSTR(channel));
}

/** Give media collaboration for an user in channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] user		User to give collaboration to.
 */
void IPTVClientApp::RequestSetChannelMediaCollaborator(wxString channel, wxString user)
{
	m_kernel.RequestSetChannelMediaCollabotator(WX_TO_CSTR(channel), WX_TO_CSTR(user));
}

/** Remove media collaboration from an user in channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] user		User to give collaboration to.
 */
void IPTVClientApp::RequestUnsetChannelMediaCollaborator(wxString channel, wxString user)
{
	m_kernel.RequestUnsetChannelMediaCollabotator(WX_TO_CSTR(channel), WX_TO_CSTR(user));
}

/** Set password for a channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] password	Password to set.
 */
void IPTVClientApp::RequestSetChannelKey(wxString channel, wxString passwd)
{
	m_kernel.RequestSetChannelKey(WX_TO_CSTR(channel), WX_TO_CSTR(passwd));
}

/** Clear password for a channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelKey(wxString channel)
{
	m_kernel.RequestUnsetChannelKey(WX_TO_CSTR(channel));
}

/** Set bit rate limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] bitRate	The bit rate limit to set.
 */
void IPTVClientApp::RequestSetChannelBitRate(wxString channel, int bitRate)
{
	m_kernel.RequestSetChannelBitRate(WX_TO_CSTR(channel), bitRate);
}

/** Clear bit rate limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelBitRate(wxString channel)
{
	m_kernel.RequestUnsetChannelBitRate(WX_TO_CSTR(channel));
}

/** Set auto bit rate change flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelAutoChangeBitRate(wxString channel)
{
	m_kernel.RequestSetChannelAutoChangeBitRate(WX_TO_CSTR(channel));
}

/** Clear auto bit rate change flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelAutoChangeBitRate(wxString channel)
{
	m_kernel.RequestUnsetChannelAutoChangeBitRate(WX_TO_CSTR(channel));
}

/** Set multiple transmission flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelMultipleTransmission(wxString channel)
{
	m_kernel.RequestSetChannelMultipleTransmission(WX_TO_CSTR(channel));
}

/** Clear multiple transmission flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelMultipleTransmission(wxString channel)
{
	m_kernel.RequestUnsetChannelMultipleTransmission(WX_TO_CSTR(channel));
}

/** Set "one audio" flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelOneAudio(wxString channel, int audioSwitchTime)
{
	m_kernel.RequestSetChannelOneAudio(WX_TO_CSTR(channel), audioSwitchTime);
}

/** Clear "one audio" flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelOneAudio(wxString channel)
{
	m_kernel.RequestUnsetChannelOneAudio(WX_TO_CSTR(channel));
}

/** Set "audio mute" flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestSetChannelAudioMute(wxString channel)
{
	m_kernel.RequestSetChannelAudioMute(WX_TO_CSTR(channel));
}

/** Unset "audio mute" flag for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelAudioMute(wxString channel)
{
	m_kernel.RequestUnsetChannelAudioMute(WX_TO_CSTR(channel));
}

/** Set band share limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] limit		Limit to set.
 */
void IPTVClientApp::RequestSetChannelBandShareLimit(wxString channel, int limit)
{
	m_kernel.RequestSetChannelBandShareLimit(WX_TO_CSTR(channel), limit);
}

/** Clear band share limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelBandShareLimit(wxString channel)
{
	m_kernel.RequestUnsetChannelBandShareLimit(WX_TO_CSTR(channel));
}

/** Set simultaneous transmission limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 * @param[in] limit		Simultaneous transmission limit to set.
 */
void IPTVClientApp::RequestSetChannelTransmissionLimit(wxString channel, int limit)
{
	m_kernel.RequestSetChannelTransmissionLimit(WX_TO_CSTR(channel), limit);
}

/** Clear simultaneous transmission limit for the IRM channel.
 * @param[in] channel	Channel on which to operate.
 */
void IPTVClientApp::RequestUnsetChannelTransmissionLimit(wxString channel)
{
	m_kernel.RequestUnsetChannelTransmissionLimit(WX_TO_CSTR(channel));
}

/** Tries to make the current user invisible.
 */
void IPTVClientApp::RequestSetUserInvisible()
{
	m_kernel.RequestSetUserInvisible();
}

/** Unsets the invisible user flag.
*/
void IPTVClientApp::RequestUnsetUserInvisible()
{
	m_kernel.RequestUnsetUserInvisible();
}

/** Sets the "cam on" user flag for the current user.
 */
void IPTVClientApp::RequestSetUserCameraOn()
{
	m_kernel.RequestSetUserCameraOn();
}

/** Clears the "cam on" user flag for the current user.
 */
void IPTVClientApp::RequestUnsetUserCameraOn()
{
	m_kernel.RequestUnsetUserCameraOn();
}

/** Sets the "mic mute" user flag for the current user.
 */
void IPTVClientApp::RequestSetUserMicMute()
{
	m_kernel.RequestSetUserMicMute();
}

/** Clear the "mic mute" user flag for the current user.
 */
void IPTVClientApp::RequestUnsetUserMicMute()
{
	m_kernel.RequestUnsetUserMicMute();
}

/** Sets the "away" user flag for the current user.
 */
void IPTVClientApp::RequestSetUserAway()
{
	m_kernel.RequestSetUserAway();
}

/** Clears the "away" user flag for the current user.
 */
void IPTVClientApp::RequestUnsetUserAway()
{
	m_kernel.RequestUnsetUserAway();
}

/** Gives media voice for an user in channel.
 * @param[in] User to grant media voice to.
 */
void IPTVClientApp::RequestSetMediaVoice(const wxString &user)
{
	if(!m_currentChannel)
		return;

	wxString channel = m_currentChannel->GetName();
	m_kernel.RequestSetMediaVoice(WX_TO_CSTR(channel), WX_TO_CSTR(user));
}

/** Clears the media voice in channel.
 */
void IPTVClientApp::RequestUnsetMediaVoice()
{
	if(!m_currentChannel)
		return;
	wxString channel = m_currentChannel->GetName();
	m_kernel.RequestUnsetMediaVoice(WX_TO_CSTR(channel));
}

/** Sends a media voice request to the server for the current user.
 */
void IPTVClientApp::SendMediaVoiceRequest()
{
	if(!m_currentChannel)
		return;
	wxString channel = m_currentChannel->GetName();
	m_kernel.SendMediaVoiceRequest(WX_TO_CSTR(channel));
	// Enable the voice request icon for myself.
	SetOwnVoiceRequest();
}

/** Sends a media voice request cancel message to the server for the current user.
 */
void IPTVClientApp::SendMediaVoiceRequestCancel()
{
	if(!m_currentChannel)
		return;
	wxString channel = m_currentChannel->GetName();
	m_kernel.SendMediaVoiceRequestCancel(WX_TO_CSTR(channel));
	// Disable the voice request icon for myself.
	ClearOwnVoiceRequest();
}

/** Clear all voice requests.
 */
void IPTVClientApp::SendMediaVoiceRequestRemoveAll()
{
	if(!m_currentChannel)
		return;
	wxString channel = m_currentChannel->GetName();
	m_kernel.SendMediaVoiceRequestRemoveAll(WX_TO_CSTR(channel));
	// Disable the voice request icon for myself.
	ClearOwnVoiceRequest();
}

// These own voice handling methods are private to IPTVClientApp.
/** (Private function) Set voice request flag for current user. 
*/
void IPTVClientApp::SetOwnVoiceRequest()
{
	KernelModeParam modeParam;
	modeParam.m_target = WX_TO_CSTR(GetCurrentChannel()->GetName());
	modeParam.m_parameter = WX_TO_CSTR(GetCurrentChannelUser()->GetNickName());
	modeParam.m_value = true;
	SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE_REQUEST, modeParam);
}
/** (Private function) Clear the voice request flag for current user.
 */
void IPTVClientApp::ClearOwnVoiceRequest()
{
	KernelModeParam modeParam;
	modeParam.m_target = WX_TO_CSTR(GetCurrentChannel()->GetName());
	modeParam.m_parameter = WX_TO_CSTR(GetCurrentChannelUser()->GetNickName());
	modeParam.m_value = false;
	SendIRMChannelUserFlagsEvent(IRM_MODE_MEDIA_VOICE_REQUEST, modeParam);
}

/** Send a set channel topic request to server.
 */
void IPTVClientApp::RequestSetTopic(wxString channel, wxString topic)
{
	m_kernel.RequestSetTopic(WX_TO_CSTR(channel), WX_TO_CSTR(topic));
}

/** Triggered on File Transfer reception begin.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionBegin(wxCommandEvent &event)
{
	FTReceptionBeginParameter *param = (FTReceptionBeginParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Begin file transfer:\n");
	wxString mediaIDStr;
	mediaIDStr.Printf(wxT("mediaID = %d\n"), param->m_mediaId);
	debugMsg += mediaIDStr;
	debugMsg += wxT("file name = ") + param->m_fileName + wxT("\n");
	wxString fileSizeStr;
	fileSizeStr.Printf(wxT("file size = %d bytes\n"), param->m_fileSize);
	debugMsg += fileSizeStr;
	wxString packetSizeStr;
	packetSizeStr.Printf(wxT("packet size = %d bytes\n"), param->m_packetSize);
	debugMsg += packetSizeStr;
	debugMsg += wxT("sender nick = ") + param->m_senderNick + wxT("\n");
	debugMsg += wxT("temp file path = ") + param->m_tempFilePath + wxT("\n");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionBegin(	param->m_mediaId,
											param->m_fileName,
											param->m_tempFilePath,
											param->m_senderNick,
											param->m_fileSize,
											param->m_packetSize);

	// Enable notifications
	m_FTReceiveNotificationsEnable = true;

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** Triggered on File Transfer reception begin when initiated from the White Board module.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionWBBegin(wxCommandEvent &event)
{
	FTReceptionBeginParameter *param = (FTReceptionBeginParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Begin Whiteboard file transfer:\n");
	wxString mediaIDStr;
	mediaIDStr.Printf(wxT("mediaID = %d\n"), param->m_mediaId);
	debugMsg += mediaIDStr;
	debugMsg += wxT("file name = ") + param->m_fileName + wxT("\n");
	wxString fileSizeStr;
	fileSizeStr.Printf(wxT("file size = %d bytes\n"), param->m_fileSize);
	debugMsg += fileSizeStr;
	wxString packetSizeStr;
	packetSizeStr.Printf(wxT("packet size = %d bytes\n"), param->m_packetSize);
	debugMsg += packetSizeStr;
	debugMsg += wxT("sender nick = ") + param->m_senderNick + wxT("\n");
	debugMsg += wxT("temp file path = ") + param->m_tempFilePath + wxT("\n");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionBegin(	param->m_mediaId,
											param->m_fileName,
											param->m_tempFilePath,
											param->m_senderNick,
											param->m_fileSize,
											param->m_packetSize,
											false);

	// Enable notifications
	m_FTReceiveNotificationsEnable = true;

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** File Transfer progress update event.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionProgress(wxCommandEvent &event)
{
	if(!m_FTReceiveNotificationsEnable)
		return;

	FTProgressParameter *param = (FTProgressParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg.Printf(wxT("File Transfer Receive Progress - %d of %d"), param->m_currentPacketIndex, param->m_lastPacketIndex);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionProgress(param->m_currentPacketIndex, param->m_lastPacketIndex);

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** File Transfer lost packet event.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionLostPacket(wxCommandEvent &event)
{
	if(!m_FTReceiveNotificationsEnable)
		return;

	FTLostPacketParameter *param = (FTLostPacketParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg.Printf(wxT("File Transfer Receive Packet Loss - %d"), param->m_lostPackets);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionLostPacket(param->m_lostPackets);

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** Triggered when File Transfer reception ends with success.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionSuccess(wxCommandEvent &event)
{
	if(!m_FTReceiveNotificationsEnable)
		return;

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Receive Success");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionSuccess();
}

/** Triggered when File Transfer reception ends with failure.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferReceptionError(wxCommandEvent &event)
{
	if(!m_FTReceiveNotificationsEnable)
		return;

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Receive Error");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferReceptionError();
}

/** Cancels file reception.
 * @param[in] mediaId	Media ID of the file transfer.
 * @return true on success, false on failure
 */
bool IPTVClientApp::FileTransferCancelReception(long mediaId)
{
	// If user want's to stop the reception we don't need more notifications
	m_FTReceiveNotificationsEnable = false;
	return m_kernel.FileTransferCancelReception(mediaId);
}

/** Enable File Transfer.
 */
void IPTVClientApp::EnableFileTransfer()
{
	m_kernel.EnableFileTransfer();
}

/** Disable File Transfer.
 */
void IPTVClientApp::DisableFileTransfer()
{
	m_kernel.DisableFileTransfer();
}

/** Initiates an outgoing File Transfer.
 * @param[in] channel	Channel on which to operate.
 * @param[in] filePath	Full path of the file to be sent.
 * @param[in] fileSize	Size of the file.
 * @param[in] bitRate	Bit rate limit for the transfer.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::FileTransferBeginTransmission(const wxString &channel, const wxString &filePath, 
												  unsigned long fileSize, unsigned bitRate)
{
	EnableFileTransfer();
	bool ret = m_kernel.FileTransferBeginTransmission(WX_TO_CSTR(channel), WX_TO_FILE_CSTR(filePath), fileSize, bitRate);	

	if (!ret)
	{
#ifdef IPTV_DEBUG
		wxString debugMsg;
		debugMsg = wxT("ERROR while File Transfer Begin Transmission\n");
		debugMsg += wxT("Channel = ") + channel + wxT("\n");
		debugMsg += wxT("File Path = ") + filePath + wxT("\n");
		wxString fileSzStr;
		fileSzStr.Printf(wxT("File Size = %d\n"), fileSize);
		debugMsg += fileSzStr;
		wxString bitrateStr;
		bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
		debugMsg += bitrateStr;
		debugMsg += wxT("Canceling File Transfer Begin Transmission\n");
		SendDebugEvent(debugMsg);
#endif		
		return ret;
	}

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Begin Transmission\n");
	debugMsg += wxT("Channel = ") + channel + wxT("\n");
	debugMsg += wxT("File Path = ") + filePath + wxT("\n");
	wxString fileSzStr;
	fileSzStr.Printf(wxT("File Size = %d\n"), fileSize);
	debugMsg += fileSzStr;
	wxString bitrateStr;
	bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
	debugMsg += bitrateStr;
	SendDebugEvent(debugMsg);
#endif

	return ret;
}

/** Initiates an outgoing File Transfer for White Board.
 * @param[in] channel	Channel on which to operate.
 * @param[in] filePath	Full path of the file to be sent.
 * @param[in] fileSize	Size of the file.
 * @param[in] bitRate	Bit rate limit for the transfer.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::FileTransferBeginWhiteboardTransmission(const wxString &channel, const wxString &filePath, 
															unsigned long fileSize, unsigned bitRate)
{
	EnableFileTransfer();
	bool ret = m_kernel.FileTransferBeginWhiteboardTransmission(WX_TO_CSTR(channel), WX_TO_FILE_CSTR(filePath), fileSize, bitRate);	

	if (!ret)
	{
#ifdef IPTV_DEBUG
		wxString debugMsg;
		debugMsg = wxT("ERROR while File Transfer Begin Whiteboard Transmission\n");
		debugMsg += wxT("Channel = ") + channel + wxT("\n");
		debugMsg += wxT("File Path = ") + filePath + wxT("\n");
		wxString fileSzStr;
		fileSzStr.Printf(wxT("File Size = %d\n"), fileSize);
		debugMsg += fileSzStr;
		wxString bitrateStr;
		bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
		debugMsg += bitrateStr;
		debugMsg += wxT("Canceling File Transfer Begin Whiteboard Transmission\n");
		SendDebugEvent(debugMsg);
#endif		
		return ret;
	}

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Begin Whiteboard Transmission\n");
	debugMsg += wxT("Channel = ") + channel + wxT("\n");
	debugMsg += wxT("File Path = ") + filePath + wxT("\n");
	wxString fileSzStr;
	fileSzStr.Printf(wxT("File Size = %d\n"), fileSize);
	debugMsg += fileSzStr;
	wxString bitrateStr;
	bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
	debugMsg += bitrateStr;
	SendDebugEvent(debugMsg);
#endif

	return ret;
}

/** Cancels outgoing File Transfer.
 * @param[in] mediaId	Media ID of the transfer.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::FileTransferCancelTransmission(long mediaId)
{
	bool ret = m_kernel.FileTransferCancelTransmission(mediaId);
	DisableFileTransfer();
	
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Cancel Transmission\n");
	SendDebugEvent(debugMsg);
#endif
	return ret;
}

//Playlist Transmission
/** Begin transmission of a movie.
 * @param[in] channel	Channel on which to operate.
 * @param[in] filePath	Full path of the file to transmit.
 * @param[in] bitRate	Bit rate limit for the transmission.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::MovieBeginTransmission(const wxString &channel, const wxString &filePath, unsigned bitRate)
{
	bool ret = m_kernel.MovieBeginTransmission(WX_TO_CSTR(channel), WX_TO_FILE_CSTR(filePath),
											   bitRate);

	if (!ret)
	{
#ifdef IPTV_DEBUG
		wxString debugMsg;
		debugMsg = wxT("ERROR while Playlist Movie Begin Transmission\n");
		debugMsg += wxT("Channel = ") + channel + wxT("\n");
		debugMsg += wxT("File Path = ") + filePath + wxT("\n");
		wxString bitrateStr;
		bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
		debugMsg += bitrateStr;
		debugMsg += wxT("Canceling Playlist Movie Begin Transmission\n");
		SendDebugEvent(debugMsg);
#endif		
		return ret;
	}

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Playlist Movie Begin Transmission\n");
	debugMsg += wxT("Channel = ") + channel + wxT("\n");
	debugMsg += wxT("File Path = ") + filePath + wxT("\n");
	wxString bitrateStr;
	bitrateStr.Printf(wxT("Bitrate = %d\n"), bitRate);
	debugMsg += bitrateStr;
	SendDebugEvent(debugMsg);
#endif

	return ret;
}

/** Cancels a movie transmission.
 * @param[in] mediaId	Media ID of the transmission.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::MovieCancelTransmission(long mediaId)
{
	bool ret = m_kernel.MovieCancelTransmission(mediaId);

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Playlist Movie Cancel Transmission\n");
	SendDebugEvent(debugMsg);
#endif

	return ret;
}

/** Triggered on outgoing File Transfer begin.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendBegin(wxCommandEvent &event)
{
	FTSendBeginParameter *param = (FTSendBeginParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg.Printf(wxT("File Transfer Send Begin - MediaID = %d\n"), param->m_mediaId);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTranferSendBegin(param->m_mediaId);

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** Outgoing File Transfer progress update event.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendProgress(wxCommandEvent &event)
{
	FTProgressParameter *param = (FTProgressParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg.Printf(wxT("File Transfer Send Progress - %d of %d"), param->m_currentPacketIndex, param->m_lastPacketIndex);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendProgress(param->m_currentPacketIndex, param->m_lastPacketIndex);

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** Outgoing File Transfer lost packet event.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendLostPacket(wxCommandEvent &event)
{
	FTLostPacketParameter *param = (FTLostPacketParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send lost Packets\n");
	debugMsg.Printf(wxT("LostPacket = %d\n"), param->m_lostPackets);
	debugMsg.Printf(wxT("LostPacketTotal = %d\n"), param->m_lostPacketsTotal);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendLostPacket(param->m_lostPackets, param->m_lostPacketsTotal);

	// Delete structure in the heap now that we're done with it
	delete param;
}

/** Triggered on end-of-file in outgoing File Transfer.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendEof(wxCommandEvent &event)
{
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send EOF");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendEof();
}

/** Triggered when the outgoing File Transfer ends with success.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendSuccess(wxCommandEvent &event)
{
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send Success");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendSuccess();
}

/** Triggered when the outgoing File Transfer ends with failure.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendError(wxCommandEvent &event)
{
#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send Error");
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendError();
}

/** This is triggered when an user joins the channel in the middle of an outgoing File Transfer.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendReceiverNew(wxCommandEvent &event)
{
	FTSendMediaIdParameter *param = (FTSendMediaIdParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send Receiver New\n");
	debugMsg.Printf(wxT("MediaId = %d\n"), param->m_mediaId);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendReceiverNew(param->m_mediaId);

	delete param;
}

/** This is triggered when a remote user cancels reception of an outgoing File Transfer.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendReceiverCancel(wxCommandEvent &event)
{
	FTSendMediaIdParameter *param = (FTSendMediaIdParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send Receiver Cancel\n");
	debugMsg.Printf(wxT("MediaId = %d\n"), param->m_mediaId);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendReceiverCancel(param->m_mediaId);

	delete param;
}

/** This is triggered when all remote users finished reception of an outgoing File Transfer.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnFileTransferSendReceiverEnd(wxCommandEvent &event)
{
	FTSendMediaIdParameter *param = (FTSendMediaIdParameter *)event.GetClientData();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("File Transfer Send Receiver End\n");
	debugMsg.Printf(wxT("MediaId = %d\n"), param->m_mediaId);
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnFileTransferSendReceiverEnd(param->m_mediaId);

	delete param;	
}

//Movie Playlist events
/** Triggered when a movie transmission begins.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMovieSendBegin(wxCommandEvent &event)
{	
	long mediaId = event.GetExtraLong();
	wxString channelName = event.GetString();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Movie Send Begin\n");
	wxString mediaIdStr;
	mediaIdStr.Printf(wxT("MediaId = %d\n"), mediaId);
	debugMsg += mediaIdStr;
	wxString channelMsg;	
	channelMsg.Printf(wxT("Channel = %s\n"), channelName.c_str());
	debugMsg += channelMsg;
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnMovieSendBegin(mediaId, channelName);
}

/** Triggered when a movie transmission ends.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnMovieSendEof(wxCommandEvent &event)
{
	long mediaId = event.GetExtraLong();
	wxString channelName = event.GetString();

#ifdef IPTV_DEBUG
	wxString debugMsg;
	debugMsg = wxT("Movie Send Eof\n");
	wxString mediaIdStr;
	mediaIdStr.Printf(wxT("MediaId = %d\n"), mediaId);
	debugMsg += mediaIdStr;
	wxString channelMsg;	
	channelMsg.Printf(wxT("Channel = %s\n"), channelName.c_str());
	debugMsg += channelMsg;
	SendDebugEvent(debugMsg);
#endif

	m_skin->OnMovieSendEof(mediaId, channelName);
}

// Poll methods
// ------------
// Event handlers
/** Triggered when a Poll is received.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnPoll(wxCommandEvent &event)
{
	if(m_activePoll)
		delete m_activePoll;
	m_activePoll = new PollInfo;
	m_activePoll->SetPollStarter(WX_FROM_CSTR(m_pollData.pollOwner));
	m_activePoll->SetQuestionText(WX_FROM_CSTR(m_pollData.question));
	std::list<PollAnswer>::iterator it;
	PollItem item;
	// NOTE: The list comes sorted from the event emitter (OnKernelNotify)
	for(it = m_pollData.answerList.begin(); it != m_pollData.answerList.end(); it++)
	{
		if(it->index >= 0)
		{
			item.SetText(WX_FROM_CSTR(it->text));
			m_activePoll->AddAnswerItem(item);
		}
	}
	// TODO: Communicate to Skin.
	m_skin->OnPoll();
}

/** Triggered when a Poll vote (answer) is received.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnPollAnswer(wxCommandEvent &event)
{
	int index = (int)event.GetExtraLong();
	if(!m_activePoll)
		return;			// Don't accept if no poll
	m_activePoll->AddVote(index);
	// TODO: Communicate to Skin.
	m_skin->OnPollVote();
}

/** Triggered when Poll stats are received.
 * @param[in] event	Event object.
 */
void IPTVClientApp::OnPollStats(wxCommandEvent &event)
{
	PollInfo info;
	info.SetPollStarter(WX_FROM_CSTR(m_pollData.pollOwner));
	info.SetQuestionText(WX_FROM_CSTR(m_pollData.question));
	std::list<PollAnswer>::iterator it;
	PollItem item;
	// NOTE: The list comes sorted from the event emitter (OnKernelNotify)
	for(it = m_pollData.answerList.begin(); it != m_pollData.answerList.end(); it++)
	{
		item.ClearAnswerCount();
		if(it->index >= 0)
		{
			if(it->text != "")
			{
				item.SetText(WX_FROM_CSTR(it->text));
				item.IncreaseAnswerCount(it->answerCount);
				info.AddAnswerItem(item);
			}
		}
		else	// blank answer
		{
			info.AddVote(-1, it->answerCount);
		}
	}
	info.ComputeTotalVotes();
	// TODO: Communicate to Skin.
	m_skin->OnPollStats(info);
}

// Command methods
// ---------------
/** AppInterface method to send a Poll.
 * @param[in] Pointer to the PollInfo structure containing Poll to be sent.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::SendPoll(PollInfo *poll)
{
	// Sanity checks
	if(m_appConfig.GetMode() != APP_MODE_CONFERENCE)
		return false;		// Can only send poll while in CONFERENCE mode.
	if(!m_currentChannel)
		return false;		// Can only send poll to current channel
	if(!poll)
		return false;		// No poll to send
	if(!poll->IsValid())
		return false;		// Invalid poll
	const ChannelUser *user = GetCurrentChannelUser();
	if(!user)
		return false;		// No user to start poll
	if(!user->HasMediaVoice())
		return false;		// Only the user with media voice can start a poll
	
	// Prevent skin from "cheating" by passing a pre-filled structure here
	poll->ClearVotes();

	// After the sanity checks, do the real thing
	PollData pollData;
	PollAnswer answer;
	// Fill up the structure
	pollData.pollOwner = WX_TO_CSTR(user->GetNickName());
	pollData.channel = WX_TO_CSTR(m_currentChannel->GetName());
	pollData.question = WX_TO_CSTR(poll->GetQuestionText());
	for(unsigned i = 0; i < poll->GetAnswerItemCount(); i++)
	{
		answer.index = i;
		answer.text = WX_TO_CSTR(poll->GetAnswerItem(i).GetText());
		pollData.answerList.push_back(answer);
	}
	// and hand it over to Kernel
	if(m_kernel.PollSend(pollData))
	{
		ResetActivePoll();
		poll->SetNumberOfUsersSentTo(m_currentChannel->GetUserCount() - 1);
		m_activePoll = poll;
		return true;
	}
	return false;
}

/** AppInterface method to vote on a Poll.
 * @param[in] index	Index of the item to vote on. Set a negative value for a blank vote.
 */
void IPTVClientApp::SendPollAnswer(int index)
{
	if(m_appConfig.GetMode() != APP_MODE_CONFERENCE)
		return;				// All of this only makes sense in CONFERENCE mode
	if(!m_activePoll)		
		return;				// Can't answer if there is no poll
	if(!m_activePoll->IsValid())
		return;				// Invalid poll
	const ChannelUser *user = GetCurrentChannelUser();
	wxString pollStarter = m_activePoll->GetPollStarter();
	if(pollStarter == user->GetNickName())
		return;				// I can't answer a poll started by myself
	if(index >= (int)m_activePoll->GetAnswerItemCount())
		return;				// Invalid answer. It must be within index range, or negative for blank.
	
	m_kernel.PollSendAnswer(WX_TO_CSTR(pollStarter), index);	// All set, fire away
}

/** AppInterface method to send stats for the active Poll.
 */
void IPTVClientApp::SendPollStats()
{
	if(!m_activePoll)
		return;				// No poll to send stats about
	if(!m_activePoll->IsValid())
		return;				// Invalid poll
	const ChannelUser *user = GetCurrentChannelUser();
	wxString pollStarter = m_activePoll->GetPollStarter();
	if(pollStarter != user->GetNickName())
		return;				// I can only send stats for a poll started by myself

	// After the sanity checks, do the real thing
	PollData pollData;
	PollAnswer answer;
	// Fill up the structure
	pollData.pollOwner = WX_TO_CSTR(user->GetNickName());
	pollData.channel = WX_TO_CSTR(m_currentChannel->GetName());
	pollData.question = WX_TO_CSTR(m_activePoll->GetQuestionText());
	// The loop starts from -1 to include the blank answer in the stats.
	for(int i = -1; i < (int)m_activePoll->GetAnswerItemCount(); i++)
	{
		answer.index = i;
		answer.text = WX_TO_CSTR(m_activePoll->GetAnswerItem(i).GetText());
		answer.answerCount = m_activePoll->GetAnswerItem(i).GetAnswerCount();
		pollData.answerList.push_back(answer);
	}
	// and hand it over to Kernel
	m_kernel.PollSendStats(pollData);	
}

/** AppInterface method to get the active poll data.
 * @return Pointer to the current Poll data.
 */
const PollInfo *IPTVClientApp::GetActivePoll() const
{
	return m_activePoll;
}

/** Private method to reset active poll.
 */
void IPTVClientApp::ResetActivePoll()
{
	if(m_activePoll)
		delete m_activePoll;
	m_activePoll = NULL;

	ResetPollIntermediateData();
}

/** Private method that resets Poll's intermediate structures.
 */
void IPTVClientApp::ResetPollIntermediateData()
{
	m_pollData.channel = "";
	m_pollData.pollOwner = "";
	m_pollData.question = "";
	m_pollData.answerList.clear();
}

/** Sends a White Board message.
 * @param[in] data	Data to be sent in message.
 * @return true on success, false on failure.
 */
bool IPTVClientApp::WhiteboardSendMessage(char* data)
{
	VBString strData = data;
	VBString strChannel = WX_TO_CSTR(m_currentChannel->GetName());

	m_kernel.SendChannelWhiteboard(strChannel, strData);
	return true;
}
