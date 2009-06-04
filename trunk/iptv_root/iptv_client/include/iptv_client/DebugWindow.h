#ifndef DEBUG_WINDOW_H
#define DEBUG_WINDOW_H

#include <wx/wx.h>

/** @brief A Debug message window.
 *
 * Enabled in Debug builds only. Displays debugging messages.
 *
 */
class DebugWindow: public wxFrame
{
private:
	wxTextCtrl *m_txtOutput;
public:
	DebugWindow();
	DebugWindow(wxWindow *parent, wxWindowID id = wxID_ANY, 
		const wxString &title = wxT("Debug Window"), 
		const wxSize &size = wxDefaultSize, const wxPoint &pos = wxDefaultPosition,
		long style = wxDEFAULT_FRAME_STYLE & ~wxCLOSE_BOX, const wxString &name = wxT("debugWindow"));

	bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, 
		const wxString &title = wxT("Debug Window"), 
		const wxSize &size = wxDefaultSize, const wxPoint &pos = wxDefaultPosition,
		long style = wxDEFAULT_FRAME_STYLE & ~wxCLOSE_BOX, const wxString &name = wxT("debugWindow"));

	void CreateControls();

	void SetDebugMessage(const wxString &msg);
};

#endif
