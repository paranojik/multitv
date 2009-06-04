#include "iptv_kernel/MediaConnectionData.h"
using namespace br::com::sbVB::VBLib;

const br::com::sbVB::VBLib::VBString MediaConnectionData::m_irmNetworkProtocolTcp		= "TCP";
const br::com::sbVB::VBLib::VBString MediaConnectionData::m_irmNetworkProtocolUdp		= "UDP";
const br::com::sbVB::VBLib::VBString MediaConnectionData::m_irmNetworkProtocolMulticast	= "MCAST";

MediaConnectionData::MediaConnectionData() :	m_networkProtocol(IRM_NETWORK_PROTOCOL_INVALID),
												m_port(0)
{

}

VBString MediaConnectionData::GetHost() const
{
	return m_host;
}

unsigned MediaConnectionData::GetPort() const
{
	return m_port;
}

IrmNetworkProtocol MediaConnectionData::GetNetworkProtocol() const
{
	return m_networkProtocol;
}

VBString MediaConnectionData::GetNetworkProtocolName() const
{
	switch(m_networkProtocol)
	{
		case IRM_NETWORK_PROTOCOL_TCP:
			return m_irmNetworkProtocolTcp;
		case IRM_NETWORK_PROTOCOL_UDP:
			return m_irmNetworkProtocolUdp;
		case IRM_NETWORK_PROTOCOL_MULTICAST:
			return m_irmNetworkProtocolMulticast;
		default:
			return "";
	}
}

bool MediaConnectionData::IsNetworkProtocolTcp() const
{
	if(m_networkProtocol == IRM_NETWORK_PROTOCOL_TCP)
		return true;

	return false;
}

bool MediaConnectionData::IsNetworkProtocolUdp() const
{
	if(m_networkProtocol == IRM_NETWORK_PROTOCOL_UDP)
		return true;

	return false;
}

bool MediaConnectionData::IsNetworkProtocolMulticast() const
{
	if(m_networkProtocol == IRM_NETWORK_PROTOCOL_MULTICAST)
		return true;

	return false;
}

void MediaConnectionData::SetHost(br::com::sbVB::VBLib::VBString host)
{
	m_host = host;
}

void MediaConnectionData::SetPort(unsigned port)
{
	m_port = port;
}

void MediaConnectionData::SetNetworkProtocol(IrmNetworkProtocol networkProtocol)
{
	m_networkProtocol = networkProtocol;
}

void MediaConnectionData::SetNetworkProtocolTcp()
{
	m_networkProtocol = IRM_NETWORK_PROTOCOL_TCP;
}

void MediaConnectionData::SetNetworkProtocolUdp()
{
	m_networkProtocol = IRM_NETWORK_PROTOCOL_UDP;
}

void MediaConnectionData::SetNetworkProtocolMulticast()
{
	m_networkProtocol = IRM_NETWORK_PROTOCOL_MULTICAST;
}
