#include <string>
#include <deque>
#include <cmath>
#include <iostream>
#include <sstream> // istringstream
using namespace std;
#include "VBLib/VBLib.h"
using namespace br::com::sbVB::VBLib;

class Figure // abstract base class
{
public:
	virtual void draw() = 0; // pure virtual
	virtual const char *getName() = 0; // pure virtual
	virtual unsigned getID() = 0; // pure virtual
};

void load_dll_late_link()
{
	VBCrossDLL dll;
	
	// definition of pointer to method
	typedef Figure* (*ObjectAllocType)();
	ObjectAllocType objectAllocFunctionPtr;
	Figure *figurePtr;

	try
	{
		const char *dllName = "circle.dll";
		
		// load dll or so. Exception if can not load
		dll.load(dllName);
		
		// check if exported names are ok
		VBAssert (dll.hasExportedName("allocFigure"));
		
		// get from dll or so the global function that allocs an object
		objectAllocFunctionPtr = (ObjectAllocType) dll.getAddress("allocFigure");

		// execute function, so the object is alloced
		figurePtr = objectAllocFunctionPtr();

		figurePtr->draw();	// call method with late bind
				
//		delete figurePtr; // delete pointer so no memory leak
		
		dll.unLoad();

		
	}
	catch(VBException_crossDLL & e)
	{
		VBAssert (false,e.getExplanation());
	}
}

void load_one_dll()
{
	VBCrossDLL dll;
	
	typedef int (*MyFunPtrType)(int);
	MyFunPtrType myFunPtr;
	
	const char *dllName = "plus_n.dll";
	
	// check if can load dll or so, but not load it
	if(!dll.loadOK(dllName))
	{
		cout << "won't be able to load dll or so" << endl;
		return;
	}
	
	// load dll or so. Exception if can not load
	dll.load("plus_n.dll");
	
	const char *exportedNamesDllMustHave[] = {
		"plus_1","plus_2","plus_3","plus_4","plus_5"
	};
	unsigned s = sizeof(exportedNamesDllMustHave)/sizeof(const char *);
	
	for (unsigned i=0; i < s ;i++)
	{
		if(!dll.hasExportedName(exportedNamesDllMustHave[i]))
		{
			cout << "this dll or so don't have all must have exported names" << endl;
			return;			
		}
	}
	
	// load a function from dll or so. Exception if can not get it
	myFunPtr = (MyFunPtrType) dll.getAddress("plus_3");
	// use the function from dll or so and check it works
	VBAssert (myFunPtr(6)==9); 
	
	// another test
	myFunPtr = (MyFunPtrType) dll.getAddress("plus_1");
	VBAssert (myFunPtr(4)==5); 
	
}

void load_all_dll_with_names()
{
	const char *exportedNamesDllMustHave[] = {
		"plus_1","plus_2","plus_3","plus_4","plus_5"
	};
	unsigned s = sizeof(exportedNamesDllMustHave)/sizeof(const char *);

	typedef int (*MyFunPtrType)(int);
	MyFunPtrType myFunPtr;

	try
	{
		VBCrossDLLContainer dllContainer;

		dllContainer.loadDllsWithAllExportedNames(exportedNamesDllMustHave,s,"c:/xxx","dll");
		
		unsigned n_dll = dllContainer.getNumberOfDlls();
		// VBAssert (n_dll==4); 
		
		unsigned i;
		for (i = 0 ; i < n_dll ; i++)
		{
			myFunPtr = (MyFunPtrType) dllContainer[i].getAddress("plus_2");
			VBAssert (myFunPtr(6)==8); 
		}
	
	}
	catch(VBException_crossDLL & e)
	{
		VBAssert (false,e.getExplanation());
	}
	catch(VBException &)
	{
		cout << "other exception" << endl;
	}
	

}

void lateBind_ExplicitLink()
{
	const char *exportedNamesDllMustHave[] = {
		"allocFigure"
	};
	unsigned s = sizeof(exportedNamesDllMustHave)/sizeof(const char *);

	// definition of pointer to method
	typedef Figure* (*ObjectAllocType)();
	ObjectAllocType objectAllocFunctionPtr;

	try
	{
		VBCrossDLLContainer dllContainer;

		dllContainer.loadDllsWithAllExportedNames(exportedNamesDllMustHave,s,"c:/xxx","dll");
		
		unsigned n_dll = dllContainer.getNumberOfDlls();

		Figure **figurePtrArray = new Figure* [n_dll];
		
		unsigned i;
		for (i = 0 ; i < n_dll ; i++)
		{
			// get from dll or so the global function that allocs an object
			objectAllocFunctionPtr = (ObjectAllocType) dllContainer[i].getAddress("allocFigure");

			// execute function, so the object is alloced
			figurePtrArray[i] = objectAllocFunctionPtr();

			VBString name = figurePtrArray[i]->getName();

			cout << "Name = " << name << ", " 
				<< "ID = " << figurePtrArray[i]->getID() << endl;
			figurePtrArray[i]->draw();


			
		}
	
	}
	catch(VBException_crossDLL & e)
	{
		VBAssert (false,e.getExplanation());
	}
	catch(VBException &)
	{
		cout << "other exception" << endl;
	}
	
}



void VBCrossDLL_test()
{
	// load_dll_late_link();
	load_one_dll();
	load_all_dll_with_names();
	lateBind_ExplicitLink();
}
