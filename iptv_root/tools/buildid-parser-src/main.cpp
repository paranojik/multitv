#include <iostream>
#include <cstring>
using namespace std;

#include "BuildIDParser.h"
#include "VBLib/VBLib.h"
using namespace br::com::sbVB::VBLib;

#define ARG_COUNT 1

int main(int argc, char **argv)
{
	if(argc <= ARG_COUNT)
	{
		cout << "ERROR: Bad number of arguments" << endl;
		cout << "Usage: " << argv[0] << " <filename>" << endl;
		return -1;
	}

	VBString versionStr = BuildIDParser::GetVersionStringFromFile(argv[1]);
	if(!versionStr)
	{
		cout << "ERROR: Error reading from input file, or it doesn't contain the required information." << endl;
		return -2;
	}

	cout << versionStr << endl;
	return 0;
}
