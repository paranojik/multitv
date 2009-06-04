// VBLib.cpp

#include "VBLib/VBLib.h"
#include "VBLib/VBLib_buildid.h"
#include <iostream>
#include <fstream>
#include <climits>
#include <cfloat>

///////////////////////////////////////////////////////////

// initialization of global variables
// typedef void (*customAssertFunctionType)(const char *);
// VBLIB_DLL_API customAssertFunctionType g_customAssertFunction = 0;
VBLIB_DLL_API void (*::br::com::sbVB::VBLib::g_customAssertFunction)(bool b, const char * message) = 0;
// VBLIB_DLL_API int ::br::com::sbVB::VBLib::myGlobal = 3;

///////////////////////////////////////////////////////////



VBLIB_DLL_API void ::br::com::sbVB::VBLib::VBAssert(bool b, const char *message)
{
	if (!b)
		if (g_customAssertFunction)
			g_customAssertFunction(b,message);
		else
			::std::cout << "VBAssert assertion failed: " << message << ::std::endl;
}

// the global functions below came from CXL

VBLIB_DLL_API char * ::br::com::sbVB::VBLib::strins(const char *instr,char *str,unsigned st_pos)
{
    register unsigned i,leninstr;

    /* get length of string to insert */
    leninstr=strlen(instr);

    /* shift affected portion of str text to the right */
    for(i=strlen(str);i>=st_pos;i--) *(str+leninstr+i)=*(str+i);

    /* insert instr text */
    for(i=0;i<leninstr;i++) *(str+st_pos+i)=*(instr+i);

    /* return address of modified string */
    return(str);
}

VBLIB_DLL_API char *::br::com::sbVB::VBLib::strdel(const char *substr,char *str)
{
    char *src,*dest;

    dest=strinc(substr,str);
    if(!dest) return(NULL);
    src=dest+strlen(substr);
    strcpy(dest,src);
    return(str);
}


VBLIB_DLL_API char *::br::com::sbVB::VBLib::strsrep(char *str,const char *search,const char *replace)
{
    char *p;

    if((p=strinc(search,str))!=NULL) {
		::br::com::sbVB::VBLib::strdel(search,str);
        ::br::com::sbVB::VBLib::strins(replace,str,(int)((unsigned long)p-(unsigned long)str));
        p=str;
    }

    return(p);
}


VBLIB_DLL_API char *::br::com::sbVB::VBLib::strinc(const char *str1,char *str2)
{
    register unsigned max;
    char *p;

    max=strlen(str1);

    for(p=str2;*p;p++) {
        if(!strncmp(str1,p,max)) return(p);
    }
    return(NULL);                       /* string1 not found in string2 */
}


VBLIB_DLL_API char *::br::com::sbVB::VBLib::strschg(char *str,const char *find,const char *replace)
{
    register unsigned count=0;
    register char *p;

    p=str;
    while((p=::br::com::sbVB::VBLib::strinc(find,p))!=NULL) {
        ::br::com::sbVB::VBLib::strsrep(p,find,replace);
/***************** line included by Sergio Villas Boas **************/
        p += (int) strlen(replace); 
/*------------------------------------------------------------------*/
        count++;
    }

	if (count) return (str);
    else return (NULL);
//    return(count?str:NULL);
} 

////////////////////////////////////////////////////////////////////
// end of the global functions that came from CXL


////////////////////////////////////////////////////////////////////
// functions to convert ascii and related

// Example: in = 15, return 'F'
// Example: in = 3, return = '3'
// p_ToAscii, p_ToAsciiReverse
VBLIB_DLL_API unsigned char br::com::sbVB::VBLib::charInt2ascii(unsigned char in)
{
	unsigned char z = toupper(in);
	if (z < 10)  
		z += '0';
	else 	     
		z += 'A' - 10;
	return z;
}

// Example: in = 'F', return = 15
// Example: in = '3', return = 3
// p_ToAscii, p_ToAsciiReverse
VBLIB_DLL_API unsigned char br::com::sbVB::VBLib::charInt2asciiReverse(unsigned char in)
{
	unsigned char z = toupper(in);
	if (z >= '0' && z <= '9')  
		z -= '0';
	if (z >= 'A' && z <= 'F')  
		z -= 'A' - 10;
	return z;
}


// Example: in = 15, return "0F" 
// p_byteToAscii
VBLIB_DLL_API ::br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::charInt2asciiString(unsigned char in)
{
	const char chars[] = "0123456789ABCDEF";
	::br::com::sbVB::VBLib::VBString ret;
	unsigned char msb;
	msb = (in & 0xf0)/16;
	ret += chars[msb];
	unsigned char lsb;
	lsb = (in & 0x0f);
	ret += chars[lsb]; 
	return ret;
}

// Example: in = "0F", return 15
// Example: in = "1F", return 31
VBLIB_DLL_API unsigned char br::com::sbVB::VBLib::asciiString2charInt(::br::com::sbVB::VBLib::VBString in)
{
	unsigned char ret = br::com::sbVB::VBLib::charInt2asciiReverse(in[0]) * 16 + 
		br::com::sbVB::VBLib::charInt2asciiReverse(in[1]);
	return ret;
}

// Example: in = 'A',  out = "41"
// p_toHex
VBLIB_DLL_API ::br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::ascii2charIntString(unsigned char in)
{
	::br::com::sbVB::VBLib::VBString ret;
	unsigned char a;
	a = (in & 0xf0)/16;
	ret += ::br::com::sbVB::VBLib::charInt2ascii(a);; // MSB
	a = (in & 0x0f);
	ret += ::br::com::sbVB::VBLib::charInt2ascii(a); // LSB
	return ret;
}

// Example: in = "41",  return = 'A'
// p_toHexReverse
VBLIB_DLL_API unsigned char br::com::sbVB::VBLib::charIntString2ascii(::br::com::sbVB::VBLib::VBString in)
{
	unsigned char ret;
	int msb, lsb;
	msb = ::br::com::sbVB::VBLib::charInt2ascii(in[0]);
	lsb = ::br::com::sbVB::VBLib::charInt2ascii(in[1]);
	ret = lsb + msb * 16;
	return ret;
}


VBLIB_DLL_API bool br::com::sbVB::VBLib::fileExist(const char *fileName)
{
	std::ifstream testFile;
	testFile.open(fileName);
	if (!testFile)
		return false;
	else
	{
		testFile.close();
		return true;
	}
}


VBLIB_DLL_API void br::com::sbVB::VBLib::getFileContentsToString(const char *fileName, VBString & contents) 
throw (::br::com::sbVB::VBLib::VBException)
{
	std::ifstream textFile;
	textFile.open(fileName);
	if (!textFile)
	{
		VBString message = "could not open file ";
		message += fileName;
		throw VBException(message);
	}

	contents = "";
	VBString aux;

	while (true)
	{	
		textFile >> aux;
		if (textFile.eof() && aux.IsEmpty()) break;
		contents += aux;
	}

}

VBLIB_DLL_API  bool br::com::sbVB::VBLib::isSpaceChar(char c)
{
	char spaceChars[] = { ' ', '\t', '\n', '\b', 0 };
	unsigned len = sizeof(spaceChars) / sizeof(char);

	for (unsigned i=0 ; i < len ; i++)
	{
		if (c==spaceChars[i])
			return true;
	}
	return false;

}

// definition of global (static) variables
// VBLIB_DLL_API  double br::com::sbVB::VBLib::Static::eps = 1e-8;
//VBLIB_DLL_API  double br::com::sbVB::VBLib::eps = 1e-8;


////////////////////////////////////////////////////////////////////
// global functions for identification in VBDllReport


VBLIB_DLL_API const char *br::com::sbVB::VBLib::getBuildID()
{
	return BUILD_ID;
}

VBLIB_DLL_API const char *br::com::sbVB::VBLib::getBuild_dateTime()
{
	return BUILD_DATETIME;
}

VBLIB_DLL_API const char *br::com::sbVB::VBLib::getBuild_dateTime_str()
{
	return BUILD_DATETIME_STR;
}

VBLIB_DLL_API const char *br::com::sbVB::VBLib::getUserVersion()
{
	return BUILD_USER_VERSION;
}

VBLIB_DLL_API const char *br::com::sbVB::VBLib::getLibraryDescription()
{
	return "sbVB general purpose library in C++ cross platform\n"
		"www.sbvb.com.br";
}

VBLIB_DLL_API bool br::com::sbVB::VBLib::nearEqual(double x, double y)
{ 
	double eps = DBL_EPSILON;
	return ::fabs(x-y) < eps; 
};

VBLIB_DLL_API void br::com::sbVB::VBLib::buildID()
{
using namespace std;
	cout << "VBLib - www.sbvb.com.br" << endl;
	cout << "Version=" << BUILD_USER_VERSION << endl;
	cout << "BuildID=" << BUILD_ID << endl;
	cout << "Build DateTime=" << BUILD_DATETIME << endl;
}


VBLIB_DLL_API int br::com::sbVB::VBLib::VBmin(int x, int y)
{
	if (x < y)
		return x;
	return y;
}

VBLIB_DLL_API int br::com::sbVB::VBLib::VBmax(int x, int y)
{
	if (x > y)
		return x;
	return y;
}

