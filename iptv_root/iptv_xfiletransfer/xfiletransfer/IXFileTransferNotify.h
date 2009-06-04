#if !defined(_IXFTPNOTIFY)
#define _IXFTPNOTIFY


enum XFTP_CODE
{
    XFTPC_NULL,
    XFTPC_EOF,
    XFTPC_RCVP1STATS,
    XFTPC_RCVP2STATS,
    XFTPC_SNDP1STATS,
    XFTPC_SNDP2STATS,
    XFTPC_SNDERR,
    XFTPC_RCVERR,
    XFTPC_RCVSUCCESS,
    XFTPC_PKTREQ,
    XFTPC_SNDEND
};

class IXFTPNotify
{
public:
    virtual void OnFileTransferEvent(XFTP_CODE _code, DWORD _dwData1, DWORD _dwData2) = 0;
};

#endif // !defined(_IXFTPNOTIFY)


