#include <wx/wx.h>

//#include "IPTVViewerApp.h"

#include "MulticastConfigDialog.h"

/** Default constructor.
*
*/
MulticastConfigDialog::MulticastConfigDialog(wxWindow *parent, wxWindowID id, 
											 const wxString &title, 
											 const wxPoint &pos, const wxSize &size,
											 long style, const wxString &name)
{
	Create(parent, id, title, pos, size, style, name);
	m_parent = parent;
}

/** Destructor.
*
*/
MulticastConfigDialog::~MulticastConfigDialog()
{
}

bool MulticastConfigDialog::Create(wxWindow *parent, wxWindowID id, 
							  const wxString &title, 
							  const wxPoint &pos, const wxSize &size, 
							  long style, const wxString &name)
{
	wxDialog::Create(parent, id, title, pos, size, style, name);
	CreateControls();
	return true;
}

void MulticastConfigDialog::CreateControls()
{
	// Create controls
	m_lblHost = new wxStaticText(this, wxID_ANY, _("Host"));
	m_lblPort = new wxStaticText(this, wxID_ANY, _("Port"));
	m_txtHost = new wxTextCtrl(this, wxID_ANY);
	m_txtPort = new wxTextCtrl(this, wxID_ANY);
	// Set controls' properties
	// Create a digit-only validator
	wxArrayString includeList;
	for(int i=0; i<10; i++)
		includeList.Add(wxString::Format(_("%d"),i));
	wxTextValidator digitOnlyValidator(wxFILTER_INCLUDE_CHAR_LIST);
	digitOnlyValidator.SetIncludes(includeList);

	m_txtPort->SetValidator(digitOnlyValidator); // Only digits (0-9) are allowed
	m_txtPort->SetMaxLength(5);	// port ranges from 0 to 65535

	// Create sizers
	wxSizer *vSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(vSizer);
	// Add controls to sizers
	hSizer->Add(m_lblHost, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND, 4);
	hSizer->Add(m_txtHost, 5, wxRIGHT | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND, 16);
	hSizer->Add(m_lblPort, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND, 4);
	hSizer->Add(m_txtPort, 3, wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL | wxEXPAND);
	vSizer->Add(hSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 16);
	vSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxRIGHT | wxBOTTOM | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 16);
	// Set layout
	hSizer->Layout();
	vSizer->Layout();
	// Set window size to the sizer
	Fit();

	m_txtHost->SetFocus();
}

/** TransferDataToWindow: Performs pre-loading upon dialog invocation
*
*/
bool MulticastConfigDialog::TransferDataToWindow()
{
	m_config = m_appInterface->GetAppConfig();

	if(m_config.GetMode() == APP_MODE_MULTICAST)
	{
		wxString strPort;
		strPort << m_config.GetMcastPort();
		m_txtHost->SetValue(m_config.GetMcastAddress());
		m_txtPort->SetValue(strPort);
	}
	return true;
}

/** TransferDataFromWindow: Performs validation before dismissing dialog
*
*/
bool MulticastConfigDialog::TransferDataFromWindow()
{
	// Get host and port from the TextCtrls
	wxString strHost = m_txtHost->GetValue();
	wxString strPort = m_txtPort->GetValue();
	// Host name: test only for empty string
	if(strHost == wxEmptyString)
	{
		wxMessageBox(_("Host name must not be blank."),
			_("Error"), wxICON_ERROR | wxOK, this);
		return false;
	}

	// Port number: test for empty string...
	if(strPort == wxEmptyString)
	{
		wxMessageBox(_("Port number must not be blank."),
			_("Error"), wxICON_ERROR | wxOK, this);
		return false;
	}
	// ... and for range.
	unsigned long port;
	strPort.ToULong(&port);
	if(port > 65535)
	{
		wxMessageBox(_("Port number must be between 0 and 65535."),
			_("Error"), wxICON_ERROR | wxOK, this);
		return false;
	}		
	// All validation OK, set config and dismiss
	m_config.SetMode(APP_MODE_MULTICAST);
	m_config.SetMcastAddress(strHost);
	m_config.SetMcastPort(port);
	return true;
}

/** Sets linkage with application.
* @param[in] appInterface Application interface
*/
void MulticastConfigDialog::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
}

/** Returns the current configuration in the dialog.
* @return AppConfig.
*/
const AppConfig &MulticastConfigDialog::GetConfig() const
{
	return m_config;
}
