#include "shared/compat.h"
#include "iptv_appsharing/include/ASCompat.h"
#include "iptv_kernel/ApplicationSharing.h"
#include "iptv_kernel/ApplicationSharingNotificationParameters.h"

using namespace br::com::sbVB::VBLib;

const unsigned ApplicationSharing::m_appSharingMediaPort = 8007;

ApplicationSharing::ApplicationSharing() : m_currentFrame(NULL)
{
	// Reseting the members.
	m_mutex = new _SEMAPHORE(SEMAPH_MUTEX);
	Reset();
}

ApplicationSharing::~ApplicationSharing()
{
	delete m_mutex;
}

void ApplicationSharing::Lock()
{
	m_mutex->Wait();
}

void ApplicationSharing::Unlock()
{
	m_mutex->Signal();
}

void ApplicationSharing::InitializeReception(long mediaId, ISockBuff *sockBuff)
{
	if(IsReceiving())
	{
		m_appSharing.Initialize(sockBuff, AS_RECEIVING, mediaId, this);
	}
}

void ApplicationSharing::StopReception()
{
	if(IsReceiving())
	{
		RequestMediaReceiveAllStop(m_receptionData.m_channel, m_irmMediaClassApplicationSharing);
		Reset();
	}
}

long ApplicationSharing::GetReceptionMediaId()
{
	return m_receptionData.m_mediaId;
}

 void ApplicationSharing::OnASEvent(int nEvent, void *pParams, unsigned int nSize)
{
	Lock();

	if(IsReceiving())
	{
		switch(nEvent)
		{
			case AS_NOTIFY_RECEIVE_ERROR:
			{
				Reset();

				// Notifying the kernel.
				ASNotificationParameter param;
				param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_appSharingCode = ASNC_RECEIVE_ERROR;
				param.m_mediaId = m_receptionData.m_mediaId;
				param.m_user = m_receptionData.m_user;
				param.m_channel = m_receptionData.m_channel;
				Notify(param);
			}
			break;
			case AS_NOTIFY_RECEIVE_START:
			{
				// Notifying the kernel.
				ASNotificationParameter param;
				param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_appSharingCode = ASNC_RECEIVE_BEGIN;
				param.m_mediaId = m_receptionData.m_mediaId;
				param.m_user = m_receptionData.m_user;
				param.m_channel = m_receptionData.m_channel;
				Notify(param);
			}
			break;
			case AS_NOTIFY_RECEIVE_STOP:
			{
				//// Notifying the kernel.
				//ASNotificationParameter param;
				//param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
				//param.m_code = IMNC_MODULE_SPECIFIC;
				//param.m_appSharingCode = ASNC_RECEIVE_END;
				//param.m_mediaId = m_receptionData.m_mediaId;
				//param.m_user = m_receptionData.m_user;
				//param.m_channel = m_receptionData.m_channel;
				//Notify(param);
			}
			break;
			case AS_NOTIFY_RECEIVE_NEW_FRAME:
			{
				if(m_currentFrame)
					delete m_currentFrame;
				m_currentFrame = new ASFrame();

				m_appSharing.GetFrame(m_currentFrame);
	
				// Notifying the kernel.
				ASReceiveFrame param;
				param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_appSharingCode = ASNC_RECEIVE_NEW_FRAME;
				param.m_mediaId = m_receptionData.m_mediaId;
				param.m_user = m_receptionData.m_user;
				param.m_channel = m_receptionData.m_channel;
				param.m_buffer = m_currentFrame->m_buffer;
				param.m_bufferlength = m_currentFrame->m_bufferlength;
				param.m_width = m_currentFrame->m_width;
				param.m_height = m_currentFrame->m_height;
				param.m_format = m_currentFrame->m_format;
				Notify(param);
			}
			break;
		}
	}

	Unlock();
}

void ApplicationSharing::OnUserPrivateMessage(VBString sender, VBString message)
{

}

void ApplicationSharing::OnChannelPrivateMessage(VBString sender, VBString channel, VBString message)
{

}

void ApplicationSharing::OnMediaNotify(	VBString notification, long mediaId, VBString mediaClass,
										VBString channel, VBString user)
{
	if(IsMediaClassAppSharing(mediaClass))
	{
		if(IsMediaNotificationSend(notification))
		{
			// There is a new Application Sharing transmission.
			if(IsIdle())
			{
				Reset();

				// Storing information about the current app sharing transmission.
				m_receptionData.m_mediaId = mediaId;
				m_receptionData.m_channel = channel;
				m_receptionData.m_user = user;

				MediaConnectionData connInfo;
				connInfo = GetReceptionData();

				SetState(AS_STATE_RECEIVING);

				RequestMediaReceiveAll(channel, mediaClass, connInfo.GetNetworkProtocolName(), connInfo.GetPort());

			}
		}

		else if(IsMediaNotificationStart(notification))
		{
			// An user accepted the Application Sharing transmission.
		}

		else if(IsMediaNotificationStop(notification))
		{
			// An user stopped receiving the Application Sharing transmission.
		}
	}
}

void ApplicationSharing::OnMediaReceive(	long mediaId, long port, VBString multiCastIp,
											long packetLength, long headerLength, VBString header,
											IrmUser user, IrmConnection conn)
{
	// App Sharing does not handle this message.
}

void ApplicationSharing::OnMediaReceiveRun(long mediaId)
{
	// App Sharing does not handle this message.
}

void ApplicationSharing::OnMediaReceiveStop(long mediaId)
{
	if(mediaId == m_receptionData.m_mediaId)
	{
		// Notifying the kernel.
		ASNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_appSharingCode = ASNC_RECEIVE_END;
		param.m_mediaId = m_receptionData.m_mediaId;
		param.m_user = m_receptionData.m_user;
		param.m_channel = m_receptionData.m_channel;
		Notify(param);
	}
}

void ApplicationSharing::OnMediaReceiveAll(	VBString channel,
											VBString mediaClass,
											long port,
											VBString multiCastIp,
											IrmUser user,
											IrmConnection conn)
{
	if(IsMediaClassAppSharing(mediaClass))
	{
		if(m_state == AS_STATE_RECEIVING)		
		{
			MediaConnectionData connInfo;
			connInfo = GetReceptionData(conn.GetHost(), port, multiCastIp);

			if(!connInfo.IsNetworkProtocolTcp())
				RequestMediaReceiveAllRun(channel, mediaClass);

			// Notifying the kernel.
			ASReceiveInit param;
			param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
			param.m_code = IMNC_MODULE_SPECIFIC;
			param.m_appSharingCode = ASNC_RECEIVE_INIT;
			param.m_mediaId = m_receptionData.m_mediaId;
			param.m_user = user.GetNick();
			param.m_channel = channel;
			param.m_mediaClass = m_irmMediaClassApplicationSharing;
			param.m_networkProtocol = connInfo.GetNetworkProtocolName();
			param.m_host = connInfo.GetHost();
			param.m_port = connInfo.GetPort();
			Notify(param);
		}
	}
}

void ApplicationSharing::OnMediaReceiveAllRun(	VBString channel,
												VBString mediaClass)
{
		// Nothing to do.
}

void ApplicationSharing::OnMediaReceiveAllStop(	VBString channel,
												VBString mediaClass)
{
		// Nothing to do.
}

void ApplicationSharing::OnMediaSend(	long mediaId,
										br::com::sbVB::VBLib::VBString mediaClass,
										br::com::sbVB::VBLib::VBString channel,
										long port,
										IrmUser &user,
										IrmConnection &conn)
{
	// Currently the module can not create a transmission.
}

void ApplicationSharing::OnMediaSendStop(long mediaId)
{
	// Currently the module can not create a transmission.
}

void ApplicationSharing::OnPacketRequest(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	// App Sharing does not handle this message.
}

bool ApplicationSharing::IsIdle() const
{
	if(m_state == AS_STATE_IDLE)
		return true;
	return false;
}

bool ApplicationSharing::IsReceiving() const
{
	if(m_state == AS_STATE_RECEIVING)
		return true;
	return false;
}

void ApplicationSharing::SetState(AppSharingState state)
{
	m_state = state;
}

unsigned ApplicationSharing::GetMediaPort() const
{
	return m_appSharingMediaPort;
}

void ApplicationSharing::Reset()
{
	// Notifying the kernel.
	ASNotificationParameter param;
	param.m_moduleId = IPTV_MODULE_ID_APP_SHARING;
	param.m_code = IMNC_MODULE_SPECIFIC;
	param.m_appSharingCode = ASNC_RECEIVE_ABORT;
	param.m_mediaId = m_receptionData.m_mediaId;
	param.m_user = m_receptionData.m_user;
	param.m_channel = m_receptionData.m_channel;
	Notify(param);

	m_appSharing.End();
	m_receptionData.Reset();
	SetState(AS_STATE_IDLE);
	if(m_currentFrame != NULL)
		delete m_currentFrame;
	m_currentFrame = NULL;
}

AppSharingData::AppSharingData()
{
	Reset();
}

void AppSharingData::Reset()
{
	m_mediaId = 0;
	m_channel = "";
	m_user = "";
}
