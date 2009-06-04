// VBFileDirectory.cpp

#include "VBLib/VBLib.h"

/////////////////////////////////////////////////////////////////////
//
// class VBFileDirectory
//
/////////////////////////////////////////////////////////////////////

// return true if the string after the last "/" is not "." nor ".."
bool br::com::sbVB::VBLib::VBFileDirectory::p_goodName(br::com::sbVB::VBLib::VBString fileName) 
{
	// "." and ".." are allowed
	if (m_allowNonGoodNames)
		return true;

	// "." and ".." are not allowed
	char tok = '/';
	fileName = fileName.strtok(tok,fileName.tokCount(tok)); // get the string after last "/"

	return ((br::com::sbVB::VBLib::VBString (".") != fileName) && (br::com::sbVB::VBLib::VBString ("..") != fileName) );

}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBFileDirectory::p_lastFileName(VBString path) 
{
	unsigned tc = path.tokCount('/');
	path = path.strtok('/',tc);
	tc = path.tokCount('\\');
	path = path.strtok('\\',tc);
	return path;
}

// default constructor
br::com::sbVB::VBLib::VBFileDirectory::VBFileDirectory () 
{ 
	m_isDir = false;
	m_allowNonGoodNames = false;
}
        
bool br::com::sbVB::VBLib::VBFileDirectory::isDir () 
{
	return m_isDir;
}
    
br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBFileDirectory::getPath () 
{
	return m_dirPath;
}
    
br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBFileDirectory::getNext() 
{
	return p_get();
}

void br::com::sbVB::VBLib::VBFileDirectory::allowNonGoodNames()
{
	m_allowNonGoodNames = true;
}

void br::com::sbVB::VBLib::VBFileDirectory::denyNonGoofNames()
{
	m_allowNonGoodNames = false;
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBFileDirectory::getFirst(const char *path) 
{
	m_path = path;
	m_dirPath = path;
	#ifdef _MSC_VER // Visual C++ only
		VBString mask = path + VBString("/");
		mask += "*.*";

		m_h = FindFirstFileA(mask, &m_f);

		#if _MSC_VER == 1200 // VC6
			m_endOfList = (m_h == 0) || (m_h == (void*)0xffffffff);
		#endif

		#if _MSC_VER > 1200 // VC2003, vc2005 or earlier
			m_endOfList = (m_h == 0) || (m_h == (HANDLE)-1);
		#endif

		// FindClose(m_h);

		if (m_endOfList) 
			return "";
		VBString ret = (const char *)m_f.cFileName;
		if (!p_goodName(ret)) 
			ret = getNext();
		// return m_path + VBString("/") + ret;
		return ret;
	#else // unix only
		m_dp = opendir(path);
		m_endOfList = (m_dp == 0);
		if (m_endOfList) return "";
		return p_get();
	#endif
}
    
bool br::com::sbVB::VBLib::VBFileDirectory::eolist() 
{
	#ifdef _MSC_VER // Visual C++ only
	#else // unix only
		if (m_endOfList) closedir(m_dp);
	#endif
	return m_endOfList;
}

// set m_isDir true if argument is a directory
void br::com::sbVB::VBLib::VBFileDirectory::p_isDirectory (VBString file) 
{
#ifdef _MSC_VER // Visual C++ only
	#if _MSC_VER == 1200 // VC6
		DWORD attribs = GetFileAttributes(file);
	#endif

	#if _MSC_VER > 1200 // VC2003, vc2005 or earlier
		DWORD attribs = GetFileAttributes((LPCWSTR)file.c_str());
	#endif

	m_isDir = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
	// if (attribs == 0xFFFFFFFF) m_isDir=false;
#else // unix only
// Andre Cotrim 20071204
// Must not pass C++ objects to C functions!
	DIR *dp = opendir(file.c_str());
	struct stat statbuf; 
	lstat(file.c_str(),&statbuf);
	m_isDir = (S_ISDIR(statbuf.st_mode) != 0);
#endif
	if (m_isDir) {
		m_dirPath = file;
	}
}

br::com::sbVB::VBLib::VBString br::com::sbVB::VBLib::VBFileDirectory::p_get() 
{
	bool isGoodFileName;
	VBString ret;
#ifdef _MSC_VER // Visual C++ only
	m_endOfList = !FindNextFileA(m_h, &m_f);
	
	// free handle
	// FindClose(m_h);

	ret = (const char *)m_f.cFileName;

#if 0
	// this loop is old and buggy. The loop below is the correct one
	do {
		isGoodFileName = p_goodName(ret);
		if (!isGoodFileName && !m_endOfList) 
			ret=p_get(); // recursive
	} while (!isGoodFileName);
#endif
	while (true)
	{
		isGoodFileName = p_goodName(ret);
		if (isGoodFileName || m_endOfList) 
			break;
		ret=p_get(); // recursive
	}
	ret = p_lastFileName(ret);
#else // unix only
	struct dirent *dirp;
	do {
		dirp = readdir(m_dp);
		m_endOfList = (dirp == 0);
		if (m_endOfList) return"";
		isGoodFileName = p_goodName(dirp->d_name);
	} while (!isGoodFileName);
	ret = dirp->d_name;
#endif
	ret = m_path + VBString("/") + ret;
	p_isDirectory(ret); // set m_isDir
	return ret;
}


