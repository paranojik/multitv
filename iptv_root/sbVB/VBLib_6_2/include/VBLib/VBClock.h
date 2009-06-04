// VBClock.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// VBClock
// a class make a clock
//
/////////////////////////////////////////////////////////////////////
class VBLIB_DLL_API VBClock 
{
	time_t m_start, m_finish;
	double m_duration;
	void checkDiffTime();
public:
	void markTime();
	VBClock();
	double getDiffTime();
	// void printDiffTime(::std::ostream & myCout = ::std::cout);
	void printDiffTime();
};

void testGlobalFunction();


}}}} // close namespace ::br::com::sbVB::VBLib

