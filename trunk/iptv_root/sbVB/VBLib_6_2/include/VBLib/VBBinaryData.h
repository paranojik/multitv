// VBBinaryData.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

class VBLIB_DLL_API VBException_binaryData : public ::br::com::sbVB::VBLib::VBException
{
public:
	VBException_binaryData (const char *m) : VBException(m) {};
};

class VBLIB_DLL_API VBBinaryData
{
	unsigned char *m_data;
	unsigned m_size;
	bool m_loadedOK;

public:
	// default constructor
	VBBinaryData();

	// copy constructor
	VBBinaryData(const VBBinaryData & obj);

	void operator=(const VBBinaryData & obj);
	void operator=(const char * str);

	// destructor
	virtual ~VBBinaryData();

	const unsigned char* getDataPointer() const;

	void setData(const char *data, unsigned size);
	void setData(const void *data, unsigned size);
	void setDataInHex(const char *data);
	void create(unsigned size);
	void createAndFill(unsigned size, unsigned char ch = 0);
	unsigned getSize() const;
	// void dump(::std::ostream & mycout = ::std::cout);
	void dump() const;
	unsigned char operator[](unsigned i) const;
	unsigned char & operator()(unsigned i);

	void myDelete();

	void loadFromFileAsBin(const char *fileName);
	void saveToFileAsBin(const char *fileName) const;
	void loadFromFileAsText(const char *fileName);
	void saveToFileAsText(const char *fileName, unsigned bytesPerLine = 20) const;

	bool operator==(const VBBinaryData & obj) const;
	bool operator!=(const VBBinaryData & obj) const;
	VBBinaryData operator+(const VBBinaryData & obj) const;

	void subObjectCopyTo(unsigned from, unsigned to, VBBinaryData & subObject) const;
	void subObjectCopyFrom(unsigned fromThis, unsigned fromSource, 
		unsigned to, const VBBinaryData & copyFromObject);

	::br::com::sbVB::VBLib::VBString getStringInHex() const;
	void setFromStringInHex(::br::com::sbVB::VBLib::VBString in);
};

}}}} // close namespace ::br::com::sbVB::VBLib

