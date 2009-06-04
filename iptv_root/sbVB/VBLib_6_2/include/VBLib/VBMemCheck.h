// VBMemCheck.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// VBMemCheck
// a class to check memory leak
//
/////////////////////////////////////////////////////////////////////
class VBLIB_DLL_API VBMemCheck 
{
public:	
#ifdef _MSC_VER
private:	
	_CrtMemState m_s1; 

	void reportToStdout()
	{
		// Send all reports to STDOUT
		_CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN,   _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR,  _CRTDBG_FILE_STDOUT);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	}

	void reportToNothing()
	{
		// Send all reports to null
		_CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN,   _CRTDBG_REPORT_FILE);
		_CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ERROR,  _CRTDBG_REPORT_FILE);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_REPORT_FILE);
	}

public:	
	VBMemCheck(int p1=0, int p2=0) 
	{  
		// dirty trick to avoid bug in memory leak detection
		// Run once the program using no parameter, and see the memory leak. 
		// copy the 2 pointers leaking to the parameters.
		// that should avoid the memory leak detection bug
		// if you mistake in using proper pointer, it's very likelly that your program will hang
		if (p1) delete (void*)p1;
		if (p2) delete (void*)p2;

	// Store memory checkpoint in s1 (global variable) memory-state structure
		_CrtMemCheckpoint(&m_s1);
	};
#endif
	void check() 
	{
#ifdef _MSC_VER
		reportToStdout();
		_CrtDumpMemoryLeaks();
#else
		::std::cout << "Memory Leak not tested" << ::std::endl;
#endif
	};

	void dump()
	{
		check();
	}

	bool testMemoryLeak()
	{
#ifdef _MSC_VER
		reportToNothing();
		return _CrtDumpMemoryLeaks() != 0;
#else
		return false;
#endif
	}
};

}}}} // close namespace ::br::com::sbVB::VBLib
