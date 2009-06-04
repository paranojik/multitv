// VBBinaryData.cpp


#include "VBLib/VBLib.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////
//
// class VBBinaryData
//
////////////////////////////////////////////////////////////////////////////


using namespace ::br::com::sbVB::VBLib;


// default constructor
br::com::sbVB::VBLib::VBBinaryData::VBBinaryData() : m_data(0), m_size(0) {};

// copy constructor
br::com::sbVB::VBLib::VBBinaryData::VBBinaryData(const VBBinaryData & obj)
{
	m_size = obj.m_size;
	if (m_size > 0)
	{
		m_data = new unsigned char [obj.m_size];
		memcpy(m_data, obj.m_data, m_size);
	}
	else 
		m_data = 0;
}

::br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBBinaryData::getStringInHex() const
{
	VBString ret;
	for (unsigned i=0 ; i < getSize() ; i++)
	{
		ret += charInt2asciiString(m_data[i]);
	}
	return ret;
}

void br::com::sbVB::VBLib::VBBinaryData::setFromStringInHex(VBString in)
{
	unsigned size = in.getLength();
	if ((size % 2) != 0)
		throw VBException_binaryData("size not even in VBBinaryData::setFromStringInHex");
	create(size/2);
	VBString byteStr;

	for (unsigned i=0 ; i < size ; i++)
	{
		if ((i % 2) == 0)
		{	// i is even (0, 2, 4, .... 
			byteStr = in.strInside(i,i+1);
			(*this)(i/2) = asciiString2charInt(byteStr);
		}
	}
}


void br::com::sbVB::VBLib::VBBinaryData::loadFromFileAsBin(const char *fileName)
{
	::std::ifstream fileRead(fileName,::std::ios::binary);
	if (!fileRead) 
		throw VBException_binaryData("file not open in VBBinaryData::loadFromFile");

	// get number of bytes of file
	fileRead.seekg(0, ::std::ios::end); 
	unsigned byteCount = fileRead.tellg(); 
	
	// close and reopen to place the file pointer to the beginning
	fileRead.close();
	fileRead.open(fileName,::std::ios::binary);
	if (!fileRead) 
		throw VBException_binaryData("file not open in VBBinaryData::loadFromFile");

	create(byteCount); // alloc data for the VBBinaryData
	fileRead.read((char*)m_data,byteCount); // read all bytes of file to buffer
	fileRead.close();
}

void br::com::sbVB::VBLib::VBBinaryData::loadFromFileAsText(const char *fileName)
{
	// this version uses only 1 copy of data in memory

	::std::ifstream file(fileName);
	if (!file)
		throw VBException_binaryData("could not open file in VBBinaryData::loadFromFileAsText");

	::br::com::sbVB::VBLib::VBString str;
	unsigned size=0;
	// loop to read entire file 
	while (true)
	{
		file >> str; // read the line
		if (file.eof()) break;
		size += str.getLength() / 2;  // / 2 because there are 2 chars for each byte
	}

	// alloc space in this VBBinaryData
	create(size);

	// put file pointer to the beginning again
	// file.seekg(0, ::std::ios::beg); // that does not work, so I'm using some other way below
	file.close();
	::std::ifstream file2(fileName);
	if (!file2)
		throw VBException_binaryData("could not open file2 in VBBinaryData::loadFromFileAsText");

	unsigned i = 0; // points to the position of byte to be inserted to this VBBinaryData
	// loop to read entire file again. This time copying data to this VBBinaryData
	while (true)
	{
		file2 >> str; // read the line
		// ::std::cout << "DEBUG:" << str << ::std::endl;
		if (file2.eof()) break;
		for (unsigned k = 0 ; k < str.getLength() / 2 ; k++)  // / 2 because each byte has 2 lettes
		{
			// for all bytes of current line, copy to this VBBinaryData
			unsigned char currByte;
			::br::com::sbVB::VBLib::VBString currByteStr = str.strInside(2*k,2*k+1);
			currByte = charInt2asciiReverse(currByteStr[0]) * 16; // MSB
			currByte += charInt2asciiReverse(currByteStr[1]); // LSB
			(*this)(i) = currByte; // place data to this
			// ::std::cout << "DEBUG:" << ::std::hex << 
			//	::br::com::sbVB::VBLib::charInt2asciiString(currByte) << ::std::endl;
			i++;
		}
	}


#if 0
// version that uses a list of string that loads all file before copying to VBBinaryObject
	::std::ifstream file(fileName);
	if (!file)
		throw VBException_binaryData("could not open file in VBBinaryData::loadFromFileAsText");

	unsigned lines = 0;
	::std::list<::br::com::sbVB::VBLib::VBString> listString;
	::std::list<::br::com::sbVB::VBLib::VBString>::iterator it;
	::br::com::sbVB::VBLib::VBString str;
	// loop to read entire file to list of strings
	while (true)
	{
		file >> str;
		if (file.eof()) break;
		listString.push_back(str);
		lines++;
	}

	// get number of bytes per line.
	// first string of file. /2 because there are 2 chars per byte
	unsigned bytesPerLine = it->length() / 2;

	unsigned length = (lines-1) * bytesPerLine;

	// iterator points to last line
	::std::list<::br::com::sbVB::VBLib::VBString>::iterator lastLineIt;
	lastLineIt = listString.end();
	lastLineIt--;
	// add number of bytes of last line
	length += lastLineIt->length() / 2;

	// set dimension of this
	create(length);

	unsigned i = 0;
	while (true)
	{
		for (unsigned k = 0 ; k < bytesPerLine ; k++)
		{
			::br::com::sbVB::VBLib::VBString currString = *it;
			unsigned char currByte;
			currByte = ::br::com::sbVB::VBLib::charInt2ascii(currString[2*k]) * 16;
			currByte += ::br::com::sbVB::VBLib::charInt2ascii(currString[2*k+1]);
			(*this)(i) = currByte; // place data to this
			i++;
		}
		it++; // get next string of list of strings
		if (it == listString.end()) break;
	}
#endif
}

void br::com::sbVB::VBLib::VBBinaryData::saveToFileAsBin(const char *fileName) const
{
	::std::ofstream fileWrite(fileName,::std::ios::binary);
	if (!fileWrite) 
		throw VBException_binaryData("file not open in VBBinaryData::saveToFile");

	fileWrite.write((char*)m_data, m_size); // write entire file 
	fileWrite.close();
}

void br::com::sbVB::VBLib::VBBinaryData::saveToFileAsText(const char *fileName, unsigned bytesPerLine) const
{
	::std::ofstream fileWrite(fileName);
	if (!fileWrite) 
		throw VBException_binaryData("file not open in VBBinaryData::saveToFile");

	for (unsigned i = 0 ; i < m_size ; i++)
	{
		if (i % bytesPerLine == 0 && i != 0)
			fileWrite << ::std::endl;
		fileWrite << charInt2asciiString(m_data[i]);
	}
	fileWrite << ::std::endl;
}


void br::com::sbVB::VBLib::VBBinaryData::create(unsigned size)
{
	if (size != m_size)
	{
		myDelete();
		if (size > 0)
		{
			m_data = new unsigned char [size];
			m_size = size;
		}
	}
}

void br::com::sbVB::VBLib::VBBinaryData::createAndFill(unsigned size, unsigned char ch)
{
	create(size);
	for (unsigned i=0 ; i < size ; i++)
		m_data[i] = ch;
}

unsigned char br::com::sbVB::VBLib::VBBinaryData::operator[](unsigned i) const
{
	if (i < m_size)
		return m_data[i];
	else
		throw VBException_binaryData("range check error in VBBinaryData::operator[]");

	// some useless return
	return 0;
}

unsigned char & br::com::sbVB::VBLib::VBBinaryData::operator()(unsigned i)
{
	if (i < m_size)
		return m_data[i];
	else
		throw VBException_binaryData("range check error in VBBinaryData::operator()");

	// some useless return
	static unsigned char ret = 0;
	return ret;
}


const unsigned char* br::com::sbVB::VBLib::VBBinaryData::getDataPointer() const
{ 
	return m_data; 
}

void br::com::sbVB::VBLib::VBBinaryData::operator=(const VBBinaryData & obj)
{
	myDelete();
	m_size = obj.m_size;
	if (m_size > 0)
	{
		m_data = new unsigned char [obj.m_size];
		memcpy(m_data, obj.m_data, m_size);
	}
}

// copy string to this VBBinaryData
void br::com::sbVB::VBLib::VBBinaryData::operator=(const char * str)
{
	this->setData(str,strlen(str));
}


void br::com::sbVB::VBLib::VBBinaryData::setData(const char *data, unsigned size)
{
	setData((const unsigned char *)data,size);
}

// void ::br::com::sbVB::VBLib::VBBinaryData::dump(::std::ostream & mycout)
void br::com::sbVB::VBLib::VBBinaryData::dump() const
{
	
	::std::cout << "VBBinaryData dump. Size = " << m_size << ::std::endl;
	VBString s;
	for (unsigned i=0 ; i < m_size ; i++)
	{
		int k = (int)m_data[i];
		// every byte should be displayed with 3 digits
		if (k < 100)
			s += "0";
		if (k < 10)
			s += "0";

		s += k; // int converted to string
		s += ","; // separator of byte to the next
		if ((i+1) % 10 == 0)
		{
			// print the intermediate lines
			::std::cout << s << ::std::endl;
			s = "";
		}
	}
	// print the last line
	::std::cout << s << ::std::endl;
}

void br::com::sbVB::VBLib::VBBinaryData::setDataInHex(const char *data)
{
	unsigned size = strlen(data);
	if (size % 2)
		throw VBException_binaryData("data length must be even in VBBinaryData::setDataInHex");

	unsigned char byte;
	this->create(size/2);
	for (unsigned i=0 ; i < size ; i++)
	{
		if (!(i % 2))
			byte = charInt2asciiReverse(data[i]) * 16;
		else
		{
			byte += charInt2asciiReverse(data[i]);
			(*this)(i/2) = byte;
		}
	}
}


void br::com::sbVB::VBLib::VBBinaryData::setData(const void *data, unsigned size)
{
	if (m_size != size)
	{
		myDelete();
		m_size = size;
	}
	if (size > 0)
	{
		m_data = new unsigned char [size];
		memcpy(m_data, data, size);
	}
}


void br::com::sbVB::VBLib::VBBinaryData::myDelete()
{
	if (m_data)
	{
		delete [] m_data;
		m_data = 0;
		m_size = 0;
	}
}

br::com::sbVB::VBLib::VBBinaryData::~VBBinaryData()
{
	myDelete();
}

unsigned br::com::sbVB::VBLib::VBBinaryData::getSize() const
{
	return m_size;
}

bool br::com::sbVB::VBLib::VBBinaryData::operator==(const VBBinaryData & obj) const
{
	if (getSize() != obj.getSize())
		return false;

	for (unsigned i = 0 ; i < m_size ; i++)
	{
		if (m_data[i] != obj.m_data[i])
			return false;
	}
	return true;
}

bool br::com::sbVB::VBLib::VBBinaryData::operator!=(const VBBinaryData & obj) const
{
	return !operator==(obj);
}


void br::com::sbVB::VBLib::VBBinaryData::subObjectCopyTo(unsigned from, unsigned to, 
	VBBinaryData & subObject) const
{
	if (to < from)
		throw VBException_binaryData("to < from in VBBinaryData::subObject");
	
	unsigned size = to - from;
	subObject.create(size);
	for (unsigned i = 0 ; i < size ; i++)
		subObject(i) = (*this)[i + from];
}

void br::com::sbVB::VBLib::VBBinaryData::subObjectCopyFrom(unsigned fromThis, unsigned fromSource, 
		unsigned nBytes, const br::com::sbVB::VBLib::VBBinaryData & copyFromObject)
{
	for (unsigned i = 0 ; i < nBytes ; i++)
		(*this)(i + fromThis) = copyFromObject[i + fromSource];
}


br::com::sbVB::VBLib::VBBinaryData br::com::sbVB::VBLib::VBBinaryData::operator+(const VBBinaryData & obj) const
{
	br::com::sbVB::VBLib::VBBinaryData ret;
	ret.create(getSize() + obj.getSize());
	ret.subObjectCopyFrom(0,0,getSize(),*this);
	ret.subObjectCopyFrom(getSize(),0,obj.getSize(),obj);
	return ret;
}


#if 0
int main_test()
{
  using namespace br::com::sbVB;
  VBBinaryData a;
  double d[] = {1.1, 2.2, 3.3, 4.4 };

  a.setData((void*)d,sizeof(d));
  a.dump();

  return 0;
}
#endif







