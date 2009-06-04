#ifndef I_INTERFACE_KERNEL_NOTIFY_H
#define I_INTERFACE_KERNEL_NOTIFY_H

#include "Data_reg.h"
#include "media_utilities.h"

class CVideoFrame;
class IInterfaceKernelNotify
{
protected:
	typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

public:

    virtual ULONG TerminateConference() = 0;
    virtual ULONG InitNetwork(LPCSTR _szFilename) = 0;
    virtual ULONG InitNetwork(LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen) = 0;
    virtual ULONG GetMediaDbgInfo(ULONG _id, MediaDbgInfo & _mediaDbgInfo) = 0;
    virtual ULONG GetVideoReg(ULONG _id, VideoFrame_reg ** _ppVideoFrameReg) = 0;
};

#endif