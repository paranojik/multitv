#ifndef IRM_QUERY_H
#define IRM_QUERY_H

#include "VBLib/VBLib.h"
#include <list>
#include "iptv_kernel/IrmQueryNotify.h"
#include "iptv_kernel/IrmUser.h"
#include "iptv_kernel/IrmConnection.h"
#include "iptv_kernel/IrmMode.h"

// disable warnings in MSVC when using shared lib
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)	// size_t to int conversion
#pragma warning(disable: 4005)	// FD_SETSIZE (winsock2.h) macro redefinition
#endif

#include "shared/compat.h"
#include "shared/semaphore.h"
#include "chat/nOpCodes.h"
#include "stub/IrmStubDll.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define IRM_NET_PROTOCOL_TCP					"TCP"
#define IRM_NET_PROTOCOL_UDP					"UDP"
#define IRM_NET_PROTOCOL_MCAST					"MCAST"

#define IRM_MEDIACLASS_NONE						"NONE"
#define IRM_MEDIACLASS_SIMPOSIUM_VIDEO			"CAM"
#define IRM_MEDIACLASS_SIMPOSIUM_SCREEN			"SCREEN"
#define IRM_MEDIACLASS_SIMPOSIUM_MOVIE			"MOVIE"
#define IRM_MEDIACLASS_VOICEIP					"VOICEIP"
#define IRM_MEDIACLASS_CONFERENCE				"CONFERENCE"
#define IRM_MEDIACLASS_AVCONF					"AVCONF"
#define IRM_MEDIACLASS_APP						"APP"

#define IRM_MEDIANOTIFY_MEDIASND				"SEND"
#define IRM_MEDIANOTIFY_MEDIARCVRUN				"START"
#define IRM_MEDIANOTIFY_MEDIARCVSTOP			"STOP"

#define IRM_AVCONF_UDP_PORT						8009
#define IRM_FILE_TRANSFER_UDP_PORT				8006

#define IRM_CHANNEL_PREFIX						'#'
#define IRM_QUERY_CTCP_PREFIX					'\x01'

enum IRMUdpListeningState	{
								IRMULS_NOT_AVAILABLE,
								IRMULS_AVAILABLE
							};

enum IRMViewerState			{
								IRMVS_NOT_INITIALIZED,
								IRMVS_INITIALIZED
							};

enum IRMConnectionState		{
								IRMCS_DISCONNECTED,
								IRMCS_CONNECTING,
								IRMCS_CONNECTED
							};

enum IRMAuthenticationState	{
								IRMAS_NOT_AUTHENTICATED,
								IRMAS_AUTHENTICATING,
								IRMAS_AUTHENTICATED
							};

enum IRMChannelJoinState	{
								IRMCJS_NOT_JOINING,
								IRMCJS_JOINING
							};

enum IRMChannelPartState	{
								IRMCPS_NOT_PARTING,
								IRMCPS_PARTING
							};

enum IRMChannelQueryState	{
								IRMCQS_NOT_QUERYING,
								IRMCQS_QUERYING
							};

enum IRMUserQueryState		{
								IRMUQS_NOT_QUERYING,
								IRMUQS_QUERYING
							};

/** @brief Provides and manages the connection with an IRM server.
 *
 * This class responsibilities are handling IRM messages related to the connection
 * and session management and provinding a higher level interface to the iptv_irm_stub
 * library.
 *
 */
class IrmQuery : public IrmStubDataSink
{
public:
	IrmQuery(IrmQueryNotify *notify = NULL);
	virtual ~IrmQuery();

	void SetNotify(IrmQueryNotify *notify);

	IRMUdpListeningState	GetUdpListeningState();
	IRMViewerState			GetViewerState();
	IRMConnectionState		GetConnectionState();
	IRMAuthenticationState	GetAuthenticationState();
	IRMChannelJoinState		GetChannelJoinState();
	IRMChannelPartState		GetChannelPartState();
	IRMChannelQueryState	GetChannelQueryState();
	IRMUserQueryState		GetUserQueryState();

	IrmConnection GetConnection();
	IrmUser GetUser();

	bool IsUdpListeningAvailable();

	bool InitializeViewer	(	
								br::com::sbVB::VBLib::VBString server,
								unsigned port
							);

	void CloseViewer();

	void RequestConnection	(	
								br::com::sbVB::VBLib::VBString server,
								unsigned port,
								br::com::sbVB::VBLib::VBString user,
								br::com::sbVB::VBLib::VBString passwd,
								br::com::sbVB::VBLib::VBString realName,
								br::com::sbVB::VBLib::VBString email,
								br::com::sbVB::VBLib::VBString serverPasswd,
								br::com::sbVB::VBLib::VBString clientVersion
							);

	void Disconnect();
	void RequestAuthentication();

	void RequestChannelJoin(br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString passwd = "");
	void RequestChannelPart(br::com::sbVB::VBLib::VBString channel);

	void RequestSetMediaVoice(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString user = "");

	void RequestSetTopic(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString topic);

	void RequestChannelModeChange(	char name,
									bool value,
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString parameter = "");

	void RequestUserModeChange(	char name,
								bool value,
								br::com::sbVB::VBLib::VBString user = "");

	void RequestSetUserAway();
	void RequestUnsetUserAway();

	void RequestInviteUser(	br::com::sbVB::VBLib::VBString user,
							br::com::sbVB::VBLib::VBString channel);

	void QueryChannel(br::com::sbVB::VBLib::VBString channel = "");
	void QueryChannelModes(br::com::sbVB::VBLib::VBString channel);
	void QueryUser(br::com::sbVB::VBLib::VBString channel);

	void SendPrivateMessage(br::com::sbVB::VBLib::VBString recipient,
							br::com::sbVB::VBLib::VBString message);

	void SendChannelPrivateMessage(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);

	void SendMediaVoiceRequest(br::com::sbVB::VBLib::VBString channel);
	void SendMediaVoiceRequestCancel(br::com::sbVB::VBLib::VBString channel);
	void SendMediaVoiceRequestRemoveAll(br::com::sbVB::VBLib::VBString channel);

	void RequestMediaReceive(	long mediaId,
								br::com::sbVB::VBLib::VBString networkProtocol,
								unsigned port);

	void RequestMediaReceiveRun(long mediaId);
	void RequestMediaReceiveStop(long mediaId);

	void RequestMediaSend(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							br::com::sbVB::VBLib::VBString networkProtocol,
							unsigned bitRate,
							unsigned headerSize,
							br::com::sbVB::VBLib::VBString header);

	void RequestMediaSendStop(long mediaId);

	void RequestMediaPacket(long mediaId,
							unsigned long initialPacketIndex,
							unsigned long finalPacketIndex);

	void RequestChannelWhiteboard(
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString data
								);

	void RequestMediaReceiveAll(
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass,
									br::com::sbVB::VBLib::VBString netProtocol,
									unsigned port
								);

	void RequestMediaReceiveAllRun(
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass
								);

	void RequestMediaReceiveAllStop(
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass
								);

	void RequestConferenceMediaReceive(br::com::sbVB::VBLib::VBString channel);

	bool IsMediaClassConference(br::com::sbVB::VBLib::VBString mediaClass);
	bool IsMediaClassExecutiveTv(br::com::sbVB::VBLib::VBString mediaClass);
	bool IsMediaClassAppSharing(br::com::sbVB::VBLib::VBString mediaClass);

private:
	ProxyIrmStub* m_IrmStubProxy;
	bool m_proxyIrmStubInitialized;
	IrmQueryNotify *m_notify;

	// Mutex.
	_SEMAPHORE* m_irmMutex;

	// IRMQuery state.
	IRMUdpListeningState	m_udpListeningState;
	IRMViewerState			m_viewerState;
	IRMConnectionState		m_connectionState;
	IRMAuthenticationState	m_authenticationState;
	IRMChannelJoinState		m_channelJoinState;
	IRMChannelPartState		m_channelPartState;
	IRMChannelQueryState	m_channelQueryState;
	IRMUserQueryState		m_userQueryState;

	// User information.
	IrmUser m_user;
	IrmConnection m_connection;

	bool m_isBeingDeleted;

	void ResetStateMachine();
	void DoDisconnect();

	void LockMutex();
	void UnlockMutex();

	void RequestModeChange(	char name,
							bool value,
							br::com::sbVB::VBLib::VBString target,
							br::com::sbVB::VBLib::VBString parameter = "");

	void NotifyIrmModeFromUserPrefix(	IrmUser irmUser,
										br::com::sbVB::VBLib::VBString channel);

	// Notification from IRM stub
	void Fire_OnMessage (
							long _cmd, 
							IrmParam& _source, IrmParam& _target, IrmParam& _data,
							IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3,
							IrmParam& _compl4, IrmParam& _compl5, IrmParam& _compl6,
							IrmParam& _compl7
						 );

	// CTCP handlers
	bool IsCtcpMessage(br::com::sbVB::VBLib::VBString message);

	// Channel tools
	br::com::sbVB::VBLib::VBString AddChannelPrefix(br::com::sbVB::VBLib::VBString channel);
	br::com::sbVB::VBLib::VBString RemoveChannelPrefix(br::com::sbVB::VBLib::VBString channel);
	bool IsChannel(br::com::sbVB::VBLib::VBString str);

	// IRM message handlers
	void OnCommandError(LPCSTR x, LPCSTR y, LPCSTR z);

	void OnConnectedHost();
	void OnConnectedChat();
	void OnDisconnectedChat();

	void OnAppPing();
	void OnUdpPing(LPCSTR user, LPCSTR ip);
	void OnUdpPong();

	void OnIdentificationRequest();
	void OnIdentify();
	void OnIdentificationError();

	void OnServerMessage(LPCSTR message);
	void OnServerRedir(LPCSTR source, LPCSTR server, long port);

	void OnUserChangeNick(LPCSTR oldNick, LPCSTR newNick);
	void OnUserQuit(LPCSTR user);

	void OnChannelJoin(LPCSTR channel, LPCSTR user);
	void OnChannelPart(LPCSTR channel, LPCSTR user);
	void OnMode(LPCSTR target, LPCSTR flags, LPCSTR parameters);
	void OnUserNotAway();
	void OnChannelMediaVoice(LPCSTR channel, LPCSTR user);

	void OnChannelQueryItem(LPCSTR channel, LPCSTR topic, LPCSTR userCount);
	void OnChannelQueryEnd();
	void OnUserQueryItem(LPCSTR channel, LPCSTR user);
	void OnUserQueryEnd(LPCSTR channel);

	void OnPrivateMessage(LPCSTR sender, LPCSTR message);
	void OnChannelPrivateMessage(LPCSTR sender, LPCSTR channel, LPCSTR message);

	void OnMedia(long mediaId, LPCSTR mediaClass, LPCSTR channel, LPCSTR user, LPCSTR multicastIp, long multicastPort, long bitRate);
	void OnMediaNotify(LPCSTR notification, long mediaId, LPCSTR mediaClass, LPCSTR channel, LPCSTR user);
	void OnMediaReceiveAll(LPCSTR channel, LPCSTR mediaClass, long port, LPCSTR multiCastIp);

	void OnUnknownMessage(long messageId);
};

#endif
