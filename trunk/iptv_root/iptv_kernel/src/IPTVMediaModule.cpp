#include "iptv_kernel/IPTVMediaModule.h"
#include "iptv_kernel/LogManager.h"
#include "chat/nOpCodes.h"

using namespace br::com::sbVB::VBLib;

bool IPTVMediaModule::m_udpConnectionAvailable							= false;
bool IPTVMediaModule::m_httpTunnelingOn									= false;

const unsigned IPTVMediaModule::m_irmHttpTunnelingPort					= 80;

const VBString IPTVMediaModule::m_irmMediaClassNone						= "NONE";
const VBString IPTVMediaModule::m_irmMediaClassAVConference				= "AVCONF";
const VBString IPTVMediaModule::m_irmMediaClassFileTransfer				= "FTP";
const VBString IPTVMediaModule::m_irmMediaClassApplicationSharing		= "APP";
const VBString IPTVMediaModule::m_irmMediaClassExecutiveTVCamera		= "CAM";
const VBString IPTVMediaModule::m_irmMediaClassExecutiveTVScreen		= "SCREEN";
const VBString IPTVMediaModule::m_irmMediaClassMovie					= "MOVIE";

const VBString IPTVMediaModule::m_irmMediaNotificationStart				= "START";
const VBString IPTVMediaModule::m_irmMediaNotificationStop				= "STOP";
const VBString IPTVMediaModule::m_irmMediaNotificationSend				= "SEND";

IPTVMediaModule::IPTVMediaModule()
{	

}

void IPTVMediaModule::OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn)
{
	switch (msg.m_irmMessageCode)
	{
	//---------------------------------------------------------------------------------------------
	// Private message events
	//---------------------------------------------------------------------------------------------
	case PRIVMSG:
		OnUserPrivateMessage(msg.m_strSource, msg.m_strData);
		break;
	//---------------------------------------------------------------------------------------------
	case PRIVMSG_CHANNEL:
		OnChannelPrivateMessage(msg.m_strSource, msg.m_strTarget, msg.m_strData);
		break;
	//---------------------------------------------------------------------------------------------
	// Media events
	//---------------------------------------------------------------------------------------------
	case MEDIANOTIFY:
		OnMediaNotify(	msg.m_strComplement1, msg.m_longData, msg.m_strComplement2,
						msg.m_strComplement3, msg.m_strTarget);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCV:
		OnMediaReceive(	msg.m_longData, msg.m_longComplement2, msg.m_strTarget,
						msg.m_longComplement1, msg.m_longComplement3, msg.m_strComplement4,
						user, conn);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCVRUN:
		OnMediaReceiveRun(msg.m_longData);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCVSTOP:
		OnMediaReceiveStop(msg.m_longData);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCVALL:
		OnMediaReceiveAll(	msg.m_strData, msg.m_strComplement1, msg.m_longComplement2,
							msg.m_strComplement3, user, conn);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCVALLRUN:
			OnMediaReceiveAllRun(msg.m_strData, msg.m_strComplement1);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIARCVALLSTOP:
		OnMediaReceiveAllStop(msg.m_strData, msg.m_strComplement1);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIASND:
		OnMediaSend(msg.m_longData, msg.m_strComplement2, msg.m_strComplement1, msg.m_longComplement3, user, conn);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIASNDSTOP:
		OnMediaSendStop(msg.m_longData);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIAPKTREQUEST:
		OnPacketRequest(msg.m_longData, msg.m_longComplement1, msg.m_longComplement2);
		break;
	//---------------------------------------------------------------------------------------------
	}
}

void IPTVMediaModule::SetUdpConnectionAvailable()
{	
	m_udpConnectionAvailable = true;
}

void IPTVMediaModule::SetUdpConnectionUnavailable()
{	
	m_udpConnectionAvailable = false;
	LogManager::Instance()->AddLogHeader("UDP Unavailable");
}

void IPTVMediaModule::SetHttpTunnelingOn()
{	
	m_httpTunnelingOn = true;
}

void IPTVMediaModule::SetHttpTunnelingOff()
{	
	m_httpTunnelingOn = false;
}

bool IPTVMediaModule::IsUdpConnectionAvailable()
{
	return m_udpConnectionAvailable;
}

bool IPTVMediaModule::IsHttpTunnelingOn()
{
	return m_httpTunnelingOn;
}

void IPTVMediaModule::EnableHttpTunneling()
{
	m_httpTunnelingOn = true;
}

void IPTVMediaModule::DisableHttpTunneling()
{
	m_httpTunnelingOn = false;
}

MediaConnectionData IPTVMediaModule::GetReceptionData(IrmNetworkProtocol defaultNetworkProtocol) const
{
	return GetReceptionData("", 0, "", defaultNetworkProtocol);
}

MediaConnectionData IPTVMediaModule::GetReceptionData(	VBString host, unsigned port,
														VBString multiCastIp, IrmNetworkProtocol defaultNetworkProtocol) const
{
	MediaConnectionData connInfo;

	if(m_httpTunnelingOn)
	{
		// Using http tunnel.
		connInfo.SetNetworkProtocolTcp();
		connInfo.SetHost(host);
		connInfo.SetPort(m_irmHttpTunnelingPort);
	}

	else
	{
		if(defaultNetworkProtocol == IRM_NETWORK_PROTOCOL_UDP)
		{
			// Forcing UDP.
			connInfo.SetNetworkProtocolUdp();
			connInfo.SetHost("");
			connInfo.SetPort(GetMediaPort());
		}

		else if(defaultNetworkProtocol == IRM_NETWORK_PROTOCOL_TCP)
		{
			// Forcing TCP.
			connInfo.SetNetworkProtocolTcp();
			connInfo.SetHost(host);
			connInfo.SetPort(port);
		}

		else if(m_udpConnectionAvailable)
		{
			// UDP connection available. The UDP or multicast protocol will be used.
			if(multiCastIp.IsEmpty())
			{
				// Using UDP because no multicast IP was supplied.
				connInfo.SetNetworkProtocolUdp();
				connInfo.SetHost("");
				connInfo.SetPort(GetMediaPort());
			}

			else
			{
				// Using Multicast because a multicast IP was supplied.
				connInfo.SetNetworkProtocolMulticast();
				connInfo.SetHost(multiCastIp);
				connInfo.SetPort(port);
			}
		}

		else
		{
			// Using TCP because UDP connection is not available.
			connInfo.SetNetworkProtocolTcp();
			connInfo.SetHost(host);
			connInfo.SetPort(port);
		}
	}

	return connInfo;
}

MediaConnectionData IPTVMediaModule::GetTransmissionData(IrmNetworkProtocol defaultNetworkProtocol) const
{
	return GetTransmissionData("", 0, defaultNetworkProtocol);
}

MediaConnectionData IPTVMediaModule::GetTransmissionData(	VBString host, unsigned port,
															IrmNetworkProtocol defaultNetworkProtocol) const
{
	MediaConnectionData connInfo;

	if(m_httpTunnelingOn)
	{
		// Using http tunnel.
		connInfo.SetNetworkProtocolTcp();
		connInfo.SetHost(host);
		connInfo.SetPort(m_irmHttpTunnelingPort);
	}

	else
	{
		connInfo.SetHost(host);
		connInfo.SetPort(port);

		if(defaultNetworkProtocol == IRM_NETWORK_PROTOCOL_UDP)
		{
			// Forcing UDP.
			connInfo.SetNetworkProtocolUdp();
		}

		else if(defaultNetworkProtocol == IRM_NETWORK_PROTOCOL_TCP)
		{
			// Forcing TCP.
			connInfo.SetNetworkProtocolTcp();
		}

		else if(m_udpConnectionAvailable)
		{
			// Using UDP because UDP connection is available.
			connInfo.SetNetworkProtocolUdp();
		}

		else
		{
			// Using TCP because UDP connection is not available.
			connInfo.SetNetworkProtocolTcp();
		}
	}

	return connInfo;
}

void IPTVMediaModule::RequestMediaReceive(long mediaId, VBString networkProtocol, unsigned port) const
{
	if(mediaId != 0)
	{
		IMNPMediaReceive parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE;
		parameter.m_mediaId = mediaId;
		parameter.m_networkProtocol = networkProtocol;
		parameter.m_port = port;
		Notify(parameter);
	}
}

void IPTVMediaModule::RequestMediaReceiveRun(long mediaId) const
{
	if(mediaId != 0)
	{
		IMNPMediaId parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE_RUN;
		parameter.m_mediaId = mediaId;
		Notify(parameter);
	}
}

void IPTVMediaModule::RequestMediaReceiveStop(long mediaId) const
{
	if(mediaId != 0)
	{
		IMNPMediaId parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE_STOP;
		parameter.m_mediaId = mediaId;
		Notify(parameter);
	}
}

void IPTVMediaModule::RequestMediaReceiveAll(	VBString channel,
												VBString mediaClass,
												VBString networkProtocol,
												unsigned port) const
{
		IMNPMediaReceiveAll parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE_ALL;
		parameter.m_channel = channel;
		parameter.m_mediaClass = mediaClass;
		parameter.m_networkProtocol = networkProtocol;
		parameter.m_port = port;
		Notify(parameter);
}

void IPTVMediaModule::RequestMediaReceiveAllRun(	VBString channel,
													VBString mediaClass) const
{
		IMNPMediaReceiveAll parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE_ALL_RUN;
		parameter.m_channel = channel;
		parameter.m_mediaClass = mediaClass;
		Notify(parameter);
}

void IPTVMediaModule::RequestMediaReceiveAllStop(	VBString channel,
													VBString mediaClass) const
{
		IMNPMediaReceiveAll parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_RECEIVE_ALL_STOP;
		parameter.m_channel = channel;
		parameter.m_mediaClass = mediaClass;
		Notify(parameter);
}


void IPTVMediaModule::RequestMediaSend(	VBString channel,
										VBString mediaClass,
										VBString networkProtocol,
										unsigned bitRate) const
{
	RequestMediaSend(channel, mediaClass, networkProtocol, bitRate, 0, "");
}

void IPTVMediaModule::RequestMediaSend(	VBString channel,
										VBString mediaClass,
										VBString networkProtocol,
										unsigned bitRate,
										unsigned headerSize,
										VBString header) const
{
		IMNPMediaSend parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_SEND;
		parameter.m_channel = channel;
		parameter.m_mediaClass = mediaClass;
		parameter.m_networkProtocol = networkProtocol;
		parameter.m_bitRate = bitRate;
		parameter.m_headerSize = headerSize;
		parameter.m_header = header;
		Notify(parameter);
}

void IPTVMediaModule::RequestMediaSendStop(long mediaId) const
{
	if(mediaId != 0)
	{
		IMNPMediaId parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_SEND_STOP;
		parameter.m_mediaId = mediaId;
		Notify(parameter);
	}
}

void IPTVMediaModule::RequestMediaPacket(	long mediaId,
											unsigned long initialPacketIndex,
											unsigned long finalPacketIndex) const
{
	if(mediaId != 0)
	{
		IMNPMediaPacket parameter;
		parameter.m_code = IMNC_REQUEST_MEDIA_PACKET;
		parameter.m_mediaId = mediaId;
		parameter.m_initialPacketIndex = initialPacketIndex;
		parameter.m_finalPacketIndex = finalPacketIndex;
		Notify(parameter);
	}
}

bool IPTVMediaModule::IsMediaClassFileTransfer(const VBString &mediaClass) const
{
	if(mediaClass == m_irmMediaClassFileTransfer)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaClassAppSharing(const VBString &mediaClass) const
{
	if(mediaClass == m_irmMediaClassApplicationSharing)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaClassAVConference(const VBString &mediaClass) const
{
	if(mediaClass == m_irmMediaClassAVConference)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaClassMovie(const VBString &mediaClass) const
{
	if(mediaClass == m_irmMediaClassMovie)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaNotificationStart(br::com::sbVB::VBLib::VBString &notification) const
{
	if(notification == m_irmMediaNotificationStart)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaNotificationStop(br::com::sbVB::VBLib::VBString &notification) const
{
	if(notification == m_irmMediaNotificationStop)
		return true;

	return false;
}

bool IPTVMediaModule::IsMediaNotificationSend(br::com::sbVB::VBLib::VBString &notification) const
{
	if(notification == m_irmMediaNotificationSend)
		return true;

	return false;
}
