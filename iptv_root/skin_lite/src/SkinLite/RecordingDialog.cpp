/////////////////////////////////////////////////////////////////////////////
// Name:        RecordingDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     04/11/2008 10:01:03
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

////@begin includes
////@end includes

#include "RecordingDialog.h"
#include "wx/dir.h"

////@begin XPM images
////@end XPM images


/*!
 * RecordingDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RecordingDialog, wxDialog )


/*!
 * RecordingDialog event table definition
 */

BEGIN_EVENT_TABLE( RecordingDialog, wxDialog )

////@begin RecordingDialog event table entries
    EVT_CHECKBOX( ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING, RecordingDialog::OnChkboxEnableRecordingClick )
	EVT_BUTTON( ID_RECORDINGDIALOG_BTN_SEARCH, RecordingDialog::OnBtnSearchClick )
    EVT_BUTTON( ID_RECORDINGDIALOG_BTN_CONFIRM, RecordingDialog::OnBtnConfirmClick )
    EVT_BUTTON( ID_RECORDINGDIALOG_BTN_CANCEL, RecordingDialog::OnBtnCancelClick )
	EVT_CLOSE(RecordingDialog::OnClose)


////@end RecordingDialog event table entries

END_EVENT_TABLE()


/** RecordingDialog default constructor.
 * 
 */
RecordingDialog::RecordingDialog()
{
    Init();
}

/** RecordingDialog constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_RECORDINGDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_RECORDINGDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_RECORDINGDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_RECORDINGDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_RECORDINGDIALOG_STYLE.
*/
RecordingDialog::RecordingDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/** RecordingDialog creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_RECORDINGDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_RECORDINGDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_RECORDINGDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_RECORDINGDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_RECORDINGDIALOG_STYLE.
*/
bool RecordingDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin RecordingDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();

	SetAffirmativeId(ID_RECORDINGDIALOG_BTN_CONFIRM);
	SetEscapeId(ID_RECORDINGDIALOG_BTN_CANCEL);
	InitControls();

    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
////@end RecordingDialog creation
    return true;
}


/** RecordingDialog destructor.
 * 
 */
RecordingDialog::~RecordingDialog()
{
////@begin RecordingDialog destruction
////@end RecordingDialog destruction
}


/*!
 * Member initialisation
 */

void RecordingDialog::Init()
{
////@begin RecordingDialog member initialisation
    m_mainVertSizer = NULL;
    m_chkBoxEnableRecording = NULL;
    m_lblFilePathCaption = NULL;
    m_filePathHorSizer = NULL;
    m_txtFilePath = NULL;
    m_btnSearch = NULL;
    m_ctrlHorSizer = NULL;
    m_btnConfirm = NULL;
    m_btnCancel = NULL;
	m_isRecordingEnable = true;
	m_filePath = wxEmptyString;
////@end RecordingDialog member initialisation
}


/*!
 * Control creation for RecordingDialog
 */

void RecordingDialog::CreateControls()
{    
////@begin RecordingDialog content construction
    RecordingDialog* itemDialog1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_mainVertSizer);

    m_chkBoxEnableRecording = new wxCheckBox( itemDialog1, ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING, _("Enable Recording"), wxDefaultPosition, wxDefaultSize, 0 );
    m_chkBoxEnableRecording->SetValue(true);
    m_mainVertSizer->Add(m_chkBoxEnableRecording, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_mainVertSizer->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_lblFilePathCaption = new wxStaticText( itemDialog1, wxID_STATIC, _("Save recordings in this folder:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_lblFilePathCaption, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_filePathHorSizer = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(m_filePathHorSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxRIGHT|wxBOTTOM, 5);

    m_txtFilePath = new wxTextCtrl( itemDialog1, ID_RECORDINGDIALOG_TXT_FILEPATH, _T(""), wxDefaultPosition, wxSize(300, -1), 0 );
    m_filePathHorSizer->Add(m_txtFilePath, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnSearch = new wxButton( itemDialog1, ID_RECORDINGDIALOG_BTN_SEARCH, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
    m_filePathHorSizer->Add(m_btnSearch, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainVertSizer->Add(m_ctrlHorSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnConfirm = new wxButton( itemDialog1, ID_RECORDINGDIALOG_BTN_CONFIRM, _("Confirm"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlHorSizer->Add(m_btnConfirm, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, ID_RECORDINGDIALOG_BTN_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlHorSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end RecordingDialog content construction
}

/** Initialize controls states.
*
*/
void RecordingDialog::InitControls()
{
	m_txtFilePath->Enable(m_isRecordingEnable);
	m_btnSearch->Enable(m_isRecordingEnable);
	m_chkBoxEnableRecording->SetValue(m_isRecordingEnable);
}

/** Sets if record must be enabled or disabled.
* @param[in] status. If status is equal true, record is going to be enabled, else is going to be disabled.
*/
void RecordingDialog::SetRecordingEnable(const bool &status)
{
	m_isRecordingEnable = status;

	m_txtFilePath->Enable(m_isRecordingEnable);
	m_btnSearch->Enable(m_isRecordingEnable);
	m_chkBoxEnableRecording->SetValue(m_isRecordingEnable);
}

/** Sets the path where record is going to be save.
* @param[in] path. Path to save the record.
*/
void RecordingDialog::SetRecordingFilePath(const wxString &path)
{
	m_filePath = path;
	m_txtFilePath->SetValue(m_filePath);
}

/*!
 * Should we show tooltips?
 */

bool RecordingDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RecordingDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin RecordingDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end RecordingDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RecordingDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin RecordingDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end RecordingDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING
 */

void RecordingDialog::OnChkboxEnableRecordingClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING in RecordingDialog.
	m_txtFilePath->Enable(event.IsChecked());
	m_btnSearch->Enable(event.IsChecked());
////@end wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_RECORDINGDIALOG_CHKBOX_ENABLERECORDING in RecordingDialog. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_SEARCH
 */

void RecordingDialog::OnBtnSearchClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_SEARCH in RecordingDialog.
    wxDirDialog dirDlg(this, _("Choose a directory"), wxEmptyString, wxDD_DIR_MUST_EXIST);
	if (wxDir::Exists(m_filePath))
		dirDlg.SetPath(m_filePath);

	if (dirDlg.ShowModal() == wxID_OK)
	{
		m_txtFilePath->SetValue(dirDlg.GetPath());
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_SEARCH in RecordingDialog. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CONFIRM
 */

void RecordingDialog::OnBtnConfirmClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CONFIRM in RecordingDialog.
    // Before editing this code, remove the block markers.
    if (m_chkBoxEnableRecording->IsChecked())
	{
		if (wxDir::Exists(m_txtFilePath->GetValue()))
		{
			m_filePath = m_txtFilePath->GetValue();
			m_isRecordingEnable = true;
			EndModal(ID_RECORDINGDIALOG_BTN_CONFIRM);
		}
		else
		{
			wxMessageBox(_("Invalid Path"), _("Error"), wxICON_ERROR|wxCENTRE, this);
			EndModal(ID_RECORDINGDIALOG_BTN_CANCEL);
		}
	}
	else
	{
		m_isRecordingEnable = false;
		EndModal(ID_RECORDINGDIALOG_BTN_CONFIRM);
	}

////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CONFIRM in RecordingDialog. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CANCEL
 */

void RecordingDialog::OnBtnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CANCEL in RecordingDialog.
    // Before editing this code, remove the block markers.
	EndModal(ID_RECORDINGDIALOG_BTN_CANCEL);
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RECORDINGDIALOG_BTN_CANCEL in RecordingDialog. 
}

void RecordingDialog::OnClose(wxCloseEvent &event)
{
	EndModal(ID_RECORDINGDIALOG_BTN_CANCEL);
}
