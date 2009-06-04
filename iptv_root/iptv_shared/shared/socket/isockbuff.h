#ifndef __ISOCK_BUFF__
#define __ISOCK_BUFF__


//STG_E_INCOMPLETE;     // Socket Error
//STG_E_ACCESSDENIED    // Socket nao conectado
//STG_E_INVALIDPOINTER  // Memory error
//STG_E_WRITEFAULT      // Erro de escrita
//S_OK                  // Ok

class ISockBuff
{

public:

    //virtual void SetUrl  (LPCTSTR _sztProtocol, LPCTSTR _sztHost, DWORD _dwPort) = 0;
    virtual BOOL OpenUrl (void) = 0;
    virtual BOOL CloseUrl(void) = 0;
    virtual BOOL IsOpened(void) = 0;
    //virtual void Abort   (void) = 0;

    virtual STDMETHODIMP Read( /* [length_is][size_is][out] */ void *pv,
                       /* [in] */ ULONG cb,
                       /* [out] */ ULONG *pcbRead) = 0;
    virtual STDMETHODIMP Write( /* [size_is][in] */ const void *pv,
                       /* [in] */ ULONG cb,
                       /* [out] */ ULONG *pcbWritten) = 0;
};


class ISockBuffNotify
{
public:
    virtual BOOL NewID    (ULONG _id, BYTE _type) = 0;
    virtual void NotifyPkt(DWORD _notifyid, ULONG _id, double _fpercent, BYTE _flags) = 0;
    virtual void NotifyEOF(void)      = 0;
};



#endif  // __ISOCK_BUFF__




