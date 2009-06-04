#ifndef _IPTV_VIEWER_APP_
#define _IPTV_VIEWER_APP_

// #define SKIN_API

/** @mainpage
 *
 * This is the IP.TV Cross-Platform Client Documentation.
 */



#include <wx/wx.h>


#include <VBLib/VBLib.h>

#include <list>
#include <deque>

#include "Skin.h"
#include "AppInterface.h"
#include "Channel.h"
#include "User.h"
#include "ChannelUser.h"
#include "ChannelList.h"
#include "UserList.h"
#include "AppConstants.h"
#include "AppConfig.h"
#include "TimeoutRegister.h"
#include "TransmissionManager.h"
#include "SettingsManager.h"
#include "VideoSink.h"
#include "CamVideoSource.h"
#include "AppSharingVideoSource.h"
#include "MediaDeviceItem.h"
#include "PollInfo.h"
#include "iptv_kernel/IPTVKernel.h"
#include "iptv_kernel/KernelNotify.h"

#ifdef IPTV_DEBUG
#include "DebugWindow.h"
#endif

// this is a queue of user names only
// attempting to deal with a strange segmentation fault issue
typedef std::deque<wxString> UserQueue;

using namespace br::com::sbVB::VBLib;

// A set of wxCommandEvents to be used by App::OnKernelNotify()
// Done to avoid GUI calls from different threads
BEGIN_DECLARE_EVENT_TYPES()
	// IRC events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CONNECTED_HOST, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CONNECTED_CHAT, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_DISCONNECTED, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AUTHENTICATED, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AUTHENTICATION_ERROR, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_QUERY_ITEM, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_QUERY_END, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_USER_QUERY_ITEM, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_USER_QUERY_END, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_USER_MODE_ON, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_USER_MODE_OFF, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_JOIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_PART, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_MODE_ON, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_MODE_OFF, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_USER_FLAGS_ON, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_USER_FLAGS_OFF, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_BAD_PASSWORD, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_PRIV_MSG, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CHANNEL_MSG, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_USER_QUIT, -1)
#ifdef IPTV_DEBUG
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_DEBUG, -1)
#endif
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_TIMEOUT, -1)

	// Media events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_NEW_TRANSMISSION, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_MEDIA_INFO, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AUDIO_ALIVE, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_VIDEO_ALIVE, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_MEDIA_PROGRESS, -1)
	// Capture event
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_CAPTURE_NEW_FRAME, -1)
	// File Transfer events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_WB_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_PROGRESS, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_LOST_PACKET, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_SUCCESS, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_RECEIVE_ERROR, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_PROGRESS, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_LOST_PACKET, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_EOF, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_SUCCESS, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_ERROR, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_NEW, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_CANCEL, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_FT_SEND_RECEIVER_END, -1)
	// Movie Playlist events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_MOVIE_SEND_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_MOVIE_SEND_EOF, -1)
	// Poll events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_QUESTION, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_OPTION, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_END, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_ANSWER, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_STATS_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_STATS_QUESTION, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_STATS_OPTION, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_POLL_STATS_END, -1)
	// Application Sharing events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AS_RECEIVE_BEGIN, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AS_RECEIVE_NEW_FRAME, -1)
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_AS_RECEIVE_END, -1)	
	// Whiteboard events
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_WB_WBOARD_CHANNEL, -1)

END_DECLARE_EVENT_TYPES()


/** @brief Main application class.
 *
 * This is the application's main class. It implements AppInterface, and performs all
 * initialization, setup and most of the business rule operation.
 *
 */
class IPTVClientApp: public wxApp, public AppInterface, public KernelNotify
{
private:
	Skin *m_skin;
	IPTVKernel m_kernel;
	wxLocale *m_locale;
	VBCrossDLL m_vbCrossDll;
	SettingsManager *m_settingsManager;

	wxString m_skinSelected;
	wxString m_languageSelected;
	int m_selectedLanguageIndex;

	bool m_FTReceiveNotificationsEnable;

#ifdef IPTV_DEBUG
	DebugWindow *m_debugWindow;
#endif

	// Holds the general application state.
	AppConfig m_appConfig;

	// Global application timer
	wxTimer m_appGlobalTimer;
	// Timeout register
	TimeoutRegister m_timeoutRegister;
	// Capture VideoSource object - provides capture video frames
	CamVideoSource m_camVideoSource;
	// App Sharing VideoSource object - provides App Sharing video frames
	AppSharingVideoSource m_asVideoSource;
	// Media transmission manager
	TransmissionManager m_transmissionManager;
	// Transmission handling toggle
	bool m_enableTransmissions;

	Channel *m_currentChannel;
	User *m_currentUser;

	ChannelList m_channelList;

	// Series Collaboration
	bool m_seriesCollabStarted;		// control variable
	bool m_scSelectAllUsers;		// auto-select all available users?
	wxDateTime m_scLastSwitchTime;	// last switch time
	wxTimeSpan m_scSwitchInterval;	// switch interval
	UserQueue m_seriesCollabQueue;	// "waiting line"

	bool m_isTransmitting;		// Flag set when we are transmitting
	PollInfo *m_activePoll;		// Data for the currently active poll.
	PollData m_pollData;

	wxDateTime m_timeStarted;	// Time when app was started
	wxDateTime m_timeConnected;	// Time when server connected
	wxTimeSpan m_sessionTime;	// Session time when connected

	// Kernel states
	IRMConnectionState m_lastConnectionState;
	IRMChannelJoinState m_lastJoinState;

	// Custom application event methods
	void SendCommandEvent(const wxEventType &type, wxString kernelMessage = wxEmptyString, void *clientData = NULL, int kernelState = -1);
	void SendCommandEvent(const wxEventType &type, long extraLong, wxString kernelMessage = wxEmptyString);
	void SendIRMUserModeEvent(int modeID, const KernelModeParam &param);
	void SendIRMChannelModeEvent(int modeID, const KernelModeParam &param);
	void SendIRMChannelUserFlagsEvent(int modeID, const KernelModeParam &param);

	// Series Collaboration helper function
	void SeriesCollaborationPass();

	// Own voice request handling
	void SetOwnVoiceRequest();
	void ClearOwnVoiceRequest();

	// Debugging (NOTE: functions listed under "Debugging" do nothing on release builds)
	void SendDebugEvent(wxString message, void *clientData = NULL);

	// List handling
	void ClearChannelList();
	void ClearUserList();

	// Poll handling
	void ResetPollIntermediateData();

	// Transmission handling
	void DisableTransmissions();
	void EnableTransmissions();
	bool TransmissionsEnabled() const;
	void AttachTransmissionVideoSink(Transmission *transm, ChannelUser *user = NULL, const wxString &userName = wxEmptyString);

	// Skin methods
	bool LoadDefaultSkin();

public:
	IPTVClientApp();
	virtual ~IPTVClientApp();
	virtual bool OnInit();
	virtual int OnExit();


	Skin *GetSkin(){return m_skin;}

	// Skin methods
	StringHash GetAvailableSkins();
	bool LoadSkin(wxString skinLibrary);

	void SetSkinSelected(wxString skinSelected);
	wxString GetSkinSelected();
	void SetLanguageSelected(wxString languageSelected);
	wxString GetLanguageSelected();


	// Language methods
	void SelectLanguage(int language, bool savePrevious = false);
	wxArrayString GetCanonicalNamesList();
	wxArrayInt GetAvailableLanguageCodes();
	int  GetLanguageCode();
	wxString GetLanguageName(int language = -1);
	wxString GetLanguageCanonicalName();
	wxString Translate(const wxString &message);

	// File system methods
	wxString GetAppPath();
	wxString GetAppFileName();

	// Configuration management
	const AppConfig &GetAppConfig() const;
	void SetAppConfig(const AppConfig &config);

	// Channel/user management
	const Channel *GetCurrentChannel() const;
	const ChannelUser *GetCurrentChannelUser() const;
	User *GetCurrentUser();

	// Transmission management
	unsigned GetTransmissionCount();

	// Channel List methods
	ChannelList &GetChannelList();

	// User List methods
	UserList *GetUserList();

	// TransmissionList sorting compare function
	static bool TransmissionCompare(Transmission *left, Transmission *right);

	// iptv_kernel notifications
	bool OnKernelNotify(KernelParam &param);

	// Time methods
	const wxDateTime &GetAppStartTime() const;
	const wxTimeSpan &GetSessionTime() const;

	// Series collaboration control
	void AddToSeriesCollaboration(const wxString &userName);
	void StartSeriesCollaboration(const wxTimeSpan &interval = SC_DEFAULT_SWITCH_INTERVAL, bool selectAllUsers = false);
	void StopSeriesCollaboration();
	bool IsSeriesCollaborationStarted() const;

	// App->kernel function implementations
	wxString GetTempDirectory();
	// Mode initialization
	void InitMulticast(const wxString &address, unsigned port);
	void InitFilePlayback(const wxString &filename);
	bool InitializeConference(wxString channel);

	// File playback control
	void FileSeek(float percent);
	void FilePlay();
	void FilePause();

	// File Transfer Reception
	bool FileTransferCancelReception(long mediaId);
	void EnableFileTransfer();
	void DisableFileTransfer();

	// File Transfer Send
	bool FileTransferBeginTransmission(const wxString &channel, const wxString &filePath, 
									   unsigned long fileSize, unsigned bitRate);
	bool FileTransferBeginWhiteboardTransmission(	const wxString &channel, const wxString &filePath, 
													unsigned long fileSize, unsigned bitRate);
	bool FileTransferCancelTransmission(long mediaId);

	//Playlist Transmission
	bool MovieBeginTransmission(const wxString &channel, const wxString &filePath, unsigned bitRate);
	bool MovieCancelTransmission(long mediaId);

	// Poll
	bool SendPoll(PollInfo *poll);
	void SendPollAnswer(int index);
	void SendPollStats();
	const PollInfo *GetActivePoll() const;
	void ResetActivePoll();

	// Whiteboard
	bool WhiteboardSendMessage(char* data);
	
	// Application Sharing
	void RegisterAppSharingVideoSink(VideoSink *sink);
	void UnregisterAppSharingVideoSink(VideoSink *sink);
	void UnregisterAllAppSharingVideoSinks();

	// Conference mode control
	void RequestConnection(const wxString &server, const wxString &serverPass, 
		const wxString &user, const wxString &pass, 
		const wxString &realName = DEFAULT_REAL_NAME, const wxString &email = DEFAULT_EMAIL);
	void CloseConnection();
	void RequestChannelList();
	void JoinChannel(const wxString &channel, const wxString &password = wxEmptyString);
	void LeaveChannel(const wxString &channel);
	void SendPrivateMessage(const wxString &user, const wxString &message);
	void SendChannelMessage(const wxString &channel, const wxString &message);
	void TerminateAllConferences();
	// Video
	bool GetVideoFrame(unsigned long id, unsigned char **data, unsigned long *width, unsigned long *height);
	// Capture control
	bool InitializeCapture();
	void ReleaseCapture();
	bool InitializeCaptureDevice(int deviceId);
	void ReleaseCaptureDevice();
	void RegisterCaptureVideoSink(VideoSink *sink);
	void UnregisterCaptureVideoSink(VideoSink *sink);
	void UnregisterAllCaptureVideoSinks();
	bool IsCaptureEnabled() const;
	//Recording control
	void EnableConferenceRecording(const wxString &path);
	void DisableConferenceRecording();

	// Outgoing transmission interface
	void InitializeMediaColaboration(const long &bitrate);
	void StopMediaColaboration();
	bool IsTransmittingMedia() const;

	// Device/codec functions
	void GetVideoDeviceList(std::list<VideoDeviceItem> &deviceList);
	void GetAudioDeviceList(std::list<AudioDeviceItem> &deviceList);
	void GetVideoCodecList(std::list<wxString> &codecList, std::list<wxString> &videoBitrateList);
	void GetAudioCodecList(std::list<wxString> &codecList);

	// get settings manager
	SettingsManager *GetSettingsManager();

	// Send text to debug window
	void SetDebugMessage(const wxString &message);

	// Version strings
	const wxString GetClientVersion() const;
	const wxString GetClientBuildID() const;
	const wxString GetClientBuildDateStr() const;
	const wxString GetCoreVersion();

	// About dialog
	void ShowAboutDialog();

	// iptv_kernel notification event handlers
	//----------------------------------------
	// IRC events
	void OnHostConnect(wxCommandEvent &event);
	void OnChatConnect(wxCommandEvent &event);
	void OnDisconnect(wxCommandEvent &event);
	void OnAuthenticationSuccess(wxCommandEvent &event);
	void OnChannelQueryItem(wxCommandEvent &event);
	void OnChannelQueryEnd(wxCommandEvent &event);
	void OnUserQueryItem(wxCommandEvent &event);
	void OnUserQueryEnd(wxCommandEvent &event);
	void OnUserMode(wxCommandEvent &event);
	void OnChannelJoin(wxCommandEvent &event);
	void OnChannelPart(wxCommandEvent &event);
	void OnChannelMode(wxCommandEvent &event);
	void OnChannelUserFlags(wxCommandEvent &event);
	void OnPrivateMessage(wxCommandEvent &event);
	void OnChannelMessage(wxCommandEvent &event);
	void OnUserQuit(wxCommandEvent &event);
	// Media events
	void OnNewTransmission(wxCommandEvent &event);
	void OnMediaInfo(wxCommandEvent &event);
	void OnMediaAudioAlive(wxCommandEvent &event);
	void OnMediaVideoAlive(wxCommandEvent &event);
	void OnMediaProgressNotify(wxCommandEvent &event);
	// Capture event
	void OnCaptureNewFrame(wxCommandEvent &event);
	// File Transfer events
	void OnFileTransferReceptionBegin(wxCommandEvent &event);
	void OnFileTransferReceptionWBBegin(wxCommandEvent &event);
	void OnFileTransferReceptionProgress(wxCommandEvent &event);
	void OnFileTransferReceptionLostPacket(wxCommandEvent &event);
	void OnFileTransferReceptionSuccess(wxCommandEvent &event);
	void OnFileTransferReceptionError(wxCommandEvent &event);
	void OnFileTransferSendBegin(wxCommandEvent &event);
	void OnFileTransferSendProgress(wxCommandEvent &event);
	void OnFileTransferSendLostPacket(wxCommandEvent &event);
	void OnFileTransferSendEof(wxCommandEvent &event);
	void OnFileTransferSendSuccess(wxCommandEvent &event);
	void OnFileTransferSendError(wxCommandEvent &event);
	void OnFileTransferSendReceiverNew(wxCommandEvent &event);
	void OnFileTransferSendReceiverCancel(wxCommandEvent &event);
	void OnFileTransferSendReceiverEnd(wxCommandEvent &event);
	//Movie Playlist events
	void OnMovieSendBegin(wxCommandEvent &event);
	void OnMovieSendEof(wxCommandEvent &event);
	// Poll events
	void OnPoll(wxCommandEvent &event);
	void OnPollAnswer(wxCommandEvent &event);
	void OnPollStats(wxCommandEvent &event);
	// Application Sharing events
	void OnAppSharingBegin(wxCommandEvent &event);
	void OnAppSharingEnd(wxCommandEvent &event);
	void OnAppSharingNewFrame(wxCommandEvent &event);
	// Whiteboard events
	void OnWhiteboardChannel(wxCommandEvent &event);
	// IRC error events
	void OnAuthenticationFailure(wxCommandEvent &event);
	void OnChannelJoinFailure(wxCommandEvent &event);
	// Timeout handlers
	void OnMediaAudioTimeout(wxCommandEvent &event);
	void OnMediaVideoTimeout(wxCommandEvent &event);
	// Timer event
	void OnTimer(wxTimerEvent &event); 
	// Skin events
	void OnSkinMainWindowClose(wxCloseEvent &event);

	// IRM mode manipulation
	void RequestSetChannelOperator(wxString channel, wxString user);
	void RequestUnsetChannelOperator(wxString channel, wxString user);

	void RequestSetChannelPrivate(wxString channel);
	void RequestUnsetChannelPrivate(wxString channel);

	void RequestSetChannelSecret(wxString channel);
	void RequestUnsetChannelSecret(wxString channel);

	void RequestSetChannelInviteOnly(wxString channel);
	void RequestUnsetChannelInviteOnly(wxString channel);

	void RequestSetChannelOpChangeTopic(wxString channel);
	void RequestUnsetChannelOpChangeTopic(wxString channel);

	void RequestSetChannelNoExternalMessages(wxString channel);
	void RequestUnsetChannelNoExternalMessages(wxString channel);

	void RequestSetChannelModerated(wxString channel);
	void RequestUnsetChannelModerated(wxString channel);

	void RequestSetChannelUserLimit(wxString channel, int limit);
	void RequestUnsetChannelUserLimit(wxString channel);

	void RequestSetChannelMediaCollaborator(wxString channel, wxString user);
	void RequestUnsetChannelMediaCollaborator(wxString channel, wxString user);

	void RequestSetChannelKey(wxString channel, wxString passwd);
	void RequestUnsetChannelKey(wxString channel);

	void RequestSetChannelBitRate(wxString channel, int bitRate);
	void RequestUnsetChannelBitRate(wxString channel);

	void RequestSetChannelAutoChangeBitRate(wxString channel);
	void RequestUnsetChannelAutoChangeBitRate(wxString channel);

	void RequestSetChannelMultipleTransmission(wxString channel);
	void RequestUnsetChannelMultipleTransmission(wxString channel);

	void RequestSetChannelOneAudio(wxString channel, int audioSwitchTime);
	void RequestUnsetChannelOneAudio(wxString channel);

	void RequestSetChannelAudioMute(wxString channel);
	void RequestUnsetChannelAudioMute(wxString channel);

	void RequestSetChannelBandShareLimit(wxString channel, int limit);
	void RequestUnsetChannelBandShareLimit(wxString channel);

	void RequestSetChannelTransmissionLimit(wxString channel, int limit);
	void RequestUnsetChannelTransmissionLimit(wxString channel);

	void RequestSetUserInvisible();
	void RequestUnsetUserInvisible();

	void RequestSetUserCameraOn();
	void RequestUnsetUserCameraOn();

	void RequestSetUserMicMute();
	void RequestUnsetUserMicMute();

	void RequestSetUserAway();
	void RequestUnsetUserAway();

	void RequestSetMediaVoice(const wxString &user);
	void RequestUnsetMediaVoice();

	void SendMediaVoiceRequest();
	void SendMediaVoiceRequestCancel();
	void SendMediaVoiceRequestRemoveAll();

	void RequestSetTopic(wxString channel, wxString topic);

#ifdef IPTV_DEBUG
	// Debug event
	void OnDebugEvent(wxCommandEvent &event);
#endif

	DECLARE_EVENT_TABLE()
};

enum{
	ID_APP_TIMER = 20000
};

DECLARE_APP(IPTVClientApp)

#endif
