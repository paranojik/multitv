#ifndef KERNEL_H
#define KERNEL_H

#include "socket/sockbuffs.h"
#include "IKernelNotify.h"
#include "IInterfaceKernelNotify.h"


class CMedia;
class IKernelInterfaceNotify;

class Kernel : public ISockBuffNotify, 
               public IMediaKernelNotify, 
               public IInterfaceKernelNotify
{
private:

    BOOL       m_bInit;

    SockBuffs *m_pSockBuffs;
    CMedia    *m_pMedia;
    
    IKernelInterfaceNotify  *m_pKernelInterfaceNotify;

    // ISockBuffNotify overrides
    virtual void  NotifyPkt(DWORD _notifyid, ULONG _id, double _fpercent, BYTE _flags) {}
    virtual void  NotifyEOF(void) {}
    virtual BOOL  NewID(ULONG _id);

    // IMediaKernelNotify overrides
    virtual BOOL OnMediaNotify(MediaParam *_pParameters);

    // IInterfaceKernelNotify overrides
    virtual ULONG InitNetwork(LPCSTR _szFilename);
    virtual ULONG InitNetwork(LPCSTR _szProtocol, LPCSTR _szHost, DWORD _dwPort, LPCSTR _szConnStr, BOOL _flListen);
    virtual ULONG TerminateConference();
    virtual ULONG GetMediaDbgInfo(ULONG _id, MediaDbgInfo & _mediaDbgInfo);
    virtual ULONG GetVideoReg(ULONG _id, VideoFrame_reg ** _ppVideoFrameReg);

public:

    Kernel();
    ~Kernel();

    void SetGraphicModule(IKernelInterfaceNotify  *_pNotify) {m_pKernelInterfaceNotify = _pNotify; }

};




#endif