/////////////////////////////////////////////////////////////////////////////
// Name:        SettingsManagerDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     14/05/2008 11:10:34
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "AppInterface.h"
#include "SkinLiteConstants.h"
#include "SettingsManagerDialog.h"
#include "MessageDialog.h"
#include "SettingsManager.h"
#include "Profile.h"


/*!
 * SettingsManagerDialog type definition
 */
IMPLEMENT_CLASS( SettingsManagerDialog, wxDialog )


/*!
 * SettingsManagerDialog event table definition
 */
BEGIN_EVENT_TABLE( SettingsManagerDialog, wxDialog )
    EVT_BUTTON( ID_SETMANFRM_BTN_SELECT, SettingsManagerDialog::OnSelectClick )
    EVT_BUTTON( ID_SETMANFRM_BTN_CANCEL, SettingsManagerDialog::OnCancelClick )
    EVT_BUTTON( ID_SETMANFRM_BTN_NEW, SettingsManagerDialog::OnNewClick )
    EVT_BUTTON( ID_SETMANFRM_BTN_DELETE, SettingsManagerDialog::OnDeleteClick )
	EVT_BUTTON( ID_SETMANFRM_BTN_SAVE, SettingsManagerDialog::OnSaveClick )
	EVT_BUTTON( ID_SETMANFRM_BTN_IMPORT, SettingsManagerDialog::OnImportClick )
	EVT_BUTTON( ID_SETMANFRM_BTN_EXPORT, SettingsManagerDialog::OnExportClick )
	EVT_LISTBOX( ID_SETMAN_FRM_LST_PROFILES, SettingsManagerDialog::OnProfileSelected )
	EVT_TEXT(ID_SETMANFRM_TXT_PROFILENAME, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_USER, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_USERPWD, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_SERVER, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_SERVERPWD, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_CHANNEL, SettingsManagerDialog::OnTextChanged )
	EVT_TEXT(ID_SETMANFRM_TXT_CHANNELPWD, SettingsManagerDialog::OnTextChanged )
END_EVENT_TABLE()


/** SettingsManagerDialog default constructor.
 * 
 */
SettingsManagerDialog::SettingsManagerDialog()
{
    Init();
}

/** SettingsManagerDialog constructor.
* @param[in] parent. Parent window.
* @param[in] iface. Interface with application.
* @param[in] id. Window id. Default SYMBOL_SETTINGSMANAGERFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SETTINGSMANAGERFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SETTINGSMANAGERFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SETTINGSMANAGERFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SETTINGSMANAGERFRAME_STYLE.
*/
SettingsManagerDialog::SettingsManagerDialog( wxWindow* parent, AppInterface* iface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
	m_appInterface = iface;
    Create( parent, id, caption, pos, size, style );
}


/** SettingsManagerDialog creator.
* @param[in] parent. Parent window.
* @param[in] iface. Interface with application.
* @param[in] id. Window id. Default SYMBOL_SETTINGSMANAGERFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SETTINGSMANAGERFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SETTINGSMANAGERFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SETTINGSMANAGERFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SETTINGSMANAGERFRAME_STYLE.
*/
bool SettingsManagerDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxDialog::Create( parent, id, caption, pos, size, style );
	SetMinSize(SYMBOL_SETTINGSMANAGERFRAME_SIZE);
    CreateControls();

	// Find Latest profile in original list and replace it with its translation
	wxArrayString profileList = m_appInterface->GetSettingsManager()->GetProfilesList();
	int position = profileList.Index(wxT("Latest"));
	profileList.Remove(wxT("Latest"));
	profileList.Insert(_("Latest"), position);

	// Fill listbox with profile list
	m_lstProfiles->InsertItems(profileList, 0);

	// If Latest profile selected, it must be translated before selection
	wxString profileName = m_appInterface->GetSettingsManager()->GetSelectedProfileName();
	if(profileName == wxT("Latest"))
		profileName = _("Latest");

	// Set selection to last profile name and fill all the textcontrols
	m_lstProfiles->SetStringSelection(profileName);
	FillTextCtrls(m_appInterface->GetSettingsManager()->GetSelectedProfile());

	if(m_lstProfiles->GetStringSelection() == _("Latest"))
	{
		// Change to LatestSelected state
		LatestSelectedState();
	}
	else
	{
		// Change to Idle state
		IdleState();
	}

    return true;
}


/** SettingsManagerDialog destructor.
 * 
 */
SettingsManagerDialog::~SettingsManagerDialog()
{
}


/*!
 * Member initialisation
 */
void SettingsManagerDialog::Init()
{
    m_mainVertSizer = NULL;
    m_mainPanel = NULL;
    m_mainPanelHorSizer = NULL;
    m_lstProfiles = NULL;
    m_mainPanelVertSizerSizer = NULL;
    m_profileConfigGridSizer = NULL;
    m_lblProfileName = NULL;
    m_txtProfileName = NULL;
    m_lblUser = NULL;
    m_txtUser = NULL;
    m_lblUserPwd = NULL;
    m_txtUserPwd = NULL;
    m_lblServer = NULL;
    m_txtServer = NULL;
    m_lblServerPwd = NULL;
    m_txtServerPwd = NULL;
    m_lblChannel = NULL;
    m_txtChannel = NULL;
    m_lblChannelPwd = NULL;
    m_txtChannelPwd = NULL;
    m_ctrlSizer = NULL;
    m_btnSelect = NULL;
    m_btnCancel = NULL;
    m_btnNew = NULL;
    m_btnDelete = NULL;
	m_btnSave = NULL;
	m_btnImport = NULL;
	m_btnExport = NULL;
}


/*!
 * Control creation for SettingsManagerDialog
 */
void SettingsManagerDialog::CreateControls()
{
    SettingsManagerDialog* itemFrame1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_SETMANFRM_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertSizer->Add(m_mainPanel, 1, wxGROW, 0);

    m_mainPanelHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanel->SetSizer(m_mainPanelHorSizer);

    m_lstProfiles = new wxListBox( m_mainPanel, ID_SETMAN_FRM_LST_PROFILES);
	m_lstProfiles->SetMinSize(wxSize(100, 200));
    m_mainPanelHorSizer->Add(m_lstProfiles, 0, wxGROW|wxALL, 0);

    m_mainPanelVertSizerSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanelHorSizer->Add(m_mainPanelVertSizerSizer, 1, wxGROW|wxALL, 0);

    m_profileConfigGridSizer = new wxFlexGridSizer(7, 3, 0, 0);
	m_mainPanelVertSizerSizer->Add(m_profileConfigGridSizer, 1, wxGROW|wxALL, 5);

	// line 1
    m_lblProfileName = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_PROFILENAME_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblProfileName, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtProfileName = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_PROFILENAME, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    m_profileConfigGridSizer->Add(m_txtProfileName, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnNew = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_NEW, SETMANFRM_BTN_CREATE_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_btnNew, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// line 2
    m_lblUser = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_USER_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblUser, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUser = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_USER, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    m_profileConfigGridSizer->Add(m_txtUser, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_btnSave = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_SAVE, SETMANFRM_BTN_SAVE_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_btnSave, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// line 3
    m_lblUserPwd = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_USERPWD_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblUserPwd, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUserPwd = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_USERPWD, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD );
    m_profileConfigGridSizer->Add(m_txtUserPwd, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnDelete = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_DELETE, SETMANFRM_BTN_DELETE_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_btnDelete, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// line 4
    m_lblServer = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_SERVER_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblServer, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtServer = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_SERVER, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    m_profileConfigGridSizer->Add(m_txtServer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_profileConfigGridSizer->AddSpacer(0);

	// line 5
    m_lblServerPwd = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_SERVERPWD_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblServerPwd, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_txtServerPwd = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_SERVERPWD, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD );
    m_profileConfigGridSizer->Add(m_txtServerPwd, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnImport = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_IMPORT, SETMANFRM_BTN_IMPORT_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_btnImport, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// line 6
    m_lblChannel = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_CHANNEL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblChannel, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtChannel = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_CHANNEL, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    m_profileConfigGridSizer->Add(m_txtChannel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnExport = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_EXPORT, SETMANFRM_BTN_EXPORT_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_btnExport, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// line 7
	m_lblChannelPwd = new wxStaticText( m_mainPanel, wxID_STATIC, SETMANFRM_LBL_CHANNELPWD_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_profileConfigGridSizer->Add(m_lblChannelPwd, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtChannelPwd = new wxTextCtrl( m_mainPanel, ID_SETMANFRM_TXT_CHANNELPWD, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD );
    m_profileConfigGridSizer->Add(m_txtChannelPwd, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_profileConfigGridSizer->AddSpacer(0);
	// end grid sizer

    m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanelVertSizerSizer->Add(m_ctrlSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    m_btnSelect = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_SELECT, SETMANFRM_BTN_SELECT_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnSelect, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( m_mainPanel, ID_SETMANFRM_BTN_CANCEL, SETMANFRM_BTN_CANCEL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_mainVertSizer->Fit(this);
}


/*!
 * Should we show tooltips?
 */
bool SettingsManagerDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */
wxBitmap SettingsManagerDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

/*!
 * Get icon resources
 */
wxIcon SettingsManagerDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETMANFRM_BTN_SELECT
 */
void SettingsManagerDialog::OnSelectClick( wxCommandEvent& event )
{
	m_appInterface->GetSettingsManager()->SetProfile(m_txtProfileName->GetValue());
	this->Close();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETMANFRM_BTN_CANCEL
 */
void SettingsManagerDialog::OnCancelClick( wxCommandEvent& event )
{
	// If canceling the save a new profile
	if(m_state == newProfile)
	{
		// Set selection to Latest profile
		m_lstProfiles->SetStringSelection(_("Latest"));
		FillTextCtrls(m_appInterface->GetSettingsManager()->GetProfile(wxT("Latest")));

		// Change to LatestSelected state
		LatestSelectedState();
	}
	// If canceling the save of a modified profile
	else if(m_state == editingProfile)
	{
		// Undo the changes
		FillTextCtrls(m_appInterface->GetSettingsManager()->GetProfile(m_lstProfiles->GetStringSelection()));

		// Change to Idle state
		IdleState();
	}
	else
	{
		this->Close();
	}
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETMANFRM_BTN_CREATE
 */
void SettingsManagerDialog::OnNewClick( wxCommandEvent& event )
{

	// Deselect
	m_lstProfiles->Deselect(m_lstProfiles->GetSelection());

	// Blank all TextCtrls
	Profile profile;
	FillTextCtrls(profile);

	// Change to NewProfile State
	NewProfileState();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETMANFRM_BTN_DELETE
 */
void SettingsManagerDialog::OnDeleteClick( wxCommandEvent& event )
{
	// cannot delete Latest
	if(m_lstProfiles->GetStringSelection() == _("Latest"))
	{
		wxMessageBox(_("Cannot delete Latest profile."));
		return;
	}

	// Confirmation dialog
	MessageDialog confirmationDialog(this,_("Delete"), wxString::Format(_("Do you want to delete the profile %s ?"), m_lstProfiles->GetStringSelection().c_str()));
	int answer = confirmationDialog.ShowModal();
	if (answer == ID_MESSAGEDIALOG_BTN_NO)
		return;

	// Delete profile
	m_appInterface->GetSettingsManager()->DeleteProfile(m_lstProfiles->GetStringSelection());

	// Delete profile name from listbox
	m_lstProfiles->Delete(m_lstProfiles->GetSelection());

	// Set selection to Latest profile
	m_lstProfiles->SetStringSelection(_("Latest"));
	FillTextCtrls(m_appInterface->GetSettingsManager()->GetProfile(wxT("Latest")));
	
	// Change to LatestSelected state
	LatestSelectedState();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SETMANFRM_BTN_SAVE
 */
void SettingsManagerDialog::OnSaveClick( wxCommandEvent& event )
{
	if(m_txtProfileName->GetValue() == wxEmptyString)
	{
		wxMessageBox(_("Cannot create profile without a name."));
		return;
	}

	Profile profile;
	profile.SetName(m_txtProfileName->GetValue());
	profile.SetUserName(m_txtUser->GetValue());
	profile.SetUserPassword(m_txtUserPwd->GetValue());
	profile.SetServerName(m_txtServer->GetValue());
	profile.SetServerPassword(m_txtServerPwd->GetValue());
	profile.SetChannelName(m_txtChannel->GetValue());
	profile.SetChannelPassword(m_txtChannelPwd->GetValue());

	// If saving a modified profile
	if(m_state == editingProfile)
	{
		// If it could be modified
		if(m_appInterface->GetSettingsManager()->ModifyProfile(m_lstProfiles->GetStringSelection(), profile))
		{
			// Delete profile name from listbox, append the new one and set selection to it
			m_lstProfiles->Delete(m_lstProfiles->GetSelection());
			m_lstProfiles->Append(profile.GetName());
			m_lstProfiles->SetStringSelection(profile.GetName());

			// Change to Idle state
			IdleState();

			wxMessageBox(_("Profile saved successfully."));
		}
		else	// If it couldn't be modified
		{
			wxMessageBox(_("Cannot save profile."));
		}
	}
	// If saving a new profile
	else
	{
		// If it could be added
		if(m_appInterface->GetSettingsManager()->AddProfile(profile))
		{
			// Append the new profile name to the listbox and set selection to it
			wxString msg = profile.GetName();
			m_lstProfiles->Append(msg);
			m_lstProfiles->SetStringSelection(profile.GetName());

			// Change to Idle state
			IdleState();

			wxMessageBox(_("Profile saved successfully."));
		}
		else	// If it couldn't be added
		{
			wxMessageBox(_("Cannot save profile."));
		}
	}
}

void SettingsManagerDialog::OnImportClick( wxCommandEvent& event )
{
	wxFileDialog fileDialog(this, _("Open File"), wxEmptyString, wxEmptyString, _("XML files (*.xml)|*.xml"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if(fileDialog.ShowModal() == wxID_OK)
	{
		Profile profile;

		// import profile
		if(!profile.ImportFromXml(fileDialog.GetPath()))
		{
			wxMessageBox(_("Cannot import profile."));
			return;
		}

		// add profile to SettingsManager
		if(m_appInterface->GetSettingsManager()->AddProfile(profile))
		{
			FillTextCtrls(profile);
			m_lstProfiles->Append(profile.GetName());
			m_lstProfiles->SetStringSelection(profile.GetName());
			wxMessageBox(_("Profile imported successfully."));
		}
		else
		{
			wxMessageBox(_("Cannot save imported profile."));
		}

	}
}

void SettingsManagerDialog::OnExportClick( wxCommandEvent& event )
{
	wxFileDialog fileDialog(this, _("Save File"), wxEmptyString, wxEmptyString, _("XML files (*.xml)|*.xml"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if(fileDialog.ShowModal() == wxID_OK)
	{
		// get the selected profile
		Profile profile = m_appInterface->GetSettingsManager()->GetProfile(m_lstProfiles->GetStringSelection());

		// write to file
		profile.ExportToXml(fileDialog.GetPath());
	}
}

void SettingsManagerDialog::OnProfileSelected( wxCommandEvent& event )
{
	if(m_lstProfiles->GetStringSelection() == _("Latest"))
	{
		// Change to LatestSelected state
		LatestSelectedState();
	}
	else
	{
		// Change to Idle state
		IdleState();
	}

	// if Latest profile it must look for Latest(in English)
	wxString profileName = m_lstProfiles->GetStringSelection();
	if(profileName == _("Latest"))
		profileName = wxT("Latest");

	FillTextCtrls(m_appInterface->GetSettingsManager()->GetProfile(profileName));
}

void SettingsManagerDialog::OnTextChanged( wxCommandEvent& event )
{
	// Can't change to editing if already in newProfile state
	if(m_state != newProfile)
	{
		// Change to EditingProfile state
		EditingProfileState();
	}
}

/** Fills all fields.
* @param[in] profile. Profile for fill the fields.
* Private method.
*/
void SettingsManagerDialog::FillTextCtrls(Profile profile)
{
	// if Latest profile it must translate before fill
	wxString profileName = profile.GetName();
	if(profileName == wxT("Latest"))
		profileName = _("Latest");

	m_txtProfileName->ChangeValue(profileName);
	m_txtUser->ChangeValue(profile.GetUserName());
	m_txtUserPwd->ChangeValue(profile.GetUserPassword());
	m_txtServer->ChangeValue(profile.GetServerName());
	m_txtServerPwd->ChangeValue(profile.GetServerPassword());
	m_txtChannel->ChangeValue(profile.GetChannelName());
	m_txtChannelPwd->ChangeValue(profile.GetChannelPassword());
}

/** Enable or disable controls when Latest profile is selected
* Private method.
*/
void SettingsManagerDialog::LatestSelectedState()
{
	// Change State
	m_state = latestSelected;

	// Title
	SetTitle(SYMBOL_SETTINGSMANAGERFRAME_TITLE);

	// TextCtrls
	m_txtProfileName->Disable();
    m_txtUser->Disable();
    m_txtUserPwd->Disable();
    m_txtServer->Disable();
    m_txtServerPwd->Disable();
    m_txtChannel->Disable();
    m_txtChannelPwd->Disable();
	
	// ListBox
	m_lstProfiles->Enable();

	// Buttons
	m_btnNew->Enable();
	m_btnSave->Disable();
	m_btnDelete->Disable();
	m_btnImport->Enable();
	m_btnExport->Disable();
	m_btnSelect->Disable();
	m_btnCancel->Enable();
}

/** Enable or disable controls when new profile is being created.
* Private method.
*/
void SettingsManagerDialog::NewProfileState()
{
	// Change State
	m_state = newProfile;

	// Title
	SetTitle(SYMBOL_SETTINGSMANAGERFRAME_TITLE_NEW);

	// TextCtrls
	m_txtProfileName->Enable();
    m_txtUser->Enable();
    m_txtUserPwd->Enable();
    m_txtServer->Enable();
    m_txtServerPwd->Enable();
    m_txtChannel->Enable();
    m_txtChannelPwd->Enable();
	
	// ListBox
	m_lstProfiles->Disable();

	// Buttons
	m_btnNew->Disable();
	m_btnSave->Enable();
	m_btnDelete->Disable();
	m_btnImport->Disable();
	m_btnExport->Disable();
	m_btnSelect->Disable();
	m_btnCancel->Enable();
}

/** Enable or disable controls when editing profile.
* Private method.
*/
void SettingsManagerDialog::EditingProfileState()
{
	// Change State
	m_state = editingProfile;

	// Title
	SetTitle(SYMBOL_SETTINGSMANAGERFRAME_TITLE_EDITING);

	// TextCtrls
	m_txtProfileName->Enable();
    m_txtUser->Enable();
    m_txtUserPwd->Enable();
    m_txtServer->Enable();
    m_txtServerPwd->Enable();
    m_txtChannel->Enable();
    m_txtChannelPwd->Enable();
	
	// ListBox
	m_lstProfiles->Disable();

	// Buttons
	m_btnNew->Disable();
	m_btnSave->Enable();
	m_btnDelete->Disable();
	m_btnImport->Disable();
	m_btnExport->Disable();
	m_btnSelect->Disable();
	m_btnCancel->Enable();
}

/** Enable or disable controls when not doing something special.
* Private method.
*/
void SettingsManagerDialog::IdleState()
{
	// Change State
	m_state = idle;

	// Title
	SetTitle(SYMBOL_SETTINGSMANAGERFRAME_TITLE);

	// TextCtrls
	m_txtProfileName->Enable();
    m_txtUser->Enable();
    m_txtUserPwd->Enable();
    m_txtServer->Enable();
    m_txtServerPwd->Enable();
    m_txtChannel->Enable();
    m_txtChannelPwd->Enable();
	
	// ListBox
	m_lstProfiles->Enable();

	// Buttons
	m_btnNew->Enable();
	m_btnSave->Disable();
	m_btnDelete->Enable();
	m_btnImport->Enable();
	m_btnExport->Enable();
	m_btnSelect->Enable();
	m_btnCancel->Enable();
}
