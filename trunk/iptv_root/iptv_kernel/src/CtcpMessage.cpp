#include "iptv_kernel/CtcpMessage.h"

using namespace br::com::sbVB::VBLib;

const char CtcpMessage::m_ctcpPrefix = '\01';

CtcpMessage::CtcpMessage(VBString message) : m_isCtcp(false)
{
	if(!message.IsEmpty())
		Parse(message);
}

void CtcpMessage::Parse(VBString message)
{
	if(message.getChar(0) != m_ctcpPrefix)
		m_isCtcp = false;
	else
	{
		m_isCtcp = true;
		m_message = message.strtok(m_ctcpPrefix, 1);
		m_parameters = message.strtok(m_ctcpPrefix, 2);
	}
}

bool CtcpMessage::IsCtcpMessage() const
{
	return m_isCtcp;
}

VBString CtcpMessage::GetCtcpMessage() const
{
	return m_message;
}

VBString CtcpMessage::GetParameters() const
{
	return m_parameters;
}

bool CtcpMessage::operator==(const VBString &rhs) const
{
	if(m_message == rhs)
		return true;

	return false;
}
