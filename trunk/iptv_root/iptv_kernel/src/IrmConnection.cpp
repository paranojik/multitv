#include "iptv_kernel/IrmConnection.h"

IrmConnection::IrmConnection() : m_port(0)
{

}

br::com::sbVB::VBLib::VBString IrmConnection::GetHost()
{
	return m_host;
}

unsigned IrmConnection::GetPort()
{
	return m_port;
}

void IrmConnection::SetHost(br::com::sbVB::VBLib::VBString host)
{
	m_host = host;
}

void IrmConnection::SetPort(unsigned port)
{
	m_port = port;
}

void IrmConnection::Reset()
{
	m_host = "";
	m_port = 0;
}
