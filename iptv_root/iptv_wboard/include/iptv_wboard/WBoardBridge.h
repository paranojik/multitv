#if !defined _WBOARDBRIDGE_H
#define _WBOARDBRIDGE_H

#include "WBoardInfo.h"


class IWBInterface
{
public:

    virtual void ToolBoxItemSel( int nId ) = 0;
    virtual void ToolBoxItemEnable( int nId, bool bOp) = 0;
    virtual void ToolBoxPenSizeSel( int nId ) = 0;
    virtual void ToolBoxColorSel( ColorDef& color ) = 0;
    virtual void ToolBoxFontSel( FontDef& font ) = 0;

    virtual bool MenuBarNewExec( bool bChanged, bool bRepaint ) = 0;
    virtual void MenuBarSaveExec( ) = 0;
    virtual void MenuBarPrintExec( ) = 0;
    virtual void MenuBarPageSetup( ) = 0;
    virtual void MenuBarCutExec( ) = 0;
    virtual void MenuBarUndoExec( ) = 0;
    virtual void MenuBarImageStopExec( ) = 0;
    virtual void MenuBarImageStopEnable( bool bOp ) = 0;

    virtual void EdtSelArea( PtPos& pt1, PtPos& pt2 ) = 0;
    virtual void EdtSelRect( PtPos& pt1, PtPos& pt2 ) = 0;
    virtual void EdtSelEllipse( PtPos& pt1, PtPos& pt2 ) = 0;
    virtual void EdtSelLine( PtPos& pt1, PtPos& pt2 ) = 0;

    virtual void EdtDrawArea( PtPos& pt1, PtPos& pt2 ) = 0;
    virtual void EdtKillArea( ) = 0;
    virtual void EdtRepaint( ) = 0;

    virtual void ScrScrollWindow( int dx, int dy ) = 0;
    virtual void ScrSetScrollPos( int posx, int posy ) = 0;

    virtual void CtlDrawRect( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags ) = 0;
    virtual void CtlDrawEllipse( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags ) = 0;
    virtual void CtlDrawLine( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bMask, WB_BYTE flags ) = 0;
    virtual void CtlDrawIndicator( PtPos& pt, WB_BYTE flags ) = 0;
    virtual void CtlDrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2, WB_BYTE flags ) = 0;
    virtual void CtlDrawTxt( WB_PCSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color, WB_BYTE flags ) = 0;
    virtual void CtlEditTxt( PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color ) = 0;
    virtual void CtlGetTxt( WB_PSTR szText ) = 0;
};


class IWBProcess
{
public:
    virtual void OnWindowCreate( IWBInterface* pInterface ) = 0;
    virtual void OnWindowClose( ) = 0;
    virtual void OnWindowDestroy( ) = 0;

    virtual void OnToolBoxItemClick( int nId ) = 0;
    virtual void OnToolBoxPenSizeClick( int nId ) = 0;
    virtual void OnToolBoxColorClick( ColorDef& color ) = 0;
    virtual void OnToolBoxFontClick( FontDef& font ) = 0;
    virtual void OnToolBoxImageSel( WB_PCSTR szFile ) = 0;

    virtual void OnMenuBarNewClick( ) = 0;
    virtual void OnMenuBarSaveClick( ) = 0;
    virtual void OnMenuBarPrintClick( ) = 0;
    virtual void OnMenuBarPageSetupClick( ) = 0;
    virtual void OnMenuBarCutClick( ) = 0;
    virtual void OnMenuBarUndoClick( ) = 0;
    virtual void OnMenuBarImageStopClick( ) = 0;

    virtual void OnMouseLButtonDown( MouseFlag flag, PtPos& point ) = 0;
    virtual void OnMouseLButtonUp( MouseFlag flag, PtPos& point ) = 0;
    virtual void OnMouseMove( MouseFlag flag, PtPos& point ) = 0;

    virtual void OnEditRedraw( ) = 0;
    virtual void OnEditCutArea( PtPos& pt1, PtPos& pt2 ) = 0;

    virtual void OnScreenScrollWindow( int dx, int dy ) = 0;
    virtual void OnScreenSetScrollPos( int posx, int posy ) = 0;
};


class WBInterfaceRef
{
public:

    void SetInterfacePtr(IWBInterface* pInterface) { pINotify = pInterface; }
    IWBInterface* GetInterfacePtr() { return pINotify; }

private:

    IWBInterface* pINotify;
};


class WBProcessRef
{
public:

    void SetProcessPtr(IWBProcess* pProcess) { pINotify = pProcess; }
    IWBProcess* GetProcessPtr() { return pINotify; }

private:

    IWBProcess* pINotify;
};


#endif //_WBOARDBRIDGE_H
