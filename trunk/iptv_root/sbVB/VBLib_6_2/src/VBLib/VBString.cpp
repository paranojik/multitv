// VBString.cpp

#include "VBLib/VBLib.h"
#include <string> // std::string
// size of char buffer used in some rare methods
#define BUF_SIZE 100


/////////////////////////////////////////////////////////////////
//
// VBString
//
/////////////////////////////////////////////////////////////////

// all entities within namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

// static variable initialization
const char *VBString::m_chgHiCaps     = "�����������������������Ǫ�";
const char *VBString::m_chgLoCaps     = "�����������������������窺";
const char *VBString::m_chgHiCapsSafe = "AEIOUAEIOUAEIOUAEIOUAONCAO";
const char *VBString::m_chgLoCapsSafe = "aeiouaeiouaeiouaeiouaoncao";

// extractor
VBLIB_DLL_API std::istream & operator>>(std::istream & stream, VBString & obj) 
{
#if 1
	std::string str;
	// std::getline(stream,str);
	std::getline(stream,str,'\n');
	obj = str.c_str();
	return stream;
#endif
#if 0
	obj = "";
	const char end = '\n';
	const unsigned length=5;
	char buff[length];
	unsigned i=0;
	bool stop;
	do
	{
		// read one more char to buffer
		// stream.getline(&buff[i],1);
		// buff[i] = stream.get();
		stream.read(&buff[i],1);

		// if the char just read is the end (\n) char
		if (buff[i]!=end)
		{
			i++; // increment buffer pointer

			// if buffer is full
			if (i==length)
			{
				i=0; // start buffer pointer to zero once more
				buff[length-1]=0; // terminate string
				std::cout << "DEBUG: " << buff << std::endl;
				obj += buff; // add buffer to return object
			}
		}
		else
			stop = true;
	} while(!stop);

	return stream;
#endif
}

// insersor
VBLIB_DLL_API std::ostream & operator<<(std::ostream & stream, const VBString & obj) 
{
	if (obj.m_size > 0) 
		stream << obj.m_str;
	return stream;
}


//////////////////////////////////////////////////////
// constructors

void VBString::privateConstructor() 
{
	initCin(); // stores a pointer to cin in a member
	m_str = NULL; // indispensable
	m_decs = 2; // number of decimals when adding float or double
#ifdef VBSTRINGDEBUG
	_stringObjectTag = _VBObject.tag++; // increment _VBObject tag
	_VBObject = { 0, 0 };
#endif // VBSTRINGDEBUG
}

// this constructor is private
VBString::VBString(const int len) 
{
	privateConstructor();
	allocStr(len);
}

// default constructor
VBString::VBString(const char* s)
{
	privateConstructor();
	if (!s) s = "";
	int len = strlen(s);
	allocStr(len);
	strcpy(m_str,s);
}

// copy constructor
VBString::VBString(const VBString & s) 
{
	privateConstructor();
	allocStr(s.m_size);
	if (s.m_size) strcpy(m_str,s.m_str);
}

VBString VBString::getFileExtension()
{
	char tok = '.';
	return strtok(tok,tokCount(tok));
}


// hash (non reversible encrypt) this using MD5 algorithm
VBString VBString::get_MD5_hash() const
{
	VBMD5 a;
	return a.MD5_hash(*this);
}

// hash (non reversible encrypt) this using MD5 algorithm
void VBString::MD5_hash()
{
	*this = get_MD5_hash();
}

VBString VBString::strAfterPosition(const unsigned pos) const 
{ 
	return strAfter(pos); 
};  

VBString VBString::strUntilPosition(const unsigned pos) const 
{ 
	return strBefore(pos+1); 
}; 

VBString VBString::strInside(unsigned from, unsigned to) const 
{
	return this->strAfterPosition(from).strUntilPosition(to-from); 
}

bool VBString::validEmail() const
{
	const char *validChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#$&*-_";
	const char *validSymbols = ".@";
	const char *at = "@";
	if (!validHelper(validChars,validSymbols,3)) return false;
	// there must be somethig after "@"
	VBString aux = afterFind(at);
	if (aux.length() == 0) return false;
	// no @ after the first @
	if (aux.existSubstring(at)) return false;
	// there must be somethig before "@"
	aux = beforeFind(at);
	if (aux.length() == 0) return false;
	return true;
}

bool VBString::validAddress() const
{
	const char *validChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"������������������������������������������������";
	const char *validSymbols = ".,- ��/: ";
	if (!validHelper(validChars,validSymbols,2)) return false;
	return true;
}

bool VBString::validCity() const
{
	const char *validChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"������������������������������������������������";
	const char *validSymbols = "-";
	if (!validHelper(validChars,validSymbols,2)) return false;
	return true;
}

// for a given 9 digits, return the 2 digits to complete a CIC
VBString VBString::rightCIC() const
{
	VBString ret;
	VBString CICleft = strInside(0,8); // the left part of CIC of this
	unsigned digit1 = findDigit (10, 9, CICleft);
	CICleft += intToChar(digit1);
	unsigned digit2 = findDigit (11, 10, CICleft);
	ret = intToChar(digit1);
	ret += intToChar(digit2);
	return ret;
}

bool VBString::validCIC() const
{
	const char *validChars = "0123456789";
	const char *validSymbols = "-.";
	if (!validHelper(validChars,validSymbols,11,15)) return false;

	// eliminate symbols
	VBString ob;
	for (unsigned i=0 ; i < length() ; i++)
	{
		if (charIsValid((*this)[i],validChars))
			ob += (*this)[i];
	}
	// now ob has the cic without the symbols. 
	// ob has only validChars, and length must be 11
	if (ob.length() != 11) return false;

#if 0
	VBString CICleft = ob.strInside(0,8); // the left part of CIC
	VBString CICright;

	// CPF algorithm. Calculate the 2 digits at right and compare
	unsigned digit1 = findDigit (10, 9, ob);
	CICleft += intToChar(digit1);
	CICright = intToChar(digit1);
	unsigned digit2 = findDigit (11, 10, CICleft);
	CICright += intToChar(digit2);
#endif
	VBString CICright = ob.rightCIC();

	// if calculated CICright does not match with given CICright, that's false CIC
	if (ob.strInside(9,10) != CICright) 
		return false;

	return true;
}

unsigned VBString::findDigit (int i, int lenght, const char *buffer) const
{
	unsigned sum=0;
	unsigned ret=0;

	for (unsigned k=0; k < lenght; k++) {
		sum += charToInt(buffer[k])*i;
		i--;
	};

	ret = 11-(sum%11);
	if ((ret==10) || (ret==11))
		ret=0;

	return ret;
}

bool VBString::validName() const
{
	const char *validChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"������������������������������������������������";
	const char *validSymbols = "-_";
	if (!validHelper(validChars,validSymbols,2)) return false;
	return true;
}

bool VBString::validPhone() const
{
	const char *validChars = "0123456789";
	const char *validSymbols = "-.";
	if (!validHelper(validChars,validSymbols,5)) return false;
	return true;
}

bool VBString::validZip() const
{
	const char *validChars = "0123456789";
	const char *validSymbols = "-.";
	if (!validHelper(validChars,validSymbols,5,10)) return false;
	return true;
}

bool VBString::validCreditCardNumber() const
{
	const char *validChars = "0123456789";
	const char *validSymbols = "-.";
	if (!validHelper(validChars,validSymbols,16,19)) return false;

	// eliminate symbols
	unsigned i;
	VBString ob;
	for (i=0 ; i < length() ; i++)
	{
		if (charIsValid((*this)[i],validChars))
			ob += (*this)[i];
	}
	// now ob has the cic without the symbols. 
	// ob has only validChars, and length must be 11
	if (ob.length() != 16) return false;

	int sum=0;
	int oddvalue=0 , evenvalue=0;
	int odd=0 , even=0;

	for (i=0 ; i < ob.length() ; i++)
	{
		if (i%2==0) 
		{
			oddvalue = charToInt(ob[i]);
			if (2*oddvalue > 9)
				oddvalue = 2*oddvalue - 9;
			else oddvalue = 2*oddvalue;
				odd += oddvalue;
		}	
		else 
		{
			evenvalue = charToInt(ob[i]);
			even += evenvalue;
		}
	}
	sum = even + odd;
	if ( (sum > 150) || (sum%10 != 0) )
		return false;
	return true;

#if 0
	const char* cardValues = CARDSTRING;
	int i=0 ,lenght=0, sum=0;
	int oddvalue=0 , evenvalue=0;
	int odd=0 , even=0;

	lenght = strlen(buffer);

	// Tests the string size
	if (lenght !=16)
		return(0);
	else {
		while (buffer[i] != 0) {
			// Checks if there are forbidden characters in the CARD field
			if (!strchr(cardValues, buffer[i]))
				return (0);
					
			if (i%2==0) {
				oddvalue = Char_To_Int(buffer[i]);
				if (2*oddvalue > 9)
					oddvalue = 2*oddvalue - 9;
				else oddvalue = 2*oddvalue;
				odd += oddvalue;
			}	
			else {
				evenvalue = Char_To_Int(buffer[i]);
				even += evenvalue;
			};
			i++;
		};
		sum = even + odd;
		if ( (sum > 150) || (sum%10 != 0) )
			return(0);
		else
			return(1);
	};	
#endif
}

bool VBString::validHelper(const char *validChars, const char* validSymbols, 
						   unsigned minSize, unsigned maxSize) const
{
	// if user sets non-zero max or min, check size
	unsigned length = getLength();
	if (minSize > 0 && length < minSize) return false;
	if (maxSize > 0 && length > maxSize) return false;
	unsigned i;

	// chars of *this must be validChars or validSymbols
	for (i = 0 ; i < length ; i++)
	{
		if ( !(charIsValid((*this)[i],validChars)
			|| charIsValid((*this)[i],validSymbols) ) ) return false;
	}

	// if a char is a symbol, the next char should not be a symbol
	for (i = 0 ; i < length-1 ; i++)
	{
		if ( charIsValid((*this)[i],validSymbols) && 
			!charIsValid((*this)[i+1],validChars) ) return false;
	}

	return true;
}

bool VBString::charIsValid(char ch,const char *validChars) const
{
	unsigned i;
	for (i = 0 ; i < strlen(validChars) ; i++)
		if (ch == validChars[i]) return true;
	return false;
}




// true if zero length
bool VBString::IsEmpty() const
{
	return (getLength() == 0);
}

// clear contents to empty
void VBString::Empty()
{
	*this = "";
}

// convert this string to debug format
VBString VBString::debug(int format) const
{
	VBString ret = *this;
	VBString aux, aux2;
	unsigned i;
	switch (format)
	{
	case 1:
		ret = "";
		for (i = 0; i < getLength() ; i++)
		{
			aux = m_str[i];
			aux.simpleEncrypt(0); // convert to hexa
			ret += aux;
			ret += ".";
		}
		break;
	case 2:
		ret = "";
		for (i = 0; i < getLength() ; i++)
		{
			char ch = m_str[i];
			bool charOK = ch >= ' ' && ch <= 'z'+4;
			if (charOK)
				aux = ch;
			else
				aux = ".";
			ret += aux;
			ret += "  ";
		}
		break;
	case 3:
		aux = ret;
		ret = aux.debug(1);
		ret += '\n';
		ret += aux.debug(2);
		break;
	default:
		{} // do nothing
	}
	return ret;
}

// getline for namespace version
void VBString::getLineNamespace(char endChar) 
{
	std::string str;
	std::getline(std::cin,str,endChar);
	*this = str.c_str();

#if 0
	std::string str;
	std::istream *pIstream = (std::istream*)m_myIstream;
	std::getline(*pIstream,str,'\n');
	*this = str.c_str();
	wipeCtrmM();
#endif
}

// old comment
// getline is a long method, usually implemented in cpp, not in a *.h.
// but it is defined in the header to avoid ambiguity error when using namespace std
// new comment: now getLine is implemented in the *.cpp
void VBString::getLine(char endChar) 
{
	// the 2 lines below are necessary to avoid unwanted chars in the buffer
	// int gc = m_myIstream.gcount();
	// if (gc > 0) m_myIstream.ignore(gc);

	// a linked list of strList objects
	VBList<strList> myList;

	// a loop to keep adding strList objects to a list, 
	// until the input stream of char is finished.
	strList b;
	bool listEnd = false;
	int elements = 0;
	int lengthOfLastBuffer;
	while (!listEnd) {
		for (int i=0 ; i < VB_N ; i++) {
			b.buffer[i] = m_myIstream->get();
			if (m_myIstream->eof() || (b.buffer[i] == endChar)) {
				b.buffer[i] = 0;  // end string
				listEnd = true;
				lengthOfLastBuffer = i;
				break;  // end for loop
			}
		}
		myList.add(b);
		elements++;
		// cout << "DEBUG: Added element " << elements << endl;
	}

	// one buffer to store the complete input string
	char *completeBuffer = new char [elements * VB_N];
	if (!completeBuffer)
		error("Alloc error !");
	
	// scans the list and copies the contents of each element
	// to completeBuffer
	strList *p = myList.GetFirst();
	for (int i=0 ; i < elements ; i++) {
		for (int k=0 ; k < VB_N ; k++)
			completeBuffer[k + VB_N*i] = p->buffer[k];
		p = myList.GetNext();
	}

	// the contents is copied. Now terminate string with the zero at the end
	int terminateStringPosition = (elements-1)*VB_N + lengthOfLastBuffer;

	// eliminate ^M char, that happens when unix programs read text files from windows
	// char lastChar = completeBuffer[terminateStringPosition-1];
	// if (lastChar == 13 && terminateStringPosition > 0) terminateStringPosition--; // ^M in decimal is 13

	completeBuffer[terminateStringPosition] = 0; // terminate string

	(*this) = completeBuffer; // copies the buffer to the object
	delete [] completeBuffer;

	wipeCtrmM();

} // end of getLine


void VBString::wipeCtrmM()
{
	char lastChar = (*this)[getLength()-1];
	if (lastChar == 13) // 13 = ^M
		*this = strInside(0,getLength()-2);
}


// return true if the string contains the sub string
bool VBString::existSubstring
(const char* subStr, bool caseSens) const
{
	bool ret;
	afterFind(subStr, ret, caseSens);

	// old code
	// bool ret = (bool) (::strstr((*this).getStr(), subStr) != 0); 
	// ::strstr is the global strstr, 
	return ret;
}

// return true if left side of this is equal to subStr
bool VBString::leftSideIs(const char* subStr) const
{
	unsigned length = strlen(subStr);
	bool ret = strInside(0,length-1) == subStr;
	return ret;
}

// return the string at right of this and leftString argument
// example: this=="abcdef", leftString="abc", return="def"
VBString VBString::separateLeftString
  (const char* leftString) const
{
	unsigned length = strlen(leftString);
	return strInside(length,getLength());
}


// Example: z_aux = 15, return 'f'
// Example: z_aux = 3, return = '3'
unsigned char VBString::p_ToAscii(unsigned char z_aux) const
{
	unsigned char z = tolower(z_aux);
	if (z < 10)  z += '0';
	else 	     z += 'a' - 10;
	return z;
}

// Example: z_aux = 'F' or 'f',  return = 15
// Example: z_aux = '3', return = 3
unsigned char VBString::p_ToAsciiReverse(unsigned char z_aux) const
{
	unsigned char z = tolower(z_aux);
	if (z < ('9'+1) )  z -= '0';
	else               z -= 'a' - 10;
	return z;
}

// Example: ch = 'A' (input) ,  ch_hex = "41" (output)
void VBString::p_toHex(unsigned char ch, char *ch_hex) const
{
	unsigned char a;
	a = (ch & 0x0f);
	ch_hex[1] = p_ToAscii(a); // LSB
	a = (ch & 0xf0)/16;
	ch_hex[0] = p_ToAscii(a);; // MSB
}

// Example: ch_hex = "41",  return = 'A'
unsigned char VBString::p_toHexReverse(char *ch_hex) const
{
	unsigned char ret;
	int msb, lsb;
	msb = p_ToAsciiReverse(ch_hex[0]);
	lsb = p_ToAsciiReverse(ch_hex[1]);
	ret = lsb + msb * 16;
	return ret;
}


// encrypts object using code [0~255]
void VBString::simpleEncrypt(unsigned short code)
{
	// code is any number from 0 to 255
	VBString ret;
	unsigned len = strlen(m_str);
	unsigned char ch;
	char ch_hex[3];
	ch_hex[2] = 0; // string terminator
	for (unsigned i=0 ; i < len ; i++) {
		ch = m_str[i] ^ code;  // ^ is bitwise xor
		p_toHex(ch,ch_hex);
		ret += ch_hex; // adding string
	}
	*this = ret;
}

// de-encrypts object using code
void VBString::simpleEncryptReverse(unsigned short code)
{
	// code is any number from 0 to 255
	VBString ret;
	unsigned len = strlen(m_str);
	char ch[2];  
	ch[1] = 0; // string terminator
	char ch_hex[2];
	for (unsigned i=0 ; i < len/2 ; i++) {
		ch_hex[0] = m_str[2*i];
		ch_hex[1] = m_str[2*i+1];
		ch[0] = p_toHexReverse(ch_hex);
		ch[0] ^= code;  // ^ is bitwise xor
		ret += ch;
	}
	*this = ret;
}

// return the number of sub-strings
// new bug free code. Thanks to Leo Huf Campos Braga, November 21th, 2005
unsigned VBString::countSubString
(const char *subStr, bool caseSens) const
{
	unsigned ret=0;
	bool found;
	VBString a = afterFind(subStr, found, caseSens);
	while (found) {
		// cout << "DEBUG: " << a << endl;
		ret++;
		// cout << "DEBUG: " << ret << endl;
		VBString b = a.afterFind(subStr, found, caseSens);
		a = b;
	}
	return ret;
}

// return the number of tokens
unsigned VBString::tokCount(char tok) const
{
	unsigned ret=0;
	unsigned len = getLength();
	for (unsigned i=0; i < len ; i++) {
		if (m_str[i]==tok)
			ret++;
	}
	return ret;
}

void VBString::strtokchg(const char tok, const unsigned n, const char *newData) 
{
	if (n > tokCount(tok)) {// if n too big
		// cout << "n too big" << endl; // debug
		return; // do nothing
	}
	unsigned tokPos_initial = strtokpos(tok,n) + 1;
	unsigned tokPos_final   = strtokpos(tok,n+1) + 1;
	if (n == tokCount(tok)) // the last string
		tokPos_final++;
	VBString before = strBefore(tokPos_initial);
	VBString after = strAfter(tokPos_final-1);
	*this = before + newData + after;
}

void VBString::myDelete() 
{
	if (m_str) {
		#ifdef VBSTRINGDEBUG
			cout << "Deleted object #: " << _stringObjectTag << endl;
		#endif // VBSTRINGDEBUG
		delete [] m_str;
		m_str = NULL;
	}
}

VBString::~VBString() 
{
	// std::cout << "DEBUG delete: " << m_str << std::endl;
	myDelete();
} 

char VBString::p_extendedToUpper(char in) 
{
	in = toupper(in);
	// change extended chars 
	int lenChg = strlen(m_chgHiCaps);
	for (int k=0 ; k < lenChg ; k++) {
		if (in == m_chgLoCaps[k]) { // test for each char of chgLoCaps
			return m_chgHiCaps[k];  // set correspondent HiCaps in its place
		}
	} // for k
	return in;
}

char VBString::p_extendedToLower(char in) 
{
	in = tolower(in);
	// change extended chars 
	int lenChg = strlen(m_chgHiCaps);
	for (int k=0 ; k < lenChg ; k++) {
		if (in == m_chgHiCaps[k]) { // test for each char of chgLoCaps
			return m_chgLoCaps[k];  // set correspondent LoCaps in its place
		}
	} // for k
	return in;
}

char VBString::p_safeToUpper(char in) 
{
	in = p_toSafe(in);
	in = toupper(in);
	return in;
}

char VBString::p_safeToLower(char in) 
{
	in = p_toSafe(in);
	in = tolower(in);
	return in;
}

// change extended chars to safe (non-extended) ones
char VBString::p_toSafe(char in) const
{
	int lenChg = strlen(m_chgHiCaps);
	for (int k=0 ; k < lenChg ; k++) {
		if (in == m_chgHiCaps[k]) { // test for each char of chgHiCaps
			return m_chgHiCapsSafe[k];  // set correspondent safe
		}
		if (in == m_chgLoCaps[k]) { // test for each char of chgLoCaps
			return m_chgLoCapsSafe[k];  // set correspondent safe
		}
	} // for k
	return in;
}

// ignore extended char
void VBString::toLower() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++)
		m_str[i] = tolower(m_str[i]);
}

 // ignore extended char
void VBString::toUpper() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++)
		m_str[i] = toupper(m_str[i]);
}

// convert to extended char lo caps
void VBString::extendedToLower() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++)
		m_str[i] = p_extendedToLower(m_str[i]);
}

// convert to extended char hi caps
void VBString::extendedToUpper() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++)
		m_str[i] = p_extendedToUpper(m_str[i]);
}

// convert to standard ascii char lo caps
void VBString::safeToLower() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++)
		m_str[i] = p_safeToLower(m_str[i]);
}

// convert to standard ascii char hi caps
void VBString::safeToUpper() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++) {
		// cout << "before=" << m_str[i] << endl;
		m_str[i] = p_safeToUpper(m_str[i]);
		// cout << "after="<< m_str[i] << endl;
	}
}

// convert to standard ascii char hi or lo caps
void VBString::toSafe() 
{
	unsigned len = strlen(m_str);
	for (unsigned i=0 ; i < len ; i++) {
		m_str[i] = p_toSafe(m_str[i]);
	}
}


// void VBString::setErrorFunction(void (*fun)(const char *)) 
// {	m_errorFunction = fun; }

void VBString::error(const char *msg) const
{
	// m_errorFunction(msg);
	throw msg;
} 

void VBString::allocStr(unsigned size_in) // redim
{ 
	// m_size = abs(size_in); // not necessary
	m_size = size_in;
	if (m_str) delete [] m_str;
	if (m_size >= 0) {
		m_str = new char [m_size+1];
		if (!m_str) 
			error("Alloc error !!!");
		#ifdef VBSTRINGDEBUG
			cout << "Alloced object #: " << _stringObjectTag << endl;
		#endif // VBSTRINGDEBUG
		m_str[0] = 0; // terminate string
	} 
	else {
		#ifdef VBSTRINGDEBUG
			cout << "Attempt to alloc empty string, object #: " << _stringObjectTag << endl;
		#endif // VBSTRINGDEBUG
		m_str = NULL;
	}
}

// global function
VBString operator+
(const char* str,const VBString & vbs) 
{
	VBString r(str);
	VBString ret = r + vbs;
	return ret;
}

// test global function
// void t5() {}

//////////////////////
// BSTR

#ifdef WIN32
void VBString::getBSTR(const BSTR & pWord) 
{
	unsigned i=0;
	while (pWord[i]) 
		i++;
	// after this loop, i has the length of the BSTR
	unsigned length = i;
	char *p = new char [length+1];
	if (!p) error("Could't alloc memory");
	for (i=0 ; i < length ; i++)
		p[i] = (char) pWord[i];
	p[length]=0; // terminate string
	(*this) = p; // place the string to object
	delete [] p; // delete p not to leak memory
}


BSTR VBString::exportBSTR() 
{
	CComVariant VarStr(*this);
	BSTR ret=VarStr.bstrVal;
	return ret;
}
#endif


char VBString::getChar(unsigned pos) const
{
	if (pos < getLength())
		return m_str[pos];
	else
		return 0; // null char
}

void VBString::putChar(unsigned pos,char ch)
{
	if (pos < getLength())
		m_str[pos] = ch;
}



/////////////////////////////////////////////////////////////
// operator=

void VBString::operator=(const VBString & s) 
{
	allocStr(s.m_size);
	if (s.m_size) strcpy(m_str,s.m_str);
}

void VBString::operator=(const char *s) 
{
	if (!s) s = "";
	unsigned len = strlen(s);
	allocStr(len);
	if (len) strcpy(m_str,s);
}

void VBString::operator= (const char ch) 
{
	VBString aux;
	aux += ch;
	*this = aux;
}

void VBString::operator=(const int i)
{
	VBString aux;
	aux += i;
	*this = aux;
}

void VBString::operator=(const unsigned i)
{
	VBString aux;
	aux += i;
	*this = aux;
}

void VBString::operator=(const float f)
{
	VBString aux;
	aux.setDecs(getDecs()); // use the number of decs of this
	aux += f;
	*this = aux;
}

void VBString::operator=(const double d)
{
	VBString aux;
	aux.setDecs(getDecs()); // use the number of decs of this
	aux += d;
	*this = aux;
}

void VBString::operator=(const long double d)
{
	VBString aux;
	aux.setDecs(getDecs()); // use the number of decs of this
	aux += d;
	*this = aux;
}


/////////////////////////////////////////////////////////////
// operator+=
void VBString::operator+=(const char ch) 
{
	VBString ret = (*this) + ch;
	operator=(ret);
}

void VBString::operator+=(const int i) 
{
	VBString ret = (*this) + i;
	operator=(ret);
}

void VBString::operator+=(const long i) 
{
	// for 32 bit compilers, long and int are the same
	(*this) += (int) i;
}

void VBString::operator+=(const unsigned i) 
{
	VBString ret = (*this) + i;
	operator=(ret);
}

void VBString::operator+=(const double d) 
{
	VBString ret = (*this) + d;
	operator=(ret);
}

void VBString::operator+=(const long double d) 
{
	VBString ret = (*this) + d;
	operator=(ret);
}

void VBString::operator+=(const float f) 
{
	VBString ret = (*this) + f;
	operator=(ret);
}

void VBString::operator+=(const char* s) 
{
	VBString ret = (*this) + s;
	operator=(ret);
}

void VBString::operator+=(const VBString & s) 
{
	VBString ret = (*this) + s;
	operator=(ret);
}

/////////////////////////////////////////////////////////////
// operator+

VBString VBString::operator+(const char ch) const
{
	char buffer[2];
	buffer[0] = ch;
	buffer[1] = 0; // terminator string
	VBString ret = (*this) + buffer; // buffer is char*
	return ret;
}

VBString VBString::operator+(const int i) const
{
/* old and buggy version
	char buffer[BUF_SIZE];
	sprintf(buffer,"%d",i);
	return operator+(buffer);
*/
	char buffer[BUF_SIZE];
	::sprintf(buffer,"%d",i);
	VBString ret = (*this) + buffer;
	return ret;
}

VBString VBString::operator+(const unsigned i) const
{
/* old and buggy version
	char buffer[BUF_SIZE];
	sprintf(buffer,"%d",i);
	return operator+(buffer);
*/
	char buffer[BUF_SIZE];
	::sprintf(buffer,"%u",i);
	VBString ret = (*this) + buffer;
	return ret;
}

VBString VBString::operator+(const float f) const
{
	VBString ret = operator+((double)f);
	return ret;
}

VBString VBString::operator+(const double d) const
{
	char buffer[BUF_SIZE];
	char sprintfBuffer[BUF_SIZE];
	::sprintf(sprintfBuffer,"%c1.%df",'%',m_decs); 
	// cout << "DEBUG=" << sprintfBuffer << endl;
	// cout << "DEBUG=" << d << endl;
	// create a buffer for next sprintf. 
	// If m_decs == 2, springBuffer is "%1.2f"
	::sprintf(buffer,sprintfBuffer,d);
	VBString ret = (*this) + buffer;
	return ret;
}

VBString VBString::operator+(const long double d) const
{
	char buffer[BUF_SIZE];
	char sprintfBuffer[BUF_SIZE];
	::sprintf(sprintfBuffer,"%c1.%dlf",'%',m_decs); 
	::sprintf(buffer,sprintfBuffer,d);
	VBString ret = (*this) + buffer;
	return ret;
}

VBString VBString::operator+(const char* s) const
{
	if (s) {
		unsigned len = strlen(s);
		if (!m_size) { // if string is null
			VBString ret(len);
			if (len) strcpy(ret.m_str,s);
			return ret;
			/*
			myDelete();
			allocStr(strlen(s));
			strcpy(str,s);
			return (*this);
			*/
		}
		VBString ret(m_size + len);
		if (m_size) strcpy(ret.m_str,m_str);
		if (len) strcat(ret.m_str,s);
		return ret;
	}
	else 
		return (VBString(0));  // return empty VBString
}


VBString VBString::operator+(const VBString & s) const
{
	if (s.m_str)
		return operator+(s.m_str);
	else
		return operator+("");
/*	
	VBString ret(size + s.size);
	strcpy(ret.str,str);
	strcat(ret.str,s.str);
	return ret;
*/
}

VBString VBString::afterFind(const char findChar, bool & found, bool caseSens) const
{
	char buffer[2];
	buffer[0] = findChar;
	buffer[1] = 0; // terminateString
	return afterFind(buffer,found,caseSens);
}

VBString VBString::afterFind(const char *find, bool & found, bool caseSens) const
{
	VBString thisTemp = *this;
	VBString findTemp = find;
	if (!caseSens) {
		thisTemp.extendedToLower();
		findTemp.extendedToLower();
	}
	found = true;
	VBString b = thisTemp.beforeFind(findTemp);
	if (b.getLength() == getLength()) {
		found = false;
		return (VBString(""));
	}
	char *inStr = m_str;
	unsigned pos = b.getLength() + strlen(find);
	VBString ret(inStr+pos);

/* OLD CODE. Before adding case sensitive option
	found = true;
	VBString b = beforeFind(find);
	if (b.getLength() == getLength()) {
		found = false;
		return (VBString(""));
	}
	char *inStr = m_str;
	unsigned pos = b.getLength() + strlen(find);
	VBString ret(inStr+pos);
*/
	return ret;
}

VBString VBString::afterFind(const char findChar) const
{
	char buffer[2];
	buffer[0] = findChar;
	buffer[1] = 0; // terminateString
	return afterFind(buffer);
}

// return the string before fing
// VBString r,a("123456789");
// r = a.afterFind("56"); // r = "789";
// r = a.afterFind("xx"); // r = ""; // string not found
VBString VBString::afterFind(const char *find) const
{
	bool found;
	return (afterFind(find,found));
}

VBString VBString::beforeFind(const char findChar) const
{
	char buffer[2];
	buffer[0] = findChar;
	buffer[1] = 0; // terminateString
	return beforeFind(buffer);
}

// return the string before fing
// VBString r,a("123456789");
// r = a.beforeFind("56"); // r = "1234";
// r = a.beforeFind("xx"); // r = "123456789"; // string not found
VBString VBString::beforeFind(const char *find) const
{
	char *inStr = new char [getLength()+1];
	if (!inStr)
		error("Alloc error !!!");
	::strcpy(inStr,getStr());
	char *r = strinc(find,inStr);
	unsigned stopLen;
	if (r) 
		stopLen = ::strlen(inStr) - ::strlen(r);
	else
		stopLen = ::strlen(inStr);
	inStr[stopLen] = 0;  // end string
	VBString ret(inStr);
	delete [] inStr;
	return ret;
}

// Same as above, but will return in a bool reference if the
// subString was in fact found.
VBString VBString::beforeFind(const char *find, bool &found, bool caseSens) const
{
	VBString thisTemp = *this;
	VBString findTemp = find;
	if (!caseSens) {
		thisTemp.extendedToLower();
		findTemp.extendedToLower();
	}
	char *inStr = new char [getLength()+1];
	if (!inStr)
		error("Alloc error !!!");
	::strcpy(inStr,getStr());
	char *inStrTemp = new char [getLength()+1];
	if (!inStrTemp)
		error("Alloc error !!!");
	::strcpy(inStrTemp,thisTemp.getStr());
	char *r = strinc(findTemp,inStrTemp);
	unsigned stopLen;
	if (r) 
		stopLen = ::strlen(inStr) - ::strlen(r);
	else
		stopLen = ::strlen(inStr);
	inStr[stopLen] = 0;  // end string
	VBString ret(inStr);
	delete [] inStr;
	delete [] inStrTemp;
	
	found = true;
	if (ret.getLength() == getLength())
		found = false;

/* OLD CODE

	char *inStr = new char [getLength()+1];
	if (!inStr)
		error("Alloc error !!!");
	::strcpy(inStr,getStr());
	char *r = strinc(find,inStr);
	unsigned stopLen;
	if (r) 
		stopLen = ::strlen(inStr) - ::strlen(r);
	else
		stopLen = ::strlen(inStr);
	inStr[stopLen] = 0;  // end string
	VBString ret(inStr);
	delete [] inStr;
	
	found = true;
	if (ret.getLength() == getLength())
		found = false;
*/
	return ret;
}
VBString VBString::beforeFind(const char findChar, bool &found, bool caseSens) const
{
	char buffer[2];
	buffer[0] = findChar;
	buffer[1] = 0; // terminateString
	return beforeFind(buffer, found, caseSens);
}


// find and replace 1 string 
void VBString::strchg(const char *find,const char *replace) 
{
	VBString ret = beforeFind(find);
	bool found;
	VBString a = afterFind(find,found);
	if (found) 
		ret += replace + a;
	(*this) = ret; // place effect to the own object
}

// find and replace all instances of find string 
void VBString::strschg(const char *find,const char *replace) {
	VBString ret, after, before;
	before = beforeFind(find);
	after = (*this);
	ret = before;
	bool stop = (after == before);
	if (!stop) ret += replace;
	while (!stop) {
		after = after.afterFind(find);
		before = after.beforeFind(find);
		stop = (after == before);
		if (!stop) {
			ret += before;
			ret += replace;
		}
		else {
			ret += before;
		}
	}
	(*this) = ret; // place effect to the own object
}

VBString VBString::strtok(char tok, unsigned n) const
{
	bool found;
	return (strtok(tok,n,found));
}

using namespace br::com::sbVB;

// VBString strtok2(const char tok, const unsigned n, bool & found) const;
// VBString VBString::strtok(char tok, unsigned long n, bool & found ) const
VBString VBString::strtok(const char tok, const unsigned n, bool & found) const
{
	unsigned tokPos;
	return strtok_helper(tok,n,found,tokPos);
}

// VBString = "abc,def,ghi"
// tok =','
// n = 2
// return = "def"
// tokpos is the position of the token in the string
VBString VBString::strtok_helper(const char tok, const unsigned n, bool & found, 
			unsigned & tokPos) const
{
	found = false;
	char *str = m_str;
	if (!str) return VBString(""); // token not found 
	unsigned len = getLength();
	unsigned tokCount = 0;
	unsigned i=0, initial=0, final=0;
	bool initOK = false, finalOK = false;

#if 0
	// fix loop in case first letter is the tok
	if ((str[0] == tok) && (n==0)) {
		initial = 0;  // initial stores the position of nth token
		initOK = true;
		tokCount = 1;
		i = 1;
	}
#endif

	// loop to identify the position of nth string
	for ( ; (i < len) && !finalOK ; i++) 
	{
		if (str[i] == tok) 
			tokCount++;
//		if ((str[0] == tok) || (!initOK && (tokCount == n))) {
		if (!initOK && (tokCount == n)) {
			initial = i;  // initial stores the position of nth token
			initOK = true;
		}
		if (tokCount == (n+1)) { 
			final = i; // final stores the position of (n+1)th token
			finalOK = true;
		}
	}
	
	if (initOK && finalOK) 
	{
		found = true;
		char *buffer = new char [final - initial + 1];
		unsigned offset;
		if (n == 0)	offset = 0;
		else offset = 1;
		for (i=initial+offset ; i < final ; i++)
			buffer[i-initial-offset] = str[i];
		buffer[i-initial-offset] = 0; // end string
		VBString ret(buffer);
		delete [] buffer;
		tokPos = initial+1; // +1 to get on the token
		if (n==0) tokPos=0;
		return ret;
	}
	if (initOK && !finalOK)  // n points to the last string
	{ 
		found = true;
		VBString ret(str+initial+1);
		tokPos = initial+1; // +1 to get on the token
		if (n==0) 
		{
			tokPos=0;
			ret = str;
		}
		return ret;
	}

	if ((str[0] == tok) && (n==0)) 
	{
		found = true; // in this case, return is "", but str is found
		tokPos = 0;
	}

	tokPos = getLength();
	return VBString(""); // token not found or n too big

} // end of method strtok_helper



/* use getLength method instead of length
unsigned long VBString::length() {
	if (m_str)
		return strlen(m_str);
	else
		return 0;
}
*/

// *this "1234567890"
// pos = 3
// ret = "4567890"
VBString VBString::strAfter(unsigned pos) const
{
	if (pos > 0) {
		VBString ret = m_str + pos;
		if (pos >= getLength())
			ret = "";
		return ret;
	}
	else 
		return *this;
}

// *this "1234567890"
// pos = 3
// ret = "123"
VBString VBString::strBefore(unsigned pos) const
{
	unsigned len = getLength();
	char *p = new char [len+1]; // +1 for the terminator char
	::strcpy(p,m_str); // copy string to other buffer
	if (pos > len) pos = len;
	p[pos] = 0;  // terminate the string
	VBString ret = p;
	delete [] p;
	return ret;
}

// return the position of the token
// VBString a = "abc,def,ghi,123,456,7890,yyy";
// int n=3;
// char tok = ',';
// int pos = a.strtokpos(tok,n); // pos=
unsigned VBString::strtokpos(char tok, unsigned n) const
{
	unsigned tokPos;
	bool found;
	strtok_helper(tok,n,found,tokPos);
	return tokPos - 1 ;
}

bool VBString::operator<(const VBString & s) const
{
	return (::strcmp(m_str,s.m_str) < 0);
}

bool VBString::operator<=(const VBString & s) const
{
	return (::strcmp(m_str,s.m_str) <= 0);
}

bool VBString::operator>(const VBString & s) const
{
	return (::strcmp(m_str,s.m_str) > 0);
}

bool VBString::operator>=(const VBString & s) const
{
	return (::strcmp(m_str,s.m_str) >= 0);
}

bool VBString::operator==(const VBString & s) const
{
	return (::strcmp(m_str,s.m_str)==0);
}

bool VBString::operator!=(const VBString & s) const
{
	return (!((*this)==s));
}

bool VBString::operator<(const char * s) const { return operator<(VBString(s)); }
bool VBString::operator<=(const char * s) const { return operator<=(VBString(s)); }
bool VBString::operator>(const char * s) const { return operator>(VBString(s)); }
bool VBString::operator>=(const char * s) const { return operator>=(VBString(s)); }
bool VBString::operator==(const char * s) const { return operator==(VBString(s)); }
bool VBString::operator!=(const char * s) const { return operator!=(VBString(s)); }



// convert object to web scape sequence
void VBString::escapeSequenceReverse() 
{

	// Convert all + chars to space chars
    replace("+"," ");

	bool hasChange;
	VBString ret, before, after = (*this);
	do {
		hasChange = false;
		for (unsigned x = 0; x < after.getLength(); x++) {
			if (after.m_str[x] == '%') {
				hasChange = true;
				before = after.strBefore(x);

				// convert the code after the '%' to ascii, and use it as replace string
				char strToReplace[2];
				strToReplace[0] = p_toHexReverse(after.m_str+x+1);
				strToReplace[1] = 0; // terminate string

				ret += before + strToReplace;
				after = after.strAfter(x+3);
				break;
			}
		}
		if (!hasChange) 
			ret += after;
	}
	while (hasChange);

	// copy 
	(*this) = ret;
}

// convert object to web unscape sequence
void VBString::escapeSequence() 
{

	// Convert % separatelly not to give problem with the general % case
    replace("%","%25");

	const char *charToConvert = "=.,?<>[]{};:'\"`~!@&#$^()+-*/_|\\����������������������������������������";

	// Convert all %xy hex codes into ASCII chars 
	bool hasChange;
	do {
		hasChange = false;
		for (unsigned x = 0; x < m_size; x++) {
			for (unsigned y = 0; y < strlen(charToConvert) ; y++) {
				if (m_str[x] == charToConvert[y]) {
					hasChange = true;
					char strToSearch[2];
					strToSearch[0] = charToConvert[y]; 
					strToSearch[1] = 0; // terminate string

					// replace string is %xx<zero>, where xx is charToConvert[y] converted to hex
					char strToReplace[4];
					strToReplace[0] = '%';
					p_toHex(charToConvert[y],strToReplace+1);
					strToReplace[3] = 0; // terminate string
					strschg(strToSearch,strToReplace); // convert
					break;
				} // if
			} // for y
		} // for x
	} while (hasChange);
    
	replace(" ","+"); // convert space char separatelly, since version 2.3 of VBMcgi
}

// this = str
// useful for expat XML processing
void VBString::setUnterminatedString(const char *str, unsigned length) 
{
	myDelete();
	m_str = new char [length + 1];
	for (unsigned i = 0; i < length ; i++)
		m_str[i] = str[i];
	m_str[length] = 0;
	m_size = length;
}

void VBString::wipeLeadingSpace() 
{
	unsigned i = 0;
	while ((m_str[i] == '\n' || m_str[i] == '\t' || m_str[i] == ' ') && i < m_size)
		i++;
	if (i != 0)
	{
		// *this = m_str+i; // can't do this
		VBString s = m_str+i;
		*this = s;
	}

}

// get next word of a stream
void VBString::getNextWord(::std::istream & stream)
{
	std::string s;
	stream >> s; // read using std string
	(*this) = s.c_str(); // copy to this

	// remove symbols if exist at right
	int len = length(); // length of this
	const char *symbols = ".,;";
	int symbol_len = strlen(symbols);
	if (len > 0)
	{
		for (int i = 0; i < symbol_len  ; i++)
		{
			if (m_str[len-1] == symbols[i]) // if last char of this is one of the symbols
				(*this) = (*this).beforeFind(symbols[i]); // remove it
		}
	}
}

/////////////////////////////////////////////////////////////////
//
// VBStringList 
//
/////////////////////////////////////////////////////////////////

bool VBStringList::operator==(const VBStringList & obj)
{
	return m_data == obj.m_data;
}

// destructor
// VBStringList::~VBStringList() 
// {
//	m_data.clear();
//}

}}}} // end namespace br::com::sbVB::VBLib
