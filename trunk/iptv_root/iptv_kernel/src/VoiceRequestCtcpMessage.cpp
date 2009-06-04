#include "iptv_kernel/VoiceRequestCtcpMessage.h"

using namespace br::com::sbVB::VBLib;

const VBString VoiceRequestCtcpMessage::m_ctcpVoiceRequest			= "MEDIAVOICEREQUEST";
const VBString VoiceRequestCtcpMessage::m_ctcpVoiceRequestCancel	= "MEDIAVOICECANCEL";
const VBString VoiceRequestCtcpMessage::m_ctcpVoiceRequestRemoveAll	= "MEDIAVOICEREMOVE";

VoiceRequestCtcpMessage::VoiceRequestCtcpMessage(VBString message) :	CtcpMessage(message),
																		m_isVoiceRequestCtcp(false),
																		m_isRequest(false),
																		m_isCancel(false),
																		m_isRemoveAll(false)
{
	if(this->operator==(m_ctcpVoiceRequest))
	{
		m_isRequest = true;
		m_isVoiceRequestCtcp = true;	
	}

	else if(this->operator==(m_ctcpVoiceRequestCancel))
	{
		m_isCancel = true;
		m_isVoiceRequestCtcp = true;
	}

	else if(this->operator==(m_ctcpVoiceRequestRemoveAll))
	{
		m_isRemoveAll = true;
		m_isVoiceRequestCtcp = true;
	}
}

bool VoiceRequestCtcpMessage::IsVoiceRequestCtcpMessage() const
{
	return m_isVoiceRequestCtcp;
}

bool VoiceRequestCtcpMessage::IsRequest() const
{
	return m_isRequest;
}

bool VoiceRequestCtcpMessage::IsCancel() const
{
	return m_isCancel;
}

bool VoiceRequestCtcpMessage::IsRemoveAll() const
{
	return m_isRemoveAll;
}
