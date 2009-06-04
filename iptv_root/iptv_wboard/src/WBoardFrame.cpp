// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/printdlg.h"
#include "wx/filename.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Images.h"
#include "WBoardFrame.h"

/*!
 * CWBFrame type definition
 */

IMPLEMENT_CLASS( CWBFrame, wxFrame )


/*!
 * CWBFrame event table definition
 */

BEGIN_EVENT_TABLE( CWBFrame, wxFrame )

////@begin CWBFrame event table entries
    EVT_CLOSE( CWBFrame::OnCloseWindow )
    EVT_MENU( IDM_WB_NEW, CWBFrame::OnMWbNewClick )
    EVT_MENU( IDM_WB_SAVE, CWBFrame::OnMWbSaveClick )
    EVT_MENU( IDM_WB_PRINT, CWBFrame::OnWbPrintClick )
    EVT_MENU( IDM_WB_PAGE_SETUP, CWBFrame::OnWbPageSetupClick )
    EVT_MENU( IDM_WB_CLOSE, CWBFrame::OnMWbCloseClick )
    EVT_MENU( IDM_WB_UNDO, CWBFrame::OnMWbUndoClick )
    EVT_MENU( IDM_WB_CUT, CWBFrame::OnMWbCutClick )
    EVT_MENU( IDM_WB_IMG_STOP, CWBFrame::OnMWbImgStopClick )
    EVT_MENU( IDM_WB_HELP_TOPICS, CWBFrame::OnMWbHelpTopicsClick )
    EVT_MENU( IDM_WB_ABOUT, CWBFrame::OnMWbAboutClick )
////@end CWBFrame event table entries

END_EVENT_TABLE()


/*!
 * CWBFrame constructors
 */

CWBFrame::CWBFrame()
{
    Init();
}

CWBFrame::CWBFrame( wxWindow* parent, IWBProcess* pProcess, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, pProcess, id, caption, pos, size, style );
}


/*!
 * WBoardFrame creator
 */

bool CWBFrame::Create( wxWindow* parent, IWBProcess* pProcess, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin CWBFrame creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (m_pwbMain && pProcess)
    {
        SetProcessPtr(pProcess);
        m_pwbMain->SetProcessObj(pProcess);
    }

    SetIcon(GetIconResource(wxT("../resource/wb_app_ico.xpm")));
    CentreOnScreen();
////@end CWBFrame creation
    return true;
}


/*!
 * CWBFrame destructor
 */

CWBFrame::~CWBFrame()
{
////@begin CWBFrame destruction
////@end CWBFrame destruction
}


/*!
 * Member initialisation
 */

void CWBFrame::Init()
{
////@begin CWBFrame member initialisation
    m_pwbMain = NULL;
////@end CWBFrame member initialisation
}


/*!
 * Control creation for WBoardFrame
 */

void CWBFrame::CreateControls()
{    
////@begin CWBFrame content construction
    CWBFrame* itemFrame1 = this;

#if wxUSE_MENUS
    m_pMenuBar = new wxMenuBar;
    m_pMenuFile = new wxMenu;
    m_pMenuFile->Append(IDM_WB_NEW, _("New"), _T(""), wxITEM_NORMAL);
    m_pMenuFile->Append(IDM_WB_SAVE, _("Save"), _T(""), wxITEM_NORMAL);
    m_pMenuFile->Append(IDM_WB_PRINT, _("Print"), _T(""), wxITEM_NORMAL);
    m_pMenuFile->Append(IDM_WB_PAGE_SETUP, _("Page setup"), _T(""), wxITEM_NORMAL);
    m_pMenuFile->AppendSeparator();
    m_pMenuFile->Append(IDM_WB_CLOSE, _("Close"), _T(""), wxITEM_NORMAL);
    m_pMenuBar->Append(m_pMenuFile, _("File"));

    m_pMenuEdit = new wxMenu;
    m_pMenuEdit->Append(IDM_WB_UNDO, _("Undo"), _T(""), wxITEM_NORMAL);
    m_pMenuEdit->AppendSeparator();
    m_pMenuEdit->Append(IDM_WB_CUT, _("Cut"), _T(""), wxITEM_NORMAL);
    m_pMenuBar->Append(m_pMenuEdit, _("Edit"));

    m_pMenuCtrl = new wxMenu;
    m_pMenuCtrl->Append(IDM_WB_IMG_STOP, _("Stop image send"), _T(""), wxITEM_NORMAL);
    m_pMenuBar->Append(m_pMenuCtrl, _("Control"));

    m_pMenuHelp = new wxMenu;
    m_pMenuHelp->Append(IDM_WB_HELP_TOPICS, _("Help Topics"), _T(""), wxITEM_NORMAL);
    m_pMenuHelp->AppendSeparator();
    m_pMenuHelp->Append(IDM_WB_ABOUT, _("About"), _T(""), wxITEM_NORMAL);
    m_pMenuBar->Append(m_pMenuHelp, _("Help"));

    itemFrame1->SetMenuBar(m_pMenuBar);
#endif // wxUSE_MENUS

    m_pwbMain = new CWBMain( itemFrame1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL );
    m_pwbMain->SetBackgroundColour(wxColour(192, 192, 192));

////@end CWBFrame content construction
}


/*!
 * Should we show tooltips?
 */

bool CWBFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap CWBFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end CWBFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBFrame icon retrieval
    if (name == _T("../resource/wb_app_ico.xpm"))
    {
        wxIcon icon(wb_app_ico);
        return icon;
    }
    return wxNullIcon;
////@end CWBFrame icon retrieval
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_NEW
 */

void CWBFrame::OnMWbNewClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarNewClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_SAVE
 */

void CWBFrame::OnMWbSaveClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarSaveClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_PRINT
 */

void CWBFrame::OnWbPrintClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarPrintClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_WB_PAGE_SETUP
 */

void CWBFrame::OnWbPageSetupClick(wxCommandEvent& event)
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarPageSetupClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_CLOSE
 */

void CWBFrame::OnMWbCloseClick( wxCommandEvent& event )
{
    Close();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_UNDO
 */

void CWBFrame::OnMWbUndoClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarUndoClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_CUT
 */

void CWBFrame::OnMWbCutClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarCutClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_IMG_STOP
 */

void CWBFrame::OnMWbImgStopClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarImageStopClick();
    else
        event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_HELP_TOPICS
 */

void CWBFrame::OnMWbHelpTopicsClick( wxCommandEvent& event )
{
    event.Skip();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_ABOUT
 */

void CWBFrame::OnMWbAboutClick( wxCommandEvent& event )
{
    wxMessageBox(wxString::Format(
                    _T("%s %d.%d.%d Release %d\n")
                    _T("\n")
                    _T("Copyright(C) 2008 VAT Tecnologia da Informacao S/A\n"),
                    _("Version"),
                    3, 11, 7, 20
                 ),
                 _("About White Board"),
                 wxOK | wxICON_INFORMATION,
                 this);
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_WBOARDFRAME
 */

void CWBFrame::OnCloseWindow( wxCloseEvent& event )
{
    if (event.CanVeto())
    {
        if (GetProcessPtr())
            GetProcessPtr()->OnWindowClose();

        event.Veto(true);

        return;
    }

    event.Skip();
}


IWBInterface* CWBFrame::GetInterfacePtr()
{
    IWBInterface* ret = NULL;

    if (m_pwbMain)
        ret = m_pwbMain->GetInterfacePtr();

    return ret;
}

bool CWBFrame::Resize (RcPos* pRc)
{
   bool ret;
   //RcPos rc;
   //int  width, height;

   ret = false;

   /*
   if (m_pwbFrame)
   {
      GetClientRect (lpWBWin->GetParent(), &rc);
      width  = rc.right - rc.left;
      height = rc.bottom - rc.top;
      lpWBWin->SetDimension (width, height);

      // Não utilizo mais esta função por causa do botão de full screen
      // lpWBWin->BringToTop ();
   }
   */

   return (ret);
}

void CWBFrame::ShowWnd (bool bOp)
{
    Show(bOp);

    if (bOp)
        Raise();
}

void CWBFrame::EnableWnd (bool bOp)
{
    if (m_pMenuFile)
        m_pMenuFile->Enable(IDM_WB_NEW, bOp);

    if (m_pMenuEdit)
    {
        m_pMenuEdit->Enable(IDM_WB_UNDO, bOp);
        m_pMenuEdit->Enable(IDM_WB_CUT, bOp);
    }

    if (m_pMenuCtrl)
        m_pMenuCtrl->Enable(IDM_WB_IMG_STOP, bOp);

    if (m_pwbMain)
        m_pwbMain->Enable(bOp);
}

void CWBFrame::EnableImgCtrl( bool bOp )
{
    if (m_pwbMain)
        m_pwbMain->ToolBoxItemEnable(ID_WB_IMG, bOp);
}

void CWBFrame::EnableImgStop( bool bOp )
{
    if (m_pMenuCtrl)
        m_pMenuCtrl->Enable(IDM_WB_IMG_STOP, bOp);

    if (m_pwbMain)
        m_pwbMain->MenuBarImageStopEnable(bOp);
}


/*!
 * CWBMenuBar type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CWBMenuBar, wxPanel )


/*!
 * CWBMenuBar event table definition
 */

BEGIN_EVENT_TABLE( CWBMenuBar, wxPanel )

////@begin CWBMenuBar event table entries
    EVT_BUTTON( ID_WB_NEW, CWBMenuBar::OnWbNewClick )
    EVT_BUTTON( ID_WB_SAVE, CWBMenuBar::OnWbSaveClick )
    EVT_BUTTON( ID_WB_PRINT, CWBMenuBar::OnWbPrintClick )
    EVT_BUTTON( ID_WB_CUT, CWBMenuBar::OnWbCutClick )
    EVT_BUTTON( ID_WB_UNDO, CWBMenuBar::OnWbUndoClick )
    EVT_BUTTON( ID_WB_FTPSTOP, CWBMenuBar::OnWbFtpstopClick )
////@end CWBMenuBar event table entries

END_EVENT_TABLE()


/*!
 * CWBMenuBar constructors
 */

CWBMenuBar::CWBMenuBar()
{
    Init();
}

CWBMenuBar::CWBMenuBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * CWBMenuBar creator
 */

bool CWBMenuBar::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin CWBMenuBar creation
    wxPanel::Create(parent, id, pos, size, style);
    CreateControls();
////@end CWBMenuBar creation
    return true;
}


/*!
 * CWBMenuBar destructor
 */

CWBMenuBar::~CWBMenuBar()
{
////@begin CWBMenuBar destruction
////@end CWBMenuBar destruction
}


/*!
 * Member initialisation
 */

void CWBMenuBar::Init()
{
////@begin CWBMenuBar member initialisation
    m_mbNew = NULL;
    m_mbSave = NULL;
    m_mbPrint = NULL;
    m_mbCut = NULL;
    m_mbUndo = NULL;
    m_mbImgStop = NULL;
////@end CWBMenuBar member initialisation
}


/*!
 * Control creation for CWBMenuBar
 */

void CWBMenuBar::CreateControls()
{    
////@begin CWBMenuBar content construction
    CWBMenuBar* itemPanel18 = this;

    this->SetBackgroundColour(wxColour(192, 192, 192));
    m_mbNew = new wxBitmapButton( itemPanel18, ID_WB_NEW, itemPanel18->GetBitmapResource(wxT("../resource/wb_new_n.xpm")), wxPoint(2, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbNewBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_new_p.xpm")));
    m_mbNew->SetBitmapSelected(m_mbNewBitmapSel);
    wxBitmap m_mbNewBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_new_h.xpm")));
    m_mbNew->SetBitmapHover(m_mbNewBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbNew->SetToolTip(_("New"));

    m_mbSave = new wxBitmapButton( itemPanel18, ID_WB_SAVE, itemPanel18->GetBitmapResource(wxT("../resource/wb_sav_n.xpm")), wxPoint(32, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbSaveBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_sav_p.xpm")));
    m_mbSave->SetBitmapSelected(m_mbSaveBitmapSel);
    wxBitmap m_mbSaveBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_sav_h.xpm")));
    m_mbSave->SetBitmapHover(m_mbSaveBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbSave->SetToolTip(_("Save"));

    m_mbPrint = new wxBitmapButton( itemPanel18, ID_WB_PRINT, itemPanel18->GetBitmapResource(wxT("../resource/wb_prt_n.xpm")), wxPoint(62, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbPrintBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_prt_p.xpm")));
    m_mbPrint->SetBitmapSelected(m_mbPrintBitmapSel);
    wxBitmap m_mbPrintBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_prt_h.xpm")));
    m_mbPrint->SetBitmapHover(m_mbPrintBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbPrint->SetToolTip(_("Print"));

    m_mbCut = new wxBitmapButton( itemPanel18, ID_WB_CUT, itemPanel18->GetBitmapResource(wxT("../resource/wb_cut_n.xpm")), wxPoint(92, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbCutBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_cut_p.xpm")));
    m_mbCut->SetBitmapSelected(m_mbCutBitmapSel);
    wxBitmap m_mbCutBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_cut_h.xpm")));
    m_mbCut->SetBitmapHover(m_mbCutBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbCut->SetToolTip(_("Cut"));

    m_mbUndo = new wxBitmapButton( itemPanel18, ID_WB_UNDO, itemPanel18->GetBitmapResource(wxT("../resource/wb_und_n.xpm")), wxPoint(122, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbUndoBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_und_p.xpm")));
    m_mbUndo->SetBitmapSelected(m_mbUndoBitmapSel);
    wxBitmap m_mbUndoBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_und_h.xpm")));
    m_mbUndo->SetBitmapHover(m_mbUndoBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbUndo->SetToolTip(_("Undo"));

    m_mbImgStop = new wxBitmapButton( itemPanel18, ID_WB_FTPSTOP, itemPanel18->GetBitmapResource(wxT("../resource/wb_ims_n.xpm")), wxPoint(152, 2), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_mbImgStopBitmapSel(itemPanel18->GetBitmapResource(wxT("../resource/wb_ims_p.xpm")));
    m_mbImgStop->SetBitmapSelected(m_mbImgStopBitmapSel);
    wxBitmap m_mbImgStopBitmapDisabled(itemPanel18->GetBitmapResource(wxT("../resource/wb_ims_x.xpm")));
    m_mbImgStop->SetBitmapDisabled(m_mbImgStopBitmapDisabled);
    wxBitmap m_mbImgStopBitmapHover(itemPanel18->GetBitmapResource(wxT("../resource/wb_ims_h.xpm")));
    m_mbImgStop->SetBitmapHover(m_mbImgStopBitmapHover);
    if (CWBMenuBar::ShowToolTips())
        m_mbImgStop->SetToolTip(_("Stop image send"));

////@end CWBMenuBar content construction
}


/*!
 * Should we show tooltips?
 */

bool CWBMenuBar::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap CWBMenuBar::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBMenuBar bitmap retrieval
    if (name == _T("../resource/wb_new_n.xpm"))
    {
        wxBitmap bitmap(wb_new_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_new_p.xpm"))
    {
        wxBitmap bitmap(wb_new_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_new_h.xpm"))
    {
        wxBitmap bitmap(wb_new_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sav_n.xpm"))
    {
        wxBitmap bitmap(wb_sav_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sav_p.xpm"))
    {
        wxBitmap bitmap(wb_sav_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sav_h.xpm"))
    {
        wxBitmap bitmap(wb_sav_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_prt_n.xpm"))
    {
        wxBitmap bitmap(wb_prt_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_prt_p.xpm"))
    {
        wxBitmap bitmap(wb_prt_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_prt_h.xpm"))
    {
        wxBitmap bitmap(wb_prt_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_cut_n.xpm"))
    {
        wxBitmap bitmap(wb_cut_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_cut_p.xpm"))
    {
        wxBitmap bitmap(wb_cut_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_cut_h.xpm"))
    {
        wxBitmap bitmap(wb_cut_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_und_n.xpm"))
    {
        wxBitmap bitmap(wb_und_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_und_p.xpm"))
    {
        wxBitmap bitmap(wb_und_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_und_h.xpm"))
    {
        wxBitmap bitmap(wb_und_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ims_n.xpm"))
    {
        wxBitmap bitmap(wb_ims_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ims_p.xpm"))
    {
        wxBitmap bitmap(wb_ims_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ims_x.xpm"))
    {
        wxBitmap bitmap(wb_ims_x);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ims_h.xpm"))
    {
        wxBitmap bitmap(wb_ims_h);
        return bitmap;
    }
    return wxNullBitmap;
////@end CWBMenuBar bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBMenuBar::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBMenuBar icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CWBMenuBar icon retrieval
}

void CWBMenuBar::ImgStopEnable( bool bOp)
{
    if (m_mbImgStop)
        m_mbImgStop->Enable(bOp);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_NEW
 */

void CWBMenuBar::OnWbNewClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarNewClick();

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SAVE
 */

void CWBMenuBar::OnWbSaveClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarSaveClick();

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PRINT
 */

void CWBMenuBar::OnWbPrintClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarPrintClick();

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CUT
 */

void CWBMenuBar::OnWbCutClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarCutClick();

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_UNDO
 */

void CWBMenuBar::OnWbUndoClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarUndoClick();

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_FTPSTOP
 */

void CWBMenuBar::OnWbFtpstopClick( wxCommandEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnMenuBarImageStopClick();

    event.Skip();
}


/*!
 * CWBToolBox type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CWBToolBox, wxPanel )


/*!
 * CWBToolBox event table definition
 */

BEGIN_EVENT_TABLE( CWBToolBox, wxPanel )

////@begin CWBToolBox event table entries
    EVT_BUTTON( ID_WB_POS, CWBToolBox::OnWbPosClick )
    EVT_BUTTON( ID_WB_DEL, CWBToolBox::OnWbDelClick )
    EVT_BUTTON( ID_WB_TXT, CWBToolBox::OnWbTxtClick )
    EVT_BUTTON( ID_WB_MRK, CWBToolBox::OnWbMrkClick )
    EVT_BUTTON( ID_WB_PTR, CWBToolBox::OnWbPtrClick )
    EVT_BUTTON( ID_WB_IMG, CWBToolBox::OnWbImgClick )
    EVT_BUTTON( ID_WB_PEN, CWBToolBox::OnWbPenClick )
    EVT_BUTTON( ID_WB_LIN, CWBToolBox::OnWbLinClick )
    EVT_BUTTON( ID_WB_SQR, CWBToolBox::OnWbSqrClick )
    EVT_BUTTON( ID_WB_SQF, CWBToolBox::OnWbSqfClick )
    EVT_BUTTON( ID_WB_CRC, CWBToolBox::OnWbCrcClick )
    EVT_BUTTON( ID_WB_CRF, CWBToolBox::OnWbCrfClick )
    EVT_BUTTON( ID_WB_LINE1, CWBToolBox::OnWbLine1Click )
    EVT_BUTTON( ID_WB_LINE2, CWBToolBox::OnWbLine2Click )
    EVT_BUTTON( ID_WB_LINE3, CWBToolBox::OnWbLine3Click )
    EVT_BUTTON( ID_WB_LINE4, CWBToolBox::OnWbLine4Click )
    EVT_BUTTON( ID_WB_COLOR_SEL, CWBToolBox::OnWbColorSelClick )
    EVT_BUTTON( ID_WB_FONT_SEL, CWBToolBox::OnWbFontSelClick )
////@end CWBToolBox event table entries

END_EVENT_TABLE()


/*!
 * CWBToolBox constructors
 */

CWBToolBox::CWBToolBox()
{
    Init();
}

CWBToolBox::CWBToolBox(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * CWBMenuBar creator
 */

bool CWBToolBox::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin CWBToolBox creation
    wxPanel::Create(parent, id, pos, size, style);
    CreateControls();
////@end CWBToolBox creation

    return true;
}


/*!
 * CWBToolBox destructor
 */

CWBToolBox::~CWBToolBox()
{
////@begin CWBToolBox destruction
////@end CWBToolBox destruction
}


/*!
 * Member initialisation
 */

void CWBToolBox::Init()
{
////@begin CWBToolBox member initialisation
    m_tbPos = NULL;
    m_tbDel = NULL;
    m_tbTxt = NULL;
    m_tbMRK = NULL;
    m_tbPtr = NULL;
    m_tbImg = NULL;
    m_tbPen = NULL;
    m_tbLin = NULL;
    m_tbSqr = NULL;
    m_tbSqf = NULL;
    m_tbCrc = NULL;
    m_tbCrf = NULL;
    m_tbPSize1 = NULL;
    m_tbPSize2 = NULL;
    m_tbPSize3 = NULL;
    m_tbPSize4 = NULL;
    m_tbColor = NULL;
    m_tbColorSel = NULL;
    m_tbFont = NULL;
    m_tbFontLen = NULL;
    m_tbFontSel = NULL;
    m_tbPSBox = NULL;
////@end CWBToolBox member initialisation

    m_CurItemId = -1;
    m_CurPenSize = -1;
}


/*!
 * Control creation for CWBMenuBar
 */

void CWBToolBox::CreateControls()
{    
////@begin CWBToolBox content construction
    CWBToolBox* itemPanel25 = this;

    this->SetBackgroundColour(wxColour(192, 192, 192));

    m_tbPosBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_pos_n.xpm"));
    m_tbPosBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_pos_s.xpm"));
    m_tbPos = new wxBitmapButton( itemPanel25, ID_WB_POS, m_tbPosBitmap, wxPoint(4, 0), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbPosBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_pos_p.xpm")));
    m_tbPos->SetBitmapSelected(m_tbPosBitmapPressed);
    wxBitmap m_tbPosBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_pos_h.xpm")));
    m_tbPos->SetBitmapHover(m_tbPosBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbPos->SetToolTip(_("Position"));

    m_tbDelBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_del_n.xpm"));
    m_tbDelBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_del_s.xpm"));
    m_tbDel = new wxBitmapButton( itemPanel25, ID_WB_DEL, m_tbDelBitmap, wxPoint(33, 0), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbDelBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_del_p.xpm")));
    m_tbDel->SetBitmapSelected(m_tbDelBitmapPressed);
    wxBitmap m_tbDelBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_del_h.xpm")));
    m_tbDel->SetBitmapHover(m_tbDelBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbDel->SetToolTip(_("Eraser"));

    m_tbTxtBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_txt_n.xpm"));
    m_tbTxtBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_txt_s.xpm"));
    m_tbTxt = new wxBitmapButton( itemPanel25, ID_WB_TXT, m_tbTxtBitmap, wxPoint(4, 25), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbTxtBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_txt_p.xpm")));
    m_tbTxt->SetBitmapSelected(m_tbTxtBitmapPressed);
    wxBitmap m_tbTxtBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_txt_h.xpm")));
    m_tbTxt->SetBitmapHover(m_tbTxtBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbTxt->SetToolTip(_("Text"));

    m_tbMRKBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_mrk_n.xpm"));
    m_tbMRKBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_mrk_s.xpm"));
    m_tbMRK = new wxBitmapButton( itemPanel25, ID_WB_MRK, m_tbMRKBitmap, wxPoint(33, 25), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbMRKBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_mrk_p.xpm")));
    m_tbMRK->SetBitmapSelected(m_tbMRKBitmapPressed);
    wxBitmap m_tbMRKBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_mrk_h.xpm")));
    m_tbMRK->SetBitmapHover(m_tbMRKBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbMRK->SetToolTip(_("Marker"));

    m_tbPtrBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_ptr_n.xpm"));
    m_tbPtrBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_ptr_s.xpm"));
    m_tbPtr = new wxBitmapButton( itemPanel25, ID_WB_PTR, m_tbPtrBitmap, wxPoint(4, 50), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbPtrBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_ptr_p.xpm")));
    m_tbPtr->SetBitmapSelected(m_tbPtrBitmapPressed);
    wxBitmap m_tbPtrBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_ptr_h.xpm")));
    m_tbPtr->SetBitmapHover(m_tbPtrBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbPtr->SetToolTip(_("Pointer"));

    m_tbImgBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_img_n.xpm"));
    m_tbImgBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_img_s.xpm"));
    m_tbImg = new wxBitmapButton( itemPanel25, ID_WB_IMG, m_tbImgBitmap, wxPoint(33, 50), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbImgBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_img_p.xpm")));
    m_tbImg->SetBitmapSelected(m_tbImgBitmapPressed);
    wxBitmap m_tbImgBitmapDisabled(itemPanel25->GetBitmapResource(wxT("../resource/wb_img_x.xpm")));
    m_tbImg->SetBitmapDisabled(m_tbImgBitmapDisabled);
    wxBitmap m_tbImgBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_img_h.xpm")));
    m_tbImg->SetBitmapHover(m_tbImgBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbImg->SetToolTip(_("Image"));

    m_tbPenBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_pen_n.xpm"));
    m_tbPenBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_pen_s.xpm"));
    m_tbPen = new wxBitmapButton( itemPanel25, ID_WB_PEN, m_tbPenBitmap, wxPoint(4, 75), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbPenBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_pen_p.xpm")));
    m_tbPen->SetBitmapSelected(m_tbPenBitmapPressed);
    wxBitmap m_tbPenBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_pen_h.xpm")));
    m_tbPen->SetBitmapHover(m_tbPenBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbPen->SetToolTip(_("Pen"));

    m_tbLinBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_lin_n.xpm"));
    m_tbLinBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_lin_s.xpm"));
    m_tbLin = new wxBitmapButton( itemPanel25, ID_WB_LIN, m_tbLinBitmap, wxPoint(33, 75), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbLinBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_lin_p.xpm")));
    m_tbLin->SetBitmapSelected(m_tbLinBitmapPressed);
    wxBitmap m_tbLinBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_lin_h.xpm")));
    m_tbLin->SetBitmapHover(m_tbLinBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbLin->SetToolTip(_("Line"));

    m_tbSqrBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_sqr_n.xpm"));
    m_tbSqrBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_sqr_s.xpm"));
    m_tbSqr = new wxBitmapButton( itemPanel25, ID_WB_SQR, m_tbSqrBitmap, wxPoint(4, 100), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbSqrBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_sqr_p.xpm")));
    m_tbSqr->SetBitmapSelected(m_tbSqrBitmapPressed);
    wxBitmap m_tbSqrBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_sqr_h.xpm")));
    m_tbSqr->SetBitmapHover(m_tbSqrBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbSqr->SetToolTip(_("Rectangle"));

    m_tbSqfBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_sqf_n.xpm"));
    m_tbSqfBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_sqf_s.xpm"));
    m_tbSqf = new wxBitmapButton( itemPanel25, ID_WB_SQF, m_tbSqfBitmap, wxPoint(33, 100), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbSqfBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_sqf_p.xpm")));
    m_tbSqf->SetBitmapSelected(m_tbSqfBitmapPressed);
    wxBitmap m_tbSqfBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_sqf_h.xpm")));
    m_tbSqf->SetBitmapHover(m_tbSqfBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbSqf->SetToolTip(_("Full Rectangle"));

    m_tbCrcBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_crc_n.xpm"));
    m_tbCrcBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_crc_s.xpm"));
    m_tbCrc = new wxBitmapButton( itemPanel25, ID_WB_CRC, m_tbCrcBitmap, wxPoint(4, 125), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbCrcBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_crc_p.xpm")));
    m_tbCrc->SetBitmapSelected(m_tbCrcBitmapPressed);
    wxBitmap m_tbCrcBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_crc_h.xpm")));
    m_tbCrc->SetBitmapHover(m_tbCrcBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbCrc->SetToolTip(_("Ellipse"));

    m_tbCrfBitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_crf_n.xpm"));
    m_tbCrfBitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_crf_s.xpm"));
    m_tbCrf = new wxBitmapButton( itemPanel25, ID_WB_CRF, m_tbCrfBitmap, wxPoint(33, 125), wxSize(29, 23), wxBU_AUTODRAW );
    wxBitmap m_tbCrfBitmapPressed(itemPanel25->GetBitmapResource(wxT("../resource/wb_crf_p.xpm")));
    m_tbCrf->SetBitmapSelected(m_tbCrfBitmapPressed);
    wxBitmap m_tbCrfBitmapHover(itemPanel25->GetBitmapResource(wxT("../resource/wb_crf_h.xpm")));
    m_tbCrf->SetBitmapHover(m_tbCrfBitmapHover);
    if (CWBToolBox::ShowToolTips())
        m_tbCrf->SetToolTip(_("Full Ellipse"));

    m_tbPSBox = new wxStaticBox( itemPanel25, wxID_STATIC, _T(""), wxPoint(2, 159), wxSize(57, 62), wxSIMPLE_BORDER );

    m_tbPSize1Bitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps1_n.xpm"));
    m_tbPSize1BitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps1_s.xpm"));
    m_tbPSize1 = new wxBitmapButton( itemPanel25, ID_WB_LINE1, m_tbPSize1Bitmap, wxPoint(3, 160), wxSize(55, 15), wxBU_AUTODRAW );
    m_tbPSize1->SetBitmapSelected(m_tbPSize1BitmapSel);

    m_tbPSize2Bitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps2_n.xpm"));
    m_tbPSize2BitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps2_s.xpm"));
    m_tbPSize2 = new wxBitmapButton( itemPanel25, ID_WB_LINE2, m_tbPSize2Bitmap, wxPoint(3, 175), wxSize(55, 15), wxBU_AUTODRAW );
    m_tbPSize2->SetBitmapSelected(m_tbPSize2BitmapSel);

    m_tbPSize3Bitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps3_n.xpm"));
    m_tbPSize3BitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps3_s.xpm"));
    m_tbPSize3 = new wxBitmapButton( itemPanel25, ID_WB_LINE3, m_tbPSize3Bitmap, wxPoint(3, 190), wxSize(55, 15), wxBU_AUTODRAW );
    m_tbPSize3->SetBitmapSelected(m_tbPSize3BitmapSel);

    m_tbPSize4Bitmap = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps4_n.xpm"));
    m_tbPSize4BitmapSel = itemPanel25->GetBitmapResource(wxT("../resource/wb_ps4_s.xpm"));
    m_tbPSize4 = new wxBitmapButton( itemPanel25, ID_WB_LINE4, m_tbPSize4Bitmap, wxPoint(3, 205), wxSize(55, 15), wxBU_AUTODRAW );
    m_tbPSize4->SetBitmapSelected(m_tbPSize4BitmapSel);

    m_tbColor = new wxStaticText( itemPanel25, ID_WB_COLOR, _T(""), wxPoint(3, 262), wxSize(57, 20), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
    m_tbColor->SetBackgroundColour(wxColour(255, 255, 255));

    m_tbColorSel = new wxButton( itemPanel25, ID_WB_COLOR_SEL, _("Color"), wxPoint(2, 285), wxSize(59, 24), 0 );
    m_tbColorSel->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));

    m_tbFont = new wxStaticText( itemPanel25, ID_WB_FONT, _("AB"), wxPoint(3, 330), wxSize(33, 22), wxALIGN_LEFT|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
    m_tbFont->SetBackgroundColour(wxColour(255, 255, 255));

    m_tbFontLen = new wxStaticText( itemPanel25, ID_WB_FONT_LEN, _("0"), wxPoint(38, 330), wxSize(22, 22), wxALIGN_RIGHT|wxST_NO_AUTORESIZE|wxSIMPLE_BORDER );
    m_tbFontLen->SetBackgroundColour(wxColour(255, 255, 255));

    m_tbFontSel = new wxButton( itemPanel25, ID_WB_FONT_SEL, _("Font"), wxPoint(2, 355), wxSize(59, 24), 0 );
    m_tbFontSel->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, wxT("Tahoma")));

////@end CWBToolBox content construction
}


/*!
 * Should we show tooltips?
 */

bool CWBToolBox::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap CWBToolBox::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBToolBox bitmap retrieval
    if (name == _T("../resource/wb_pos_n.xpm"))
    {
        wxBitmap bitmap(wb_pos_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pos_p.xpm"))
    {
        wxBitmap bitmap(wb_pos_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pos_h.xpm"))
    {
        wxBitmap bitmap(wb_pos_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pos_s.xpm"))
    {
        wxBitmap bitmap(wb_pos_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_del_n.xpm"))
    {
        wxBitmap bitmap(wb_del_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_del_p.xpm"))
    {
        wxBitmap bitmap(wb_del_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_del_h.xpm"))
    {
        wxBitmap bitmap(wb_del_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_del_s.xpm"))
    {
        wxBitmap bitmap(wb_del_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_txt_n.xpm"))
    {
        wxBitmap bitmap(wb_txt_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_txt_p.xpm"))
    {
        wxBitmap bitmap(wb_txt_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_txt_h.xpm"))
    {
        wxBitmap bitmap(wb_txt_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_txt_s.xpm"))
    {
        wxBitmap bitmap(wb_txt_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_mrk_n.xpm"))
    {
        wxBitmap bitmap(wb_mrk_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_mrk_p.xpm"))
    {
        wxBitmap bitmap(wb_mrk_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_mrk_h.xpm"))
    {
        wxBitmap bitmap(wb_mrk_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_mrk_s.xpm"))
    {
        wxBitmap bitmap(wb_mrk_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ptr_n.xpm"))
    {
        wxBitmap bitmap(wb_ptr_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ptr_p.xpm"))
    {
        wxBitmap bitmap(wb_ptr_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ptr_h.xpm"))
    {
        wxBitmap bitmap(wb_ptr_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ptr_s.xpm"))
    {
        wxBitmap bitmap(wb_ptr_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_img_n.xpm"))
    {
        wxBitmap bitmap(wb_img_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_img_p.xpm"))
    {
        wxBitmap bitmap(wb_img_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_img_x.xpm"))
    {
        wxBitmap bitmap(wb_img_x);
        return bitmap;
    }
    else if (name == _T("../resource/wb_img_h.xpm"))
    {
        wxBitmap bitmap(wb_img_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_img_s.xpm"))
    {
        wxBitmap bitmap(wb_img_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pen_n.xpm"))
    {
        wxBitmap bitmap(wb_pen_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pen_p.xpm"))
    {
        wxBitmap bitmap(wb_pen_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pen_h.xpm"))
    {
        wxBitmap bitmap(wb_pen_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_pen_s.xpm"))
    {
        wxBitmap bitmap(wb_pen_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_lin_n.xpm"))
    {
        wxBitmap bitmap(wb_lin_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_lin_p.xpm"))
    {
        wxBitmap bitmap(wb_lin_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_lin_h.xpm"))
    {
        wxBitmap bitmap(wb_lin_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_lin_s.xpm"))
    {
        wxBitmap bitmap(wb_lin_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqr_n.xpm"))
    {
        wxBitmap bitmap(wb_sqr_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqr_p.xpm"))
    {
        wxBitmap bitmap(wb_sqr_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqr_h.xpm"))
    {
        wxBitmap bitmap(wb_sqr_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqr_s.xpm"))
    {
        wxBitmap bitmap(wb_sqr_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqf_n.xpm"))
    {
        wxBitmap bitmap(wb_sqf_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqf_p.xpm"))
    {
        wxBitmap bitmap(wb_sqf_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqf_h.xpm"))
    {
        wxBitmap bitmap(wb_sqf_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_sqf_s.xpm"))
    {
        wxBitmap bitmap(wb_sqf_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crc_n.xpm"))
    {
        wxBitmap bitmap(wb_crc_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crc_p.xpm"))
    {
        wxBitmap bitmap(wb_crc_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crc_h.xpm"))
    {
        wxBitmap bitmap(wb_crc_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crc_s.xpm"))
    {
        wxBitmap bitmap(wb_crc_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crf_n.xpm"))
    {
        wxBitmap bitmap(wb_crf_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crf_p.xpm"))
    {
        wxBitmap bitmap(wb_crf_p);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crf_h.xpm"))
    {
        wxBitmap bitmap(wb_crf_h);
        return bitmap;
    }
    else if (name == _T("../resource/wb_crf_s.xpm"))
    {
        wxBitmap bitmap(wb_crf_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps1_n.xpm"))
    {
        wxBitmap bitmap(wb_ps1_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps1_s.xpm"))
    {
        wxBitmap bitmap(wb_ps1_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps2_n.xpm"))
    {
        wxBitmap bitmap(wb_ps2_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps2_s.xpm"))
    {
        wxBitmap bitmap(wb_ps2_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps3_n.xpm"))
    {
        wxBitmap bitmap(wb_ps3_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps3_s.xpm"))
    {
        wxBitmap bitmap(wb_ps3_s);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps4_n.xpm"))
    {
        wxBitmap bitmap(wb_ps4_n);
        return bitmap;
    }
    else if (name == _T("../resource/wb_ps4_s.xpm"))
    {
        wxBitmap bitmap(wb_ps4_s);
        return bitmap;
    }
    return wxNullBitmap;
////@end CWBToolBox bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBToolBox::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBToolBox icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CWBToolBox icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_POS
 */

void CWBToolBox::OnWbPosClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CUR in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_POS );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CUR in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_DEL
 */

void CWBToolBox::OnWbDelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_DEL in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_DEL );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_DEL in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_TXT
 */

void CWBToolBox::OnWbTxtClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_TXT in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_TXT );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_TXT in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_MRK
 */

void CWBToolBox::OnWbMrkClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_MRK in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_MRK );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_MRK in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PTR
 */

void CWBToolBox::OnWbPtrClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PTR in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_PTR );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PTR in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_IMG
 */

void CWBToolBox::OnWbImgClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_IMG in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_IMG );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_IMG in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PEN
 */

void CWBToolBox::OnWbPenClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PEN in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_PEN );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PEN in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LIN
 */

void CWBToolBox::OnWbLinClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LIN in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_LIN );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LIN in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQR
 */

void CWBToolBox::OnWbSqrClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQR in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_SQR );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQR in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQF
 */

void CWBToolBox::OnWbSqfClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQF in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_SQF );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQF in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRC
 */

void CWBToolBox::OnWbCrcClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRC in WBoardFrame.
    // Before editing this code, remove the block markers.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_CRC );

    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRC in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRF
 */

void CWBToolBox::OnWbCrfClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRF in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxItemClick( ID_WB_CRF );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRF in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE1
 */

void CWBToolBox::OnWbLine1Click( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE1 in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxPenSizeClick( ID_WB_LINE1 );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE1 in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE2
 */

void CWBToolBox::OnWbLine2Click( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE2 in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxPenSizeClick( ID_WB_LINE2 );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE2 in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE3
 */

void CWBToolBox::OnWbLine3Click( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE3 in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxPenSizeClick( ID_WB_LINE3 );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE3 in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE4
 */

void CWBToolBox::OnWbLine4Click( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE4 in WBoardFrame.

    if ( GetProcessPtr() )
        GetProcessPtr()->OnToolBoxPenSizeClick( ID_WB_LINE4 );

    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE4 in WBoardFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_COLOR_SEL
 */

void CWBToolBox::OnWbColorSelClick( wxCommandEvent& event )
{
    wxColourData clData;
    wxColourDialog clDlg;

    clData.SetColour(m_Colour);

    if (clDlg.Create(this, &clData))
    {
        clDlg.Centre();

        if (clDlg.ShowModal() == wxID_OK)
        {
            wxColour colour = clDlg.GetColourData().GetColour();

            if (GetProcessPtr())
            {
                ColorDef sel;

                sel.SetAttr(colour.Red(), colour.Green(), colour.Blue());
                GetProcessPtr()->OnToolBoxColorClick(sel);
            }
        }
    }

    event.Skip();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_FONT_SEL
 */

void CWBToolBox::OnWbFontSelClick( wxCommandEvent& event )
{
    wxFontData fnData;
    wxFontDialog fnDlg;

    fnData.SetInitialFont(m_Font);
    fnData.SetColour(m_Colour);

    if (fnDlg.Create(this, fnData))
    {
        fnDlg.Centre();

        if (fnDlg.ShowModal() == wxID_OK)
        {
            wxFont font = fnDlg.GetFontData().GetChosenFont();
            wxColour colour = fnDlg.GetFontData().GetColour();

            if (GetProcessPtr())
            {
                if (font != m_Font)
                {
                    FontDef sel;
                    wxString name;
                    WB_BYTE effect;

                    sel.Clear();

                    effect = 0;
                    if (font.GetUnderlined())
                        effect |= FD_UNDERLINE;
                    if (font.GetStyle() == wxFONTSTYLE_ITALIC)
                        effect |= FD_ITALIC;
                    if (font.GetWeight() == wxFONTWEIGHT_BOLD)
                        effect |= FD_BOLD;

                    name = font.GetFaceName();
                    sel.SetAttr(name.char_str(), font.GetPointSize(), effect);

                    GetProcessPtr()->OnToolBoxFontClick(sel);
                }

                if (colour != m_Colour)
                {
                    ColorDef sel;
                    sel.SetAttr(colour.Red(), colour.Green(), colour.Blue());
                    GetProcessPtr()->OnToolBoxColorClick(sel);
                }
            }
        }
    }

    event.Skip();
}


void CWBToolBox::SelectItem(int nId)
{
    switch (m_CurItemId)
    {
        case ID_WB_POS:
            m_tbPos->SetBitmapLabel(m_tbPosBitmap);
            break;
        case ID_WB_DEL:
            m_tbDel->SetBitmapLabel(m_tbDelBitmap);
            break;
        case ID_WB_TXT:
            m_tbTxt->SetBitmapLabel(m_tbTxtBitmap);
            break;
        case ID_WB_MRK:
            m_tbMRK->SetBitmapLabel(m_tbMRKBitmap);
            break;
        case ID_WB_PTR:
            m_tbPtr->SetBitmapLabel(m_tbPtrBitmap);
            break;
        case ID_WB_IMG:
            m_tbImg->SetBitmapLabel(m_tbImgBitmap);
            break;
        case ID_WB_PEN:
            m_tbPen->SetBitmapLabel(m_tbPenBitmap);
            break;
        case ID_WB_LIN:
            m_tbLin->SetBitmapLabel(m_tbLinBitmap);
            break;
        case ID_WB_SQR:
            m_tbSqr->SetBitmapLabel(m_tbSqrBitmap);
            break;
        case ID_WB_SQF:
            m_tbSqf->SetBitmapLabel(m_tbSqfBitmap);
            break;
        case ID_WB_CRC:
            m_tbCrc->SetBitmapLabel(m_tbCrcBitmap);
            break;
        case ID_WB_CRF:
            m_tbCrf->SetBitmapLabel(m_tbCrfBitmap);
            break;
    }

    switch (nId)
    {
        case ID_WB_POS:
            m_tbPos->SetBitmapLabel(m_tbPosBitmapSel);
            break;
        case ID_WB_DEL:
            m_tbDel->SetBitmapLabel(m_tbDelBitmapSel);
            break;
        case ID_WB_TXT:
            m_tbTxt->SetBitmapLabel(m_tbTxtBitmapSel);
            break;
        case ID_WB_MRK:
            m_tbMRK->SetBitmapLabel(m_tbMRKBitmapSel);
            break;
        case ID_WB_PTR:
            m_tbPtr->SetBitmapLabel(m_tbPtrBitmapSel);
            break;
        case ID_WB_IMG:
            m_tbImg->SetBitmapLabel(m_tbImgBitmapSel);
            break;
        case ID_WB_PEN:
            m_tbPen->SetBitmapLabel(m_tbPenBitmapSel);
            break;
        case ID_WB_LIN:
            m_tbLin->SetBitmapLabel(m_tbLinBitmapSel);
            break;
        case ID_WB_SQR:
            m_tbSqr->SetBitmapLabel(m_tbSqrBitmapSel);
            break;
        case ID_WB_SQF:
            m_tbSqf->SetBitmapLabel(m_tbSqfBitmapSel);
            break;
        case ID_WB_CRC:
            m_tbCrc->SetBitmapLabel(m_tbCrcBitmapSel);
            break;
        case ID_WB_CRF:
            m_tbCrf->SetBitmapLabel(m_tbCrfBitmapSel);
            break;
    }

    m_CurItemId = nId;
}


void CWBToolBox::EnableItem(int nId, bool bOp)
{
    if (m_CurItemId == nId)
    {
        if (GetProcessPtr())
            GetProcessPtr()->OnToolBoxItemClick(ID_WB_POS);
    }

    switch (nId)
    {
        case ID_WB_POS:
            m_tbPos->Enable(bOp);
            break;
        case ID_WB_DEL:
            m_tbDel->Enable(bOp);
            break;
        case ID_WB_TXT:
            m_tbTxt->Enable(bOp);
            break;
        case ID_WB_MRK:
            m_tbMRK->Enable(bOp);
            break;
        case ID_WB_PTR:
            m_tbPtr->Enable(bOp);
            break;
        case ID_WB_IMG:
            m_tbImg->Enable(bOp);
            break;
        case ID_WB_PEN:
            m_tbPen->Enable(bOp);
            break;
        case ID_WB_LIN:
            m_tbLin->Enable(bOp);
            break;
        case ID_WB_SQR:
            m_tbSqr->Enable(bOp);
            break;
        case ID_WB_SQF:
            m_tbSqf->Enable(bOp);
            break;
        case ID_WB_CRC:
            m_tbCrc->Enable(bOp);
            break;
        case ID_WB_CRF:
            m_tbCrf->Enable(bOp);
            break;
    }
}

void CWBToolBox::SelectPenSize(int nId)
{
    switch (m_CurPenSize)
    {
        case ID_WB_LINE1:
            m_tbPSize1->SetBitmapLabel(m_tbPSize1Bitmap);
            break;
        case ID_WB_LINE2:
            m_tbPSize2->SetBitmapLabel(m_tbPSize2Bitmap);
            break;
        case ID_WB_LINE3:
            m_tbPSize3->SetBitmapLabel(m_tbPSize3Bitmap);
            break;
        case ID_WB_LINE4:
            m_tbPSize4->SetBitmapLabel(m_tbPSize4Bitmap);
            break;
    }

    switch (nId)
    {
        case ID_WB_LINE1:
            m_tbPSize1->SetBitmapLabel(m_tbPSize1BitmapSel);
            break;
        case ID_WB_LINE2:
            m_tbPSize2->SetBitmapLabel(m_tbPSize2BitmapSel);
            break;
        case ID_WB_LINE3:
            m_tbPSize3->SetBitmapLabel(m_tbPSize3BitmapSel);
            break;
        case ID_WB_LINE4:
            m_tbPSize4->SetBitmapLabel(m_tbPSize4BitmapSel);
            break;
    }

    m_CurPenSize = nId;
}

void CWBToolBox::SelectColour(wxColour& colour)
{
    m_Colour = colour;

    if (m_tbColor)
    {
        m_tbColor->SetBackgroundColour(m_Colour);
        m_tbColor->Refresh();
    }
}

void CWBToolBox::SelectFont(wxFont& font)
{
    m_Font = font;

    if (m_tbFont)
    {
        wxFont fnName = m_Font;
        fnName.SetPointSize(FONT_SIZE_DEF);
        m_tbFont->SetFont(fnName);
        m_tbFont->SetLabel(wxT("AB"));
    }

    if (m_tbFontLen)
    {
        wxFont fnSize = m_tbFontLen->GetFont();
        fnSize.SetPointSize(FONT_SIZE_DEF);
        m_tbFontLen->SetFont(fnSize);

        wxString len;
        len.Printf(wxT("%d"), m_Font.GetPointSize());
        m_tbFontLen->SetLabel(len);
    }
}


/*!
 * CWBMain type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CWBMain, wxPanel )


/*!
 * CWBMain event table definition
 */

BEGIN_EVENT_TABLE( CWBMain, wxPanel )

////@begin CWBMain event table entries
////@end CWBMain event table entries

END_EVENT_TABLE()


/*!
 * CWBMain constructors
 */

CWBMain::CWBMain()
{
    Init();
}

CWBMain::CWBMain(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * CWBMain creator
 */

bool CWBMain::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin CWBMain creation
    wxPanel::Create(parent, id, pos, size, style);
    CreateControls();
    if (GetSizer())
        GetSizer()->Fit(this);
////@end CWBMain creation
    return true;
}


/*!
 * CWBMain destructor
 */

CWBMain::~CWBMain()
{
////@begin CWBMain destruction
    if (GetProcessPtr())
    {
        GetProcessPtr()->OnWindowDestroy();
    }
////@end CWBMain destruction
}


/*!
 * Member initialisation
 */

void CWBMain::Init()
{
////@begin CWBMain member initialisation
    SetProcessPtr(NULL);

    m_pwbToolBox = NULL;
    m_pwbMenuBar = NULL;
    m_pwbScreen = NULL;
    m_pwbEdit = NULL;
    m_MainSizer = NULL;
////@end CWBMain member initialisation
}


/*!
 * Control creation for CWBMain
 */

void CWBMain::CreateControls()
{    
////@begin CWBMain content construction
    CWBMain* itemPanel15 = this;

    this->SetBackgroundColour(wxColour(192, 192, 192));
    m_MainSizer = new wxFlexGridSizer(1, 2, 0, 0);
    m_MainSizer->AddGrowableRow(1);
    m_MainSizer->AddGrowableCol(1);
    itemPanel15->SetSizer(m_MainSizer);

    m_MainSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_pwbMenuBar = new CWBMenuBar( itemPanel15, wxID_ANY, wxPoint(70, 0), wxSize(600, 40), wxNO_BORDER|wxTAB_TRAVERSAL );
    m_pwbMenuBar->SetBackgroundColour(wxColour(192, 192, 192));
    m_pwbMenuBar->SetProcessPtr(NULL);
    m_MainSizer->Add(m_pwbMenuBar, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_pwbToolBox = new CWBToolBox( itemPanel15, wxID_ANY, wxPoint(0, 40), wxSize(70, 400), wxNO_BORDER|wxTAB_TRAVERSAL );
    m_pwbToolBox->SetBackgroundColour(wxColour(192, 192, 192));
    m_pwbToolBox->SetProcessPtr(NULL);
    m_MainSizer->Add(m_pwbToolBox, 0, wxALIGN_CENTER_HORIZONTAL|wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_pwbScreen = new CWBScreen( itemPanel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB );
    m_pwbScreen->SetBackgroundColour(wxColour(192, 192, 192));
    m_pwbEdit = m_pwbScreen->GetWBEdit();
    m_pwbEdit->SetProcessPtr(NULL);
    m_MainSizer->Add(m_pwbScreen, 0, wxGROW|wxGROW|wxALL, 0);

////@end CWBMain content construction
}


/*!
 * Should we show tooltips?
 */

bool CWBMain::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap CWBMain::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBMain bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end CWBMain bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBMain::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBMain icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CWBMain icon retrieval
}

void CWBMain::SetProcessObj(IWBProcess* pProcess)
{
    if (pProcess && GetHandle())
    {
        pProcess->OnWindowCreate( GetInterfacePtr() );
    }

    SetProcessPtr(pProcess);

    if (m_pwbToolBox)
        m_pwbToolBox->SetProcessPtr(pProcess);

    if (m_pwbMenuBar)
        m_pwbMenuBar->SetProcessPtr(pProcess);

    if (m_pwbScreen)
        m_pwbScreen->SetProcessPtr(pProcess);

    if (m_pwbEdit)
        m_pwbEdit->SetProcessPtr(pProcess);
}

IWBInterface* CWBMain::GetInterfacePtr()
{
    return (IWBInterface*) this;
}

void CWBMain::ToolBoxItemSel( int nId )
{
    if ( m_pwbToolBox )
        m_pwbToolBox->SelectItem(nId);

    if ( m_pwbEdit )
        m_pwbEdit->SetControl(nId);
}

void CWBMain::ToolBoxItemEnable( int nId, bool bOp)
{
    if ( m_pwbToolBox )
        m_pwbToolBox->EnableItem(nId, bOp);
}

void CWBMain::ToolBoxPenSizeSel( int nId )
{
    if ( m_pwbToolBox )
        m_pwbToolBox->SelectPenSize(nId);

    if ( m_pwbEdit )
        m_pwbEdit->SetPenSize(nId);
}

void CWBMain::ToolBoxColorSel( ColorDef& color )
{
    wxColor clSel(color.cdRed, color.cdGreen, color.cdBlue);;

    if ( m_pwbToolBox )
        m_pwbToolBox->SelectColour(clSel);

    if ( m_pwbEdit )
        m_pwbEdit->SetFrgColour(clSel);
}

void CWBMain::ToolBoxFontSel( FontDef& font )
{
    wxFont fnSel;

    if ( font.fdName[0] != '\0' )
    {
        wxString name = wxString::FromAscii(font.fdName);

        fnSel.SetFaceName(name);
        fnSel.SetPointSize(font.fdSize);

        if (font.fdEffect & FD_UNDERLINE)
            fnSel.SetUnderlined(true);
        if (font.fdEffect & FD_ITALIC)
            fnSel.SetStyle(wxFONTSTYLE_ITALIC);
        if (font.fdEffect & FD_BOLD)
            fnSel.SetWeight(wxFONTWEIGHT_BOLD);
    }
    else
    {
        fnSel = GetFont();
        fnSel.SetPointSize(FONT_SIZE_DEF);
    }

    if ( m_pwbToolBox )
        m_pwbToolBox->SelectFont(fnSel);

    if ( m_pwbEdit )
        m_pwbEdit->SetTxtFont(fnSel);
}

bool CWBMain::MenuBarNewExec( bool bChanged, bool bRepaint )
{
    bool ret = false;

    if ( m_pwbEdit )
        ret = m_pwbEdit->NewExec(bChanged, bRepaint);

    return ret;
}

void CWBMain::MenuBarSaveExec( )
{
    if ( m_pwbEdit )
        m_pwbEdit->SaveExec();
}

void CWBMain::MenuBarPrintExec( )
{
    if ( m_pwbEdit )
        m_pwbEdit->PrintExec();
}

void CWBMain::MenuBarPageSetup( )
{
    if ( m_pwbEdit )
        m_pwbEdit->PageSetup();
}

void CWBMain::MenuBarCutExec( )
{
    if ( m_pwbEdit )
        m_pwbEdit->CutExec();
}

void CWBMain::MenuBarUndoExec( )
{
}

void CWBMain::MenuBarImageStopExec( )
{
}

void CWBMain::MenuBarImageStopEnable( bool bOp )
{
    if ( m_pwbMenuBar )
        m_pwbMenuBar->ImgStopEnable(bOp);
}

void CWBMain::EdtSelArea( PtPos& pt1, PtPos& pt2 )
{
    if ( m_pwbEdit )
        m_pwbEdit->SelArea(pt1, pt2);
}

void CWBMain::EdtSelRect( PtPos& pt1, PtPos& pt2 )
{
    if ( m_pwbEdit )
        m_pwbEdit->SelRect(pt1, pt2);
}

void CWBMain::EdtSelEllipse( PtPos& pt1, PtPos& pt2 )
{
    if ( m_pwbEdit )
        m_pwbEdit->SelEllipse(pt1, pt2);
}

void CWBMain::EdtSelLine( PtPos& pt1, PtPos& pt2 )
{
    if ( m_pwbEdit )
        m_pwbEdit->SelLine(pt1, pt2);
}

void CWBMain::EdtDrawArea( PtPos& pt1, PtPos& pt2 )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawArea(pt1, pt2);
}

void CWBMain::EdtKillArea( )
{
    if ( m_pwbEdit )
        m_pwbEdit->KillArea();
}

void CWBMain::EdtRepaint( )
{
    if ( m_pwbEdit )
        m_pwbEdit->Repaint();
}

void CWBMain::ScrScrollWindow( int dx, int dy )
{
    if (m_pwbScreen)
    {
        m_pwbScreen->ScrollWnd(dx, dy);
    }
}

void CWBMain::ScrSetScrollPos(int posx, int posy)
{
    if (m_pwbEdit)
    {
        m_pwbEdit->SetScrollPos(posx, posy);
    }
}

void CWBMain::CtlDrawRect( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawRect(pt1, pt2, nLin, color, bFull, flags);
}

void CWBMain::CtlDrawEllipse( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawEllipse(pt1, pt2, nLin, color, bFull, flags);
}

void CWBMain::CtlDrawLine(PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bMask, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawLine(pt1, pt2, nLin, color, bMask, flags);
}

void CWBMain::CtlDrawIndicator( PtPos& pt, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawIndicator(pt, flags);
}

void CWBMain::CtlDrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawImage(szFile, pt1, pt2, flags);
}

void CWBMain::CtlDrawTxt( WB_PCSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color, WB_BYTE flags )
{
    if ( m_pwbEdit )
        m_pwbEdit->DrawTxt(szText, pt1, pt2, font, color, flags);
}

void CWBMain::CtlEditTxt( PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color )
{
    if ( m_pwbEdit )
        m_pwbEdit->EditTxt(pt1, pt2, font, color);
}

void CWBMain::CtlGetTxt( WB_PSTR szText )
{
    if ( m_pwbEdit )
        m_pwbEdit->GetTxt(szText);
}


/*!
 * CWBScreen type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CWBScreen, wxScrolledWindow )


/*!
 * CWBScreen event table definition
 */

BEGIN_EVENT_TABLE( CWBScreen, wxScrolledWindow )

////@begin CWBScreen event table entries
    EVT_SCROLLWIN_LINEUP( CWBScreen::OnScrLineUp )
    EVT_SCROLLWIN_LINEDOWN( CWBScreen::OnScrLineDown )
    EVT_SCROLLWIN_PAGEUP( CWBScreen::OnScrPageUp )
    EVT_SCROLLWIN_PAGEDOWN( CWBScreen::OnScrPageDown )
    EVT_SCROLLWIN_THUMBRELEASE( CWBScreen::OnScrThumbRelease )
    EVT_SIZE( CWBScreen::OnSize )
    EVT_PAINT( CWBScreen::OnPaint )

////@end CWBScreen event table entries

END_EVENT_TABLE()


/*!
 * CWBScreen constructors
 */

CWBScreen::CWBScreen()
{
    Init();
}

CWBScreen::CWBScreen(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * CWBScreen creator
 */

bool CWBScreen::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin CWBScreen creation
    wxScrolledWindow::Create(parent, id, pos, size, style);
    CreateControls();
////@end CWBScreen creation
    return true;
}


/*!
 * CWBScreen destructor
 */

CWBScreen::~CWBScreen()
{
////@begin CWBScreen destruction
////@end CWBScreen destruction
}


/*!
 * Member initialisation
 */

void CWBScreen::Init()
{
////@begin CWBScreen member initialisation
    m_pwbEdit = NULL;
    m_EditSizer = NULL;
    m_bIsScrolled = false;
////@end CWBScreen member initialisation
}


/*!
 * Control creation for CWBScreen
 */

void CWBScreen::CreateControls()
{    
////@begin CWBScreen content construction
    CWBScreen* itemScrolledWindow48 = this;

    this->SetBackgroundColour(wxColour(192, 192, 192));
    this->SetScrollbars(1, 1, 0, 0);
    m_EditSizer = new wxGridSizer(1, 1, 0, 0);
    itemScrolledWindow48->SetSizer(m_EditSizer);

    m_pwbEdit = new CWBEdit( itemScrolledWindow48, IDW_WB_EDIT, wxPoint(0, 0), wxSize(A4_WIDTH, A4_HEIGHT), wxSIMPLE_BORDER|wxTAB_TRAVERSAL );
    m_pwbEdit->SetBackgroundColour(wxColour(255, 255, 255));
    m_pwbEdit->SetControl(ID_WB_POS);
    m_EditSizer->Add(m_pwbEdit, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL, 0);

    this->SetMinSize(wxSize(100, 100));
////@end CWBScreen content construction
}


/*!
 * Should we show tooltips?
 */

bool CWBScreen::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap CWBScreen::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBScreen bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end CWBScreen bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBScreen::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBScreen icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CWBScreen icon retrieval
}


void CWBScreen::OnScrLineUp( wxScrollWinEvent& event )
{
    AdjScrollPos();
    event.Skip();
}


void CWBScreen::OnScrLineDown( wxScrollWinEvent& event )
{
    AdjScrollPos();
    event.Skip();
}


void CWBScreen::OnScrPageUp( wxScrollWinEvent& event )
{
    AdjScrollPos();
    event.Skip();
}


void CWBScreen::OnScrPageDown( wxScrollWinEvent& event )
{
    AdjScrollPos();
    event.Skip();
}


void CWBScreen::OnScrThumbRelease( wxScrollWinEvent& event )
{
    AdjScrollPos();
    event.Skip();
}


void CWBScreen::OnSize( wxSizeEvent& event )
{
    if ( m_bIsScrolled )
    {
        AdjScrollPos();
        Refresh();
    }

    event.Skip();
}


void CWBScreen::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc(this);
}


void CWBScreen::ScrollWnd(int dx, int dy)
{
    int X, Y, nx, ny;

    GetScrollPixelsPerUnit(&nx, &ny);

    X = (nx > 0) ? (dx / nx) : dx;
    Y = (ny > 0) ? (dy / ny) : dx;

    Scroll(X, Y);
}

void CWBScreen::AdjScrollPos()
{
    if ( GetProcessPtr() )
    {
        int X, Y, nx, ny;

        GetViewStart(&X, &Y);
        GetScrollPixelsPerUnit(&nx, &ny);

        if (X > 0 || Y > 0)
            m_bIsScrolled = true;
        else
            m_bIsScrolled = false;

        GetProcessPtr()->OnScreenSetScrollPos(X*nx, Y*ny);
    }
}


/*!
 * CWBEdit type definition
 */

IMPLEMENT_DYNAMIC_CLASS( CWBEdit, wxPanel )


/*!
 * CWBEdit event table definition
 */

BEGIN_EVENT_TABLE( CWBEdit, wxPanel )

////@begin CWBEdit event table entries
    EVT_LEFT_DOWN( CWBEdit::OnLeftDown )
    EVT_LEFT_UP( CWBEdit::OnLeftUp )
    EVT_MOTION( CWBEdit::OnMotion )
    EVT_PAINT( CWBEdit::OnPaint )
    EVT_SET_FOCUS( CWBEdit::OnSetFocus )
    EVT_TEXT(ID_WB_EDIT_TEXT, CWBEdit::OnCtlTextUpdated)
////@end CWBEdit event table entries

END_EVENT_TABLE()


/*!
 * CWBEdit constructors
 */

CWBEdit::CWBEdit()
{
    Init();
}

CWBEdit::CWBEdit(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*!
 * CWBEdit creator
 */

bool CWBEdit::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin CWBEdit creation
    wxPanel::Create(parent, id, pos, size, style);
    CreateControls();
////@end CWBEdit creation
    return true;
}


/*!
 * CWBEdit destructor
 */

CWBEdit::~CWBEdit()
{
////@begin CWBEdit destruction
    if (m_printData)
        delete m_printData;
    if (m_pageSetupData)
        delete m_pageSetupData;
////@end CWBEdit destruction
}


/*!
 * Member initialisation
 */

void CWBEdit::Init()
{
////@begin CWBEdit member initialisation
    m_scrPosX = 0;
    m_scrPosY = 0;
    m_bAreaSelected = false;
    m_pText = NULL;
////@end CWBEdit member initialisation
}


/*!
 * Control creation for CWBEdit
 */

void CWBEdit::CreateControls()
{    
////@begin CWBEdit content construction
    this->SetBackgroundColour(wxColour(255, 255, 255));
    m_curPos = GetCursorResource(wxT("../resource/wb_pos_cur.xpm"));
    m_curDel = GetCursorResource(wxT("../resource/wb_del_cur.xpm"));
    m_curTxt = GetCursorResource(wxT("../resource/wb_txt_cur.xpm"));
    m_curMRK = GetCursorResource(wxT("../resource/wb_mrk_cur.xpm"));
    m_curPtr = GetCursorResource(wxT("../resource/wb_ptr_cur.xpm"));
    m_curImg = GetCursorResource(wxT("../resource/wb_img_cur.xpm"));
    m_curPen = GetCursorResource(wxT("../resource/wb_pen_cur.xpm"));
    m_curLin = GetCursorResource(wxT("../resource/wb_lin_cur.xpm"));
    m_curSqr = GetCursorResource(wxT("../resource/wb_sqr_cur.xpm"));
    m_curSqf = GetCursorResource(wxT("../resource/wb_sqf_cur.xpm"));
    m_curCrc = GetCursorResource(wxT("../resource/wb_crc_cur.xpm"));
    m_curCrf = GetCursorResource(wxT("../resource/wb_crf_cur.xpm"));
////@end CWBEdit content construction

    m_bmpPtr = wxBitmap(wb_ptr_ico);
    wxMask* pmask = new wxMask(m_bmpPtr, *wxGREEN);
    m_bmpPtr.SetMask(pmask);

    m_bmpEdit.Create(A4_WIDTH, A4_HEIGHT);

    m_printData = new wxPrintData;
    if (m_printData)
    {
        m_pageSetupData = new wxPageSetupDialogData;

        if (m_pageSetupData)
            (*m_pageSetupData) = *m_printData;
    }
    else
    {
        m_pageSetupData = NULL;
    }

    wxInitAllImageHandlers();
    NewExec(false, true);
}

void CWBEdit::SetControl(int nId)
{
    switch (nId)
    {
        case ID_WB_POS:
            m_Cursor = m_curPos;
            break;
        case ID_WB_DEL:
            m_Cursor = m_curDel;
            break;
        case ID_WB_TXT:
            m_Cursor = m_curTxt;
            break;
        case ID_WB_MRK:
            m_Cursor = m_curMRK;
            break;
        case ID_WB_PTR:
            m_Cursor = m_curPtr;
            break;
        case ID_WB_IMG:
            SelImage();
            m_Cursor = m_curImg;
            break;
        case ID_WB_PEN:
            m_Cursor = m_curPen;
            break;
        case ID_WB_LIN:
            m_Cursor = m_curLin;
            break;
        case ID_WB_SQR:
            m_Cursor = m_curSqr;
            break;
        case ID_WB_SQF:
            m_Cursor = m_curSqf;
            break;
        case ID_WB_CRC:
            m_Cursor = m_curCrc;
            break;
        case ID_WB_CRF:
            m_Cursor = m_curCrf;
            break;
    }

    SetCursor(m_Cursor);
}

void CWBEdit::SetPenSize(int nId)
{
    switch (nId)
    {
        case ID_WB_LINE1:
            m_Pen.SetWidth(1);
            break;
        case ID_WB_LINE2:
            m_Pen.SetWidth(3);
            break;
        case ID_WB_LINE3:
            m_Pen.SetWidth(5);
            break;
        case ID_WB_LINE4:
            m_Pen.SetWidth(7);
            break;
    }

    m_Pen.SetColour(m_Colour);
}

void CWBEdit::SetFrgColour(wxColour& colour)
{
    m_Colour = colour;
    m_Pen.SetColour(m_Colour);
}

void CWBEdit::SetTxtFont(wxFont& font)
{
    m_Font = font;
}

void CWBEdit::DrawArea( PtPos& pt1, PtPos& pt2 )
{
    wxClientDC dc(this);
    wxPen pen(*wxBLACK, 1, wxDOT);

    int   oldLogicalFunction = dc.GetLogicalFunction();
    wxPen oldPen = dc.GetPen();

    dc.SetPen(pen);
    dc.SetLogicalFunction( wxEQUIV );
    dc.DrawRectangle(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);
    dc.DrawRectangle(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    m_AreaPos1.SetAttr( pt1.X, pt1.Y );
    m_AreaPos2.SetAttr( pt2.X, pt2.Y );

    dc.SetLogicalFunction(oldLogicalFunction);
    dc.SetPen(oldPen);

    m_bAreaSelected = true;
}

void CWBEdit::KillArea( )
{
    if (m_bAreaSelected)
    {
        int difx = (m_AreaPos1.X < m_AreaPos2.X) ? 1 : -1;
        int dify = (m_AreaPos1.Y < m_AreaPos2.Y) ? 1 : -1;

        wxRect rc(m_AreaPos1.X - difx, m_AreaPos1.Y - dify, m_AreaPos2.X - m_AreaPos1.X + 2, m_AreaPos2.Y - m_AreaPos1.Y + 2);

        m_bAreaSelected = false;
        RefreshRect(rc);

        m_AreaPos1.SetAttr( 0, 0 );
        m_AreaPos2.SetAttr( 0, 0 );
    }
}

void CWBEdit::Repaint( )
{
    wxClientDC dc(this);
    dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::DrawRect( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags )
{
    wxClientDC dc(this);
    wxColour ctlColor(color.cdRed, color.cdGreen, color.cdBlue);
    wxPen ctlPen(ctlColor, nLin, wxSOLID);
    wxBrush brush(ctlColor);
    RcPos rect;

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    if (bFull)
        memdc.SetBrush(brush);
    else
        memdc.SetBrush(*wxTRANSPARENT_BRUSH);

    memdc.SetLogicalFunction( wxCOPY );
    memdc.SetPen(ctlPen);

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        AdjScroll(rect);
    }

    memdc.DrawRectangle(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    memdc.SelectObject( wxNullBitmap );

    m_bAreaSelected = false;
    m_AreaPos1.SetAttr(0, 0);
    m_AreaPos2.SetAttr(0, 0);

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::DrawEllipse( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags )
{
    wxClientDC dc(this);
    wxColour ctlColor(color.cdRed, color.cdGreen, color.cdBlue);
    wxPen ctlPen(ctlColor, nLin, wxSOLID);
    wxBrush brush(ctlColor);
    RcPos rect;

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    if (bFull)
        memdc.SetBrush(brush);
    else
        memdc.SetBrush(*wxTRANSPARENT_BRUSH);

    memdc.SetLogicalFunction( wxCOPY );
    memdc.SetPen(ctlPen);

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        AdjScroll(rect);
    }

    memdc.DrawEllipse(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    memdc.SelectObject( wxNullBitmap );

    m_bAreaSelected = false;
    m_AreaPos1.SetAttr(0, 0);
    m_AreaPos2.SetAttr(0, 0);

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::DrawLine( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bMask, WB_BYTE flags )
{
    wxClientDC dc(this);
    wxColour ctlColor(color.cdRed, color.cdGreen, color.cdBlue);
    wxPen ctlPen(ctlColor, nLin, wxSOLID);
    RcPos rect;

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    if (bMask)
        memdc.SetLogicalFunction( wxAND );
    else
        memdc.SetLogicalFunction( wxCOPY );

    memdc.SetPen(ctlPen);

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        AdjScroll(rect);
    }

    memdc.DrawLine(pt1.X, pt1.Y, pt2.X, pt2.Y);

    memdc.SelectObject( wxNullBitmap );

    m_bAreaSelected = false;
    m_AreaPos1.SetAttr(0, 0);
    m_AreaPos2.SetAttr(0, 0);

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::DrawIndicator( PtPos& pt, WB_BYTE flags )
{
    wxClientDC dc(this);
    RcPos rect;

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    wxIcon icoPtr;
    icoPtr.CopyFromBitmap(m_bmpPtr);

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt.X - PTR_WIDTH, pt.Y - PTR_HEIGHT, pt.X, pt.Y);
        AdjScroll(rect);
    }

    // Para manter a compatibilidade com a versão anterior
    memdc.DrawIcon(icoPtr, pt.X - PTR_WIDTH, pt.Y - PTR_HEIGHT);

    memdc.SelectObject( wxNullBitmap );

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::DrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2, WB_BYTE flags )
{
    wxClientDC dc(this);
    RcPos rect;

    wxBitmap bmpImage;
    int imgWidth, imgHeight;

    int edtWidth  = 0;
    int edtHeight = 0;

    wxString strFile = wxString::FromAscii(szFile);

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    GetClientSize(&edtWidth, &edtHeight);

    strFile.FromAscii(szFile);

    wxString extension;
    wxFileName::SplitPath(strFile, NULL, NULL, &extension);

    if ( extension.Lower() == _T("bmp") )
    {
        bmpImage.LoadFile(strFile, wxBITMAP_TYPE_BMP);
    }
    else if ( extension.Lower() == _T("jpg") )
    {
        bmpImage.LoadFile(strFile, wxBITMAP_TYPE_JPEG);
    }

    // Calcula largura e altura, considerando a área de edição
    imgWidth  = edtWidth - pt1.X;
    imgHeight = edtHeight - pt1.Y;

    if (imgWidth < bmpImage.GetWidth() || imgHeight < bmpImage.GetHeight())
    {
        float fw, fh;

        if (imgWidth < bmpImage.GetWidth())
            fw = (float)imgWidth / (float)bmpImage.GetWidth();
        else
            fw = 1;

        if (imgHeight < bmpImage.GetHeight())
            fh = (float)imgHeight / (float)bmpImage.GetHeight();
        else
            fh = 1;

        if (fw < fh)
        {
            imgWidth  = (int) (bmpImage.GetWidth() * fw);
            imgHeight = (int) (bmpImage.GetHeight() * fw);
        }
        else
        {
            imgWidth  = (int) (bmpImage.GetWidth() * fh);
            imgHeight = (int) (bmpImage.GetHeight() * fh);
        }

        wxImage image = bmpImage.ConvertToImage();
        bmpImage = wxBitmap(image.Scale(imgWidth, imgHeight));
    }
    else
    {
        imgWidth  = bmpImage.GetWidth();
        imgHeight = bmpImage.GetHeight();
    }

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        AdjScroll(rect);
    }

    memdc.DrawBitmap(bmpImage, pt1.X, pt1.Y);

    memdc.SelectObject( wxNullBitmap );

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

void CWBEdit::EditTxt( PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color )
{
    m_bAreaSelected = false;
    m_AreaPos1.SetAttr(0, 0);
    m_AreaPos2.SetAttr(0, 0);

    m_pText = new wxTextCtrl( this, ID_WB_EDIT_TEXT, _T(""), wxPoint(pt1.X, pt1.Y),
        wxSize(pt2.X - pt1.X + SB_SIZE, pt2.Y - pt1.Y), wxALIGN_LEFT|wxTE_MULTILINE|wxSIMPLE_BORDER );

    if (m_pText)
    {
        m_pText->Show();
        m_pText->SetFocus();
        m_pText->SetBackgroundColour(wxColour(255, 255, 255));
        m_pText->SetForegroundColour(wxColour(color.cdRed, color.cdGreen, color.cdBlue));
        m_pText->SetFont(GetTextFont(font));
    }
}

void CWBEdit::GetTxt( WB_PSTR szText )
{
    if (szText)
    {
        wxString text;

        if (m_pText)
        {
            text = m_pText->GetValue();
            strcpy(szText, text.ToAscii());
        }
        else
        {
            szText[0] = '\0';
        }
    }
}

void CWBEdit::DrawTxt( WB_PCSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color, WB_BYTE flags )
{
    wxClientDC dc(this);
    RcPos rect;

    wxString text = wxString::FromAscii(szText);

    wxMemoryDC memdc;
    memdc.SelectObject( m_bmpEdit );

    if (flags & DC_AUTOSCROLL)
    {
        rect.SetAttr(pt1.X, pt1.Y, pt2.X, pt2.Y);
        AdjScroll(rect);
    }

    if (m_pText)
    {
        delete m_pText;
        m_pText = NULL;
    }

    if (!text.IsEmpty())
    {
        wxRect rcTxt;
        wxString wraptxt;

        rcTxt = wxRect(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

        memdc.SetBrush(*wxTRANSPARENT_BRUSH);
        memdc.SetTextForeground(wxColour(color.cdRed, color.cdGreen, color.cdBlue));
        memdc.SetFont(GetTextFont(font));

        WrapText(text, memdc, rcTxt, wraptxt);
        memdc.DrawLabel(wraptxt, rcTxt);
    }

    memdc.SelectObject( wxNullBitmap );

    if (flags & DC_REPAINT)
        dc.DrawBitmap(m_bmpEdit, 0, 0);
}

bool CWBEdit::NewExec( bool bChanged, bool bRepaint )
{
    bool bNew = true;

    if (bChanged)
    {
        if (wxMessageBox(_("The document was not saved. Proceed?"), _("White Board"),
                 wxYES_NO | wxICON_QUESTION, this) == wxNO)
        {
            bNew = false;
        }
    }

    if (bNew)
    {
        RcPos rect;

        wxClientDC dc(this);

        wxMemoryDC memdc;
        memdc.SelectObject( m_bmpEdit );
        memdc.SetBrush( *wxWHITE_BRUSH );
        memdc.SetPen( *wxWHITE_PEN );
        memdc.DrawRectangle(0, 0, A4_WIDTH, A4_HEIGHT);
        memdc.SelectObject( wxNullBitmap );

        rect.SetAttr(0, 0, 0, 0);
        AdjScroll(rect);

        if (bRepaint)
            dc.DrawBitmap( m_bmpEdit, 0, 0 );
    }

    return bNew;
}

void CWBEdit::SaveExec( )
{
#if wxUSE_FILEDLG
    wxImage image = m_bmpEdit.ConvertToImage();

    Enable(false);

    wxString savefilename = wxFileSelector( _("Save"),
                                            wxEmptyString,
                                            wxEmptyString,
                                            (const wxChar *)NULL,
                                            wxT("BMP (*.bmp)|*.bmp|")
                                            wxT("JPEG (*.jpg)|*.jpg"),
                                            wxFD_SAVE,
                                            this);

    if ( savefilename.empty() )
        return;

    wxString extension;
    wxFileName::SplitPath(savefilename, NULL, NULL, &extension);

    if ( extension.Lower() == _T("bmp") )
    {
        image.SaveFile(savefilename, wxBITMAP_TYPE_BMP);
    }
    else if ( extension.Lower() == _T("jpg") )
    {
        image.SaveFile(savefilename, wxBITMAP_TYPE_JPEG);
    }

    Enable(true);
#endif // wxUSE_FILEDLG
}

void CWBEdit::PrintExec( )
{
    wxPrintDialogData printDialogData(*m_printData);

    wxPrinter printer(&printDialogData);

    CWBPrintout printout(_("IP.TV White Board"));
    printout.SetPageSetupData(m_pageSetupData);
    printout.SetPageImage(&m_bmpEdit);

    Enable(false);

    if (!printer.Print(this, &printout, true))
    {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
            wxMessageBox(_("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
        else
            wxMessageBox(_("You canceled printing"), _("Printing"), wxOK);
    }
    else
    {
        (*m_printData) = printer.GetPrintDialogData().GetPrintData();
    }

    Enable(true);
}

void CWBEdit::PageSetup( )
{
    (*m_pageSetupData) = *m_printData;

    wxPageSetupDialog pageSetupDialog(this, m_pageSetupData);
    pageSetupDialog.ShowModal();

    (*m_printData) = pageSetupDialog.GetPageSetupDialogData().GetPrintData();
    (*m_pageSetupData) = pageSetupDialog.GetPageSetupDialogData();
}

void CWBEdit::CutExec( )
{
    if (m_bAreaSelected)
    {
        if (GetProcessPtr())
            GetProcessPtr()->OnEditCutArea(m_AreaPos1, m_AreaPos2);
    }
}

void CWBEdit::SelArea( PtPos& pt1, PtPos& pt2 )
{
    wxClientDC dc(this);
    wxPen pen(*wxBLACK, 1, wxDOT);

    int   oldLogicalFunction = dc.GetLogicalFunction();
    wxPen oldPen = dc.GetPen();

    dc.SetLogicalFunction( wxEQUIV );
    dc.SetPen(pen);
    dc.DrawRectangle(m_AreaPos1.X, m_AreaPos1.Y, m_AreaPos2.X - m_AreaPos1.X, m_AreaPos2.Y - m_AreaPos1.Y);
    dc.DrawRectangle(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    m_AreaPos1.SetAttr( pt1.X, pt1.Y );
    m_AreaPos2.SetAttr( pt2.X, pt2.Y );

    dc.SetLogicalFunction(oldLogicalFunction);
    dc.SetPen(oldPen);
}

void CWBEdit::SelRect( PtPos& pt1, PtPos& pt2 )
{
    wxClientDC dc(this);
    wxPen pen(*wxBLACK, 1);

    int   oldLogicalFunction = dc.GetLogicalFunction();
    wxPen oldPen = dc.GetPen();

    dc.SetLogicalFunction( wxEQUIV );
    dc.SetPen(pen);
    dc.DrawRectangle(m_AreaPos1.X, m_AreaPos1.Y, m_AreaPos2.X - m_AreaPos1.X, m_AreaPos2.Y - m_AreaPos1.Y);
    dc.DrawRectangle(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    m_AreaPos1.SetAttr( pt1.X, pt1.Y );
    m_AreaPos2.SetAttr( pt2.X, pt2.Y );

    dc.SetLogicalFunction(oldLogicalFunction);
    dc.SetPen(oldPen);
}

void CWBEdit::SelEllipse( PtPos& pt1, PtPos& pt2 )
{
    wxClientDC dc(this);
    wxPen pen(*wxBLACK, 1);

    int   oldLogicalFunction = dc.GetLogicalFunction();
    wxPen oldPen = dc.GetPen();

    dc.SetLogicalFunction( wxEQUIV );
    dc.SetPen(pen);
    dc.DrawEllipse(m_AreaPos1.X, m_AreaPos1.Y, m_AreaPos2.X - m_AreaPos1.X, m_AreaPos2.Y - m_AreaPos1.Y);
    dc.DrawEllipse(pt1.X, pt1.Y, pt2.X - pt1.X, pt2.Y - pt1.Y);

    m_AreaPos1.SetAttr( pt1.X, pt1.Y );
    m_AreaPos2.SetAttr( pt2.X, pt2.Y );

    dc.SetLogicalFunction(oldLogicalFunction);
    dc.SetPen(oldPen);
}

void CWBEdit::SelLine( PtPos& pt1, PtPos& pt2 )
{
    wxClientDC dc(this);
    wxPen pen(*wxBLACK, 1);

    int   oldLogicalFunction = dc.GetLogicalFunction();
    wxPen oldPen = dc.GetPen();

    dc.SetLogicalFunction( wxEQUIV );
    dc.SetPen(pen);
    dc.DrawLine(m_AreaPos1.X, m_AreaPos1.Y, m_AreaPos2.X, m_AreaPos2.Y);
    dc.DrawLine(pt1.X, pt1.Y, pt2.X, pt2.Y);

    m_AreaPos1.SetAttr( pt1.X, pt1.Y );
    m_AreaPos2.SetAttr( pt2.X, pt2.Y );

    dc.SetLogicalFunction(oldLogicalFunction);
    dc.SetPen(oldPen);
}

bool CWBEdit::SelImage( )
{
    bool ret = false;

#if wxUSE_FILEDLG
    Enable(false);

    wxString openfilename = wxFileSelector( _("Open"),
                                            wxEmptyString,
                                            wxEmptyString,
                                            (const wxChar *)NULL,
                                            wxT("BMP (*.bmp)|*.bmp|")
                                            wxT("JPEG (*.jpg)|*.jpg"),
                                            wxFD_OPEN|wxFD_FILE_MUST_EXIST,
                                            this);

    if ( openfilename.empty() )
        return ret;

    if (GetProcessPtr())
    {
        GetProcessPtr()->OnToolBoxImageSel( openfilename.ToAscii() );
        ret = true;
    }

    Enable(true);
#endif // wxUSE_FILEDLG

    return ret;
}

void CWBEdit::SetScrollPos( int posx, int posy )
{
    m_scrPosX = posx;
    m_scrPosY = posy;

    AdjClipRect();
}


/*!
 * Should we show tooltips?
 */

bool CWBEdit::ShowToolTips()
{
    return true;
}

wxFont CWBEdit::GetTextFont(FontDef& fd)
{
    wxFont fnSel;

    if ( fd.fdName[0] != '\0' )
    {
        wxString name = wxString::FromAscii(fd.fdName);

        fnSel.SetFaceName(name);
        fnSel.SetPointSize(fd.fdSize);

        if (fd.fdEffect & FD_UNDERLINE)
            fnSel.SetUnderlined(true);
        if (fd.fdEffect & FD_ITALIC)
            fnSel.SetStyle(wxFONTSTYLE_ITALIC);
        if (fd.fdEffect & FD_BOLD)
            fnSel.SetWeight(wxFONTWEIGHT_BOLD);
    }
    else
    {
        fnSel = GetFont();
        fnSel.SetPointSize(FONT_SIZE_DEF);
    }

    return fnSel;
}

void CWBEdit::WrapText(const wxString& src, wxDC& dc, wxRect& rect, wxString& dest)
{
    int cpos = 0;
    int fpos = 0;
    int lspc = -1;
    int nlin = 1;

    dest.Clear();

    if (!src.IsEmpty())
    {
        wxString line;
        wxSize lsize;
        bool bEnter = false;

        while (cpos < (int)src.Len())
        {
            dest.Append(src.GetChar(cpos));

            if ( wxString(dest.Last()) == _T(" ") )
            {
                if (bEnter)
                {
                    bEnter = false;
                    lspc = cpos;
                }
            }
            else
            {
                bEnter = true;
            }

            line = dest.Mid(fpos);
            lsize = dc.GetTextExtent(line);

            if (lsize.GetWidth() > rect.GetWidth())
            {
                if (lspc >= 0)
                {
                    dest.Remove(lspc);
                    dest.Append(_T("\n"));

                    cpos = lspc;
                    fpos = cpos + 1;
                    lspc = -1;
                    nlin++;
                }
            }

            if ( (nlin * lsize.GetHeight()) > rect.GetWidth() )
            {
                cpos = (int) src.Len();
            }

            cpos++;
        }
    }
}

void CWBEdit::AdjClipRect()
{
    wxWindow* pwnd = GetParent();

    if (pwnd)
    {
        int cx, cy, width, height;
        wxRect rc = GetRect();

        pwnd->GetClientSize(&cx, &cy);

        if (rc.GetWidth() < (m_scrPosX+cx))
            width = rc.GetWidth() - m_scrPosX;
        else
            width = cx;

        if (rc.GetHeight() < (m_scrPosY+cy))
            height = rc.GetHeight() - m_scrPosY;
        else
            height = cy;

        m_rcClipRect = wxRect(m_scrPosX, m_scrPosY, width, height);
    }
}

void CWBEdit::AdjScroll(RcPos& rc)
{
    wxWindow* pwnd = GetParent();

    if (pwnd)
    {
        int cx, cy, cr, cb;

        int dx = 0;
        int dy = 0;

        pwnd->GetClientSize(&cx, &cy);

        cr = m_scrPosX + cx;
        cb = m_scrPosY + cy;

        if (rc.Left < m_scrPosX || rc.Right < m_scrPosX)
        {
            if (rc.Left < rc.Right)
                dx = rc.Left - m_scrPosX;
            else
                dx = rc.Right - m_scrPosX;
        }
        else if (rc.Left > cr || rc.Right > cr)
        {
            if (rc.Left > rc.Right)
                dx = rc.Left - cr;
            else
                dx = rc.Right - cr;
        }

        if (rc.Top < m_scrPosY || rc.Bottom < m_scrPosY)
        {
            if (rc.Top < rc.Bottom)
                dy = rc.Top - m_scrPosY;
            else
                dy = rc.Bottom - m_scrPosY;
        }
        else if (rc.Top > cb || rc.Bottom > cb)
        {
            if (rc.Top > rc.Bottom)
                dy = rc.Top - cb;
            else
                dy = rc.Bottom - cb;
        }

        if (dx != 0 || dy != 0)
        {
            m_scrPosX += dx;
            m_scrPosY += dy;

            if ( m_scrPosX < 0 )
                m_scrPosX = 0;
            if ( m_scrPosY < 0 )
                m_scrPosY = 0;

            AdjClipRect();

            if (GetProcessPtr())
                GetProcessPtr()->OnScreenScrollWindow( m_scrPosX, m_scrPosY );
        }
    }
}


/*!
 * Get bitmap resources
 */

wxBitmap CWBEdit::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin CWBEdit bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end CWBEdit bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon CWBEdit::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBEdit icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end CWBEdit icon retrieval
}

wxCursor CWBEdit::GetCursorResource( const wxString& name )
{
    // Icon retrieval
////@begin CWBEdit cursor retrieval
    wxImage image;

    if (name == _T("../resource/wb_del_cur.xpm"))
    {
        image = wxImage(wb_del_cur);
    }
    else if (name == _T("../resource/wb_txt_cur.xpm"))
    {
        image = wxImage(wb_txt_cur);
    }
    else if (name == _T("../resource/wb_mrk_cur.xpm"))
    {
        image = wxImage(wb_mrk_cur);
    }
    else if (name == _T("../resource/wb_pen_cur.xpm"))
    {
        image = wxImage(wb_pen_cur);
    }
    else if (name == _T("../resource/wb_ptr_cur.xpm"))
    {
        image = wxImage(wb_ptr_cur);
    }
    else if (name == _T("../resource/wb_img_cur.xpm"))
    {
        image = wxImage(wb_img_cur);
    }
    else if (name == _T("../resource/wb_lin_cur.xpm"))
    {
        image = wxImage(wb_lin_cur);
    }
    else if (name == _T("../resource/wb_sqr_cur.xpm"))
    {
        image = wxImage(wb_sqr_cur);
    }
    else if (name == _T("../resource/wb_sqf_cur.xpm"))
    {
        image = wxImage(wb_sqf_cur);
    }
    else if (name == _T("../resource/wb_crc_cur.xpm"))
    {
        image = wxImage(wb_crc_cur);
    }
    else if (name == _T("../resource/wb_crf_cur.xpm"))
    {
        image = wxImage(wb_crf_cur);
    }
    else
    {
        image = wxImage(wb_pos_cur);
    }

    image.SetMaskColour(0, 255, 0);
    wxCursor cursor(image);

    return cursor;
////@end CWBEdit cursor retrieval
}

/*!
 * wxEVT_LEFT_DOWN event handler for IDW_WB_EDIT
 */

void CWBEdit::OnLeftDown( wxMouseEvent& event )
{
    if (!m_bAreaSelected)
    {
        m_AreaPos1.SetAttr( 0, 0 );
        m_AreaPos2.SetAttr( 0, 0 );
    }

    if ( GetProcessPtr() )
    {
        PtPos pos;

        pos.SetAttr( event.GetX(), event.GetY() );
        GetProcessPtr()->OnMouseLButtonDown( WB_KEY_NONE, pos );
    }

    CaptureMouse();

    event.Skip();
}

/*!
 * wxEVT_LEFT_UP event handler for IDW_WB_EDIT
 */

void CWBEdit::OnLeftUp( wxMouseEvent& event )
{
    if (HasCapture())
        ReleaseMouse();

    if ( GetProcessPtr() )
    {
        PtPos pos;

        pos.SetAttr( event.GetX(), event.GetY() );
        GetProcessPtr()->OnMouseLButtonUp( WB_KEY_NONE, pos );
    }

    event.Skip();
}

/*!
 * wxEVT_MOTION event handler for IDW_WB_EDIT
 */

void CWBEdit::OnMotion( wxMouseEvent& event )
{
    MouseFlag flag;
    PtPos pos;

    if ( event.LeftIsDown() )
        flag = WB_KEY_LBUTTON;
    else
        flag = WB_KEY_NONE;

    if (flag == WB_KEY_LBUTTON)
    {
        int cx, cy;

        int posX = event.GetX();
        int posY = event.GetY();

        GetClientSize(&cx, &cy);

        if ( posX < 0 )
            posX = 0;
        else if ( posX > cx )
            posX = cx;

        if ( posY < 0 )
            posY = 0;
        else if ( posY > cy )
            posY = cy;

        if ( GetProcessPtr() )
        {
            int scrX = 0;
            int scrY = 0;

            if ( posX < m_rcClipRect.GetRight() && posX >= (m_rcClipRect.GetRight() - DIF_SCROLL) )
                scrX++;
            else if ( m_rcClipRect.GetLeft() > 0 && posX <= (m_rcClipRect.GetLeft() + DIF_SCROLL) )
                scrX--;

            if ( posY < m_rcClipRect.GetBottom() && posY >= (m_rcClipRect.GetBottom() - DIF_SCROLL) )
                scrY++;
            else if ( m_rcClipRect.GetTop() > 0 && posY <= (m_rcClipRect.GetTop() + DIF_SCROLL) )
                scrY--;

            if ( scrX != 0 || scrY != 0 )
            {
                m_scrPosX += scrX * DIF_SCROLL;
                m_scrPosY += scrY * DIF_SCROLL;

                if ( m_scrPosX < 0 )
                    m_scrPosX = 0;
                if ( m_scrPosY < 0 )
                    m_scrPosY = 0;

                AdjClipRect();

                GetProcessPtr()->OnScreenScrollWindow( m_scrPosX, m_scrPosY );

                WarpPointer(posX, posY);
            }

            pos.SetAttr( posX, posY );
            GetProcessPtr()->OnMouseMove( flag, pos );
        }
    }
    else
    {
        if ( GetProcessPtr() )
        {
            pos.SetAttr( event.GetX(), event.GetY() );
            GetProcessPtr()->OnMouseMove( flag, pos );
        }
    }

    event.Skip();
}

void CWBEdit::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc(this);

    dc.DrawBitmap(m_bmpEdit, 0, 0);

    if (m_bAreaSelected)
    {
        wxPen pen(*wxBLACK, 1, wxDOT);

        int   oldLogicalFunction = dc.GetLogicalFunction();
        wxPen oldPen = dc.GetPen();

        dc.SetPen(pen);
        dc.SetLogicalFunction( wxEQUIV );
        dc.DrawRectangle(m_AreaPos1.X, m_AreaPos1.Y, m_AreaPos2.X - m_AreaPos1.X, m_AreaPos2.Y - m_AreaPos1.Y);

        dc.SetLogicalFunction(oldLogicalFunction);
        dc.SetPen(oldPen);
    }
}

void CWBEdit::OnSetFocus( wxFocusEvent& event )
{
    if (GetProcessPtr())
        GetProcessPtr()->OnScreenScrollWindow(m_scrPosX, m_scrPosY);

    AdjClipRect();

    event.Skip();
}

void CWBEdit::OnCtlTextUpdated( wxCommandEvent &event )
{
    if (m_pText)
    {
        wxString text = m_pText->GetValue();

        // Se o último caracter for ENTER, elimina da string
        if (text.Right(1) == _T("\n"))
        {
            text.RemoveLast();
            m_pText->ChangeValue(text);
            m_pText->SetInsertionPointEnd();
        }
    }

    event.Skip();
}

//////////////////////////////////////////////////////////////
// CWBPrintout Class

CWBPrintout::CWBPrintout(const wxChar *title) : wxPrintout( title )
{
    m_pageSetupData = NULL;
    m_bmpImage = NULL;
}

bool CWBPrintout::OnPrintPage(int page)
{
    bool ret = false;

    wxDC *dc = GetDC();

    if (dc)
    {
        DrawPage();
        ret = true;
    }
    else
    {
        ret = false;
    }

    return ret;
}

bool CWBPrintout::OnBeginDocument(int startPage, int endPage)
{
    bool ret = true;

    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        ret = false;

    return ret;
}

void CWBPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool CWBPrintout::HasPage(int pageNum)
{
    return (pageNum == 1);
}

void CWBPrintout::DrawPage()
{
    wxDC *dc = GetDC();

    if (dc && m_pageSetupData && m_bmpImage)
    {
        // Valores máximos para a área de edição
        wxCoord maxX = A4_WIDTH;
        wxCoord maxY = A4_HEIGHT;

        // Ajusta as dimensões da área de edição ao papel selecionado
        FitThisSizeToPageMargins(wxSize(maxX, maxY), *m_pageSetupData);
        wxRect fitRect = GetLogicalPageMarginsRect(*m_pageSetupData);

        // Centraliza a área de edição
        wxCoord xoff = (fitRect.width - maxX) / 2;
        wxCoord yoff = (fitRect.height - maxY) / 2;
        OffsetLogicalOrigin(xoff, yoff);

        // Imprime a imagem
        dc->DrawBitmap(*m_bmpImage, 0, 0);
    }
}
