#include <iostream>
#include "iptv_kernel/IPTVKernel.h"

using namespace std;

class KernelNotifyTest : public KernelNotify
{
	bool OnKernelNotify(KernelParam& param);
};

bool KernelNotifyTest::OnKernelNotify(KernelParam& param)
{
	cout << "Kernel notification message: " << param.m_code << endl;
	return true;
}

int main()
{
	cout << "*** BEGIN IPTV Kernel instantiation test ***" << endl;

	IPTVKernel kernel;

	KernelNotifyTest notify;
	kernel.SetNotify(&notify);

	cout << "*** END IPTV Kernel instantiation test ***" << endl;

	return 0;
}
