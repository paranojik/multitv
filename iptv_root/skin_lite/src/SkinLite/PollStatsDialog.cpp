#include <wx/wx.h>
#include <vector>
using namespace std;

#include "PollStatsDialog.h"
#include "PollGraphicFrame.h"

/** PollStatsDialog default constructor.
*
*/
PollStatsDialog::PollStatsDialog() : wxDialog()
{
	m_controlsCreated = false;
}

/** PollStatsDialog constructor.
* @param[in] info. Receive the statistics in this parameter.
*/
PollStatsDialog::PollStatsDialog(const PollInfo &info) : wxDialog()
{
	m_controlsCreated = false;
	m_info = info;
}

/** PollStatsDialog constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Poll Statistics").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("dialogBox")
*/
PollStatsDialog::PollStatsDialog(wxWindow *parent,
								 wxWindowID id, const wxString &title,
								 const wxPoint &pos, const wxSize &size,
								 long style, const wxString &name)
{
	m_controlsCreated = false;
	Create(parent, id, title, pos, size, style, name);
}

/** PollStatsDialog constructor.
* @param[in] info. Receive the statistics in this parameter.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Poll Statistics").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("dialogBox")
*/
PollStatsDialog::PollStatsDialog(const PollInfo &info, wxWindow *parent, 
								 wxWindowID id, const wxString &title,
								 const wxPoint &pos, const wxSize &size,
								 long style, const wxString &name)
{
	m_controlsCreated = false;
	m_info = info;
	Create(parent, id, title, pos, size, style, name);
}

/** PollAnswerDialog constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default wxID_ANY.
* @param[in] title. Window title. Default _("Poll").
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxDEFAULT_DIALOG_STYLE.
* @param[in] name. Window name. Default wxT("PollAnswerDialog")
* @return Return true if controls were created or false if an error occurred.
*/
bool PollStatsDialog::Create(wxWindow *parent,
 								 wxWindowID id, const wxString &title,
								 const wxPoint &pos, const wxSize &size,
								 long style, const wxString &name)
{
	bool ret = wxDialog::Create(parent, id, title, pos, size, style, name);
	if(ret)
	{
		CreateControls();
		Connect(ID_POLL_STATS_DIALOG_BTN_GRAPHIC, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PollStatsDialog::OnGraphicButtonClick), NULL, this);
		Connect(wxID_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PollStatsDialog::OnCloseButtonClick), NULL, this);
	}
	return ret;
}

/** Sets informations of the poll.
* @param[in] info. Receive the statistics in this parameter.
*/
void PollStatsDialog::SetPollInfo(const PollInfo &info)
{
	m_info = info;
	UpdateInfo();
}

/** Updates the fields with the statistics passed from info in instantiation or SetPollInfo.
*
*/
void PollStatsDialog::UpdateInfo()
{
	if(m_info.IsValid())
	{
		m_lblQuestionText->SetLabel(m_info.GetQuestionText());
		bool hasOptionLabels = !m_lblOptions.empty();
		float votePercent;
		int totalVotes = m_info.GetTotalVotes();
		int blankVotes = m_info.GetNumberOfVotes(-1);
		
		m_lblBlankVoteText->SetLabel(wxString(wxT("(-)")) << m_info.GetAnswerItem(-1).GetText());

		if(totalVotes != 0)
		{
			votePercent = 100.0f * (blankVotes / (float)totalVotes);
			m_btnGraphic->Enable(true);
		}
		else
		{
			votePercent = 0.0f;
			m_btnGraphic->Enable(false);
		}
		
		m_lblTotalVotes->SetLabel(wxString() << totalVotes);
		m_lblBlankVotes->SetLabel(wxString::Format(_("%d votes (%0.02f%%)"), blankVotes, votePercent));

		for(int i = 0; i < m_info.GetAnswerItemCount(); i++)
		{
			const PollItem &item = m_info.GetAnswerItem(i);
			if(!hasOptionLabels)
			{
				m_lblOptions.push_back(new wxStaticText(this, wxID_ANY, wxT("")));
				m_lblStatValues.push_back(new wxStaticText(this, wxID_ANY, wxT("")));
				m_optionsGridSizer->Add(m_lblOptions[i], 0, wxLEFT | wxEXPAND | wxALIGN_LEFT);
				m_optionsGridSizer->Add(m_lblStatValues[i], 1, wxEXPAND | wxALIGN_RIGHT);
			}

			wxChar optionChar = 'A' + i;
			m_lblOptions[i]->SetLabel(wxString(wxT("(")) << wxString(optionChar) << wxT(") ") << item.GetText());
			
			if(m_info.GetTotalVotes() != 0)
				votePercent = 100.0f * (item.GetAnswerCount() / (float)totalVotes);
			else
				votePercent = 0.0f;
			m_lblStatValues[i]->SetLabel(wxString::Format(_("%d votes (%0.02f%%)"), item.GetAnswerCount(), votePercent));
		}
		SetMaxSize(POLL_STATS_DIALOG_MAX_SIZE);
		m_optionsGridSizer->Layout();
		GetSizer()->Layout();
		Fit();
	}
}

void PollStatsDialog::CreateControls()
{
	wxBoxSizer *controlsSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *textSizer = new wxBoxSizer(wxVERTICAL);
	m_optionsGridSizer = new wxFlexGridSizer(0, 2, 4, 4);	// variable rows, 2 columns, 4 pixels gap (H, V)
	wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	
	m_lblQuestionText = new wxStaticText(this, wxID_ANY, wxT(""));
	m_lblTotalVotes = new wxStaticText(this, wxID_ANY, wxT("0"));
	m_lblBlankVotes = new wxStaticText(this, wxID_ANY, wxT(""));
	m_lblBlankVoteText = new wxStaticText(this, wxID_ANY, wxT(""));

	m_btnGraphic = new wxButton(this, ID_POLL_STATS_DIALOG_BTN_GRAPHIC, _("Graphic"));
	m_btnClose = new wxButton(this, wxID_CLOSE, _("Close"));

	m_optionsGridSizer->AddGrowableCol(0);	// first column holds the option text labels
	// Total
	m_optionsGridSizer->Add(new wxStaticText(this, wxID_ANY, _("Total votes")), 0, wxLEFT | wxEXPAND, wxALIGN_LEFT);
	m_optionsGridSizer->Add(m_lblTotalVotes, 0, wxEXPAND | wxALIGN_RIGHT);
	// Spacer. Must add a spacer to each column
	for(int j = 0; j < m_optionsGridSizer->GetCols(); j++) 
		m_optionsGridSizer->AddSpacer(4);	
	// Blank votes
	m_optionsGridSizer->Add(m_lblBlankVoteText, 0, wxLEFT | wxEXPAND | wxALIGN_LEFT);
	m_optionsGridSizer->Add(m_lblBlankVotes, 0, wxEXPAND | wxALIGN_RIGHT);


	textSizer->Add(m_lblQuestionText, 0, wxALL | wxEXPAND, 4);
	textSizer->AddSpacer(16);
	textSizer->Add(m_optionsGridSizer, 1, wxLEFT | wxEXPAND, 16);
	
	buttonsSizer->Add(m_btnGraphic, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 8);
	buttonsSizer->Add(m_btnClose, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 8);
	controlsSizer->Add(textSizer, 1, wxALL | wxEXPAND, 8);
	controlsSizer->Add(buttonsSizer, 1, wxALL | wxEXPAND, 8);

	SetSizer(controlsSizer);

	UpdateInfo();

	m_controlsCreated = true;
}

void PollStatsDialog::OnGraphicButtonClick(wxCommandEvent &event)
{
	if(m_info.IsValid())
	{
		std::vector<wxString> vecAnswerLabels;
		std::vector<double> vecAnswerTotalVotes;

		wxString question = m_info.GetQuestionText();
		int totalVotes = m_info.GetTotalVotes();
		int blankVotes = m_info.GetNumberOfVotes(-1);

		vecAnswerLabels.push_back(m_info.GetAnswerItem(-1).GetText());
		vecAnswerTotalVotes.push_back((double)blankVotes);

		for(int i = 0; i < m_info.GetAnswerItemCount(); i++)
		{
			const PollItem &item = m_info.GetAnswerItem(i);
			
			wxChar optionChar = 'A' + i;
			wxString answerLabel = wxString(wxT("(")) << wxString(optionChar) << wxT(")");
			
			int answerVoteCount = item.GetAnswerCount();

			vecAnswerLabels.push_back(answerLabel);
			vecAnswerTotalVotes.push_back((double)answerVoteCount);
		}

		PollGraphicFrame *pollGraphicFrame = (PollGraphicFrame *)FindWindowById(ID_POLLGRAPHICFRAME, this);

		if (pollGraphicFrame == NULL)
			pollGraphicFrame = new PollGraphicFrame(this, ID_POLLGRAPHICFRAME, _("Poll Graphic result"));
			
		pollGraphicFrame->DrawChart(POLL_STATS_DIALOG_PNG_PATH, question, vecAnswerLabels, vecAnswerTotalVotes);
		pollGraphicFrame->Show();
	}
}

void PollStatsDialog::OnCloseButtonClick(wxCommandEvent &event)
{
	Close();
}
