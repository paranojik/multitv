#ifndef _SKINLITE_PANEL_
#define _SKINLITE_PANEL_

#include <wx/wx.h>

enum
{
	ID_SL_PANEL1 =1
};

/** @brief Class thats create the SkinLite panel.
*
*/
class SkinLitePanel:public wxPanel
{
protected:
	wxFrame *m_parent;
public:
	SkinLitePanel(wxFrame *parent, wxWindowID id = ID_SL_PANEL1);
	~SkinLitePanel();
	void OnMouseLeftDClick(wxMouseEvent &event);

	DECLARE_EVENT_TABLE()
};


#endif
