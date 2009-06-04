#include <wx/wx.h>

#include <vector>
using namespace std;

#include "PollInfo.h"
#include "AppInterface.h"
#include "PollAnswerDialog.h"

BEGIN_EVENT_TABLE(PollAnswerDialog, wxDialog)
	EVT_BUTTON(PAD_BUTTON_ID_BLANK, PollAnswerDialog::OnBlankButtonClick)
	EVT_BUTTON(PAD_BUTTON_ID_VOTE, PollAnswerDialog::OnVoteButtonClick)
END_EVENT_TABLE()

/** PollAnswerDialog default constructor.
*
*/
PollAnswerDialog::PollAnswerDialog()
{
	Init();
}

/** PollAnswerDialog constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Poll").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("PollAnswerDialog")
*/
PollAnswerDialog::PollAnswerDialog(wxWindow *parent, wxWindowID id, const wxString &title,
								   const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
	Init();
	Create(parent, id, title, pos, size, style, name);
}

void PollAnswerDialog::Init()
{
	m_appInterface = NULL;
}

/** PollAnswerDialog creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Poll").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("PollAnswerDialog")
* @return Return true if controls were created or false if an error occurred.
*/
bool PollAnswerDialog::Create(wxWindow *parent, wxWindowID id, const wxString &title,
								   const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
	bool ret = wxDialog::Create(parent, id, title, pos, size, style, name);
	if(ret)
		CreateControls();
	return ret;
}

/** Sets linkage with application.
* @param[in] appInterface. Application interface.
*/
void PollAnswerDialog::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
}

void PollAnswerDialog::CreateControls()
{
	m_pollQuestionCtrl = new PollQuestionCtrl(this, wxID_ANY, POLL_MIN_ANSWERS, POLL_MAX_ANSWERS, 
		POLL_MIN_ANSWERS, POLLQUESTIONCTRL_MODE_RECEIVEPOLL, wxDefaultPosition, wxSize(400, 300));
	m_btnBlank = new wxButton(this, PAD_BUTTON_ID_BLANK, _("Blank"));
	m_btnVote = new wxButton(this, PAD_BUTTON_ID_VOTE, _("Vote"));
	
	wxBoxSizer *controlsSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);

	buttonsSizer->AddStretchSpacer();
	buttonsSizer->Add(m_btnBlank, 0, wxALL, 8);
	buttonsSizer->Add(m_btnVote, 0, wxALL, 8);
	buttonsSizer->AddSpacer(16);

	controlsSizer->Add(m_pollQuestionCtrl, 0, wxALL | wxEXPAND, 8);
	controlsSizer->Add(buttonsSizer, 0, wxALL | wxEXPAND, 8);
	
	SetSizer(controlsSizer);
	UpdateAnswerArea();
	controlsSizer->Layout();
	Fit();
}

/** Update the answer area filling your fields.
* Private method.
*/
void PollAnswerDialog::UpdateAnswerArea()
{
	if(!m_appInterface)
		return;

	const PollInfo *info = m_appInterface->GetActivePoll();
	if(info)
	{
		if(info->IsValid())
		{
			vector<wxString> strVectorAnswers;
			for(unsigned i = 0; i < info->GetAnswerItemCount(); i++)
				strVectorAnswers.push_back(info->GetAnswerItem(i).GetText());
			
			m_pollQuestionCtrl->SetAnswersText(strVectorAnswers);
			m_pollQuestionCtrl->SetQuestionText(info->GetQuestionText());
			m_pollQuestionCtrl->SetPollCtrlLayout();
		}
	}
}

// Event handlers
void PollAnswerDialog::OnBlankButtonClick(wxCommandEvent &event)
{
	if(m_appInterface)
		m_appInterface->SendPollAnswer(-1);
	
	Close();
}

void PollAnswerDialog::OnVoteButtonClick(wxCommandEvent &event)
{
	int selectedIndex = m_pollQuestionCtrl->GetAnswerChoice();
	if(selectedIndex < 0)
		wxMessageBox(_("Please select an item, or click \"Blank\" if you want to send a blank answer."),
		_("Information"), wxICON_INFORMATION, this);
	else
	{
		if(m_appInterface)
			m_appInterface->SendPollAnswer(selectedIndex);
		
		Close();
	}
}
