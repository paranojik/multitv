// VBException.cpp

#include "VBLib/VBLib.h"

/////////////////////////////////////////////////////////////////
//
// VBException
//
/////////////////////////////////////////////////////////////////


::br::com::sbVB::VBLib::VBException::VBException(const char* message) 
{
	m_message = message;
}

const char *::br::com::sbVB::VBLib::VBException::getExplanation() const
{
	return m_message;
}
