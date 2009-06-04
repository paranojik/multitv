// ----------------------------------------------------------------------------
// wboardproc.cpp
// ----------------------------------------------------------------------------
 
#include "shared/compat.h"
#include "shared/mem_x.h"
#include "shared/file_x.h"

#include "Resource.h"
#include "WBoardProc.h"


// ----------------------------------------------------------------------------
// CWBProcess Class
// ----------------------------------------------------------------------------

CWBProcess::CWBProcess (IWBoardNotify* pNotify) : m_bCreateWnd( false ),
                                                  m_bCtrlTxt  ( false ),
                                                  m_bLeftDown ( false ),
                                                  m_bChanged  ( false )
{
    m_pIWBNotify = pNotify;

    SetInterfacePtr(NULL);

    m_CurIdent = ID_WB_POS;
    m_MousePos.SetAttr(0, 0);
    m_MouseIni.SetAttr(0, 0);

    m_CurLine = 1;
    m_DelColor.SetAttr(255, 255, 255);
    m_MrkColor.SetAttr(255, 255, 128);
    m_CurColor.SetAttr(0, 0, 0);
    m_CurFont.Clear();

    m_pControls = new ListT<CtrlAttr> (true);
}

CWBProcess::~CWBProcess (void)
{
    if (m_pControls)
    {
        ResetControls();
        delete m_pControls;
    }
}

WB_PCSTR CWBProcess::GetVersion (void)
{
   return NULL;
}

CtrlAttr * CWBProcess::GetCtrlAttr (CtrlAttr *lpAttr)
{
    CtrlAttr *lpCtrl;
    LPSTR     lpStr;

    lpCtrl = new CtrlAttr ();

    if (lpAttr)
    {
      lpCtrl->ctName  = lpAttr->ctName;
      lpCtrl->ctColor = lpAttr->ctColor;
      lpCtrl->ctLine  = lpAttr->ctLine;
      mov_mem (&lpAttr->ctPos, &lpCtrl->ctPos, sizeof(RcPos));
      mov_mem (&lpAttr->ctFont, &lpCtrl->ctFont, sizeof(FontDef));
      switch (lpCtrl->ctName)
      {
         case WB_TXT:
            lpStr = lpAttr->ctText.tdText;
            if (lpStr)
               lpCtrl->ctText.CopyTxt (lpStr, lpAttr->ctText.tdLen);
            break;
         case WB_IMG:
            lpStr = lpAttr->ctImage.idName;
            if (FileExist(lpStr))
               lpCtrl->ctImage.SetName (lpStr);
            break;
         default:{} // this line avoids switch warnings in gcc.
      }
    }

    return lpCtrl;
}

IWBProcess* CWBProcess::GetProcessPtr()
{
    return (IWBProcess*) this;
}

void CWBProcess::OnWindowCreate( IWBInterface* pInterface )
{
    m_bCreateWnd = true;

    SetInterfacePtr(pInterface);
}

void CWBProcess::OnWindowClose( )
{
    if (m_pIWBNotify)
        m_pIWBNotify->OnWhiteBoardMessage(WBN_CLOSEWND, NULL);
}

void CWBProcess::OnWindowDestroy( )
{
    m_bCreateWnd = false;
}

void CWBProcess::OnToolBoxItemClick( int nId )
{
    if (m_bCtrlTxt)
    {
        m_bCtrlTxt = false;

        if ( GetInterfacePtr() )
        {
            GetInterfacePtr()->CtlGetTxt(m_CtrlAttr.ctText.tdText);
            _DrawText(m_CtrlAttr.ctText.tdText, m_MouseIni, m_MousePos, m_CurFont, m_CurColor);
        }
    }

    m_CurIdent = nId;

    if ( GetInterfacePtr() )
        GetInterfacePtr()->ToolBoxItemSel( nId );
}

void CWBProcess::OnToolBoxPenSizeClick( int nId )
{
    switch (nId)
    {
        case ID_WB_LINE1:
            m_CurLine = 1;
            break;
        case ID_WB_LINE2:
            m_CurLine = 3;
            break;
        case ID_WB_LINE3:
            m_CurLine = 5;
            break;
        case ID_WB_LINE4:
            m_CurLine = 7;
            break;
        default:{} // this line avoids switch warnings in gcc.
    }

    if ( GetInterfacePtr() )
        GetInterfacePtr()->ToolBoxPenSizeSel( nId );
}

void CWBProcess::OnToolBoxColorClick( ColorDef& color )
{
    m_CurColor.SetAttr(color.cdRed, color.cdGreen, color.cdBlue);

    if ( GetInterfacePtr() )
        GetInterfacePtr()->ToolBoxColorSel( color );
}

void CWBProcess::OnToolBoxFontClick( FontDef& font )
{
    m_CurFont.SetAttr(font.fdName, font.fdSize, font.fdEffect);

    if ( GetInterfacePtr() )
        GetInterfacePtr()->ToolBoxFontSel( font );
}

void CWBProcess::OnToolBoxImageSel( WB_PCSTR szFile )
{
    m_CtrlAttr.Clear();
    m_CtrlAttr.ctName = WB_IMG;
    m_CtrlAttr.ctImage.SetName(szFile);

    if (m_pIWBNotify)
        m_pIWBNotify->OnWhiteBoardMessage(WBN_IMGTRANSFER, (WB_HMSG)szFile);
}

void CWBProcess::OnMenuBarNewClick( )
{
    if ( GetInterfacePtr() )
    {
        if (GetInterfacePtr()->MenuBarNewExec(m_bChanged, true))
        {
            ResetControls();
            m_bChanged = false;

            if (m_pIWBNotify)
                m_pIWBNotify->OnWhiteBoardMessage(WBN_NEWEXEC, NULL);
        }
    }
}

void CWBProcess::OnMenuBarSaveClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarSaveExec( );
}

void CWBProcess::OnMenuBarPrintClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarPrintExec( );
}

void CWBProcess::OnMenuBarPageSetupClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarPageSetup( );
}

void CWBProcess::OnMenuBarCutClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarCutExec( );
}

void CWBProcess::OnMenuBarUndoClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarUndoExec();

    if ( _EdtUndo() )
    {
        if (m_pIWBNotify)
            m_pIWBNotify->OnWhiteBoardMessage(WBN_UNDOEXEC, NULL);
    }
}

void CWBProcess::OnMenuBarImageStopClick( )
{
    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarImageStopExec();

    if (m_pIWBNotify)
        m_pIWBNotify->OnWhiteBoardMessage(WBN_IMGSTOP, NULL);
}

void CWBProcess::OnMouseLButtonDown( MouseFlag flag, PtPos& point )
{
    m_bLeftDown = true;

    if (GetInterfacePtr())
    {
        RcPos rcMouse;
        PtPos fp;

        rcMouse.SetAttr(point.X, point.Y, point.X, point.Y);

        switch (m_CurIdent)
        {
            case ID_WB_POS:
                GetInterfacePtr()->EdtKillArea();
                break;

            case ID_WB_TXT:
                if (m_bCtrlTxt)
                {
                    m_bCtrlTxt = false;

                    if ( GetInterfacePtr() )
                    {
                        GetInterfacePtr()->CtlGetTxt(m_CtrlAttr.ctText.tdText);
                        _DrawText(m_CtrlAttr.ctText.tdText, m_MouseIni, m_MousePos, m_CurFont, m_CurColor);
                    }
                }
               break;

            case ID_WB_DEL:
                m_CtrlAttr.Clear();
                m_CtrlAttr.Init (WB_DEL, 0, m_CurLine+DIF_PEN, rcMouse);
                m_CtrlAttr.ctColor.SetAttr(m_DelColor.cdRed, m_DelColor.cdGreen, m_DelColor.cdBlue);
                fp.SetAttr(point.X, point.Y);
                m_CtrlAttr.ctPoints.InsPoint(fp);
                m_CtrlAttr.ctPos.SetAttr(point.X, point.Y, point.X, point.Y);
                break;

            case ID_WB_PEN:
                m_CtrlAttr.Clear();
                m_CtrlAttr.Init (WB_PEN, 0, m_CurLine, rcMouse);
                m_CtrlAttr.ctColor.SetAttr(m_CurColor.cdRed, m_CurColor.cdGreen, m_CurColor.cdBlue);
                fp.SetAttr(point.X, point.Y);
                m_CtrlAttr.ctPoints.InsPoint(fp);
                m_CtrlAttr.ctPos.SetAttr(point.X, point.Y, point.X, point.Y);
                break;

            case ID_WB_MRK:
                m_CtrlAttr.Clear();
                m_CtrlAttr.Init (WB_MRK, 0, m_CurLine+DIF_PEN, rcMouse);
                m_CtrlAttr.ctColor.SetAttr(m_MrkColor.cdRed, m_MrkColor.cdGreen, m_MrkColor.cdBlue);
                fp.SetAttr(point.X, point.Y);
                m_CtrlAttr.ctPoints.InsPoint(fp);
                m_CtrlAttr.ctPos.SetAttr(point.X, point.Y, point.X, point.Y);
                break;

            default:{} // this line avoids switch warnings in gcc.
        }

        m_MouseIni.SetAttr(point.X, point.Y);
        m_MousePos.SetAttr(point.X, point.Y);
    }
}

void CWBProcess::OnMouseLButtonUp( MouseFlag flag, PtPos& point )
{
    if (m_bLeftDown && GetInterfacePtr())
    {
        switch (m_CurIdent)
        {
            case ID_WB_POS:
                GetInterfacePtr()->EdtDrawArea(m_MouseIni, point);
                break;
            case ID_WB_PTR:
                _DrawIndicator(point);
                break;
            case ID_WB_IMG:
                _DrawImage(m_CtrlAttr.ctImage.idName, point, point);
                // Para selecionar o controle POS apos desenhar a imagem
                GetInterfacePtr()->ToolBoxItemSel(ID_WB_POS);
                break;
            case ID_WB_TXT:
                m_CtrlAttr.Clear();
                m_CtrlAttr.ctName = WB_TXT;
                m_bCtrlTxt = true;
                GetInterfacePtr()->CtlEditTxt(m_MouseIni, point, m_CurFont, m_CurColor);
                break;
            case ID_WB_LIN:
                _DrawLine(m_MouseIni, point, m_CurLine, m_CurColor);
                break;
            case ID_WB_SQR:
                _DrawRect(m_MouseIni, point, m_CurLine, m_CurColor, false);
                break;
            case ID_WB_SQF:
                _DrawRect(m_MouseIni, point, m_CurLine, m_CurColor, true);
                break;
            case ID_WB_CRC:
                _DrawEllipse(m_MouseIni, point, m_CurLine, m_CurColor, false);
                break;
            case ID_WB_CRF:
                _DrawEllipse(m_MouseIni, point, m_CurLine, m_CurColor, true);
                break;
            case ID_WB_DEL:
                _DrawPoint(NULL, m_CurLine, m_DelColor, false);
                break;
            case ID_WB_PEN:
                _DrawPoint(NULL, m_CurLine, m_CurColor, false);
                break;
            case ID_WB_MRK:
                _DrawPoint(NULL, m_CurLine, m_MrkColor, true);
                break;
        }

        m_MousePos.SetAttr(point.X, point.Y);
    }

    m_bLeftDown = false;
}

void CWBProcess::OnMouseMove( MouseFlag flag, PtPos& point )
{
    if (flag == WB_KEY_LBUTTON)
    {
        if (GetInterfacePtr())
        {
            switch (m_CurIdent)
            {
                case ID_WB_POS:
                    GetInterfacePtr()->EdtSelArea(m_MouseIni, m_MousePos);
                    break;
                case ID_WB_SQR:
                case ID_WB_SQF:
                case ID_WB_TXT:
                    GetInterfacePtr()->EdtSelRect(m_MouseIni, m_MousePos);
                    break;
                case ID_WB_DEL:
                    _DrawPoint(&point, m_CurLine+DIF_PEN, m_DelColor, false);
                    break;
                case ID_WB_PEN:
                    _DrawPoint(&point, m_CurLine, m_CurColor, false);
                    break;
                case ID_WB_MRK:
                    _DrawPoint(&point, m_CurLine+DIF_PEN, m_MrkColor, true);
                    break;
                case ID_WB_CRC:
                case ID_WB_CRF:
                    GetInterfacePtr()->EdtSelEllipse(m_MouseIni, m_MousePos);
                    break;
                case ID_WB_LIN:
                    GetInterfacePtr()->EdtSelLine(m_MouseIni, m_MousePos);
                    break;
            }

            m_MousePos.SetAttr(point.X, point.Y);
        }
    }
}

void CWBProcess::OnEditRedraw( )
{
    if (m_pControls)
    {
        CtrlAttr* attr;
        CtrlAttr* last;

        if (GetInterfacePtr())
            GetInterfacePtr()->MenuBarNewExec(false, false);

        last = m_pControls->last();
        attr = m_pControls->first();

        while (attr)
        {
            if (attr == last)
                DrawControl(attr, DC_AUTOSCROLL);
            else
                DrawControl(attr, 0);

            attr = m_pControls->next();
        }

        if (GetInterfacePtr())
            GetInterfacePtr()->EdtRepaint();
    }
}

void CWBProcess::OnEditCutArea( PtPos& pt1, PtPos& pt2 )
{
    CtrlAttr *attr;

    attr = new CtrlAttr ();

    if (attr)
    {
        attr->ctName = WB_CUT;
        attr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);

        if (InsertControl(attr) && m_pIWBNotify)
            m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, attr);

        m_bChanged = true;
    }

    if (GetInterfacePtr())
    {
        GetInterfacePtr()->CtlDrawRect(pt1, pt2, 1, m_DelColor, true, DC_REPAINT|DC_AUTOSCROLL);
        GetInterfacePtr()->EdtKillArea();
    }
}

void CWBProcess::OnScreenScrollWindow( int dx, int dy )
{
    if (GetInterfacePtr())
        GetInterfacePtr()->ScrScrollWindow(dx, dy);
}

void CWBProcess::OnScreenSetScrollPos( int posx, int posy )
{
    if (GetInterfacePtr())
        GetInterfacePtr()->ScrSetScrollPos(posx, posy);
}

bool CWBProcess::AddElement(CtrlAttr* attr)
{
    bool ret = false;

    if (!GetInterfacePtr())
        return ret;

    if (attr && InsertControl(attr))
    {
        ret = true;
        DrawControl(attr, DC_REPAINT|DC_AUTOSCROLL);
    }

    return (ret);
}

bool CWBProcess::AddPoints(CtrlAttr* attr)
{
    bool ret = false;

    return ret;
}

void CWBProcess::NewExec()
{
    if (GetInterfacePtr())
    {
        if (GetInterfacePtr()->MenuBarNewExec(false, true))
        {
            ResetControls();
            m_bChanged = false;
        }
    }
}

void CWBProcess::UndoExec()
{
    _EdtUndo();
}

void CWBProcess::ResetControls()
{
    m_pControls->reset();
}

bool CWBProcess::InsertControl(CtrlAttr* attr)
{
    bool ret = false;

    if (m_pControls)
    {
        if (attr)
        {
            if (!m_pControls->search(attr) && m_pControls->append(attr))
            {
                ret = true;
            }
        }
    }

    return ret;
}

void CWBProcess::DrawControl(CtrlAttr* attr, WB_BYTE flags)
{
    if (attr)
    {
        PtPos pt1, pt2;
        int ind;

        switch (attr->ctName)
        {
            case WB_DEL:
            case WB_PEN:
                pt1.SetAttr(attr->ctPoints.ptPoint[0].X, attr->ctPoints.ptPoint[0].Y);
                for (ind = 1; ind < attr->ctPoints.ptNum; ind++)
                {
                    pt2.SetAttr(attr->ctPoints.ptPoint[ind].X, attr->ctPoints.ptPoint[ind].Y);
                    GetInterfacePtr()->CtlDrawLine(pt1, pt2, attr->ctLine, attr->ctColor, false, flags);
                    pt1.SetAttr(pt2.X, pt2.Y);
                }
                break;
            case WB_TXT:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawTxt(attr->ctText.tdText, pt1, pt2, attr->ctFont, attr->ctColor, flags);
                break;
            case WB_MRK:
                pt1.SetAttr(attr->ctPoints.ptPoint[0].X, attr->ctPoints.ptPoint[0].Y);
                for (ind = 1; ind < attr->ctPoints.ptNum; ind++)
                {
                    pt2.SetAttr(attr->ctPoints.ptPoint[ind].X, attr->ctPoints.ptPoint[ind].Y);
                    GetInterfacePtr()->CtlDrawLine(pt1, pt2, attr->ctLine, attr->ctColor, true, flags);
                    pt1.SetAttr(pt2.X, pt2.Y);
                }
                break;
            case WB_SET:
                pt1.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawIndicator(pt1, flags);
                break;
            case WB_IMG:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawImage(attr->ctImage.idName, pt1, pt2, flags);
                break;
            case WB_LIN:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawLine(pt1, pt2, attr->ctLine, attr->ctColor, false, flags);
                break;
            case WB_RCE:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawRect(pt1, pt2, attr->ctLine, attr->ctColor, false, flags);
                break;
            case WB_RCF:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawRect(pt1, pt2, attr->ctLine, attr->ctColor, true, flags);
                break;
            case WB_CRE:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawEllipse(pt1, pt2, attr->ctLine, attr->ctColor, false, flags);
                break;
            case WB_CRF:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawEllipse(pt1, pt2, attr->ctLine, attr->ctColor, true, flags);
                break;
            case WB_CUT:
                pt1.SetAttr(attr->ctPos.Left, attr->ctPos.Top);
                pt2.SetAttr(attr->ctPos.Right, attr->ctPos.Bottom);
                GetInterfacePtr()->CtlDrawRect(pt1, pt2, 1, m_DelColor, true, flags);
                break;
            default:{} // this line avoids switch warnings in gcc.
        }

        m_bChanged = true;
    }
}

void CWBProcess::_DrawPoint(PtPos* pPoint, UINT nLin, ColorDef& color, bool bMask)
{
    CtrlAttr *lpAttr;
    PtPos fp;

    if (pPoint == NULL || m_CtrlAttr.ctPoints.CanTrans())
    {
        lpAttr = new CtrlAttr();
        if (lpAttr)
        {
            mov_mem (&m_CtrlAttr, lpAttr, sizeof(CtrlAttr));
            if (InsertControl(lpAttr) && m_pIWBNotify)
                m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
        }

        if (pPoint == NULL)
        {
            m_CtrlAttr.Clear();
        }
        else
        {
            m_CtrlAttr.ctPoints.Clear();
            fp.SetAttr(m_MousePos.X, m_MousePos.Y);
            m_CtrlAttr.ctPoints.InsPoint(fp);
            m_CtrlAttr.ctPos.SetAttr(fp.X, fp.Y, fp.X, fp.Y);
        }
    }

    if (pPoint)
    {
        if (GetInterfacePtr())
            GetInterfacePtr()->CtlDrawLine(m_MousePos, *pPoint, nLin, color, bMask, DC_AUTOSCROLL|DC_REPAINT);

        m_MousePos.SetAttr(pPoint->X, pPoint->Y);

        m_CtrlAttr.ctPoints.InsPoint(*pPoint);
        m_CtrlAttr.ctPos.Right  = pPoint->X;
        m_CtrlAttr.ctPos.Bottom = pPoint->Y;
    }

    m_bChanged = true;
}

void CWBProcess::_DrawLine(PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color)
{
    CtrlAttr *lpAttr;

    lpAttr = new CtrlAttr();
    if (lpAttr)
    {
        lpAttr->ctName = WB_LIN;
        lpAttr->ctColor.SetAttr(color.cdRed, color.cdGreen, color.cdBlue);
        lpAttr->ctLine = nLin;
        lpAttr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        if (InsertControl(lpAttr) && m_pIWBNotify)
        {
            m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
        }
    }

    if (GetInterfacePtr())
        GetInterfacePtr()->CtlDrawLine(pt1, pt2, nLin, color, false, DC_AUTOSCROLL|DC_REPAINT);

    m_bChanged = true;
}

void CWBProcess::_DrawRect(PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color, bool bFull)
{
    CtrlAttr *lpAttr;

    lpAttr = new CtrlAttr();
    if (lpAttr)
    {
        if (bFull)
            lpAttr->ctName = WB_RCF;
        else
            lpAttr->ctName = WB_RCE;

        lpAttr->ctColor.SetAttr(color.cdRed, color.cdGreen, color.cdBlue);
        lpAttr->ctLine = nLin;
        lpAttr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        if (InsertControl(lpAttr) && m_pIWBNotify)
        {
            m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
        }
    }

    if (GetInterfacePtr())
        GetInterfacePtr()->CtlDrawRect(pt1, pt2, nLin, color, bFull, DC_AUTOSCROLL|DC_REPAINT);

    m_bChanged = true;
}

void CWBProcess::_DrawEllipse(PtPos& pt1, PtPos& pt2, UINT nLin, ColorDef& color, bool bFull)
{
    CtrlAttr *lpAttr;

    lpAttr = new CtrlAttr();
    if (lpAttr)
    {
        if (bFull)
            lpAttr->ctName = WB_CRF;
        else
            lpAttr->ctName = WB_CRE;

        lpAttr->ctColor.SetAttr(color.cdRed, color.cdGreen, color.cdBlue);
        lpAttr->ctLine = nLin;
        lpAttr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        if (InsertControl(lpAttr) && m_pIWBNotify)
        {
            m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
        }
    }

    if (GetInterfacePtr())
        GetInterfacePtr()->CtlDrawEllipse(pt1, pt2, nLin, color, bFull, DC_AUTOSCROLL|DC_REPAINT);

    m_bChanged = true;
}

void CWBProcess::_DrawIndicator(PtPos& pt)
{
    CtrlAttr *lpAttr;
    PtPos point;

    // Para manter a compatibilidade com a versão anterior
    point.SetAttr(pt.X + PTR_WIDTH, pt.Y + PTR_HEIGHT);

    lpAttr = new CtrlAttr();
    if (lpAttr)
    {
        lpAttr->ctName = WB_SET;
        lpAttr->ctPos.SetAttr(pt.X, pt.Y, point.X, point.Y);
        if (InsertControl(lpAttr) && m_pIWBNotify)
        {
            m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
        }
    }

    if (GetInterfacePtr())
        GetInterfacePtr()->CtlDrawIndicator(point, DC_AUTOSCROLL|DC_REPAINT);

    m_bChanged = true;
}

void CWBProcess::_DrawImage(WB_PCSTR szFile, PtPos& pt1, PtPos& pt2)
{
    CtrlAttr *lpAttr;

    if (FileExist(szFile))
    {
        lpAttr = new CtrlAttr();
        if (lpAttr)
        {
            lpAttr->ctName = WB_IMG;
            lpAttr->ctImage.SetName(szFile);
            lpAttr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
            if (InsertControl(lpAttr) && m_pIWBNotify)
            {
                m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
            }
        }

        if (GetInterfacePtr())
            GetInterfacePtr()->CtlDrawImage(szFile, pt1, pt2, DC_AUTOSCROLL|DC_REPAINT);

        m_bChanged = true;
    }
}

void CWBProcess::_DrawText(WB_PSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color)
{
    CtrlAttr *lpAttr;

    if (szText && !comp_branco(szText))
    {
        lpAttr = new CtrlAttr ();
        if (lpAttr)
        {
            lpAttr->ctName = WB_TXT;
            lpAttr->ctColor.SetAttr(color.cdRed, color.cdGreen, color.cdBlue);
            lpAttr->ctFont.SetAttr(font.fdName, font.fdSize, font.fdEffect);
            lpAttr->ctText.CopyTxt(szText, (int)strlen(szText));
            lpAttr->ctPos.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
            if (InsertControl(lpAttr) && m_pIWBNotify)
            {
                m_pIWBNotify->OnWhiteBoardMessage(WBN_ADDELEMENT, lpAttr);
            }
        }
    }

    if (GetInterfacePtr())
        GetInterfacePtr()->CtlDrawTxt(szText, pt1, pt2, font, color, DC_AUTOSCROLL|DC_REPAINT);

    m_bChanged = true;
}

bool CWBProcess::_EdtUndo( )
{
    bool ret = false;

    if ( GetInterfacePtr() )
        GetInterfacePtr()->MenuBarUndoExec( );

    if (m_pControls)
    {
        long itens = m_pControls->get_nitens();

        if (itens)
        {
            CtrlAttr* attr;

            attr = m_pControls->last();

            if (attr)
            {
                m_pControls->eraseCur();

                OnEditRedraw();

                ret = true;

                itens--;
            }

            if (!itens)
                m_bChanged = false;
        }
    }

    return ret;
}
