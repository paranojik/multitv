#ifndef __WBOARDPROC
#define __WBOARDPROC

#include "shared/ListT.h"

#include "IWBoardNotify.h"
#include "WBoardBridge.h"


// ----------------------------------------------------------------------------
// CWBProcess class
// ----------------------------------------------------------------------------

class CWBProcess : public IWBProcess, public WBInterfaceRef
{
public:

    CWBProcess  (IWBoardNotify* pNotify);
    ~CWBProcess (void);

    WB_PCSTR GetVersion (void);

    CtrlAttr *GetCtrlAttr(CtrlAttr *lpAttr = NULL);

    bool IsWindowCreated() { return m_bCreateWnd; }

    IWBProcess* GetProcessPtr();

    void OnWindowCreate( IWBInterface* pInterface );
    void OnWindowClose( );
    void OnWindowDestroy( );

    void OnToolBoxItemClick( int nId );
    void OnToolBoxPenSizeClick( int nId );
    void OnToolBoxColorClick( ColorDef& color );
    void OnToolBoxFontClick( FontDef& font );
    void OnToolBoxImageSel( WB_PCSTR szFile );

    void OnMenuBarNewClick( );
    void OnMenuBarSaveClick( );
    void OnMenuBarPrintClick( );
    void OnMenuBarPageSetupClick( );
    void OnMenuBarCutClick( );
    void OnMenuBarUndoClick( );
    void OnMenuBarImageStopClick( );

    void OnMouseLButtonDown( MouseFlag flag, PtPos& point );
    void OnMouseLButtonUp( MouseFlag flag, PtPos& point );
    void OnMouseMove( MouseFlag flag, PtPos& point );

    void OnEditRedraw( );
    void OnEditCutArea( PtPos& pt1, PtPos& pt2 );

    void OnScreenScrollWindow( int dx, int dy );
    void OnScreenSetScrollPos( int posx, int posy );

    bool AddElement(CtrlAttr* attr);
    bool AddPoints(CtrlAttr* attr);
    void NewExec();
    void UndoExec();

protected:

    void ResetControls();
    bool InsertControl(CtrlAttr* attr);
    void DrawControl(CtrlAttr* attr, WB_BYTE flags);

    void _DrawRect( PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color, bool bFull );
    void _DrawEllipse( PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color, bool bFull );
    void _DrawLine( PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color );
    void _DrawPoint( PtPos* pPoint, UINT nLin, ColorDef& color, bool bMask);
    void _DrawIndicator( PtPos& pt );
    void _DrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2 );
    void _DrawText( WB_PSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color );

    bool _EdtUndo( );

private:

    bool      m_bCreateWnd;
    bool      m_bCtrlTxt;
    bool      m_bLeftDown;
    bool      m_bChanged;

    UINT      m_CurIdent;
    ColorDef  m_CurColor;
    FontDef   m_CurFont;
    UINT      m_CurLine;

    ColorDef  m_DelColor;
    ColorDef  m_MrkColor;

    PtPos     m_MouseIni;
    PtPos     m_MousePos;

    CtrlAttr  m_CtrlAttr;

    ListT<CtrlAttr> *m_pControls;

    IWBoardNotify* m_pIWBNotify;
};

#endif // __WBOARDPROC
