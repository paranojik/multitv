// VBFileDirectory.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// class VBFileDirectory
//
/////////////////////////////////////////////////////////////////////

class VBLIB_DLL_API VBFileDirectory 
{
#ifdef _MSC_VER // Visual C++ only
	WIN32_FIND_DATAA m_f;
	HANDLE m_h;
#else // unix only
	DIR *m_dp;
#endif
	::br::com::sbVB::VBLib::VBString m_path, m_dirPath;
	bool m_endOfList;
	bool m_isDir;
	bool m_allowNonGoodNames;

	bool p_goodName(::br::com::sbVB::VBLib::VBString fileaName);
	void p_isDirectory (::br::com::sbVB::VBLib::VBString file); 
	::br::com::sbVB::VBLib::VBString p_get(); 
	::br::com::sbVB::VBLib::VBString p_lastFileName(::br::com::sbVB::VBLib::VBString path);
public:
	VBFileDirectory (); // default constructor     
	bool isDir ();
	::br::com::sbVB::VBLib::VBString getPath();       
	::br::com::sbVB::VBLib::VBString getNext();
	::br::com::sbVB::VBLib::VBString getFirst(const char *path);        
	bool eolist();
	void allowNonGoodNames();
	void denyNonGoofNames();
};

}}}} // close namespace ::br::com::sbVB::VBLib
