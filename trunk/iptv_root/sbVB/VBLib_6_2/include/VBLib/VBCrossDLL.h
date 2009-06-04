// VBCrossDLL.h

#include <deque>
#include <list>

// additional includes for dll and so
#ifdef WIN32
	// code for Windows
	#include <windows.h>
#else
	// code for Unix
	#include <dlfcn.h>
//	#define GetFunctionFromModule dlsym
//	#define CloseModule dlclose
//	typedef (void*) LpHandleType;
#endif


// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

class VBLIB_DLL_API VBException_crossDLL : public ::br::com::sbVB::VBLib::VBException
{
public:
	VBException_crossDLL (const char *m) : VBException(m) {};
};

// cross platform DLL loader (DLL in Windows and so in unix)
class VBLIB_DLL_API VBCrossDLL
{
	bool m_isLoaded;
	bool m_throwExceptionIfNotFound;
	::br::com::sbVB::VBLib::VBString m_dllName;
#ifdef WIN32
	// code for Windows
	HINSTANCE m_dllHandle;
#else
	// code for Unix
	void *m_soHandle;
#endif

public:
	VBCrossDLL();
	virtual ~VBCrossDLL();

	// copy constructor 
	VBCrossDLL(const VBCrossDLL &);

	// operator=
	void operator=(const VBCrossDLL & obj);

	void setThrowExceptionIfNotFound(bool b);
	bool getThrowExceptionIfNotFound();

	// get dll name
	::br::com::sbVB::VBLib::VBString getName() const;

	// load dll, throw exception if can not load
	void load(const char *dllName) throw (VBException_crossDLL);

	// check if dll or so load is ok, but does not load
	bool loadOK(const char *dllName);

	// unload
	void unLoad() throw (VBException_crossDLL);

	// get address of function or object from dll or so
	void *getAddress(const char *exportedName) throw (VBException_crossDLL);

	// return true if dll or so has exportedName
	bool hasExportedName(const char *exportedName) throw (VBException_crossDLL);

	// return true if dll or so has all names in strArray as exported
	bool hasAllNeededExportedNames
	(const char **exportedNamesDllMustHave, unsigned size);

	static void getDllsWithAllExportedNames(const char **exportedNamesDllMustHave,unsigned size,
		::std::list<VBString> & dllNames,const char *directory, const char *extension) 
		throw (VBException_crossDLL);

};


class VBLIB_DLL_API VBCrossDLLContainer
{
	::std::deque<br::com::sbVB::VBLib::VBCrossDLL> m_dllContainer;
public:
	void loadDllsWithAllExportedNames(const char **exportedNamesDllMustHave,unsigned size,
		const char *directory, const char *extension) 
		throw (VBException_crossDLL);

	unsigned getNumberOfDlls();

	VBCrossDLL & operator[](unsigned i) throw (VBException_crossDLL);

	// unload
	void unLoad() throw (VBException_crossDLL);

	// destructor
	~VBCrossDLLContainer();
};


}}}} // close namespace ::br::com::sbVB::VBLib



