#ifndef ASF_FULL_HEADER_H
#define ASF_FULL_HEADER_H

#include "ASFHeader.h"

// it combines the ASF Header + Data Object header

class ASFFullHeader : public ASFHeader  
{
public:

	QWORD GetObjectSize() { return m_p ? *((QWORD*) &m_p[16]) + 50 : 0; }
};

#endif

