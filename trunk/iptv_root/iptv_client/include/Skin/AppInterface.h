#ifndef APPINTERFACE_H
#define APPINTERFACE_H

#include <wx/wx.h>

#include <list>

#include "User.h"
#include "ChannelUser.h"
#include "Channel.h"
#include "ChannelList.h"
#include "Constants.h"
#include "AppConfig.h"
#include "SettingsManager.h"
#include "VideoSink.h"
#include "MediaDeviceItem.h"
#include "PollInfo.h"
//#include "DebugWindow.h"

WX_DECLARE_STRING_HASH_MAP(wxString, StringHash);

/** @brief Defines application functions to be used by the skins.
 *
 * This abstract class is an indirection layer that further isolates the Skin from the App,
 * avoiding the need for duplicate IPTVClientApp.cpp (which really shouldn't be contained
 * within this library). All application methods that may be used by Skins are contained here,
 * and implemented by IPTVClientApp.
 *
 */
class AppInterface
{
public:
	// Skin loading methods
	virtual bool LoadSkin(wxString skinLibrary) = 0;
	virtual StringHash GetAvailableSkins() = 0;
	virtual void SetSkinSelected(wxString skinSelected) = 0;
	virtual wxString GetSkinSelected() = 0;
	
	
	// Language-related methods
	virtual void SelectLanguage(int language, bool savePrevious = false) = 0;
	virtual wxArrayInt GetAvailableLanguageCodes() = 0;
	virtual wxString GetLanguageName(int language = -1) = 0;
	virtual void SetLanguageSelected(wxString languageSelected) = 0;
	virtual wxString GetLanguageSelected() = 0;
	virtual wxString Translate(const wxString &message) = 0;

	// Configuration parameters
	virtual const AppConfig &GetAppConfig() const = 0;
	virtual void SetAppConfig(const AppConfig &config) = 0;

	// Current user/channel
	virtual const Channel *GetCurrentChannel() const = 0;
	virtual const ChannelUser *GetCurrentChannelUser() const = 0;
	virtual User *GetCurrentUser() = 0;

	// App time functions
	virtual const wxDateTime &GetAppStartTime() const = 0;
	virtual const wxTimeSpan &GetSessionTime() const = 0;

	// Channel list functions
	virtual ChannelList &GetChannelList() = 0;

	// User list functions
	virtual UserList *GetUserList() = 0;

	// Transmission management functions
	virtual unsigned GetTransmissionCount() = 0;

	// Sets a control to receive video capture frames.
	virtual void RegisterCaptureVideoSink(VideoSink *sink) = 0;
	virtual void UnregisterCaptureVideoSink(VideoSink *sink) = 0;
	virtual void UnregisterAllCaptureVideoSinks() = 0;
	
	// Controls for reception of Application Sharing video frames
	virtual void RegisterAppSharingVideoSink(VideoSink *sink) = 0;
	virtual void UnregisterAppSharingVideoSink(VideoSink *sink) = 0;
	virtual void UnregisterAllAppSharingVideoSinks() = 0;


	// Kernel communication functions
	// ------------------------------
	virtual wxString GetTempDirectory() = 0;
	// Mode initialization
	virtual void InitMulticast(const wxString &address, unsigned port) = 0;
	virtual void InitFilePlayback(const wxString &filename) = 0;
	// File playback control
	virtual void FileSeek(float percent) = 0;
	virtual void FilePlay() = 0;
	virtual void FilePause() = 0;
	// IRM connection functions
	virtual void RequestConnection(const wxString &server, const wxString &serverPass, 
		const wxString &user, const wxString &pass, 
		const wxString &realName = DEFAULT_REAL_NAME, const wxString &email = DEFAULT_EMAIL) = 0;
	virtual void CloseConnection() = 0;
	virtual void RequestChannelList() = 0;
	virtual void JoinChannel(const wxString &channel, const wxString &password = wxEmptyString) = 0;
	virtual void LeaveChannel(const wxString &channel) = 0;
	virtual void SendPrivateMessage(const wxString &user, const wxString &message) = 0;
	virtual void SendChannelMessage(const wxString &channel, const wxString &message) = 0;
	// Media flow
	virtual bool GetVideoFrame(unsigned long id, unsigned char **data, unsigned long *width, unsigned long *height) = 0;
	// Video capture control
	virtual bool InitializeCapture() = 0;
	virtual void ReleaseCapture() = 0;
	virtual bool IsCaptureEnabled() const = 0;
	//Recording control
	virtual void EnableConferenceRecording(const wxString &path) = 0;
	virtual void DisableConferenceRecording() = 0;
	// File transfer
	virtual bool FileTransferCancelReception(long mediaId) = 0;
	virtual bool FileTransferBeginTransmission(const wxString &channel, const wxString &filePath, unsigned long fileSize, unsigned bitRate) = 0;
	virtual bool FileTransferBeginWhiteboardTransmission(const wxString &channel, const wxString &filePath, unsigned long fileSize, unsigned bitRate) = 0;
	virtual bool FileTransferCancelTransmission(long mediaId) = 0;
	virtual void EnableFileTransfer() = 0;
	virtual void DisableFileTransfer() = 0;
	//Playlist Transmission
	virtual bool MovieBeginTransmission(const wxString &channel, const wxString &filePath, unsigned bitRate) = 0;
	virtual bool MovieCancelTransmission(long mediaId) = 0;
	// Poll
	virtual bool SendPoll(PollInfo *poll) = 0;
	virtual void SendPollAnswer(int index) = 0;
	virtual void SendPollStats() = 0;
	virtual const PollInfo *GetActivePoll() const = 0;
	virtual void ResetActivePoll() = 0;
	// Whiteboard
	virtual bool WhiteboardSendMessage(char* data) = 0;
	// Transmission control
	virtual void InitializeMediaColaboration(const long &bitrate) = 0;
	virtual void StopMediaColaboration() = 0;
	virtual bool IsTransmittingMedia() const = 0;
	// CONFERENCE mode control
	virtual bool InitializeConference(wxString channel) = 0;
	virtual void TerminateAllConferences() = 0;
	// Device/codec listing
	virtual void GetVideoDeviceList(std::list<VideoDeviceItem> &deviceList) = 0;
	virtual void GetAudioDeviceList(std::list<AudioDeviceItem> &deviceList) = 0;
	virtual void GetVideoCodecList(std::list<wxString> &deviceList, std::list<wxString> &videoBitrateList) = 0;
	virtual void GetAudioCodecList(std::list<wxString> &deviceList) = 0;

	// get settings manager
	virtual SettingsManager *GetSettingsManager() = 0;

	// Set debug message. Useful for debugging Skin problems.
	virtual void SetDebugMessage(const wxString &message) = 0;

	// About dialog
	virtual void ShowAboutDialog() = 0;

	// Version strings
	virtual const wxString GetClientVersion() const = 0;
	virtual const wxString GetClientBuildID() const = 0;
	virtual const wxString GetClientBuildDateStr() const = 0;
	virtual const wxString GetCoreVersion() = 0;

	// Series collaboration
	virtual void AddToSeriesCollaboration(const wxString &userName) = 0;
	virtual void StartSeriesCollaboration(const wxTimeSpan &interval = SC_DEFAULT_SWITCH_INTERVAL, bool selectAllUsers = false) = 0;
	virtual void StopSeriesCollaboration() = 0;
	virtual bool IsSeriesCollaborationStarted() const = 0;

	// IRM mode manipulation
	virtual void RequestSetChannelOperator(wxString channel, wxString user) = 0;
	virtual void RequestUnsetChannelOperator(wxString channel, wxString user) = 0;

	virtual void RequestSetChannelPrivate(wxString channel) = 0;
	virtual void RequestUnsetChannelPrivate(wxString channel) = 0;

	virtual void RequestSetChannelSecret(wxString channel) = 0;
	virtual void RequestUnsetChannelSecret(wxString channel) = 0;

	virtual void RequestSetChannelInviteOnly(wxString channel) = 0;
	virtual void RequestUnsetChannelInviteOnly(wxString channel) = 0;

	virtual void RequestSetChannelOpChangeTopic(wxString channel) = 0;
	virtual void RequestUnsetChannelOpChangeTopic(wxString channel) = 0;

	virtual void RequestSetChannelNoExternalMessages(wxString channel) = 0;
	virtual void RequestUnsetChannelNoExternalMessages(wxString channel) = 0;

	virtual void RequestSetChannelModerated(wxString channel) = 0;
	virtual void RequestUnsetChannelModerated(wxString channel) = 0;

	virtual void RequestSetChannelUserLimit(wxString channel, int limit) = 0;
	virtual void RequestUnsetChannelUserLimit(wxString channel) = 0;

	virtual void RequestSetChannelMediaCollaborator(wxString channel, wxString user) = 0;
	virtual void RequestUnsetChannelMediaCollaborator(wxString channel, wxString user) = 0;

	virtual void RequestSetChannelKey(wxString channel, wxString passwd) = 0;
	virtual void RequestUnsetChannelKey(wxString channel) = 0;

	virtual void RequestSetChannelBitRate(wxString channel, int bitRate) = 0;
	virtual void RequestUnsetChannelBitRate(wxString channel) = 0;

	virtual void RequestSetChannelAutoChangeBitRate(wxString channel) = 0;
	virtual void RequestUnsetChannelAutoChangeBitRate(wxString channel) = 0;

	virtual void RequestSetChannelMultipleTransmission(wxString channel) = 0;
	virtual void RequestUnsetChannelMultipleTransmission(wxString channel) = 0;

	virtual void RequestSetChannelOneAudio(wxString channel, int audioSwitchTime) = 0;
	virtual void RequestUnsetChannelOneAudio(wxString channel) = 0;

	virtual void RequestSetChannelAudioMute(wxString channel) = 0;
	virtual void RequestUnsetChannelAudioMute(wxString channel) = 0;

	virtual void RequestSetChannelBandShareLimit(wxString channel, int limit) = 0;
	virtual void RequestUnsetChannelBandShareLimit(wxString channel) = 0;

	virtual void RequestSetChannelTransmissionLimit(wxString channel, int limit) = 0;
	virtual void RequestUnsetChannelTransmissionLimit(wxString channel) = 0;

	virtual void RequestSetUserInvisible() = 0;
	virtual void RequestUnsetUserInvisible() = 0;

	virtual void RequestSetUserCameraOn() = 0;
	virtual void RequestUnsetUserCameraOn() = 0;

	virtual void RequestSetUserMicMute() = 0;
	virtual void RequestUnsetUserMicMute() = 0;

	virtual void RequestSetUserAway() = 0;
	virtual void RequestUnsetUserAway() = 0;

	virtual void RequestSetMediaVoice(const wxString &user) = 0;
	virtual void RequestUnsetMediaVoice() = 0;

	virtual void SendMediaVoiceRequest() = 0;
	virtual void SendMediaVoiceRequestCancel() = 0;
	virtual void SendMediaVoiceRequestRemoveAll() = 0;

	virtual void RequestSetTopic(wxString channel, wxString topic) = 0;

	//Video configuration
	//virtual void GetVideoDeviceList(std::list) = 0;
	//Audio configuration
};

#endif	// APPINTERFACE_H
