#include "iptv_kernel/IPTVModule.h"

using namespace br::com::sbVB::VBLib;

IPTVModule::IPTVModule() :	m_notificationSink(NULL),
							m_enabled(false)
{

}

void IPTVModule::SetNotificationSink(IPTVModuleNotificationSink *sink)
{
	m_notificationSink = sink;
}

void IPTVModule::Notify(IPTVModuleNotificationParameter &parameter) const
{
	if(m_notificationSink != NULL)
		m_notificationSink->OnIPTVModuleNotification(parameter);
}

void IPTVModule::SendPrivateMessage(VBString recipient, VBString message)
{
	if(!message.IsEmpty())
	{
		IMNPPrivateMessage parameter;
		parameter.m_code = IMNC_SEND_PRIVATE_MESSAGE;
		parameter.m_recipient = recipient;
		parameter.m_message = message;
		Notify(parameter);
	}
}

void IPTVModule::SendChannelPrivateMessage(VBString channel, VBString message)
{
	if(!message.IsEmpty())
	{
		IMNPPrivateMessage parameter;
		parameter.m_code = IMNC_SEND_CHANNEL_PRIVATE_MESSAGE;
		parameter.m_recipient = channel;
		parameter.m_message = message;
		Notify(parameter);
	}
}

void IPTVModule::RequestChannelWhiteboard(VBString channel, VBString data)
{
		IMNPWhiteboard parameter;
		parameter.m_code = IMNC_REQUEST_CHANNEL_WHITEBOARD;
		parameter.m_channel = channel;
		parameter.m_data = data;
		Notify(parameter);
}

bool IPTVModule::IsEnabled() const
{
	return m_enabled;
}

void IPTVModule::Enable()
{
	m_enabled = true;
}

void IPTVModule::Disable()
{
	m_enabled = false;
}
