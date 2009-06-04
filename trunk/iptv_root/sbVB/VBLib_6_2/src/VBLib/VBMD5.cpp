// VBMD5.cpp

#include "VBLib/VBLib.h"


/////////////////////////////////////////////////////////////////
//
// VBMD5
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// public methods

	// return as string
	// br::com::sbVB::VBLib::VBString MD5_hash(const br::com::sbVB::VBLib::VBString source);
	// br::com::sbVB::VBLib::VBString MD5_file(const br::com::sbVB::VBLib::VBString fileName);
	// br::com::sbVB::VBLib::VBString MD5_Bin(const br::com::sbVB::VBLib::VBBinaryData & data);

	// return as binary
	// br::com::sbVB::VBLib::VBBinaryData MD5_hashBin(const br::com::sbVB::VBLib::VBString source);
	// br::com::sbVB::VBLib::VBBinaryData MD5_fileBin(const br::com::sbVB::VBLib::VBString fileName);
	// br::com::sbVB::VBLib::VBBinaryData MD5_BinBin(const br::com::sbVB::VBLib::VBBinaryData & data);


// this works
// using namespace ::br::com::sbVB;
// VBString VBMD5::MD5_hash(const VBString source) 
// this does not work
// br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBMD5::MD5_hash(const br::com::sbVB::VBLib::VBString source) 

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBMD5::MD5_hash(const br::com::sbVB::VBLib::VBString source) 
{
	char acDigest[65] = {0};
	char acHex[129] = {0};

	AddData(source.c_str(), source.length());
	FinalDigest(acDigest);
	Binary2Hex((unsigned char*)acDigest, 16, acHex);

	return br::com::sbVB::VBLib::VBString(acHex);
}

br::com::sbVB::VBLib::VBBinaryData br::com::sbVB::VBLib::VBMD5::MD5_hashBin(const br::com::sbVB::VBLib::VBString source)
{
	char acDigest[65] = {0};
	char acHex[129] = {0};
	AddData(source.c_str(), source.length());
	FinalDigest(acDigest);
	// Binary2Hex((unsigned char*)acDigest, 16, acHex);

	VBBinaryData ret;
	ret.setData((void*)acDigest,16);
	return ret;
}


br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBMD5::MD5_file(const VBString fileName)
{
	typedef unsigned char TYPE;
	
	TYPE buffer;

	::std::ifstream readFile;   

	// open file to check if it exists
	readFile.open(fileName);  
	if (!readFile) 
		throw VBException_MD5("file does not exist");
	readFile.close();

	// open again, this time to use
	readFile.open(fileName, ::std::ios::binary);   
	if (!readFile)     
		throw VBException_MD5("could not open file"); 

	char s[2];
	s[1] = 0; // terminate string
	// loop to read entire file, byte by byte
	while (true) 
	{     
		readFile.read((char*)&buffer, sizeof(TYPE));     
		if (readFile.eof()) break;     
		s[0] = buffer; 
		AddData((const char *)s,1);
	} 

	br::com::sbVB::VBLib::VBString ret;
	char acDigest[65] = {0};
	char acHex[129] = {0};

	FinalDigest(acDigest);
	Binary2Hex((unsigned char*)acDigest, 16, acHex);

	return br::com::sbVB::VBLib::VBString(acHex);
}

br::com::sbVB::VBLib::VBBinaryData br::com::sbVB::VBLib::VBMD5::MD5_fileBin(const br::com::sbVB::VBLib::VBString fileName)
{
	typedef unsigned char TYPE;
	
	TYPE buffer;

	::std::ifstream readFile;   

	// open file to check if it exists
	readFile.open(fileName);  
	if (!readFile) 
		throw VBException_MD5("file does not exist");
	readFile.close();

	// open again, this time to use
	readFile.open(fileName, ::std::ios::binary);   
	if (!readFile)     
		throw VBException_MD5("could not open file"); 

	char s[2];
	s[1] = 0; // terminate string
	// loop to read entire file, byte by byte
	while (true) 
	{     
		readFile.read((char*)&buffer, sizeof(TYPE));     
		if (readFile.eof()) break;     
		s[0] = buffer; 
		AddData((const char *)s,1);
	} 

	char acDigest[65] = {0};
	FinalDigest(acDigest);
	VBBinaryData ret;
	ret.setData((void*)acDigest,16);
	return ret;
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBMD5::MD5_Bin(const br::com::sbVB::VBLib::VBBinaryData & data)
{
	AddData((const char *)data.getDataPointer(),data.getSize());
	char acDigest[65] = {0};
	char acHex[129] = {0};
	FinalDigest(acDigest);
	Binary2Hex((unsigned char*)acDigest, 16, acHex);
	return br::com::sbVB::VBLib::VBString(acHex);
}

br::com::sbVB::VBLib::VBBinaryData br::com::sbVB::VBLib::VBMD5::MD5_BinBin(const br::com::sbVB::VBLib::VBBinaryData & data)
{
	AddData((const char *)data.getDataPointer(),data.getSize());
	char acDigest[65] = {0};
	FinalDigest(acDigest);
	VBBinaryData ret;
	ret.setData((void*)acDigest,16);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////
// private methods

inline unsigned int br::com::sbVB::VBLib::VBMD5::F1(unsigned int x, unsigned int y, unsigned int z)
{
	return (z ^ (x & (y ^ z))); //OR (x & y | ~x & z)
}

inline unsigned int br::com::sbVB::VBLib::VBMD5::F2(unsigned int x, unsigned int y, unsigned int z)
{
	return F1(z, x, y);
}

inline unsigned int br::com::sbVB::VBLib::VBMD5::F3(unsigned int x, unsigned int y, unsigned int z)
{
	return x ^ y ^ z;
}

inline unsigned int br::com::sbVB::VBLib::VBMD5::F4(unsigned int x, unsigned int y, unsigned int z)
{
	return (y ^ (x | ~z));
}

//This is the central step in the MD5 algorithm.
inline void br::com::sbVB::VBLib::VBMD5::MD5STEP(unsigned int (*f)(unsigned int x, unsigned int y, unsigned int z),
	unsigned int& w, unsigned int x, unsigned int y, unsigned int z, unsigned int data, unsigned int s)
{
	w += f(x, y, z) + data;
	w = w << s | w >> (32-s);
	w += x;
}


br::com::sbVB::VBLib::VBMD5::VBMD5()
{
	m_auiBuf[0] = 0x67452301;
	m_auiBuf[1] = 0xefcdab89;
	m_auiBuf[2] = 0x98badcfe;
	m_auiBuf[3] = 0x10325476;
	m_auiBits[0] = 0;
	m_auiBits[1] = 0;

	//Reset the flag
	m_bAddData = false;
}


//Update context to reflect the concatenation of another buffer of bytes.
void br::com::sbVB::VBLib::VBMD5::AddData(char const* pcData, int iDataLength)
{
/*	if(iDataLength < 0)
		throw runtime_error(VBString("FileDigest ERROR: in VBMD5::AddData(), Data Length should be >= 0!")); */
	unsigned int uiT;
	//Update bitcount
	uiT = m_auiBits[0];
	if((m_auiBits[0] = uiT + ((unsigned int)iDataLength << 3)) < uiT)
		m_auiBits[1]++; //Carry from low to high
	m_auiBits[1] += iDataLength >> 29;
	uiT = (uiT >> 3) & (BLOCKSIZE-1); //Bytes already
	//Handle any leading odd-sized chunks
	if(uiT != 0)
	{
		unsigned char *puc = (unsigned char *)m_aucIn + uiT;
		uiT = BLOCKSIZE - uiT;
		if(iDataLength < uiT)
		{
			memcpy(puc, pcData, iDataLength);
			return;
		}
		memcpy(puc, pcData, uiT);
		Transform();
		pcData += uiT;
		iDataLength -= uiT;
	}
	//Process data in 64-byte chunks
	while(iDataLength >= BLOCKSIZE)
	{
		memcpy(m_aucIn, pcData, BLOCKSIZE);
		Transform();
		pcData += BLOCKSIZE;
		iDataLength -= BLOCKSIZE;
	}
	//Handle any remaining bytes of data
	memcpy(m_aucIn, pcData, iDataLength);
	//Set the flag
	m_bAddData = true;
}

//Final wrapup - pad to 64-byte boundary with the bit pattern 
//1 0*(64-bit count of bits processed, MSB-first)
void br::com::sbVB::VBLib::VBMD5::FinalDigest(char* pcDigest)
{
	//Is the User's responsability to ensure that pcDigest has
	//at least 16 bytes allocated
/*	if(false == m_bAddData)
		throw runtime_error(VBString("FileDigest ERROR: in VBMD5::FinalDigest(), No data Added before call!")); */
	unsigned int uiCount;
	unsigned char* puc;
	//Compute number of bytes mod 64
	uiCount = (m_auiBits[0] >> 3) & (BLOCKSIZE-1);
	//Set the first char of padding to 0x80. This is safe since there is
	//always at least one byte free
	puc = m_aucIn + uiCount;
	*puc++ = 0x80;
	//Bytes of padding needed to make 64 bytes
	uiCount = BLOCKSIZE - uiCount - 1;
	//Pad out to 56 mod 64
	if(uiCount < 8)
	{
		//Two lots of padding:  Pad the first block to 64 bytes
		memset(puc, 0, uiCount);
		Transform();
		//Now fill the next block with 56 bytes
		memset(m_aucIn, 0, BLOCKSIZE-8);
	}
	else
	{
		//Pad block to 56 bytes
		memset(puc, 0, uiCount - 8);
	}
	//Append length in bits and transform
	((unsigned int*)m_aucIn)[(BLOCKSIZE>>2)-2] = m_auiBits[0];
	((unsigned int*)m_aucIn)[(BLOCKSIZE>>2)-1] = m_auiBits[1];
	Transform();
	memcpy(pcDigest, m_auiBuf, MD128LENGTH<<2);
	//Reinitialize
	Reset();
}

//Reset current operation in order to prepare a new one
void br::com::sbVB::VBLib::VBMD5::Reset()
{
	//Reinitialize
	m_auiBuf[0] = 0x67452301;
	m_auiBuf[1] = 0xefcdab89;
	m_auiBuf[2] = 0x98badcfe;
	m_auiBuf[3] = 0x10325476;
	m_auiBits[0] = 0;
	m_auiBits[1] = 0;
	//Reset the flag
	m_bAddData = false;
}

//The core of the MD5 algorithm, this alters an existing MD5 hash to
//reflect the addition of 16 longwords of new data. MD5Update blocks
//the data and converts bytes into longwords for this routine.
void br::com::sbVB::VBLib::VBMD5::Transform()
{
	unsigned int* puiIn = (unsigned int*)m_aucIn;
	register unsigned int a, b, c, d;
	a = m_auiBuf[0];
	b = m_auiBuf[1];
	c = m_auiBuf[2];
	d = m_auiBuf[3];
	//
	MD5STEP(F1, a, b, c, d, puiIn[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, puiIn[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, puiIn[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, puiIn[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, puiIn[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, puiIn[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, puiIn[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, puiIn[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, puiIn[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, puiIn[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, puiIn[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, puiIn[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, puiIn[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, puiIn[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, puiIn[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, puiIn[15] + 0x49b40821, 22);
	//
	MD5STEP(F2, a, b, c, d, puiIn[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, puiIn[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, puiIn[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, puiIn[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, puiIn[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, puiIn[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, puiIn[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, puiIn[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, puiIn[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, puiIn[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, puiIn[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, puiIn[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, puiIn[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, puiIn[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, puiIn[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, puiIn[12] + 0x8d2a4c8a, 20);
	//
	MD5STEP(F3, a, b, c, d, puiIn[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, puiIn[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, puiIn[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, puiIn[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, puiIn[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, puiIn[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, puiIn[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, puiIn[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, puiIn[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, puiIn[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, puiIn[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, puiIn[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, puiIn[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, puiIn[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, puiIn[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, puiIn[2] + 0xc4ac5665, 23);
	//
	MD5STEP(F4, a, b, c, d, puiIn[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, puiIn[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, puiIn[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, puiIn[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, puiIn[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, puiIn[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, puiIn[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, puiIn[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, puiIn[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, puiIn[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, puiIn[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, puiIn[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, puiIn[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, puiIn[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, puiIn[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, puiIn[9] + 0xeb86d391, 21);
	//
	m_auiBuf[0] += a;
	m_auiBuf[1] += b;
	m_auiBuf[2] += c;
	m_auiBuf[3] += d;
}

void br::com::sbVB::VBLib::VBMD5::Char2Hex(unsigned char ch, char* szHex)
{
	static unsigned char saucHex[] = "0123456789ABCDEF";
	szHex[0] = saucHex[ch >> 4];
	szHex[1] = saucHex[ch&0xF];
	szHex[2] = 0;
}

void br::com::sbVB::VBLib::VBMD5::Binary2Hex(unsigned char const* pucBinStr, int iBinSize, char* pszHexStr)
{
	int i;
	char szHex[3];
	unsigned char const* pucBinStr1 = pucBinStr;
	*pszHexStr = 0;
	for(i=0; i<iBinSize; i++,pucBinStr1++)
	{
		Char2Hex(*pucBinStr1, szHex);
		strcat(pszHexStr, szHex);
	}
}

