#include <wx/wx.h>

#include "AboutDialog.h"
#include "IPTVClientApp.h"

/** Default constructor.
 *
 */
AboutDialog::AboutDialog() : wxDialog()
{

}
/** Constructor similar to wxDialog constructor, creating the dialog
 * in a single step.
 * @param[in] parent	Parent window.
 * @param[in] id	Window ID.
 * @param[in] title	Window title (displayed in title bar).
 * @param[in] pos	Window position(X,Y).
 * @param[in] size	Window size(width, height).
 * @param[in] style	Window style. See wxWidgets documentation.
 * @param[in] name	Window name. See wxWidgets documentation.
 */
AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id,
		const wxString &title,
		const wxPoint &pos, const wxSize &size,
		long style,	const wxString &name) : wxDialog()
{
	Create(parent, id, title, pos, size, style, name);
}

/** Destructor.
 *
 */
AboutDialog::~AboutDialog()
{
}

/** Creates the dialog. Used in two-step dialog creation.
 * @param[in] parent	Parent window.
 * @param[in] id	Window ID.
 * @param[in] title	Window title (displayed in title bar).
 * @param[in] pos	Window position(X,Y).
 * @param[in] size	Window size(width, height).
 * @param[in] style	Window style. See wxWidgets documentation.
 * @param[in] name	Window name. See wxWidgets documentation.
 * @remarks Do not call this function if the single-step creation constructor is used.
 */
bool AboutDialog::Create(wxWindow *parent, wxWindowID id, 
						 const wxString &title, 
						 const wxPoint &pos, const wxSize &size, 
						 long style, const wxString &name)
{
	bool ret = wxDialog::Create(parent, id, title, pos, size, style, name);
	if(ret)
		CreateControls();
	return ret;
}

/** Creates the controls in the dialog.
 * @remarks This also contains the text used in the About dialog text box.
 */
void AboutDialog::CreateControls()
{
	IPTVClientApp &app = wxGetApp();
	// Create sizers
	wxSizer *horizSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSizer *vertSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *windowSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *skinInfoHeaderLabel = new wxStaticText(this, wxID_ANY, wxT(""));

	// Create controls
	m_sbmpIcon = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxImage(ABOUT_ICON_FILE_NAME)));
	m_lblAbout = new wxStaticText(this, wxID_ANY, wxT(""));
	m_txtAbout = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 
		wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH);
	m_lblSkinInfo = new wxStaticText(this, wxID_ANY, wxT(""));
	m_btnOK = new wxButton(this, wxID_OK);

	// Slap text on the controls
	skinInfoHeaderLabel->SetLabel(_("Skin information:"));

	wxString aboutLabelText;
	aboutLabelText 
		<< wxT("IP.TV (")
#ifdef WIN32
		<< wxT("Windows")
#endif
#ifdef _LINUX
		<< wxT("Linux")
#endif
		<< wxT(")\n")
		<< _("Version ") << app.GetClientVersion() << wxT(" build ") << app.GetClientBuildID() << wxT(" (") << app.GetClientBuildDateStr() << wxT(")\n")
		<< _("Core version ") << app.GetCoreVersion() << wxT("\n")
		<< wxT("Copyright 2008, VAT Tecnologia da Informa\xE7\xE3o S/A");
	m_lblAbout->SetLabel(aboutLabelText);
	*m_txtAbout << _("Project Coordinator") << wxT("\n");
	*m_txtAbout << wxT("\tSergio Barbosa Villas-Boas, Ph.D.\n");
	*m_txtAbout << wxT("\n");
	*m_txtAbout << _("Digital Video Consultant") << wxT("\n");
	*m_txtAbout << wxT("\tLisandro Lovisolo, D.Sc.\n");
	*m_txtAbout << wxT("\n");
	*m_txtAbout << _("Software Development Manager") << wxT("\n");
	*m_txtAbout << wxT("\tGuilherme Caldeira De Lello\n");
	*m_txtAbout << wxT("\n");
	*m_txtAbout << _("Software Engineers") << wxT("\n");
	*m_txtAbout << wxT("\tAndr\xE9 Cotrim de Carvalho\n");
	*m_txtAbout << wxT("\tDaniel Dominguez Gouv") << wxT("\xEA") << wxT("a\n");
	*m_txtAbout << wxT("\tIvan Carlos da Silva Lopes\n");
	*m_txtAbout << wxT("\tVin") << wxT("\xED") << wxT("cius Heineck dos Santos\n");
	*m_txtAbout << wxT("\n");
	*m_txtAbout << _("Development Support Team") << wxT("\n");
	*m_txtAbout << wxT("\tDaniel Chaves Toscano Dantas\n");
	*m_txtAbout << wxT("\tR") << wxT("\xF4") << wxT("mulo Fernandes Ruas da Costa\n");
	*m_txtAbout << wxT("\tRobson Iamake\n");
	m_txtAbout->SetInsertionPoint(0);

	// Add controls to sizer
	vertSizer->Add(m_lblAbout, 0, wxEXPAND);
	vertSizer->AddSpacer(8);
	vertSizer->Add(m_txtAbout, 0, wxEXPAND);
	vertSizer->AddSpacer(8);
	vertSizer->Add(skinInfoHeaderLabel, 0, wxEXPAND);
	vertSizer->Add(m_lblSkinInfo, 0, wxEXPAND);
	vertSizer->Layout();
	horizSizer->Add(m_sbmpIcon, 0, wxALL, 8);
	horizSizer->AddSpacer(16);
	horizSizer->Add(vertSizer, 1, wxEXPAND);
	horizSizer->AddSpacer(8);
	horizSizer->Layout();
	windowSizer->Add(horizSizer, 1, wxALL | wxEXPAND, 8);
	windowSizer->AddSpacer(16);
	windowSizer->Add(m_btnOK, 0, wxALL | wxALIGN_CENTER, 8);
	SetSizer(windowSizer);
	windowSizer->Layout();
	windowSizer->SetSizeHints(this);
}

/** Sets the string displayed under "Skin Information:".
 * @param[in] info	String to be displayed.
 */
void AboutDialog::SetSkinInfo(const wxString &info)
{
	m_lblSkinInfo->SetLabel(info);
	Refresh();
}
