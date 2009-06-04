// VBMD5.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {


class VBLIB_DLL_API VBException_MD5 : public ::br::com::sbVB::VBLib::VBException
{
public:
	VBException_MD5 (const char *m) : VBException(m) {};
};
	

/////////////////////////////////////////////////////////////////////
//
// VBMD5 is the Villas-Boas class for MD5 hash encryption
// read about md5 in the url below
// http://www.fourmilab.ch/md5/rfc1321.html
// by sbVB [Villas-Boas] 
// (villas@del.ufrj.br, www.del.ufrj.br/~villas, www.sbVB.com.br)
// based on code from Thomas Pflüger (unixtom@sensorme.de)
/************* sample code begin **********************
#include "vblib.h"
void main ()
{
	VBString source = "abc";
	VBMD5 a;
	VBString s;
	s = a.MD5_hash(source);
	cout << s << endl;
	s = source;
	s.MD5_hash();
	cout << s << endl;
	cout << "Hash of: " << source << " is '" << source.get_MD5_hash() << "'" << endl;
}
**********************sample code end ******************/

class VBLIB_DLL_API VBMD5
{
public:
	VBMD5(); // default constructor

	// return as string
	::br::com::sbVB::VBLib::VBString MD5_hash
		(const ::br::com::sbVB::VBLib::VBString source);
	::br::com::sbVB::VBLib::VBString MD5_file
		(const ::br::com::sbVB::VBLib::VBString fileName);
	::br::com::sbVB::VBLib::VBString MD5_Bin
		(const br::com::sbVB::VBLib::VBBinaryData & data);

	// return as binary
	::br::com::sbVB::VBLib::VBBinaryData MD5_hashBin
		(const br::com::sbVB::VBLib::VBString source);
	::br::com::sbVB::VBLib::VBBinaryData MD5_fileBin
		(const ::br::com::sbVB::VBLib::VBString fileName);
	::br::com::sbVB::VBLib::VBBinaryData MD5_BinBin
		(const ::br::com::sbVB::VBLib::VBBinaryData & data);

private:
	// those methods might be public

	//Update context to reflect the concatenation of another buffer of bytes.
	void AddData(char const* pcData, int iDataLength);
	//Final wrapup - pad to 64-byte boundary with the bit pattern 
	//1 0*(64-bit count of bits processed, MSB-first)
	void FinalDigest(char* pcDigest);
	//Reset current operation in order to prepare a new one
	void Reset();

private:
	// those methods are definitivelly private
	enum { BLOCKSIZE=64 };
	bool m_bAddData;
	void Char2Hex(unsigned char ch, char* szHex);
	void Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr);
	enum { MD128LENGTH=4 };;
	//Context Variables
	unsigned int m_auiBuf[4];
	unsigned int m_auiBits[2];
	unsigned char m_aucIn[64];
	//
	static unsigned int F1(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F2(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F3(unsigned int x, unsigned int y, unsigned int z);
	static unsigned int F4(unsigned int x, unsigned int y, unsigned int z);
	//This is the central step in the MD5 algorithm.
	static void MD5STEP(unsigned int (*f)(unsigned int x, unsigned int y, unsigned int z),
		unsigned int& w, unsigned int x, unsigned int y, unsigned int z, unsigned int data, unsigned int s);
	//
	//The core of the MD5 algorithm, this alters an existing MD5 hash to
	//reflect the addition of 16 longwords of new data. MD5Update blocks
	//the data and converts bytes into longwords for this routine.
	void Transform();
};

}}}} // close namespace ::br::com::sbVB::VBLib


