/////////////////////////////////////////////////////////////////////////////
// Name:        RecordingDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     04/11/2008 10:01:03
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RECORDINGDIALOG_H_
#define _RECORDINGDIALOG_H_


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
enum{ID_RECORDINGDIALOG = 10003,
	 ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING,
	 ID_RECORDINGDIALOG_TXT_FILEPATH,
	 ID_RECORDINGDIALOG_BTN_SEARCH,
	 ID_RECORDINGDIALOG_BTN_CONFIRM,
	 ID_RECORDINGDIALOG_BTN_CANCEL
};

#define SYMBOL_RECORDINGDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_RECORDINGDIALOG_TITLE _("Recording Properties")
#define SYMBOL_RECORDINGDIALOG_IDNAME ID_RECORDINGDIALOG
#define SYMBOL_RECORDINGDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_RECORDINGDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/** @brief Class thats implements recording settings dialog.
*
* This dialog define if the conferece must be recorded and where.
*/
class RecordingDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( RecordingDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RecordingDialog();
    RecordingDialog( wxWindow* parent, wxWindowID id = SYMBOL_RECORDINGDIALOG_IDNAME, const wxString& caption = SYMBOL_RECORDINGDIALOG_TITLE, const wxPoint& pos = SYMBOL_RECORDINGDIALOG_POSITION, const wxSize& size = SYMBOL_RECORDINGDIALOG_SIZE, long style = SYMBOL_RECORDINGDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RECORDINGDIALOG_IDNAME, const wxString& caption = SYMBOL_RECORDINGDIALOG_TITLE, const wxPoint& pos = SYMBOL_RECORDINGDIALOG_POSITION, const wxSize& size = SYMBOL_RECORDINGDIALOG_SIZE, long style = SYMBOL_RECORDINGDIALOG_STYLE );

    /// Destructor
    ~RecordingDialog();

    /// Initialises member variables
    void Init();

	void InitControls();

    /// Creates the controls and sizers
    void CreateControls();

	void SetRecordingEnable(const bool &status);
	bool GetRecordingStatus(){return m_isRecordingEnable;}

	void SetRecordingFilePath(const wxString &path);
	wxString GetRecordingFilePath(){return m_filePath;}

////@begin RecordingDialog event handler declarations

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING
    void OnChkboxEnableRecordingClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_SEARCH
    void OnBtnSearchClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CONFIRM
    void OnBtnConfirmClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CANCEL
    void OnBtnCancelClick( wxCommandEvent& event );

	void OnClose(wxCloseEvent &event);

////@end RecordingDialog event handler declarations

////@begin RecordingDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end RecordingDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin RecordingDialog member variables
    wxBoxSizer* m_mainVertSizer;
    wxCheckBox* m_chkBoxEnableRecording;
    wxStaticText* m_lblFilePathCaption;
    wxBoxSizer* m_filePathHorSizer;
    wxTextCtrl* m_txtFilePath;
    wxButton* m_btnSearch;
    wxBoxSizer* m_ctrlHorSizer;
    wxButton* m_btnConfirm;
    wxButton* m_btnCancel;

	bool m_isRecordingEnable;
	wxString m_filePath;
////@end RecordingDialog member variables
};

#endif
    // _RECORDINGDIALOG_H_
