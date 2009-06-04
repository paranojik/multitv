// VBClock.cpp

#include "VBLib/VBLib.h"

/////////////////////////////////////////////////////////////////
//
// VBClock
//
/////////////////////////////////////////////////////////////////


void br::com::sbVB::VBLib::VBClock::checkDiffTime() 
{
	m_finish = clock();
	m_duration = (double)(m_finish - m_start) / CLOCKS_PER_SEC;
}

void br::com::sbVB::VBLib::VBClock::markTime() 
{
	m_start = clock();
}

// constructor
br::com::sbVB::VBLib::VBClock::VBClock() 
{ 
	markTime(); 
}

double br::com::sbVB::VBLib::VBClock::getDiffTime() 
{
	checkDiffTime();
	return m_duration;
}

void br::com::sbVB::VBLib::VBClock::printDiffTime() 
{
	checkDiffTime();
	::std::cout.precision(2);
	::std::cout << "Elapsed time (in seconds) is " << getDiffTime() << ::std::endl;
	// myCout << "Elapsed time (in seconds) is " << ::std::endl;
}

