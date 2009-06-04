#ifndef __WBOARD_DLL
#define __WBOARD_DLL

#include "IWBoardNotify.h"

#ifdef WIN32
#ifdef WBOARD_EXPORTS
#define WBOARD_API __declspec(dllexport)
#else
#define WBOARD_API __declspec(dllimport)
#endif
#else
#define WBOARD_API
#endif


class CWBProcess;
class CWBInterface;

class WBOARD_API WBoardModule
{
public:

    WBoardModule  ( );
    ~WBoardModule ( );

    bool       Initialize      ( WB_PWND pParent, int nId, IWBoardNotify* pNotify, RcPos* pRc = NULL );
    void       Delete          ( );

    bool       Resize          ( RcPos* pRc );
    void       ShowWindow      ( bool bOp );
    void       EnableWindow    ( bool bOp );
    void       EnableImageCtrl ( bool bOp );
    void       EnableImageStop ( bool bOp );

    WB_RESULT  FilterWBMessage ( WB_PCSTR pbMsg, WB_HMSG * phMsg );
    WB_RESULT  GetWBMessage    ( long lEvent, WB_HMSG hMsg, WB_PSTR pbMsg, long szbMsg );
    WB_RESULT  AddMetaElement  ( WB_HMSG hMsg );

    WB_PCSTR   GetVersion      ( );


private:

    CWBProcess*     m_pWBProcess;
    CWBInterface*   m_pWBInterface;
};

#endif      // __WBOARD_DLL
