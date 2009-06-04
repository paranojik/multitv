/////////////////////////////////////////////////////////////////////////////
// Name:        MessageDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     18/03/2008 10:59:54
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _MESSAGEDIALOG_H_
#define _MESSAGEDIALOG_H_


/*!
 * Forward declarations
 */
class wxBoxSizer;

/*!
 * Control identifiers
 */
enum{
	ID_MESSAGEDIALOG = 8000,
	ID_MESSAGEDIALOG_BTN_YES,
	ID_MESSAGEDIALOG_BTN_NO
};

////@begin control identifiers
#define SYMBOL_MESSAGEDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_MESSAGEDIALOG_TITLE _("Disconnect Dialog")
#define SYMBOL_MESSAGEDIALOG_IDNAME ID_MESSAGEDIALOG
#define SYMBOL_MESSAGEDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_MESSAGEDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/** @brief Class thats implements message dialog.
*
*/
class MessageDialog: public wxDialog
{
////@begin MessageDialog member variables
    wxBoxSizer* m_labelSizer;
    wxStaticText* m_lblLabel;
    wxBoxSizer* m_ctrlSizer;
    wxButton* m_btnYes;
    wxButton* m_btnNo;
	wxString m_label;
////@end MessageDialog member variables

public:
    // Constructors
    MessageDialog();
    MessageDialog( wxWindow* parent, wxWindowID id = SYMBOL_MESSAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_MESSAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_MESSAGEDIALOG_POSITION, const wxSize& size = SYMBOL_MESSAGEDIALOG_SIZE, long style = SYMBOL_MESSAGEDIALOG_STYLE );
	MessageDialog(wxWindow *parent, const wxString &title, const wxString &label);
    // Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MESSAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_MESSAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_MESSAGEDIALOG_POSITION, const wxSize& size = SYMBOL_MESSAGEDIALOG_SIZE, long style = SYMBOL_MESSAGEDIALOG_STYLE );

    // Destructor
    ~MessageDialog();

    // Initialises member variables
    void Init();

    // Creates the controls and sizers
    void CreateControls();

	//MessageDialog event handler declarations
	void OnClose(wxCloseEvent &event);
	void OnBtnNoClick( wxCommandEvent& event );
	void OnBtnYesClick( wxCommandEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif
    // _MESSAGEDIALOG_H_
