// VBShowSTLContainer.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {

/////////////////////////////////////////////////////////////////////
//
// VBShowSTLContainer
// generic global function to show a STL container
// if n==0, show the complete container, 
// if n > 0, show only n first elements
//
/////////////////////////////////////////////////////////////////////
template <class containerType> 
void VBShowSTLContainer(const containerType & containerObj, const int sampleCount = 0,
						::std::ostream & myCout = ::std::cout) 
{   
	typename containerType::const_iterator iteratorObj;   
	if (containerObj.empty()) {     
		myCout << "======Container empty" << ::std::endl;
		return;   
	}   
	myCout << "======show the STL container ";
	if (sampleCount>0) 
		myCout << "sample(" << sampleCount << ")";
	else
		myCout << "contents";
	myCout << ", size=" << containerObj.size() << ::std::endl;   
	int n = 0;
	for (iteratorObj = containerObj.begin() ;     
		(iteratorObj != containerObj.end()) && ( !(sampleCount>0) || (n < sampleCount) ) ;     
		iteratorObj++) {
		myCout << *iteratorObj << ::std::endl; 
		n++;
	}
}; 

}}}} // close namespace ::br::com::sbVB::VBLib

