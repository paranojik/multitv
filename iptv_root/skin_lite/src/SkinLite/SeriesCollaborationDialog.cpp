#include <wx/wx.h>
#include <wx/checklst.h>
#include <wx/spinctrl.h>

#include "SeriesCollaborationDialog.h"

BEGIN_EVENT_TABLE(SeriesCollaborationDialog, wxDialog)
	EVT_CHECKLISTBOX(SCD_ID_CHK_LIST, SeriesCollaborationDialog::OnCheckListToggle)
	EVT_CHECKBOX(SCD_ID_CHK_AUTO_SELECT_ALL, SeriesCollaborationDialog::OnAutoSelectAllCheckboxClick)
	EVT_SPINCTRL(SCD_ID_SPIN_CONTROL, SeriesCollaborationDialog::OnSpinControlChange)
END_EVENT_TABLE()

/** SeriesCollaborationDialog default constructor.
*
*/
SeriesCollaborationDialog::SeriesCollaborationDialog(): wxDialog()
{
	Init();
}

/** SeriesCollaborationDialog constructor.
* @param[in] parent. Parent window.
* @param[in] displayedList. List with collaborators. Default NULL.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Series Collaboration").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("dialogBox").
*/
SeriesCollaborationDialog::SeriesCollaborationDialog(wxWindow *parent, const UserList *displayedList, 
	wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, 
	long style, const wxString& name)
{
	Init();
	Create(parent, displayedList, id, title, pos, size, style, name);
}

/** SeriesCollaborationDialog creator.
* @param[in] parent. Parent window.
* @param[in] displayedList. List with collaborators. Default NULL.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Series Collaboration").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("dialogBox").
* @return Return true if controls were created or false if an error occurred.
*/
bool SeriesCollaborationDialog::Create(wxWindow* parent, const UserList *displayedList, 
	wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, 
	long style, const wxString& name)
{
	bool ret = wxDialog::Create(parent, id, title, pos, size, style, name);
	if(ret)
	{
		CreateControls();
		SetDisplayedUserList(displayedList);
	}
	return ret;
}	

void SeriesCollaborationDialog::Init()
{
	m_pUserList = NULL;
	m_uSwitchInterval = 1;
}

void SeriesCollaborationDialog::CreateControls()
{
	// Label over the wxCheckListBox
	wxStaticText *ckListBoxLabel = new wxStaticText(this, wxID_ANY, _("Select users to participate:"));
	// the wxCheckListBox
	m_lstUsers = new wxCheckListBox(this, SCD_ID_CHK_LIST, wxDefaultPosition, wxDefaultSize, 
		0, NULL, wxLB_MULTIPLE | wxLB_NEEDED_SB);
	m_lstUsers->SetMinSize(wxSize(-1,160));
	// the "auto-select-all" check box
	m_chkAutoSelectAll = new wxCheckBox(this, SCD_ID_CHK_AUTO_SELECT_ALL, _("Automatically select all users"));
	// label before the wxSpinCtrl
	wxStaticText *spinCtrlLabel = new wxStaticText(this, wxID_ANY, _("Switch interval:"));
	// the wxSpinCtrl
	m_spinSwitchInterval = new wxSpinCtrl(this, SCD_ID_SPIN_CONTROL, wxString() << (int)SCD_SWITCH_INTERVAL_DEFAULT, 
		wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, SCD_SWITCH_INTERVAL_MIN, SCD_SWITCH_INTERVAL_MAX,
		SCD_SWITCH_INTERVAL_DEFAULT);
	// label after the wxSpinCtrl
	wxStaticText *spinCtrlUnitLabel = new wxStaticText(this, wxID_ANY, _("seconds"));
	
	// command buttons
	m_btnOk = new wxButton(this, SCD_ID_BTN_OK, _("OK"));
	m_btnCancel = new wxButton(this, SCD_ID_BTN_CANCEL, _("Cancel"));
	// a bunch o' sizers
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *spinCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *cmdButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	
	SetSizer(mainSizer);
	// Layout for the SpinCtrl
	spinCtrlSizer->AddStretchSpacer();
	spinCtrlSizer->Add(spinCtrlLabel, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 4);
	spinCtrlSizer->Add(m_spinSwitchInterval, 1, wxRIGHT, 4);
	spinCtrlSizer->Add(spinCtrlUnitLabel, 0, wxALIGN_CENTER_VERTICAL, 0);
	spinCtrlSizer->Layout();
	// Layout for the command buttons
	cmdButtonSizer->AddStretchSpacer();
	cmdButtonSizer->Add(m_btnOk, 0, wxRIGHT, 4);
	cmdButtonSizer->Add(m_btnCancel, 0, 0, 0);
	cmdButtonSizer->Layout();
	// Overall window layout
	mainSizer->Add(ckListBoxLabel, 0, wxALL | wxEXPAND, 4);
	mainSizer->Add(m_lstUsers, 1, wxLEFT | wxRIGHT | wxEXPAND, 8);
	mainSizer->AddSpacer(4);
	mainSizer->Add(m_chkAutoSelectAll, 0, wxLEFT | wxRIGHT | wxEXPAND, 8);
	mainSizer->AddSpacer(8);
	mainSizer->Add(spinCtrlSizer, 0, wxLEFT | wxRIGHT | wxEXPAND, 4);
	mainSizer->AddSpacer(4);
	mainSizer->Add(cmdButtonSizer, 0, wxALL | wxEXPAND, 4);
	mainSizer->Layout();
	Fit();
	// Additional setup
	SetAffirmativeId(SCD_ID_BTN_OK);
	SetEscapeId(SCD_ID_BTN_CANCEL);
}

/** Checks if auto select all is checked.
* @return Return true or false for this condition.
*/
bool SeriesCollaborationDialog::IsAutoSelectAllChecked() const
{
	return m_flgAutoSelectAll;
}

/** Adds the nicknames on users list.
* @param[in] list. List containing the user names.
*/
void SeriesCollaborationDialog::SetDisplayedUserList(const UserList *list)
{
	m_pUserList = list;
	if(m_pUserList)
	{
		// Make an array with the nicknames of users in list
		wxArrayString choicesArray;
		for(UserIterator it = m_pUserList->GetBeginIterator(); it != m_pUserList->GetEndIterator(); it++)
			choicesArray.Add(it->GetNickName());
		// and add them to the list
		m_lstUsers->Set(choicesArray);
	}
}

// TODO: const-correctness check
/** Gets the checked Users list.
*
*/
UserList &SeriesCollaborationDialog::GetCheckedUserList()
{
	return m_checkedUserList;
}

/** Gets the switch interval.
*
*/
unsigned SeriesCollaborationDialog::GetSwitchInterval() const
{
	return m_uSwitchInterval;
}

// Event handlers
void SeriesCollaborationDialog::OnAutoSelectAllCheckboxClick(wxCommandEvent &event)
{
	m_flgAutoSelectAll = m_chkAutoSelectAll->IsChecked();
}

void SeriesCollaborationDialog::OnCheckListToggle(wxCommandEvent &event)
{

}

void SeriesCollaborationDialog::OnSpinControlChange(wxSpinEvent &event)
{
	m_uSwitchInterval = m_spinSwitchInterval->GetValue();
}
