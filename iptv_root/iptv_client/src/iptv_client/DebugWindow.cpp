#include <wx/wx.h>
#include "DebugWindow.h"

/** Default constructor.
 */
DebugWindow::DebugWindow() : wxFrame()
{

}

/** Constructor that creates the window in a single pass.
 * @param[in] parent	Parent window. May be NULL.
 * @param[in] id	Window ID.
 * @param[in] title	Window title.
 * @param[in] size	Window size.
 * @param[in] pos	Window position.
 * @param[in] style	Window style. See wxWidgets documentation.
 * @param[in] name	Window name. See wxWidgets documentation.
 */
DebugWindow::DebugWindow(wxWindow *parent, wxWindowID id, const wxString &title, 
						 const wxSize &size, const wxPoint &pos, long style, const wxString &name) : wxFrame()
{
	Create(parent, id, title, size, pos, style, name);
}

/** Creates the window. Used for two-step window creation.
 * @param[in] parent	Parent window. May be NULL.
 * @param[in] id	Window ID.
 * @param[in] title	Window title.
 * @param[in] size	Window size.
 * @param[in] pos	Window position.
 * @param[in] style	Window style. See wxWidgets documentation.
 * @param[in] name	Window name. See wxWidgets documentation.
 * @remarks Do not call this function if the single-step creation constructor is used.
 */
bool DebugWindow::Create(wxWindow *parent, wxWindowID id, const wxString &title, 
						 const wxSize &size, const wxPoint &pos, long style, const wxString &name)
{
	bool ret = wxFrame::Create(parent, id, title, pos, size, style, name);
	if (ret) CreateControls();
	return ret;
}

/** Creates the controls inside the window.
 */
void DebugWindow::CreateControls()
{
	m_txtOutput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
}

/** Adds a message line to the output text box.
 * @param[in] msg	Message to be added.
 */
void DebugWindow::SetDebugMessage(const wxString &msg)
{
	wxString timeStr;
	timeStr << wxT("[") << wxDateTime::UNow().FormatTime() << wxT("] ");
	m_txtOutput->AppendText(timeStr + msg + wxT("\n"));
}
