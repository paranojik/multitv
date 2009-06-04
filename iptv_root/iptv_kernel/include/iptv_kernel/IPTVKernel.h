#ifndef IPTV_KERNEL_H
#define IPTV_KERNEL_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif

#include "shared/compat.h"
#include "iptv_appsharing/include/ASCompat.h"
#include "mediapkt.h"
#include "shared/socket/filemw.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "VBLib/VBLib.h"

#include <list>
#include <deque>

#include "iptv_kernel/KernelNotify.h"
#include "iptv_kernel/IrmQueryNotify.h"
#include "iptv_kernel/MediaTransceiverNotify.h"

#include "iptv_kernel/IrmQuery.h"
#include "iptv_kernel/MediaTransceiver.h"
#include "iptv_kernel/MediaCapture.h"
#include "iptv_kernel/MediaConference.h"
#include "iptv_kernel/FileTransfer.h"
#include "iptv_kernel/Poll.h"
#include "iptv_kernel/Whiteboard.h"
#include "iptv_kernel/ApplicationSharing.h"
#include "iptv_kernel/Movie.h"

#include "iptv_kernel/IPTVModuleNotificationSink.h"
#include "iptv_kernel/MediaCaptureNotify.h"

#include "iptv_video_capture/include/VideoCapture.h"

#ifdef IPTV_USING_MEDIA
// This is only done if the media library is being used, which is done by
// defining the IPTV_USING_MEDIA macro.
#include "iptv_kernel/MediaProcessor.h"
#endif

#define CORE_VERSION_ID "4.15.53.1295"


/** @brief Main IP.TV Cross-Platform Kernel class.
 *
 * This class is a Facade to the classes that actually implement the IP.TV rules. An
 * IP.TV application must call it's methods to communicate with an IRM server.
 *
 */
class IPTVKernel :	public IrmQueryNotify,
					public MediaTransceiverNotify,
					public MediaProcessorNotify,
					public MediaCaptureNotify,
					public IPTVModuleNotificationSink
{
public:
	IPTVKernel(KernelNotify *notify = NULL);
	~IPTVKernel();

	void SetNotify(KernelNotify *notify);

	br::com::sbVB::VBLib::VBString GetCoreVersion();

	IRMUdpListeningState		GetUdpListeningState();
	IRMConnectionState			GetConnectionState();
	IRMAuthenticationState		GetAuthenticationState();
	IRMChannelJoinState			GetChannelJoinState();
	IRMChannelPartState			GetChannelPartState();
	IRMChannelQueryState		GetChannelQueryState();
	IRMUserQueryState			GetUserQueryState();

	static bool IsUdpConnectionAvailable();
	static bool IsHttpTunnelingOn();

	static void EnableHttpTunneling();
	static void DisableHttpTunneling();

	void EnableMediaConference();
	void DisableMediaConference();
	void EnableFileTransfer();
	void DisableFileTransfer();
	void EnablePoll();
	void DisablePoll();
	void EnableWhiteboard();
	void DisableWhiteboard();

	void RequestConnection(	
							br::com::sbVB::VBLib::VBString server,
							unsigned port,
							br::com::sbVB::VBLib::VBString user,
							br::com::sbVB::VBLib::VBString passwd,
							br::com::sbVB::VBLib::VBString realName,
							br::com::sbVB::VBLib::VBString email,
							br::com::sbVB::VBLib::VBString serverPasswd
						);

	void Disconnect();

	void RequestChannelJoin(br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString passwd = "");
	void RequestChannelPart(br::com::sbVB::VBLib::VBString channel);
	void QueryChannel(br::com::sbVB::VBLib::VBString channel = "");
	void QueryUser(br::com::sbVB::VBLib::VBString channel);
	void SendPrivateMessage(br::com::sbVB::VBLib::VBString recipient,
							br::com::sbVB::VBLib::VBString message);
	void SendChannelPrivateMessage(	br::com::sbVB::VBLib::VBString recipient,
									br::com::sbVB::VBLib::VBString message);

	void SendMediaVoiceRequest(br::com::sbVB::VBLib::VBString channel);
	void SendMediaVoiceRequestCancel(br::com::sbVB::VBLib::VBString channel);
	void SendMediaVoiceRequestRemoveAll(br::com::sbVB::VBLib::VBString channel);

	void RequestSetMediaVoice(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString user);

	void RequestUnsetMediaVoice(br::com::sbVB::VBLib::VBString channel);

	void RequestSetTopic(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString topic);
	// IRM mode manipulation
	void RequestSetChannelOperator(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString user);
	void RequestUnsetChannelOperator(	br::com::sbVB::VBLib::VBString channel,
										br::com::sbVB::VBLib::VBString user);

	void RequestSetChannelPrivate(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelPrivate(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelSecret(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelSecret(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelInviteOnly(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelInviteOnly(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelOpChangeTopic(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelOpChangeTopic(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelNoExternalMessages(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelNoExternalMessages(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelModerated(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelModerated(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelUserLimit(br::com::sbVB::VBLib::VBString channel, int limit);
	void RequestUnsetChannelUserLimit(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelMediaCollabotator(br::com::sbVB::VBLib::VBString channel,
											br::com::sbVB::VBLib::VBString user);

	void RequestUnsetChannelMediaCollabotator(	br::com::sbVB::VBLib::VBString channel,
												br::com::sbVB::VBLib::VBString user);

	void RequestSetChannelKey(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString passwd);

	void RequestUnsetChannelKey(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelBitRate(br::com::sbVB::VBLib::VBString channel, int bitRate);
	void RequestUnsetChannelBitRate(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelAutoChangeBitRate(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelAutoChangeBitRate(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelMultipleTransmission(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelMultipleTransmission(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelOneAudio(br::com::sbVB::VBLib::VBString channel, int audioSwitchTime);
	void RequestUnsetChannelOneAudio(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelAudioMute(br::com::sbVB::VBLib::VBString channel);
	void RequestUnsetChannelAudioMute(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelBandShareLimit(br::com::sbVB::VBLib::VBString channel, int limit);
	void RequestUnsetChannelBandShareLimit(br::com::sbVB::VBLib::VBString channel);

	void RequestSetChannelTransmissionLimit(br::com::sbVB::VBLib::VBString channel, int limit);
	void RequestUnsetChannelTransmissionLimit(br::com::sbVB::VBLib::VBString channel);

	void RequestSetUserInvisible();
	void RequestUnsetUserInvisible();

	void RequestSetUserCameraOn();
	void RequestUnsetUserCameraOn();

	void RequestSetUserMicMute();
	void RequestUnsetUserMicMute();

	void RequestSetUserAway();
	void RequestUnsetUserAway();

	void RequestInviteUser(	br::com::sbVB::VBLib::VBString user,
							br::com::sbVB::VBLib::VBString channel);

	void TerminateAllMediaTransmissions();
	bool InitializeMulticast(br::com::sbVB::VBLib::VBString host, unsigned port);
	bool InitializeConference(br::com::sbVB::VBLib::VBString channel);
	bool InitializeFilePlayback(br::com::sbVB::VBLib::VBString fileName);
	void SeekFilePlayback(double percent);
	void PlayFilePlayback();
	void PauseFilePlayback();
	bool GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
									unsigned long *width, unsigned long *height);

	bool RequestMediaData(unsigned long mediaId);

	// File Transfer methods.
	bool FileTransferCancelReception(long mediaId);
	bool FileTransferBeginTransmission(	br::com::sbVB::VBLib::VBString channel,
										br::com::sbVB::VBLib::VBString filePath,
										unsigned long fileSize,
										unsigned bitRate);
	bool FileTransferBeginWhiteboardTransmission(	br::com::sbVB::VBLib::VBString channel,
													br::com::sbVB::VBLib::VBString filePath,
													unsigned long fileSize,
													unsigned bitRate);
	bool FileTransferCancelTransmission(long mediaId);

	// Movie methods.
	bool MovieBeginTransmission(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString filePath,
									unsigned bitRate);

	bool MovieCancelTransmission(long mediaId);

	// Poll methods.
	bool PollSend(const PollData &pollData);
	bool PollSendAnswer(br::com::sbVB::VBLib::VBString recipient, int optionIndex);
	bool PollSendStats(const PollData &pollData);

	// Whiteboard methods.
	void SendChannelWhiteboard(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString data);

	// Capture and transmission related methods
	bool InitializeVideoCapture(unsigned videoDevice = 0);
	void ReleaseVideoCapture();

	void InitializeVideoEncoder(br::com::sbVB::VBLib::VBString videoCodec,
											br::com::sbVB::VBLib::VBString videoBitrate);

	void InitializeMediaColaboration(	br::com::sbVB::VBLib::VBString channel,
													int audioDevice,
													br::com::sbVB::VBLib::VBString audioCodec,
													br::com::sbVB::VBLib::VBString audioBitrate,
													br::com::sbVB::VBLib::VBString videoCodec,
													br::com::sbVB::VBLib::VBString videoBitrate,
													unsigned transmissionBitrate);
	void StopMediaColaboration();

	void EnableConferenceRecording(br::com::sbVB::VBLib::VBString filePath);
	void DisableConferenceRecording();

	void GetVideoDeviceList(std::deque<tv::ip::VideoDeviceInfo> &deviceList);
	void GetAudioDeviceList(std::deque<std::string> &deviceList);

	void GetVideoEncoders(std::deque<std::string> &videoEncoders, std::deque<std::string> &videoBitrates);
	void GetAudioEncoders(std::deque<std::string> &audioEncoders);

private:
	void OnIPTVModuleNotification(IPTVModuleNotificationParameter &parameter);

	KernelNotify *m_notify;
	IrmQuery m_irmQuery;
	MediaTransceiver m_mediaTransceiver;
	MediaTransceiver m_appSharingTransceiver;
	MediaCapture m_mediaCapture;
	MediaConference m_mediaConference;
	FileTransfer m_fileTransfer;
	Poll m_poll;
	Whiteboard m_whiteboard;
	ApplicationSharing m_appSharing;
	Movie m_movie;

	void InitializeConferenceRecording();
	void StartConferenceRecording();
	void DestroyConferenceRecording();

	FileMW *m_fileMediaWriteConference;
	bool m_conferenceRecordingEnabled;
	br::com::sbVB::VBLib::VBString m_conferenceRecordingFilePath;

	bool m_isConferenceViewerRunning;

	// Capture and transmission related
	int m_colaborationAudioDevice;
	bool m_videoCaptureEnable;
	bool m_mediaColaborationEnable;

	bool m_videoEncoderEnabled;

	// Enables handling in the client the transmissions originated from itself.
	bool m_handleOwnTransmission;

	long m_mediaConferenceMediaId;

#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	MediaProcessor m_mediaProcessor;
	bool OnMediaProcessorNotify(MediaProcessorParam &param);
#endif

	void NotifyIrmMode(IrmQueryParam &param, KernelNotifyCode notificationCode);
	void OnIrmQueryNotify(IrmQueryParam &param);
	bool OnMediaTransceiverNotify(MediaTransceiverParam &param);
	void OnMediaCaptureNotify(MediaCaptureParam &param);
};

#endif
