#ifndef MEDIA_CONNECTION_DATA_H
#define MEDIA_CONNECTION_DATA_H

#include "VBLib/VBLib.h"

enum IrmNetworkProtocol
{
	IRM_NETWORK_PROTOCOL_INVALID,
	IRM_NETWORK_PROTOCOL_TCP,
	IRM_NETWORK_PROTOCOL_UDP,
	IRM_NETWORK_PROTOCOL_MULTICAST,
};

class MediaConnectionData
{
public:
	MediaConnectionData();

	br::com::sbVB::VBLib::VBString GetHost() const;
	unsigned GetPort() const;
	IrmNetworkProtocol GetNetworkProtocol() const;
	br::com::sbVB::VBLib::VBString GetNetworkProtocolName() const;

	bool IsNetworkProtocolTcp() const;
	bool IsNetworkProtocolUdp() const;
	bool IsNetworkProtocolMulticast() const;

	void SetHost(br::com::sbVB::VBLib::VBString host);
	void SetPort(unsigned port);
	void SetNetworkProtocol(IrmNetworkProtocol networkProtocol);
	void SetNetworkProtocolTcp();
	void SetNetworkProtocolUdp();
	void SetNetworkProtocolMulticast();

private:
	br::com::sbVB::VBLib::VBString m_host;
	unsigned m_port;
	IrmNetworkProtocol m_networkProtocol;

	static const br::com::sbVB::VBLib::VBString m_irmNetworkProtocolTcp;
	static const br::com::sbVB::VBLib::VBString m_irmNetworkProtocolUdp;
	static const br::com::sbVB::VBLib::VBString m_irmNetworkProtocolMulticast;
};

#endif
