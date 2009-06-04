#ifndef IRM_MESSAGE_H
#define IRM_MESSAGE_H

#include "VBLib/VBLib.h"

/** @brief Represents an IRM message.
 *
 */
struct IrmMessage
{
	long m_irmMessageCode;

	br::com::sbVB::VBLib::VBString m_strSource;
	br::com::sbVB::VBLib::VBString m_strTarget;
	br::com::sbVB::VBLib::VBString m_strData;
	br::com::sbVB::VBLib::VBString m_strComplement1;
	br::com::sbVB::VBLib::VBString m_strComplement2;
	br::com::sbVB::VBLib::VBString m_strComplement3;
	br::com::sbVB::VBLib::VBString m_strComplement4;
	br::com::sbVB::VBLib::VBString m_strComplement5;
	br::com::sbVB::VBLib::VBString m_strComplement6;
	br::com::sbVB::VBLib::VBString m_strComplement7;

	long m_longSource;
	long m_longTarget;
	long m_longData;
	long m_longComplement1;
	long m_longComplement2;
	long m_longComplement3;
	long m_longComplement4;
	long m_longComplement5;
	long m_longComplement6;
	long m_longComplement7;
};

#endif

