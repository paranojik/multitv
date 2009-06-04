#ifndef INOTIFY_KERNEL_H
#define INOTIFY_KERNEL_H

#include "IKernelNotify.h"

class IMediaKernelNotify;

class ISendKernelNotify
{
protected:
    
    IMediaKernelNotify  *m_pNotify;

public:
    ISendKernelNotify() {m_pNotify = NULL; }

    virtual ULONG NotifyKernel(MediaCodeNotify _code) = 0;

    void SetKernelNotify(IMediaKernelNotify  *_pNotify) {m_pNotify = _pNotify; }
};


#endif

