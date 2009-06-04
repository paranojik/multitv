#ifndef BASE_64_H
#define BASE_64_H

#include "VBLib/VBLib.h"
#include "shared/common.h"
#include <list>

/** @brief This is an OO wrapper to the base 64 encoding/decoding 'C' functions.
 *
 *
 */
class Base64
{
public:
	Base64();
	~Base64();

	int Encode64(	char *in, int inSize,
					br::com::sbVB::VBLib::VBString &out);

	int Decode64(	const br::com::sbVB::VBLib::VBString &in,
					char **out);
private:
	std::list<char *> m_bufferList;
};

#endif
