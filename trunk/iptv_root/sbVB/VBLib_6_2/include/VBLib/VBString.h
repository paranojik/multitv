// VBString.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {


////////////////////////////////////////////////////////////////////
//
// VBString class
//
/////////////////////////////////////////////////////////////////////

/* VB_N is the number of chars per buffer in strList
this number can be anything more than 1, so that the 
usability of VBString is not changed.
Internally, in operator>> (for instance), "chunks" of
VB_N bytes will be alloced  in a list to store
input stream of bytes.
*/

// size of string blocks when using getline method.
// changing this number does not affect usage of the VBString class
static const int VB_N = 80;

class VBLIB_DLL_API VBString
{
	// code for debug purposes only
	struct VBObjectType // struct for debug
	{
	#ifdef VBSTRINGDEBUG
		char debug;
		int tag;  // object tag
	#endif // VBSTRINGDEBUG
	};
	static VBObjectType _VBObject;
	// end of code for debug

	struct strList // char used in linked list, in method getLine (operator>>)
	{
		char buffer[VB_N];

		// useless, but needed to compile in unix
		bool operator==(strList) {return true;};
	};

	void allocStr(unsigned size_in);  // redim

	unsigned m_size;		// number of char inside the string
	unsigned m_decs;		// number of decimals when adding float or double
	char *m_str;			// the pointer to the string data
	::std::istream * m_myIstream;	// pointer to copy of cin

	static const char *m_chgHiCaps; //     = "¡…Õ”⁄¿»Ã“ŸƒÀœ÷‹¬ Œ‘€√’—«™∫";
	static const char *m_chgLoCaps; //     = "·ÈÌÛ˙‡ËÏÚ˘‰ÎÔˆ¸‚ÍÓÙ˚„ıÒÁ™∫";
	static const char *m_chgHiCapsSafe; // = "AEIOUAEIOUAEIOUAEIOUAONCAO";
	static const char *m_chgLoCapsSafe; // = "aeiouaeiouaeiouaeiouaoncao";

	// this consctuctor is private
	// code must be in the header due to initialization of m_myIstream
	// VBString(const int len) : m_myIstream(cin)
	VBString(const int len);

	void error(const char *) const;

	VBString strtok_helper(const char tok, const unsigned n, bool & found, unsigned & tokPos) const;
	void initCin() { m_myIstream = & ::std::cin; } // code nas to be in header due to instantiation of cin
	void privateConstructor();
	char p_extendedToUpper(char in);
	char p_extendedToLower(char in);
	char p_safeToUpper(char in);
	char p_safeToLower(char in);
	#ifdef VBSTRINGDEBUG
		int _stringObjectTag;
	#endif // VBSTRINGDEBUG
	
	unsigned char p_ToAscii(unsigned char z) const;
	unsigned char p_ToAsciiReverse(unsigned char z) const;
	void p_toHex(unsigned char ch, char *ch_hex) const;
	unsigned char p_toHexReverse(char *ch_hex) const;
	char p_toSafe(char in) const;
	void strchg(const char *find,const char *replace);

	VBString strAfter(unsigned pos) const;
	VBString strBefore(unsigned pos) const;

	void getLineNamespace(char endChar = '\n'); // auxiliary method for extractor
public:

	// constructors
	VBString(const VBString & s); // copy constructor
	VBString(const char* s = ""); // default contructor

	// destructor
	virtual ~VBString();
	
	// const comparison operators
	bool operator== (const VBString & s) const;
	bool operator!= (const VBString & s) const;
	bool operator<  (const VBString & s) const;
	bool operator<= (const VBString & s) const;
	bool operator>  (const VBString & s) const;
	bool operator>= (const VBString & s) const;

	// const comparison operators (continuation)
	bool operator== (const char * s) const;
	bool operator!= (const char * s) const;
	bool operator<  (const char * s) const;
	bool operator<= (const char * s) const;
	bool operator>  (const char * s) const;
	bool operator>= (const char * s) const;
	
	// const operator+
	VBString operator+(const VBString & s) const;
	VBString operator+(const char* s) const;
	VBString operator+(const char ch) const;
	VBString operator+(const int i) const;
	VBString operator+(const unsigned i) const;
	VBString operator+(const float f) const;
	VBString operator+(const double d) const;
	VBString operator+(const long double d) const;
	// global function to contatenate strings
	VBLIB_DLL_API friend VBString operator+(const char* str,const VBString & vbs);

	// non-const operator=
	void operator=(const VBString & s);
	void operator=(const char* s);
	void operator=(const char ch);
	void operator=(const int i);
	void operator=(const unsigned i);
	void operator=(const float f);
	void operator=(const double d);
	void operator=(const long double d);

	// non-const operator+=
	void operator+=(const VBString & s);
	void operator+=(const char* s);
	void operator+=(const char ch);
	void operator+=(const int i);
	void operator+=(const long i);
	void operator+=(const unsigned i);
	void operator+=(const float f);
	void operator+=(const double d);
	void operator+=(const long double d);

	// basic utilities
	unsigned getLength() const {return m_size;}; // get the string length (number of chars)
	unsigned length() const {return m_size;}; // get the string length (number of chars)
	unsigned size() const {return m_size;}; // get the string length (number of chars)
	bool IsEmpty() const; // true if zero length
	void Empty(); // clear contents to empty
	void myDelete(); // release memory used by object
	operator const char*() const {return m_str;};  // VBString type cast for "const char*"
	const char *getStr() const {return m_str;};
	const char *c_str() const {return m_str;};
	char getChar(unsigned pos) const; // return char at position
	void putChar(unsigned pos,char ch); // put char at position, if this has enough length

	// handling number of decimal digits after the dot, when converting float point to VBString
	void setDecs(const unsigned decs) { m_decs = decs; };
	unsigned getDecs() const { return m_decs; };

	// handling of token
	
	// return the number of sub-strings
	unsigned countSubString (const char *subStr, bool caseSens = true) const;

	unsigned tokCount(const char tok) const;
	VBString strtok(const char tok, const unsigned n) const;
	VBString strtok(const char tok, const unsigned n, bool & found) const;
	void strtokchg(const char tok, const unsigned n, const char *newData);

	// advanced string manipulation
	VBString strAfterPosition(const unsigned pos) const;
	VBString strUntilPosition(const unsigned pos) const;
	VBString strInside(unsigned from, unsigned to) const; 
	unsigned strtokpos(const char tok, const unsigned n) const;
	
	VBString afterFind(const char findChar) const;
	VBString afterFind(const char *find) const;
	VBString afterFind(const char findChar, bool & found, bool caseSens = true) const;
	VBString afterFind(const char *find, bool & found, bool caseSens = true) const;
	
	VBString beforeFind(const char *find) const;
	VBString beforeFind(const char findChar) const;
	VBString beforeFind(const char *find, bool & found, bool caseSens = true) const;
	VBString beforeFind(const char findChar, bool & found, bool caseSens = true) const;

	void strschg(const char *find,const char *replace); // deprecated method. Use "replace" instead
	void replace(const char *find,const char *replaceStr) 
		{ strschg(find,replaceStr); }; // alternative name for strschg
	bool existSubstring(const char* subStr, bool caseSens = true) const;
	bool leftSideIs(const char* subStr) const;
	VBString separateLeftString(const char* leftString) const;
	VBString debug(int format = 1) const;
	void setUnterminatedString(const char *str, unsigned length); // this = str
	void wipeLeadingSpace(); // wipe leading spaces, tabs and returns
	void wipeCtrmM(); // wipe ^M (happens when reading windows text files in Windows
	bool eofDetected() const;

	void setIstream(::std::istream & is) { m_myIstream = &is; };
	void getLine(char endChar = '\n'); // auxiliary method for extractor
	void getNextWord(::std::istream & stream);


	// input "My.name.txt"
	// output "txt"
	VBString getFileExtension();

	// overload insersor and extractor
	friend VBLIB_DLL_API std::istream & operator>>(std::istream & stream, VBString & obj); // extractor
	friend VBLIB_DLL_API std::ostream & operator<<(std::ostream & stream, const VBString & obj); // insersor

	// char case manipulation, allowing accented char
	void toLower(); // ignore extended char
	void toUpper(); // ignore extended char
	void extendedToLower(); // convert to extended char lo caps
	void extendedToUpper(); // convert to extended char hi caps
	void safeToLower(); // convert to standard ascii char lo caps
	void safeToUpper(); // convert to standard ascii char hi caps
	void toSafe(); // convert to safe (non-extended) chars

	// simple encryption handling
	void simpleEncrypt(unsigned short code = 46);
	void simpleEncryptReverse(unsigned short code = 46);
	void MD5_hash(); // hash (non reversible encrypt) this using MD5 algorithm
					 // after MD5_hash method, *this becomes a 32 char long string 
					 // (independendant from original length).
	VBString get_MD5_hash() const; // hash (non reversible encrypt) this using MD5 algorithm
					 // return a 32 char long string (independendant from original length).

	// http escape sequence handling
	void escapeSequence();        // convert object to web escape sequence
	void escapeSequenceReverse(); // convert object to web reverse escape sequence

#ifdef _MSC_VER  // Visual C++ only !!!
	// methods for BSTR
	void getBSTR(const BSTR & pWord);
	BSTR exportBSTR();
#endif

	// check for valid methods
	bool validEmail() const;
	bool validAddress() const;
	bool validCity() const;
	bool validCIC() const;
	VBString rightCIC() const;
	bool validName() const;
	bool validPhone() const;
	bool validZip() const;
	bool validCreditCardNumber() const;
	bool validHelper(const char *validChars, const char* validSymbols, 
		unsigned minSize=0, unsigned maxSize=0) const;
	bool charIsValid(char ch,const char *validChars) const;
private:
	unsigned findDigit (int i, int lenght, const char *buffer) const;
	char intToChar (int num) const { return (char) (num+48); }
	int charToInt (char ch) const { return (ch-48); }

}; // end of VBString

////////////////////////////////////////////////////////////////////
//
// VBStringList class
//
/////////////////////////////////////////////////////////////////////

// It can turn into bug to pass "deque<VBString> &" directly as an argument
// So, pass "VBStringList"
// don't use this class as a return from a function 
// "deque<VBString> &" from a dll won't work!!!

struct VBLIB_DLL_API VBStringList
{
	std::deque<VBString> m_data;
	bool operator==(const VBStringList & obj);
	typedef std::deque<VBString>::iterator iterator;


	// ~VBStringList(); // destructor
};

}}}} // close namespace ::br::com::sbVB::VBLib

