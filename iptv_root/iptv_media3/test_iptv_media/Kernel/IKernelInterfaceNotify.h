#ifndef I_KERNEL_INTERFACE_NOTIFY_H
#define I_KERNEL_INTERFACE_NOTIFY_H

#include "media_utilities.h"

class IKernelInterfaceNotify
{

public:

    virtual ~IKernelInterfaceNotify() {}
	virtual ULONG SetMediaAlive(ULONG _id, MediaSpec _mediaSpec) = 0;
};

#endif