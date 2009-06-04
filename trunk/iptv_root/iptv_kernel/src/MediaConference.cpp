#include "iptv_kernel/MediaConference.h"
#include "iptv_kernel/LogManager.h"
#include "chat/nOpCodes.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

const unsigned MediaConference::m_mediaPort = 8009;

MediaConference::MediaConference() : m_state(MC_STATE_IDLE)
{

}

void MediaConference::StartTransmission(VBString channel, unsigned bitRate)
{
	MediaConnectionData connData;
	connData = GetTransmissionData(IRM_NETWORK_PROTOCOL_UDP);

	RequestMediaSend(channel, m_irmMediaClassAVConference, connData.GetNetworkProtocolName(), bitRate);
}

void MediaConference::OnUserPrivateMessage(VBString sender, VBString message)
{

}

void MediaConference::OnChannelPrivateMessage(VBString sender, VBString channel, VBString message)
{

}

void MediaConference::OnMediaNotify(VBString notification, long mediaId, VBString mediaClass,
								 VBString channel, VBString user)
{

}

void MediaConference::OnMediaReceive(	long mediaId, long port, VBString multiCastIp,
										long packetLength, long headerLength, VBString header,
										IrmUser user, IrmConnection conn)
{

}

void MediaConference::OnMediaReceiveRun(long mediaId)
{

}

void MediaConference::OnMediaReceiveStop(long mediaId)
{

}

void MediaConference::OnMediaReceiveAll(	VBString channel,
											VBString mediaClass,
											long port,
											VBString multiCastIp,
											IrmUser user,
											IrmConnection conn)
{
	// TODO
	// The AVCONF reception is currently handled in IRMQuery.
}

void MediaConference::OnMediaReceiveAllRun(	VBString channel,
											VBString mediaClass)
{
	// TODO
	// The AVCONF reception is currently handled in IRMQuery.
}

void MediaConference::OnMediaReceiveAllStop(	VBString channel,
												VBString mediaClass)
{
	// TODO
	// The AVCONF reception is currently handled in IRMQuery.
}

void MediaConference::OnMediaSend(	long mediaId,
									br::com::sbVB::VBLib::VBString mediaClass,
									br::com::sbVB::VBLib::VBString channel,
									long port,
									IrmUser &user,
									IrmConnection &conn)
{
	if(IsMediaClassAVConference(mediaClass))
	{
		MediaConnectionData connData;
		connData = GetTransmissionData(conn.GetHost(), port, IRM_NETWORK_PROTOCOL_UDP);

		IMNPMediaSendReply param;
		param.m_moduleId = IPTV_MODULE_ID_AV_CONFERENCE;
		param.m_code = IMNC_REPLY_MEDIA_SEND;
		param.m_mediaId = mediaId;
		param.m_channel = channel;
		param.m_networkProtocol = connData.GetNetworkProtocolName();
		param.m_host = connData.GetHost();
		param.m_port = connData.GetPort();

		Notify(param);
	}
}

void MediaConference::OnMediaSendStop(long mediaId)
{

}

void MediaConference::OnPacketRequest(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	// The Media Conference module does not handle package requests.
}

void MediaConference::SetState(MediaConferenceState state)
{
	m_state = state;
}

bool MediaConference::IsIdle() const
{
	if(m_state == MC_STATE_IDLE)
		return true;
	return false;
}

unsigned MediaConference::GetMediaPort() const
{
	return m_mediaPort;
}

void MediaConference::Reset()
{

}
