// ----------------------------------------------------------------------------
// wboardwnd.cpp
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
 
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "WBoardFrame.h"
#include "WBoardWnd.h"


// ----------------------------------------------------------------------------
// CWBInterface Class
// ----------------------------------------------------------------------------

CWBInterface::CWBInterface ( WB_PWND pParent, IWBProcess* pProcess, int nId, RcPos* pRc )
{
    wxWindow* pwnd = (wxWindow*) pParent;

    m_pWBFrame = new CWBFrame(pwnd, pProcess, nId, _T("White Board"));

    if ( m_pWBFrame )
    {
        // Note:
        // Guilherme Lello, 2008-11-10.
        // Sometimes we don't want the frame to be shown when we create it.
        //m_pWBFrame->Show(true);

        m_pWBFrame->EnableImgStop(false);

        if (GetInterfacePtr())
        {
            ColorDef clDef;
            FontDef fnDef;

            clDef.SetAttr(0, 0, 0);
            fnDef.Clear();

            GetInterfacePtr()->ToolBoxItemSel(ID_WB_POS);
            GetInterfacePtr()->ToolBoxColorSel(clDef);
            GetInterfacePtr()->ToolBoxFontSel(fnDef);
            GetInterfacePtr()->ToolBoxPenSizeSel(ID_WB_LINE1);
        }
    }
}

CWBInterface::~CWBInterface ( void )
{
    if (m_pWBFrame)
    {
        delete m_pWBFrame;
        m_pWBFrame = NULL;
    }
}

IWBInterface* CWBInterface::GetInterfacePtr()
{
    IWBInterface* ret = NULL;

    if (m_pWBFrame)
        ret = m_pWBFrame->GetInterfacePtr();

    return ret;
}

bool CWBInterface::Resize (RcPos* pRc)
{
   bool ret = false;

   if (m_pWBFrame)
       ret = m_pWBFrame->Resize(pRc);

   return ret;
}

void CWBInterface::ShowWnd (bool bOp)
{
   if (m_pWBFrame)
   {
       m_pWBFrame->ShowWnd(bOp);
   }
}

void CWBInterface::EnableWnd (bool bOp)
{
   if (m_pWBFrame)
   {
       m_pWBFrame->EnableWnd(bOp);
   }
}

void CWBInterface::EnableImgCtrl( bool bOp )
{
   if (m_pWBFrame)
   {
       m_pWBFrame->EnableImgCtrl(bOp);
   }
}

void CWBInterface::EnableImgStop( bool bOp )
{
   if (m_pWBFrame)
   {
       m_pWBFrame->EnableImgStop(bOp);
   }
}
