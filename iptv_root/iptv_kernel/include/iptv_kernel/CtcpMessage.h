#ifndef CTCP_MESSAGE_H
#define CTCP_MESSAGE_H

#include "VBLib/VBLib.h"

/** @brief Parses IRC CTCP messages.
 *
 */
class CtcpMessage
{
public:
	CtcpMessage(br::com::sbVB::VBLib::VBString message);

	bool IsCtcpMessage() const;
	bool operator==(const br::com::sbVB::VBLib::VBString &rhs) const;

protected:
	br::com::sbVB::VBLib::VBString GetCtcpMessage() const;
	br::com::sbVB::VBLib::VBString GetParameters() const;
	static const char m_ctcpPrefix;

private:
	void Parse(br::com::sbVB::VBLib::VBString message);
	bool m_isCtcp;
	br::com::sbVB::VBLib::VBString m_message;
	br::com::sbVB::VBLib::VBString m_parameters;
};

#endif

