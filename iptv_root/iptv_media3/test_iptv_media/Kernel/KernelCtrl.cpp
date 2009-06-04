#include "compat.h"
#include "KernelCtrl.h"

#define FILE_PATH "c:\\ArquivosIMC\\ArquivoIMC38.imc"

KernelCtrl::KernelCtrl()
{
    m_pKernel = new Kernel();
}

KernelCtrl::~KernelCtrl()
{
    if (m_pKernel)
        delete m_pKernel;
}