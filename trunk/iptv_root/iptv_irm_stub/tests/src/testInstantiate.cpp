#include <iostream>

#include "stub/IrmStubDll.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "\nTest the instantiation of iptv_irm_stub classes.\n\n" << endl;

	//==========================================================
	ProxyIrmStub _ProxyIrmStub;
	cout << "ProxyIrmStub instantiated in the stack.\n" << endl;

	ProxyIrmStub *pProxyIrmStub;
	cout << "ProxyIrmStub pointer instantiated in the stack." << endl;
	pProxyIrmStub = new ProxyIrmStub();
	cout << "ProxyIrmStub instantiated in the heap." << endl;
	delete pProxyIrmStub;
	cout << "ProxyIrmStub deleted.\n" << endl;

	//==========================================================
	cout << endl;
	return(0);
}
