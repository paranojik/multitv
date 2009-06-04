#ifndef MULTICASTCONFIGDIALOG_H
#define MULTICASTCONFIGDIALOG_H

#include <wx/wx.h>
#include "AppInterface.h"
#include "AppConfig.h"

/** @brief Dialog to begin the conference in multicast mode. 
*
*/
class MulticastConfigDialog: public wxDialog
{
private:
	wxTextCtrl *m_txtHost, *m_txtPort;
	wxStaticText *m_lblHost, *m_lblPort;
	AppConfig m_config;
	AppInterface *m_appInterface;
	wxWindow *m_parent;
public:
	MulticastConfigDialog(wxWindow* parent, 
		wxWindowID id = wxID_ANY, const wxString& title = _("Multicast Settings"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _("dialogBox"));
	
	virtual ~MulticastConfigDialog();

	bool Create(wxWindow* parent, 
		wxWindowID id = wxID_ANY, const wxString& title = _("Multicast Settings"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = _("dialogBox"));

	void CreateControls();

	void SetAppInterface(AppInterface *iface);

	const AppConfig &GetConfig() const;

	// wxWidgets virtual "transfer data" methods
	bool TransferDataToWindow();
	bool TransferDataFromWindow();
};

#endif
