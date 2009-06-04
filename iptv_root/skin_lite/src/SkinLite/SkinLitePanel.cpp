#include "SkinLitePanel.h"
#include "SkinLite.h"

BEGIN_EVENT_TABLE(SkinLitePanel, wxPanel)
	EVT_LEFT_DCLICK(SkinLitePanel::OnMouseLeftDClick)
END_EVENT_TABLE()

SkinLitePanel::SkinLitePanel(wxFrame *parent, wxWindowID id):wxPanel(parent, 
						   											     id,
																		 wxDefaultPosition,
																		 wxDefaultSize, 
																		 wxTAB_TRAVERSAL)
{
	m_parent = parent;
}

SkinLitePanel::~SkinLitePanel()
{
}

void SkinLitePanel::OnMouseLeftDClick(wxMouseEvent &event)
{
	/*wxMessageBox(_("M_PANEL DOUBLE_CLICK"));*/
	event.ResumePropagation(1);
	event.Skip();
}

