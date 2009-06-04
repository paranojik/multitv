#ifndef KERNEL_CTRL_H
#define KERNEL_CTRL_H

#include "KernelCtrl.h"
#include "Kernel.h"

class KernelCtrl
{
private:
    Kernel *m_pKernel;

public:
    KernelCtrl();
    ~KernelCtrl();
};

#endif