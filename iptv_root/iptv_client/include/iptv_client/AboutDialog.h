#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <wx/wx.h>

#define ABOUT_ICON_FILE_NAME	wxT("Graphics/Logo_small.png")

/** @brief About Dialog.
 *
 * This provides the About dialog. It belongs to the App and is Skin-independent.
 *
 */
class AboutDialog: public wxDialog
{
private:
	wxStaticBitmap *m_sbmpIcon;
	wxStaticText *m_lblAbout, *m_lblSkinInfo;
	wxTextCtrl *m_txtAbout;
	wxButton *m_btnOK;
public:
	AboutDialog();
	AboutDialog(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxString &title = _("About"),
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE & ~wxSYSTEM_MENU,
		const wxString &name = wxT("aboutdlg"));
	virtual ~AboutDialog();

	bool Create(wxWindow *parent = NULL, wxWindowID id = wxID_ANY,
		const wxString &title = _("About"),
		const wxPoint &pos = wxDefaultPosition,
		const wxSize &size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE & ~wxSYSTEM_MENU,
		const wxString &name = wxT("aboutdlg"));

	void CreateControls();

	void SetSkinInfo(const wxString &info);
};

#endif
