/////////////////////////////////////////////////////////////////////////////
// Name:        SettingsManagerDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     14/05/2008 11:10:34
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef SETTINGS_MANAGER_DIALOG_H
#define SETTINGS_MANAGER_DIALOG_H


/*!
 * Forward declarations
 */
class wxBoxSizer;
class wxGridSizer;

/*!
 * Control identifiers
 */
#define SYMBOL_SETTINGSMANAGERFRAME_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxFRAME_NO_TASKBAR
#define SYMBOL_SETTINGSMANAGERFRAME_IDNAME ID_SETMANFRM
#define SYMBOL_SETTINGSMANAGERFRAME_SIZE wxSize(200, 200)
#define SYMBOL_SETTINGSMANAGERFRAME_POSITION wxDefaultPosition


enum{
	ID_SETMANFRM = 600,
	ID_SETMANFRM_MAINPANEL,
	ID_SETMAN_FRM_LST_PROFILES,
	ID_SETMANFRM_TXT_PROFILENAME,
	ID_SETMANFRM_TXT_USER,
	ID_SETMANFRM_TXT_USERPWD,
	ID_SETMANFRM_TXT_SERVER,
	ID_SETMANFRM_TXT_SERVERPWD,
	ID_SETMANFRM_TXT_CHANNEL,
	ID_SETMANFRM_TXT_CHANNELPWD,
	ID_SETMANFRM_BTN_SELECT,
	ID_SETMANFRM_BTN_CANCEL,
	ID_SETMANFRM_BTN_NEW,
	ID_SETMANFRM_BTN_DELETE,
	ID_SETMANFRM_BTN_SAVE,
	ID_SETMANFRM_BTN_IMPORT,
	ID_SETMANFRM_BTN_EXPORT
};


/** @brief Class thats implements settings manager dialog.
*
* Manages the profile creation, editing, and others features.
*/
class SettingsManagerDialog: public wxDialog
{
private:
    DECLARE_CLASS( SettingsManagerDialog )
    DECLARE_EVENT_TABLE()

	enum State
	{
		idle,
		latestSelected,
		newProfile,
		editingProfile
	};

	AppInterface* m_appInterface;
	State m_state;

    wxBoxSizer* m_mainVertSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_mainPanelHorSizer;
    wxListBox* m_lstProfiles;
    wxBoxSizer* m_mainPanelVertSizerSizer;
    wxFlexGridSizer* m_profileConfigGridSizer;
    wxStaticText* m_lblProfileName;
    wxTextCtrl* m_txtProfileName;
    wxStaticText* m_lblUser;
    wxTextCtrl* m_txtUser;
    wxStaticText* m_lblUserPwd;
    wxTextCtrl* m_txtUserPwd;
    wxStaticText* m_lblServer;
    wxTextCtrl* m_txtServer;
    wxStaticText* m_lblServerPwd;
    wxTextCtrl* m_txtServerPwd;
    wxStaticText* m_lblChannel;
    wxTextCtrl* m_txtChannel;
    wxStaticText* m_lblChannelPwd;
    wxTextCtrl* m_txtChannelPwd;
    wxBoxSizer* m_ctrlSizer;
    wxButton* m_btnSelect;
    wxButton* m_btnCancel;
    wxButton* m_btnNew;
    wxButton* m_btnDelete;
	wxButton* m_btnSave;
	wxButton* m_btnImport;
	wxButton* m_btnExport;
	
	void FillTextCtrls(Profile profile);
	void LatestSelectedState();
	void NewProfileState();
	void EditingProfileState();
	void IdleState();

public:
    // Constructors
    SettingsManagerDialog();
    SettingsManagerDialog( wxWindow* parent, AppInterface* iface , wxWindowID id = SYMBOL_SETTINGSMANAGERFRAME_IDNAME, const wxString& caption = SYMBOL_SETTINGSMANAGERFRAME_TITLE, const wxPoint& pos = SYMBOL_SETTINGSMANAGERFRAME_POSITION, const wxSize& size = SYMBOL_SETTINGSMANAGERFRAME_SIZE, long style = SYMBOL_SETTINGSMANAGERFRAME_STYLE);

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SETTINGSMANAGERFRAME_IDNAME, const wxString& caption = SYMBOL_SETTINGSMANAGERFRAME_TITLE, const wxPoint& pos = SYMBOL_SETTINGSMANAGERFRAME_POSITION, const wxSize& size = SYMBOL_SETTINGSMANAGERFRAME_SIZE, long style = SYMBOL_SETTINGSMANAGERFRAME_STYLE );

    // Destructor
    ~SettingsManagerDialog();

    // Initialises member variables
    void Init();

    // Creates the controls and sizers
    void CreateControls();

    // Events
    void OnSelectClick( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
    void OnNewClick( wxCommandEvent& event );
    void OnDeleteClick( wxCommandEvent& event );
    void OnSaveClick( wxCommandEvent& event );
	void OnImportClick( wxCommandEvent& event );
	void OnExportClick( wxCommandEvent& event );
	void OnProfileSelected( wxCommandEvent& event );
	void OnTextChanged( wxCommandEvent& event );

    // Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    // Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    // Should we show tooltips?
    static bool ShowToolTips();
};

#endif
    // SETTINGS_MANAGER_DIALOG_H
