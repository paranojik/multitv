#ifndef BUILDIDPARSER_H
#define BUILDIDPARSER_H

#include "VBLib/VBLib.h"

class BuildIDParser
{
public:
	static br::com::sbVB::VBLib::VBString GetVersionStringFromFile(const char *fileName);
	static br::com::sbVB::VBLib::VBString MakeVersionString(const char *versionNumber, const char *buildID);
};

#endif
