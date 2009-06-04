#ifndef __WBOARDWND
#define __WBOARDWND

#include "WBoardInfo.h"
#include "WBoardBridge.h"

// ----------------------------------------------------------------------------
// CWBProcess class
// ----------------------------------------------------------------------------

class CWBFrame;

class CWBInterface
{
public:

    CWBInterface  ( WB_PWND pParent, IWBProcess* pProcess, int nId, RcPos* pRc );
    ~CWBInterface ( void );

    IWBInterface* GetInterfacePtr();

    bool Resize(RcPos* pRc);
    void ShowWnd(bool bOp);
    void EnableWnd(bool bOp);
    void EnableImgCtrl( bool bOp );
    void EnableImgStop( bool bOp );

private:

    CWBFrame*  m_pWBFrame;
};

#endif // __WBOARDWND
