#ifndef IRM_CONNECTION_H
#define IRM_CONNECTION_H

#include "VBLib/VBLib.h"

/** @brief Stores data related to the connection with an IRM server.
 *
 */
class IrmConnection
{
public:
	IrmConnection();

	br::com::sbVB::VBLib::VBString GetHost();
	unsigned GetPort();

	void SetHost(br::com::sbVB::VBLib::VBString host);
	void SetPort(unsigned port);
	void Reset();

private:
	br::com::sbVB::VBLib::VBString m_host;
	unsigned m_port;
};

#endif
