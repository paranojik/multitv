#include "SkinnableSingleChoiceDlg.h"
/*
// BEGIN_EVENT_TABLE(SkinnableSingleChoiceDlg, wxSingleChoiceDialog)
	//EVT_LEFT_DCLICK(SkinnableSingleChoiceDlg::OnMouseLDClick)
//#ifdef __WXGTK__
//	EVT_WINDOW_CREATE(SkinnableSingleChoiceDlg::OnWindowCreate)
//#endif
// END_EVENT_TABLE()
*/
SkinnableSingleChoiceDlg::SkinnableSingleChoiceDlg(Skin* parent,
												   const wxString& message, 
						                           const wxString& caption, 
						                           int n, 
						                           const wxString* choices, 
												   char** clientData, 
												   long style, 
												   const wxPoint& pos) : wxSingleChoiceDialog(parent,
																							  message,
																							  caption,
																							  n,
																							  choices,
																							  clientData,
																							  style,
																							  pos)
{
	m_parent = parent;
	m_parent->DisableDrag();
}

SkinnableSingleChoiceDlg::SkinnableSingleChoiceDlg(Skin* parent,
							 const wxString& message, 
							 const wxString& caption, 
							 const wxArrayString& choices, 
							 char** clientData, 
							 long style, 
							 const wxPoint& pos) : 	wxSingleChoiceDialog(parent,
																	   message,
																	   caption,
																	   choices,
																	   clientData,
																	   style,
																	   pos)
{
	m_parent = parent;
	m_parent->DisableDrag();
}

SkinnableSingleChoiceDlg::~SkinnableSingleChoiceDlg()
{	
	m_parent->EnableDrag();
}
