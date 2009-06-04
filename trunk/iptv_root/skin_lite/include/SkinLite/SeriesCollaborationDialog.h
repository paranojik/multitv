#ifndef SERIES_COLLABORATION_H
#define SERIES_COLLABORATION_H

#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/spinctrl.h>

#include "UserList.h"

#define SCD_SWITCH_INTERVAL_MIN		1
#define SCD_SWITCH_INTERVAL_MAX		24 * 3600
#define SCD_SWITCH_INTERVAL_DEFAULT	20

enum
{
	SCD_ID_FIRST = wxID_HIGHEST + 20000,
	SCD_ID_CHK_LIST,
	SCD_ID_CHK_AUTO_SELECT_ALL,
	SCD_ID_SPIN_CONTROL,
	SCD_ID_BTN_OK,
	SCD_ID_BTN_CANCEL,
	SCD_ID_LAST
};

/** @brief Class thats implements series collaboration dialog.
* 
*/
class SeriesCollaborationDialog: public wxDialog
{
public:
	SeriesCollaborationDialog();
	SeriesCollaborationDialog(wxWindow* parent, const UserList *displayedList = NULL, 
	wxWindowID id = wxID_ANY, const wxString& title = _("Series Collaboration"), 
	const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
	long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));
	
	bool Create(wxWindow* parent, const UserList *displayedList = NULL, 
	wxWindowID id = wxID_ANY, const wxString& title = _("Series Collaboration"), 
	const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
	long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));
	
	bool IsAutoSelectAllChecked() const;
	
	void SetDisplayedUserList(const UserList *list);
	
	UserList &GetCheckedUserList();
	unsigned GetSwitchInterval() const;
	
private:
	// Variables
	const UserList *m_pUserList;	// list of enabled users (displayed in the check list box)
	UserList m_checkedUserList;		// list of checked users
	bool m_flgAutoSelectAll;		// "auto select everyone" flag
	unsigned m_uSwitchInterval;		// switch interval, seconds
	// Controls
	wxBoxSizer *m_mainSizer,
	           *m_checkListSizer,
	           *m_switchIntervalSizer,
	           *m_buttonsSizer;
	wxButton *m_btnOk,
	         *m_btnCancel;
	wxCheckListBox *m_lstUsers;
	wxCheckBox *m_chkAutoSelectAll;
	wxSpinCtrl *m_spinSwitchInterval;
	
	// Member initialization
	void Init();
	
	// Control creation
	void CreateControls();
	
	// Event handlers
	void OnAutoSelectAllCheckboxClick(wxCommandEvent &event);
	void OnCheckListToggle(wxCommandEvent &event);
	void OnSpinControlChange(wxSpinEvent &event);
	
	DECLARE_EVENT_TABLE()
};

#endif

