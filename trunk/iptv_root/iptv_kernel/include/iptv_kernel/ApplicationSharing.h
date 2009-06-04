#ifndef APPLICATION_SHARING_H
#define APPLICATION_SHARING_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVMediaModule.h"
#include "shared/compat.h"
#include "shared/semaphore.h"
#include "shared/socket/isockbuff.h"
#include "iptv_appsharing/include/ASModuleCross.h"
#include "iptv_appsharing/include/ASListener.h"

enum AppSharingState	{
							AS_STATE_IDLE,
							AS_STATE_RECEIVING
						};

struct AppSharingData
{
	AppSharingData();
	void Reset();

	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_user;
};

/** @brief Implements the Application Sharing module.
 *
 * This class responsibilities are handling Application Sharing IRM messages and provinding a higher
 * level interface to the iptv_appsharing library that implements the Application Sharing rules.
 *
 */
class ApplicationSharing :	public IPTVMediaModule,
							public CxAppSharingListener
{
public:
	ApplicationSharing();
	~ApplicationSharing();
	void InitializeReception(long mediaId, ISockBuff *sockBuff);
	void StopReception();
	long GetReceptionMediaId();

private:
	 void OnASEvent(int nEvent, void *pParams, unsigned int nSize);

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
	bool IsReceiving() const;
	void SetState(AppSharingState state);

	ASModule m_appSharing;
	ASFrame* m_currentFrame;

	AppSharingState m_state;

	AppSharingData m_receptionData;

	static const unsigned m_appSharingMediaPort;

	void Lock();
	void Unlock();

	_SEMAPHORE *m_mutex;
};

#endif
