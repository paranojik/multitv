#ifndef _SKINNABLE_SINGLE_CHOICE_DLG_
#define _SKINNABLE_SINGLE_CHOICE_DLG_

#define SKIN_API
#include "wx/wx.h"
#include "Skin.h"

/** @brief Skin selection dialog.
*
*/
class SkinnableSingleChoiceDlg: public wxSingleChoiceDialog
{
private:
protected:
	Skin *m_parent;
	


// SkinnableSingleChoiceDlg methods
public:
	
	SkinnableSingleChoiceDlg(Skin* parent,
					         const wxString& message, 
						     const wxString& caption, 
						     int n, 
						     const wxString* choices, 
						     char** clientData = NULL, 
						     long style = wxCHOICEDLG_STYLE, 
						     const wxPoint& pos = wxDefaultPosition);

	SkinnableSingleChoiceDlg(Skin* parent,
							 const wxString& message, 
							 const wxString& caption, 
							 const wxArrayString& choices, 
							 char** clientData = NULL, 
							 long style = wxCHOICEDLG_STYLE, 
							 const wxPoint& pos = wxDefaultPosition);
	virtual ~SkinnableSingleChoiceDlg();

//	DECLARE_EVENT_TABLE();
};


#endif
