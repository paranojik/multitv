#ifndef _WBOARDFRAME_H_
#define _WBOARDFRAME_H_

/*!
 * Includes
 */

////@begin includes
#include "wx/wx.h"
////@end includes

#include "wx/print.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

#include "Resource.h"
#include "WBoardBridge.h"

////@begin forward declarations
class CWBMain;
class wxFlexGridSizer;
class CWBToolBox;
class CWBMenuBar;
class CWBScreen;
class wxGridSizer;
class CWBEdit;
////@end forward declarations

/*!
 * Control identifiers
 */

#define SYMBOL_CWBFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX
#define SYMBOL_CWBFRAME_TITLE _("White Board")
#define SYMBOL_CWBFRAME_IDNAME ID_WBOARDFRAME
#define SYMBOL_CWBFRAME_SIZE wxSize(800, 600)
#define SYMBOL_CWBFRAME_POSITION wxDefaultPosition
#define SYMBOL_CWBMAIN_STYLE wxNO_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_CWBMAIN_IDNAME wxID_ANY
#define SYMBOL_CWBMAIN_SIZE wxDefaultSize
#define SYMBOL_CWBMAIN_POSITION wxDefaultPosition
#define SYMBOL_CWBTOOLBOX_STYLE wxNO_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_CWBTOOLBOX_IDNAME wxID_ANY
#define SYMBOL_CWBTOOLBOX_SIZE wxSize(600, 40)
#define SYMBOL_CWBTOOLBOX_POSITION wxPoint(0, 40)
#define SYMBOL_CWBMENUBAR_STYLE wxNO_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_CWBMENUBAR_IDNAME wxID_ANY
#define SYMBOL_CWBMENUBAR_SIZE wxSize(70, 400)
#define SYMBOL_CWBMENUBAR_POSITION wxPoint(70, 0)
#define SYMBOL_CWBSCREEN_STYLE wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB
#define SYMBOL_CWBSCREEN_IDNAME wxID_ANY
#define SYMBOL_CWBSCREEN_SIZE wxSize(100, 100)
#define SYMBOL_CWBSCREEN_POSITION wxDefaultPosition
#define SYMBOL_CWBEDIT_STYLE wxSIMPLE_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_CWBEDIT_IDNAME IDW_WB_EDIT
#define SYMBOL_CWBEDIT_SIZE wxSize(800, 600)
#define SYMBOL_CWBEDIT_POSITION wxPoint(0, 0)


/*!
 * CWBFrame class declaration
 */

class CWBFrame: public wxFrame, public WBProcessRef
{    
    DECLARE_CLASS( CWBFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CWBFrame();
    CWBFrame( wxWindow* parent, IWBProcess* pProcess, wxWindowID id = SYMBOL_CWBFRAME_IDNAME, const wxString& caption = SYMBOL_CWBFRAME_TITLE, const wxPoint& pos = SYMBOL_CWBFRAME_POSITION, const wxSize& size = SYMBOL_CWBFRAME_SIZE, long style = SYMBOL_CWBFRAME_STYLE );

    bool Create( wxWindow* parent, IWBProcess* pProcess, wxWindowID id = SYMBOL_CWBFRAME_IDNAME, const wxString& caption = SYMBOL_CWBFRAME_TITLE, const wxPoint& pos = SYMBOL_CWBFRAME_POSITION, const wxSize& size = SYMBOL_CWBFRAME_SIZE, long style = SYMBOL_CWBFRAME_STYLE );

    /// Destructor
    ~CWBFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin CWBFrame event handler declarations

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_NEW
    void OnMWbNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_SAVE
    void OnMWbSaveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_WB_PRINT
    void OnWbPrintClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_WB_PAGE_SETUP
    void OnWbPageSetupClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_CLOSE
    void OnMWbCloseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_UNDO
    void OnMWbUndoClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_CUT
    void OnMWbCutClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_IMG_STOP
    void OnMWbImgStopClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_HELP_TOPICS
    void OnMWbHelpTopicsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for IDM_WB_ABOUT
    void OnMWbAboutClick( wxCommandEvent& event );

    /// wxEVT_CLOSE_WINDOW event handler for ID_WBOARDFRAME
    void OnCloseWindow( wxCloseEvent& event );

////@end CWBFrame event handler declarations

////@begin CWBFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CWBFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CWBFrame member variables
////@end CWBFrame member variables

    IWBInterface* GetInterfacePtr();

    bool Resize(RcPos* pRc);
    void ShowWnd(bool bOp);
    void EnableWnd(bool bOp);
    void EnableImgCtrl( bool bOp );
    void EnableImgStop( bool bOp );

private:

    wxMenuBar* m_pMenuBar;
    wxMenu*    m_pMenuFile;
    wxMenu*    m_pMenuEdit;
    wxMenu*    m_pMenuCtrl;
    wxMenu*    m_pMenuHelp;
    CWBMain*   m_pwbMain;
};

/*!
 * CWBMenuBar class declaration
 */

class CWBMenuBar: public wxPanel, public WBProcessRef
{    
    DECLARE_DYNAMIC_CLASS( CWBMenuBar )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CWBMenuBar();
    CWBMenuBar(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint(70, 0), const wxSize& size = wxSize(600, 40), long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint(70, 0), const wxSize& size = wxSize(600, 40), long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~CWBMenuBar();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin CWBMenuBar event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_NEW
    void OnWbNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SAVE
    void OnWbSaveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PRINT
    void OnWbPrintClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CUT
    void OnWbCutClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_UNDO
    void OnWbUndoClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_FTPSTOP
    void OnWbFtpstopClick( wxCommandEvent& event );

////@end CWBMenuBar event handler declarations

////@begin CWBMenuBar member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    void ImgStopEnable( bool bOp);
////@end CWBMenuBar member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CWBMenuBar member variables
    wxBitmapButton* m_mbNew;
    wxBitmapButton* m_mbSave;
    wxBitmapButton* m_mbPrint;
    wxBitmapButton* m_mbCut;
    wxBitmapButton* m_mbUndo;
    wxBitmapButton* m_mbImgStop;
////@end CWBMenuBar member variables
};

/*!
 * CWBToolBox class declaration
 */

class CWBToolBox: public wxPanel, public WBProcessRef
{    
    DECLARE_DYNAMIC_CLASS( CWBToolBox )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CWBToolBox();
    CWBToolBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint(0, 40), const wxSize& size = wxSize(70, 400), long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint(0, 40), const wxSize& size = wxSize(70, 400), long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~CWBToolBox();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin CWBToolBox event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CUR
    void OnWbPosClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_DEL
    void OnWbDelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_TXT
    void OnWbTxtClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_MRK
    void OnWbMrkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PTR
    void OnWbPtrClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_IMG
    void OnWbImgClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_PEN
    void OnWbPenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LIN
    void OnWbLinClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQR
    void OnWbSqrClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_SQF
    void OnWbSqfClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRC
    void OnWbCrcClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_CRF
    void OnWbCrfClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE1
    void OnWbLine1Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE2
    void OnWbLine2Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE3
    void OnWbLine3Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_LINE4
    void OnWbLine4Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_COLOR_SEL
    void OnWbColorSelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WB_FONT_SEL
    void OnWbFontSelClick( wxCommandEvent& event );

////@end CWBToolBox event handler declarations

////@begin CWBToolBox member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CWBToolBox member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    wxBitmap m_tbPSize1Bitmap;
    wxBitmap m_tbPSize1BitmapSel;
    wxBitmap m_tbPSize2Bitmap;
    wxBitmap m_tbPSize2BitmapSel;
    wxBitmap m_tbPSize3Bitmap;
    wxBitmap m_tbPSize3BitmapSel;
    wxBitmap m_tbPSize4Bitmap;
    wxBitmap m_tbPSize4BitmapSel;

    wxBitmap m_tbPosBitmap;
    wxBitmap m_tbPosBitmapSel;
    wxBitmap m_tbDelBitmap;
    wxBitmap m_tbDelBitmapSel;
    wxBitmap m_tbTxtBitmap;
    wxBitmap m_tbTxtBitmapSel;
    wxBitmap m_tbMRKBitmap;
    wxBitmap m_tbMRKBitmapSel;
    wxBitmap m_tbPtrBitmap;
    wxBitmap m_tbPtrBitmapSel;
    wxBitmap m_tbImgBitmap;
    wxBitmap m_tbImgBitmapSel;
    wxBitmap m_tbPenBitmap;
    wxBitmap m_tbPenBitmapSel;
    wxBitmap m_tbLinBitmap;
    wxBitmap m_tbLinBitmapSel;
    wxBitmap m_tbSqrBitmap;
    wxBitmap m_tbSqrBitmapSel;
    wxBitmap m_tbSqfBitmap;
    wxBitmap m_tbSqfBitmapSel;
    wxBitmap m_tbCrcBitmap;
    wxBitmap m_tbCrcBitmapSel;
    wxBitmap m_tbCrfBitmap;
    wxBitmap m_tbCrfBitmapSel;

////@begin CWBToolBox member variables
    wxBitmapButton* m_tbPos;
    wxBitmapButton* m_tbDel;
    wxBitmapButton* m_tbTxt;
    wxBitmapButton* m_tbMRK;
    wxBitmapButton* m_tbPtr;
    wxBitmapButton* m_tbImg;
    wxBitmapButton* m_tbPen;
    wxBitmapButton* m_tbLin;
    wxBitmapButton* m_tbSqr;
    wxBitmapButton* m_tbSqf;
    wxBitmapButton* m_tbCrc;
    wxBitmapButton* m_tbCrf;
    wxBitmapButton* m_tbPSize1;
    wxBitmapButton* m_tbPSize2;
    wxBitmapButton* m_tbPSize3;
    wxBitmapButton* m_tbPSize4;
    wxStaticText* m_tbColor;
    wxButton* m_tbColorSel;
    wxStaticText* m_tbFont;
    wxStaticText* m_tbFontLen;
    wxButton* m_tbFontSel;
    wxStaticBox* m_tbPSBox;
////@end CWBToolBox member variables

    void SelectItem(int nId);
    void EnableItem(int nId, bool bOp);
    void SelectPenSize(int nId);
    void SelectColour(wxColour& colour);
    void SelectFont(wxFont& font);

private:
    int m_CurItemId;
    int m_CurPenSize;
    wxColour m_Colour;
    wxFont   m_Font;
};

/*!
 * CWBMain class declaration
 */

class CWBMain: public wxPanel, public IWBInterface, public WBProcessRef
{    
    DECLARE_DYNAMIC_CLASS( CWBMain )
    DECLARE_EVENT_TABLE()

public:

    /// Constructors
    CWBMain();
    CWBMain(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~CWBMain();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin CWBMain event handler declarations

////@end CWBMain event handler declarations

////@begin CWBMain member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CWBMain member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CWBMain member variables
    wxFlexGridSizer* m_MainSizer;
////@end CWBMain member variables

    void SetProcessObj(IWBProcess* pProcess);
    IWBInterface* GetInterfacePtr();

    /////////////////////////////////////////////////
    // IWBInterface class functions

    void ToolBoxItemSel( int nId );
    void ToolBoxItemEnable( int nId, bool bOp);
    void ToolBoxPenSizeSel( int nId );
    void ToolBoxColorSel( ColorDef& color );
    void ToolBoxFontSel( FontDef& font );

    bool MenuBarNewExec( bool bChanged, bool bRepaint );
    void MenuBarSaveExec( );
    void MenuBarPrintExec( );
    void MenuBarPageSetup( );
    void MenuBarCutExec( );
    void MenuBarUndoExec( );
    void MenuBarImageStopExec( );
    void MenuBarImageStopEnable( bool bOp );

    void EdtSelArea( PtPos& pt1, PtPos& pt2 );
    void EdtSelRect( PtPos& pt1, PtPos& pt2 );
    void EdtSelEllipse( PtPos& pt1, PtPos& pt2 );
    void EdtSelLine( PtPos& pt1, PtPos& pt2 );

    void EdtDrawArea( PtPos& pt1, PtPos& pt2 );
    void EdtKillArea( );
    void EdtRepaint( );

    void ScrScrollWindow( int dx, int dy );
    void ScrSetScrollPos( int posx, int posy );

    void CtlDrawRect( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags );
    void CtlDrawEllipse( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags );
    void CtlDrawLine( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bMask, WB_BYTE flags );
    void CtlDrawIndicator( PtPos& pt, WB_BYTE flags );
    void CtlDrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2, WB_BYTE flags );
    void CtlDrawTxt( WB_PCSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color, WB_BYTE flags );
    void CtlEditTxt( PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color );
    void CtlGetTxt( WB_PSTR szText );

    /////////////////////////////////////////////////

protected:

//    void 

private:

    CWBToolBox*   m_pwbToolBox;
    CWBMenuBar*   m_pwbMenuBar;
    CWBScreen*    m_pwbScreen;
    CWBEdit*      m_pwbEdit;
};

/*!
 * CWBScreen class declaration
 */

class CWBScreen: public wxScrolledWindow, public WBProcessRef
{    
    DECLARE_DYNAMIC_CLASS( CWBScreen )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CWBScreen();
    CWBScreen(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(100, 100), long style = wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(100, 100), long style = wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL|wxALWAYS_SHOW_SB);

    /// Destructor
    ~CWBScreen();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    CWBEdit* GetWBEdit() { return m_pwbEdit; }

////@begin CWBScreen event handler declarations

    /// EVT_SCROLLWIN_LINEUP event handler for wxID_ANY
    void OnScrLineUp( wxScrollWinEvent& event );

    /// EVT_SCROLLWIN_LINEDOWN event handler for wxID_ANY
    void OnScrLineDown( wxScrollWinEvent& event );

    /// EVT_SCROLLWIN_PAGEUP event handler for wxID_ANY
    void OnScrPageUp( wxScrollWinEvent& event );

    /// EVT_SCROLLWIN_PAGEDOWN event handler for wxID_ANY
    void OnScrPageDown( wxScrollWinEvent& event );

    /// EVT_SCROLLWIN_THUMBRELEASE event handler for wxID_ANY
    void OnScrThumbRelease( wxScrollWinEvent& event );

    /// wxEVT_SIZE event handler for wxID_ANY
    void OnSize( wxSizeEvent& event );

    /// wxEVT_PAINT event handler for wxID_ANY
    void OnPaint( wxPaintEvent& event );

////@end CWBScreen event handler declarations

////@begin CWBScreen member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end CWBScreen member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CWBScreen member variables
    wxGridSizer* m_EditSizer;
    bool m_bIsScrolled;
////@end CWBScreen member variables

    void ScrollWnd(int dx, int dy);
    void AdjScrollPos();

private:
    CWBEdit* m_pwbEdit;
};

/*!
 * CWBEdit class declaration
 */

class CWBEdit: public wxPanel, public WBProcessRef
{    
    DECLARE_DYNAMIC_CLASS( CWBEdit )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    CWBEdit();
    CWBEdit(wxWindow* parent, wxWindowID id = IDW_WB_EDIT, const wxPoint& pos = wxPoint(0, 0), const wxSize& size = wxSize(800, 600), long style = wxSIMPLE_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = IDW_WB_EDIT, const wxPoint& pos = wxPoint(0, 0), const wxSize& size = wxSize(800, 600), long style = wxSIMPLE_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~CWBEdit();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    void SetControl(int nId);
    void SetPenSize(int nId);
    void SetFrgColour(wxColour& colour);
    void SetTxtFont(wxFont& font);
    
    void DrawArea( PtPos& pt1, PtPos& pt2 );
    void KillArea( );
    void Repaint( );

    void DrawRect( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags );
    void DrawEllipse( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bFull, WB_BYTE flags );
    void DrawLine( PtPos& pt1, PtPos& pt2, int nLin, ColorDef& color, bool bMask, WB_BYTE flags );
    void DrawIndicator( PtPos& pt, WB_BYTE flags );
    void DrawImage( WB_PCSTR szFile, PtPos& pt1, PtPos& pt2, WB_BYTE flags );
    void DrawTxt( WB_PCSTR szText, PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color, WB_BYTE flags );
    void EditTxt( PtPos& pt1, PtPos& pt2, FontDef& font, ColorDef& color );
    void GetTxt( WB_PSTR szText );

    bool NewExec( bool bChanged, bool bRepaint );
    void SaveExec( );
    void PrintExec( );
    void PageSetup( );
    void CutExec( );

    void SelArea( PtPos& pt1, PtPos& pt2 );
    void SelRect( PtPos& pt1, PtPos& pt2 );
    void SelEllipse( PtPos& pt1, PtPos& pt2 );
    void SelLine( PtPos& pt1, PtPos& pt2 );

    bool SelImage();

    void SetScrollPos( int posx, int posy );

////@begin CWBEdit event handler declarations

    /// wxEVT_LEFT_DOWN event handler for IDW_WB_EDIT
    void OnLeftDown( wxMouseEvent& event );

    /// wxEVT_LEFT_UP event handler for IDW_WB_EDIT
    void OnLeftUp( wxMouseEvent& event );

    /// wxEVT_MOTION event handler for IDW_WB_EDIT
    void OnMotion( wxMouseEvent& event );

    /// wxEVT_PAINT event handler for wxID_ANY
    void OnPaint( wxPaintEvent& event );

    /// wxEVT_SET_FOCUS event handler
    void OnSetFocus( wxFocusEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler
    void OnCtlTextUpdated( wxCommandEvent &event );

////@end CWBEdit event handler declarations

////@begin CWBEdit member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    /// Retrieves cursor resources
    wxCursor GetCursorResource( const wxString& name );
////@end CWBEdit member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin CWBEdit member variables
////@end CWBEdit member variables

protected:

    wxFont GetTextFont(FontDef& fd);
    void WrapText(const wxString& src, wxDC& dc, wxRect& rect, wxString& dest);

    void AdjClipRect();
    void AdjScroll(RcPos& rc);

private:

    wxPen          m_Pen;
    wxColour       m_Colour;
    wxFont         m_Font;
    wxCursor       m_Cursor,
                   m_curPos,
                   m_curDel,
                   m_curTxt,
                   m_curMRK,
                   m_curPtr,
                   m_curImg,
                   m_curPen,
                   m_curLin,
                   m_curCrc,
                   m_curCrf,
                   m_curSqr,
                   m_curSqf;
    wxBitmap       m_bmpPtr;
    int            m_scrPosX,
                   m_scrPosY;
    wxRect         m_rcClipRect;
    bool           m_bAreaSelected;
    PtPos          m_AreaPos1,
                   m_AreaPos2;
    wxBitmap       m_bmpEdit;
    wxTextCtrl*    m_pText;

    wxPrintData* m_printData;
    wxPageSetupDialogData* m_pageSetupData;
};


class CWBPrintout: public wxPrintout
{
public:

    CWBPrintout(const wxChar *title);

    void SetPageSetupData(wxPageSetupDialogData* pData) { m_pageSetupData = pData; }
    void SetPageImage(wxBitmap* pBmp) { m_bmpImage = pBmp; }

    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

    void DrawPage();

private:

    wxBitmap* m_bmpImage;
    wxPageSetupDialogData* m_pageSetupData;
};

#endif
    // _WBOARDFRAME_H_
