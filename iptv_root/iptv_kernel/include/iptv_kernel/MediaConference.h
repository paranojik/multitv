#ifndef MEDIA_CONFERENCE_H
#define MEDIA_CONFERENCE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVMediaModule.h"

enum MediaConferenceState	{
								MC_STATE_IDLE
							};

/** @brief Implements the Media Conference module.
 *
 * This class responsibilities are handling Media Conference IRM messages.
 *
 */
class MediaConference :	public IPTVMediaModule
{
public:
	MediaConference();

	void StartTransmission(br::com::sbVB::VBLib::VBString channel, unsigned bitRate);

private:
	unsigned GetMediaPort() const;

	void Reset();

	void OnUserPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
								br::com::sbVB::VBLib::VBString message);

	void OnChannelPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);

	void OnMediaNotify(	br::com::sbVB::VBLib::VBString notification,
						long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						br::com::sbVB::VBLib::VBString user);

	void OnMediaReceive(long mediaId,
						long port,
						br::com::sbVB::VBLib::VBString multiCastIp,
						long packetLength,
						long headerLength,
						br::com::sbVB::VBLib::VBString header,
						IrmUser user,
						IrmConnection conn);

	void OnMediaReceiveRun(long mediaId);

	void OnMediaReceiveStop(long mediaId);

	void OnMediaReceiveAll(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							long port,
							br::com::sbVB::VBLib::VBString multiCastIp,
							IrmUser user,
							IrmConnection conn);

	void OnMediaReceiveAllRun(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaReceiveAllStop(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaSend(	long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						long port,
						IrmUser &user,
						IrmConnection &conn);

	void OnMediaSendStop(long mediaId);

	void OnPacketRequest(	long mediaId,
							unsigned long initialPacketIndex,
							unsigned long finalPacketIndex);

	bool IsIdle() const;
	void SetState(MediaConferenceState state);

	MediaConferenceState m_state;
	static const unsigned m_mediaPort;
};

#endif
