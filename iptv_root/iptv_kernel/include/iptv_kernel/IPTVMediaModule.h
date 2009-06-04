#ifndef IPTV_MEDIA_MODULE_H
#define IPTV_MEDIA_MODULE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModule.h"
#include "iptv_kernel/MediaConnectionData.h"

enum IrmMediaClass
{
	IRM_MEDIA_CLASS_NONE,
	IRM_MEDIA_CLASS_AV_CONFERENCE,
	IRM_MEDIA_CLASS_FILE_TRANSFER,
	IRM_MEDIA_CLASS_APPLICATION_SHARING,
	IRM_MEDIA_CLASS_EXECUTIVE_TV_CAMERA,
	IRM_MEDIA_CLASS_EXECUTIVE_TV_SCREEN,
	IRM_MEDIA_CLASS_EXECUTIVE_TV_MOVIE
};

enum IrmMediaNotification
{
	IRM_MEDIA_NOTIFICATION_START,
	IRM_MEDIA_NOTIFICATION_STOP,
	IRM_MEDIA_NOTIFICATION_SEND
};

/** @brief Implements tasks that are common to all IP.TV modules that stream data.
 *
 */
class IPTVMediaModule : public IPTVModule
{
public:
	IPTVMediaModule();

	void OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn);

	static void SetUdpConnectionAvailable();
	static void SetUdpConnectionUnavailable();
	static void SetHttpTunnelingOn();
	static void SetHttpTunnelingOff();

	static bool IsUdpConnectionAvailable();
	static bool IsHttpTunnelingOn();

	static void EnableHttpTunneling();
	static void DisableHttpTunneling();

protected:
	MediaConnectionData GetReceptionData(IrmNetworkProtocol defaultNetworkProtocol = IRM_NETWORK_PROTOCOL_INVALID) const;
	MediaConnectionData GetReceptionData(	br::com::sbVB::VBLib::VBString host,
											unsigned port,
											br::com::sbVB::VBLib::VBString multiCastIp = "",
											IrmNetworkProtocol defaultNetworkProtocol = IRM_NETWORK_PROTOCOL_INVALID) const;

	MediaConnectionData GetTransmissionData(IrmNetworkProtocol defaultNetworkProtocol = IRM_NETWORK_PROTOCOL_INVALID) const;
	MediaConnectionData GetTransmissionData(	br::com::sbVB::VBLib::VBString host,
												unsigned port,
												IrmNetworkProtocol defaultNetworkProtocol = IRM_NETWORK_PROTOCOL_INVALID) const;

	void RequestMediaReceive(	long mediaId,
								br::com::sbVB::VBLib::VBString networkProtocol,
								unsigned port) const;

	void RequestMediaReceiveRun(long mediaId) const;

	void RequestMediaReceiveStop(long mediaId) const;

	void RequestMediaReceiveAll(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass,
									br::com::sbVB::VBLib::VBString networkProtocol,
									unsigned port) const;

	void RequestMediaReceiveAllRun(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass) const;

	void RequestMediaReceiveAllStop(	br::com::sbVB::VBLib::VBString channel,
										br::com::sbVB::VBLib::VBString mediaClass) const;

	void RequestMediaSend(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							br::com::sbVB::VBLib::VBString networkProtocol,
							unsigned bitRate) const;

	void RequestMediaSend(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							br::com::sbVB::VBLib::VBString networkProtocol,
							unsigned bitRate,
							unsigned headerSize,
							br::com::sbVB::VBLib::VBString header) const;

	void RequestMediaSendStop(long mediaId) const;

	void RequestMediaPacket(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex) const;

	bool IsMediaClassFileTransfer(const br::com::sbVB::VBLib::VBString &mediaClass) const;
	bool IsMediaClassAppSharing(const br::com::sbVB::VBLib::VBString &mediaClass) const;
	bool IsMediaClassAVConference(const br::com::sbVB::VBLib::VBString &mediaClass) const;
	bool IsMediaClassMovie(const br::com::sbVB::VBLib::VBString &mediaClass) const;

	bool IsMediaNotificationStart(br::com::sbVB::VBLib::VBString &notification) const;
	bool IsMediaNotificationStop(br::com::sbVB::VBLib::VBString &notification) const;
	bool IsMediaNotificationSend(br::com::sbVB::VBLib::VBString &notification) const;

	static const br::com::sbVB::VBLib::VBString m_irmMediaClassNone;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassAVConference;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassFileTransfer;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassApplicationSharing;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassExecutiveTVCamera;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassExecutiveTVScreen;
	static const br::com::sbVB::VBLib::VBString m_irmMediaClassMovie;

	static const br::com::sbVB::VBLib::VBString m_irmMediaNotificationStart;
	static const br::com::sbVB::VBLib::VBString m_irmMediaNotificationStop;
	static const br::com::sbVB::VBLib::VBString m_irmMediaNotificationSend;

private:
	virtual unsigned GetMediaPort() const = 0;

	// IRM messages handling
	virtual void OnUserPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
										br::com::sbVB::VBLib::VBString message) = 0;

	virtual void OnChannelPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
											br::com::sbVB::VBLib::VBString channel,
											br::com::sbVB::VBLib::VBString message) = 0;

	virtual void OnMediaNotify(	br::com::sbVB::VBLib::VBString notification,
								long mediaId,
								br::com::sbVB::VBLib::VBString mediaClass,
								br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString user) = 0;

	virtual void OnMediaReceive(long mediaId,
								long port,
								br::com::sbVB::VBLib::VBString multiCastIp,
								long packetLength,
								long headerLength,
								br::com::sbVB::VBLib::VBString header,
								IrmUser user,
								IrmConnection conn) = 0;

	virtual void OnMediaReceiveRun(long mediaId) = 0;

	virtual void OnMediaReceiveStop(long mediaId) = 0;

	virtual void OnMediaReceiveAll(	br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString mediaClass,
									long port,
									br::com::sbVB::VBLib::VBString multiCastIp,
									IrmUser user,
									IrmConnection conn) = 0;

	virtual void OnMediaReceiveAllRun(	br::com::sbVB::VBLib::VBString channel,
										br::com::sbVB::VBLib::VBString mediaClass) = 0;

	virtual void OnMediaReceiveAllStop(	br::com::sbVB::VBLib::VBString channel,
										br::com::sbVB::VBLib::VBString mediaClass) = 0;

	virtual void OnMediaSend(	long mediaId,
								br::com::sbVB::VBLib::VBString mediaClass,
								br::com::sbVB::VBLib::VBString channel,
								long port,
								IrmUser &user,
								IrmConnection &conn) = 0;

	virtual void OnMediaSendStop(long mediaId) = 0;

	virtual void OnPacketRequest(	long mediaId,
									unsigned long initialPacketIndex,
									unsigned long finalPacketIndex) = 0;

	static bool m_udpConnectionAvailable;
	static bool m_httpTunnelingOn;

	static const unsigned m_irmHttpTunnelingPort;
};

#endif
