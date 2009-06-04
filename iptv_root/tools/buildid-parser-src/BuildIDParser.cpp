#include "BuildIDParser.h"

#include <iostream>
#include <string>
#include <cstring>

#include "VBLib/VBLib.h"
using namespace br::com::sbVB::VBLib;

#define VERSION_NUMBER_DEFINE	"BUILD_USER_VERSION"
#define BUILD_ID_DEFINE			"BUILD_ID"

VBString BuildIDParser::GetVersionStringFromFile(const char *filename)
{
	if(!filename)
		return "";

	std::ifstream inputFile;
	inputFile.open(filename);
	if(!inputFile)
		return "";
	std::string lineBuf;
	VBString convBuf;
	VBString temp1, temp2;
	VBString versionNumber, buildID;
	bool foundVersion = false, foundID = false;

	while(!inputFile.eof())
	{
		getline(inputFile, lineBuf);
		convBuf = lineBuf.c_str();
		// Line must begin with "#define"
		convBuf = convBuf.afterFind("#define");
		// Get version number
		temp1 = convBuf.afterFind(VERSION_NUMBER_DEFINE);
		if(temp1 != "")
		{
			temp1.wipeLeadingSpace();
			versionNumber = temp1;
			versionNumber.replace("\"", "");
			versionNumber.replace("\r", "");	// Dealing with Windows end-of-lines
			foundVersion = true;
		}
		// Get build ID
		temp2 = convBuf.afterFind(BUILD_ID_DEFINE);
		if(temp2 != "")
		{
			temp2.wipeLeadingSpace();
			buildID = temp2;
			buildID.replace("\"", "");
			buildID.replace("\r", "");
			foundID = true;
		}
	}
	if(!foundVersion && !foundID)
		return "";
	
	return MakeVersionString(versionNumber, buildID);
}

VBString BuildIDParser::MakeVersionString(const char *versionNumber, const char *buildID)
{
	if((!versionNumber) || (!buildID))
		return "";
	VBString result = VBString(versionNumber) + "." + VBString(buildID);
	return result;
}
