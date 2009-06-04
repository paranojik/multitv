// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "UnicastConfigDialog.h"
#include "Profile.h"


BEGIN_EVENT_TABLE( UnicastConfigDialog, wxDialog )
	EVT_BUTTON( ID_BTN_OK, UnicastConfigDialog::OnBtnOkClick )
	EVT_BUTTON( ID_BTN_CANCEL, UnicastConfigDialog::OnBtnCancelClick )
END_EVENT_TABLE()

/** UnicastConfigDialog default constructor.
 *
 */
UnicastConfigDialog::UnicastConfigDialog()
{
    Init();
}

/** UnicastConfigDialog constructor.
* @param[in] parent. Parent window.
* @param[in] iface. Interface with application.
* @param[in] id. Window id. Default SYMBOL_UNICASTDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_UNICASTDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_UNICASTDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_UNICASTDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_UNICASTDIALOG_STYLE.
*/
UnicastConfigDialog::UnicastConfigDialog( wxWindow* parent, AppInterface* iface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
	m_appInterface = iface;
    Create(parent, id, caption, pos, size, style);
}


/** UnicastConfigDialog creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_UNICASTDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_UNICASTDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_UNICASTDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_UNICASTDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_UNICASTDIALOG_STYLE.
*/
bool UnicastConfigDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );
    
	CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    return true;
}


/** UnicastConfigDialog destructor.
 * 
 */
UnicastConfigDialog::~UnicastConfigDialog()
{
}


/*!
 * Member initialisation
 */
void UnicastConfigDialog::Init()
{
    m_dialogSizer = NULL;
    m_serverBox = NULL;
    m_serverSizer = NULL;
    m_lblServerName = NULL;
    m_txtServerName = NULL;
    m_lblServerPassword = NULL;
    m_txtServerPassword = NULL;
    m_userBox = NULL;
    m_userSizer = NULL;
    m_lblUserName = NULL;
    m_txtUserName = NULL;
    m_lblUserPassword = NULL;
    m_txtUserPassword = NULL;
    m_ctrlSizer = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;
}


/*!
 * Control creation for UnicastConfigDialog
 */
void UnicastConfigDialog::CreateControls()
{
    UnicastConfigDialog* itemDialog1 = this;

    m_dialogSizer = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_dialogSizer);

    m_serverBox = new wxStaticBox(itemDialog1, wxID_ANY, LABEL_DLG_UNICAST_SERVER_BOX);
    m_serverSizer = new wxStaticBoxSizer(m_serverBox, wxHORIZONTAL);
    m_dialogSizer->Add(m_serverSizer, 0, wxGROW|wxALL, 5);

    m_lblServerName = new wxStaticText( itemDialog1, wxID_STATIC, LABEL_DLG_UNICAST_SERVER_NAME, wxDefaultPosition, wxDefaultSize, 0 );
    m_serverSizer->Add(m_lblServerName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtServerName = new wxTextCtrl( itemDialog1, ID_TXT_SERVER_NAME, wxEmptyString, wxDefaultPosition, wxSize(150, -1), 0 );
    m_serverSizer->Add(m_txtServerName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblServerPassword = new wxStaticText( itemDialog1, wxID_STATIC, LABEL_DLG_UNICAST_SERVER_PASSWORD, wxDefaultPosition, wxDefaultSize, 0 );
    m_serverSizer->Add(m_lblServerPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtServerPassword = new wxTextCtrl( itemDialog1, ID_TXT_SERVER_PASSWORD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_serverSizer->Add(m_txtServerPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_userBox = new wxStaticBox(itemDialog1, wxID_ANY, LABEL_DLG_UNICAST_USER_BOX);
    m_userSizer = new wxStaticBoxSizer(m_userBox, wxHORIZONTAL);
    m_dialogSizer->Add(m_userSizer, 0, wxGROW|wxALL, 5);

    m_lblUserName = new wxStaticText( itemDialog1, wxID_STATIC, LABEL_DLG_UNICAST_USER_NAME, wxDefaultPosition, wxDefaultSize, 0 );
    m_userSizer->Add(m_lblUserName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUserName = new wxTextCtrl( itemDialog1, ID_TXT_USER_NAME, wxEmptyString, wxDefaultPosition, wxSize(150, -1), 0 );
    m_userSizer->Add(m_txtUserName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblUserPassword = new wxStaticText( itemDialog1, wxID_STATIC, LABEL_DLG_UNICAST_USER_PASSWORD, wxDefaultPosition, wxDefaultSize, 0 );
    m_userSizer->Add(m_lblUserPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_txtUserPassword = new wxTextCtrl( itemDialog1, ID_TXT_USER_PASSWORD, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    m_userSizer->Add(m_txtUserPassword, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_dialogSizer->Add(m_ctrlSizer, 0, wxALIGN_RIGHT|wxALL, 5);

    m_btnOk = new wxButton( itemDialog1, ID_BTN_OK, BUTTON_OK_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, ID_BTN_CANCEL, BUTTON_CANCEL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	// Load info from profile
	Profile profile = m_appInterface->GetSettingsManager()->GetSelectedProfile();
	m_txtServerName->SetValue(profile.GetServerName());
	m_txtServerPassword->SetValue(profile.GetServerPassword());
	m_txtUserName->SetValue(profile.GetUserName());
	m_txtUserPassword->SetValue(profile.GetUserPassword());

	//Set focus to txtServerName
	m_txtServerName->SetFocus();

	// Set layout
	m_serverSizer->Layout();
	m_userSizer->Layout();
	m_ctrlSizer->Layout();
	// Set window size to the sizer
	Fit();
}

// TransferDataToWindow: Performs pre-loading upon dialog invocation
bool UnicastConfigDialog::TransferDataToWindow()
{
	return true;
}

/** Performs validation before dismissing dialog.
* @return Returns true if validate occurred successfully or false if somthing generated a error.
*/
bool UnicastConfigDialog::TransferDataFromWindow()
{
	// Get server(name and password) and user(name and password) from the TextCtrls
	wxString strServerName = m_txtServerName->GetValue();
	wxString strServerPassword = m_txtServerPassword->GetValue();
	wxString strUserName = m_txtUserName->GetValue();
	wxString strUserPassword = m_txtUserPassword->GetValue();
	// Replace blank password with a default one
	if(strUserPassword == wxEmptyString)
		strUserPassword = DEFAULT_PASSWORD;
	// Server name: test only for empty string
	if(strServerName == wxEmptyString)
	{
		wxMessageBox(DLG_UNICAST_SERVER_NAME_EMPTY,
			MESSAGE_ERROR_CAPTION, wxICON_ERROR | wxOK, this);
		return false;
	}
	// User name: test only for empty string
	if(strUserName == wxEmptyString)
	{
		wxMessageBox(DLG_UNICAST_USER_NAME_EMPTY,
			MESSAGE_ERROR_CAPTION, wxICON_ERROR | wxOK, this);			
		return false;
	}
	// Password is not validated.
	m_config.SetMode(APP_MODE_CONFERENCE);
	m_config.SetConfServer(strServerName);
	m_config.SetConfServerPass(strServerPassword);
	m_config.SetConfUser(strUserName);
	m_config.SetConfPass(strUserPassword);
	m_config.SetConfRealName(DEFAULT_REAL_NAME);
	m_config.SetConfEmail(DEFAULT_EMAIL);
	return true;
}

/** Returns the current configuration in the dialog.
*
*/
const AppConfig &UnicastConfigDialog::GetConfig() const
{
	return m_config;
}

void UnicastConfigDialog::OnBtnOkClick(wxCommandEvent& event)
{
	if(TransferDataFromWindow())
	{
		Profile lastProfile = m_appInterface->GetSettingsManager()->GetSelectedProfile();

		// Create a profile with the new values and the old chann
		Profile profile(wxT("Latest"), m_txtUserName->GetValue(), m_txtUserPassword->GetValue(),
						m_txtServerName->GetValue(), m_txtServerPassword->GetValue(),
						lastProfile.GetChannelName(), lastProfile.GetChannelPassword());

		// Modify the Latest profile with the new information stored in profile
		m_appInterface->GetSettingsManager()->ModifyProfile(wxT("Latest"), profile);

		// Set the last profile used as Latest profile
		m_appInterface->GetSettingsManager()->SetProfile(wxT("Latest"));

		EndModal(wxID_OK);
	}
}

void UnicastConfigDialog::OnBtnCancelClick(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

/*!
 * Should we show tooltips?
 */
bool UnicastConfigDialog::ShowToolTips()
{
    return true;
}
