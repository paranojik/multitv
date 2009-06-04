#include "PollQuestionCtrl.h"
#include "SkinLiteConstants.h"
#include "MessageDialog.h"

#define POLLQUESTIONCTRL_MINIMAL_ANSWER_RANGE 0
#define POLLQUESTIONCTRL_INSIDE_ANSWER_RANGE 1
#define POLLQUESTIONCTRL_MAXIMUM_ANSWER_RANGE 2
#define POLLQUESTIONCTRL_SCROLLBAR_SCROLL_UNITS	100
#define POLLQUESIONCTRL_GETANSWERCHOICE_NOANSWER 0

BEGIN_EVENT_TABLE(PollQuestionCtrl, wxControl)
	EVT_BUTTON(ID_POLLQUESTIONCTRL_BTN_ADDANSWER, PollQuestionCtrl::OnBtnAddAnswerClick)
	EVT_BUTTON(ID_POLLQUESTIONCTRL_BTN_REMOVEANSWER, PollQuestionCtrl::OnBtnRemoveAnswerClick)
	EVT_SIZE(PollQuestionCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(PollQuestionCtrl, wxControl)

/** PollQuestionCtrl constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id.
* @param[in] minAnswers. Minimum number of answers thats this control can have.
* @param[in] maxAnswers. Maximum number of answers thats this control can have.
* @param[in] defaultAnswers. Number of answers on creation of this control.
* @param[in] mode. Sets the control to be receive or send poll, POLLQUESTIONCTRL_MODE_RECEIVEPOLL or POLLQUESTIONCTRL_MODE_SENDPOLL, respectively. Default POLLQUESTIONCTRL_MODE_SENDPOLL.
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxSUNKEN_BORDER|wxTAB_TRAVERSAL.
*/
PollQuestionCtrl::PollQuestionCtrl(wxWindow *parent, wxWindowID id, int minAnswers, 
													int maxAnswers,
													int defaultAnswers, bool mode,
													const wxPoint& pos,
													const wxSize& size,
													long style)
{
	Initialize();	
	Create(parent, id, minAnswers, maxAnswers, defaultAnswers, mode, pos, size, style);
}

/** PollQuestionCtrl creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id.
* @param[in] minAnswers. Minimum number of answers thats this control can have.
* @param[in] maxAnswers. Maximum number of answers thats this control can have.
* @param[in] defaultAnswers. Number of answers on creation of this control.
* @param[in] mode. Sets the control to be receive or send poll, POLLQUESTIONCTRL_MODE_RECEIVEPOLL or POLLQUESTIONCTRL_MODE_SENDPOLL, respectively. Default POLLQUESTIONCTRL_MODE_SENDPOLL.
* @param[in] pos. Window position. Default wxDefaultPosition.
* @param[in] size. Window size. Default wxDefaultSize.
* @param[in] style. Window style. Default wxSUNKEN_BORDER|wxTAB_TRAVERSAL.
* @return Return true if control was created and false if a error occurred.
*/
bool PollQuestionCtrl::Create(wxWindow *parent, wxWindowID id, int minAnswers, 
							  int maxAnswers, int defaultAnswers,
							  bool mode,
							  const wxPoint &pos, 
							  const wxSize &size, long style)
{
	if (!wxControl::Create(parent, id, pos, size, style)) return false;
	
	m_minAnswers = minAnswers;
	m_maxAnswers = maxAnswers;
	m_defaultAnswers = defaultAnswers;
	m_numberOfAnswers = defaultAnswers;
	m_mode = mode;
	m_maxAnswerInputLength = POLLQUESTIONCTRL_DEFAULT_MAX_INPUT_LENGTH;
	
	CreateControls();

	return true;
}

/** PollQuestionCtrl destructor.
*
*/
PollQuestionCtrl::~PollQuestionCtrl()
{
}

void PollQuestionCtrl::Initialize()
{
	m_parent = NULL;
	m_numberOfAnswers = 0;
	m_itemAnswer = 65; //ASCII CODE for char 'A'
	m_statusAnswerRange = -1;
}

/** Set the layout of the control.
*
*/
void PollQuestionCtrl::SetPollCtrlLayout()
{
	m_spinCtrlHorSizer->Layout();
	m_pollQuestionVertSizer->Layout();
}

wxSize PollQuestionCtrl::DoGetBestSize() const
{
	return GetSize();
}

void PollQuestionCtrl::CreateControls()
{
	m_pollQuestionHorSizer = new wxBoxSizer(wxHORIZONTAL);
	this->SetInitialSize(this->GetSize());
	this->SetSizer(m_pollQuestionHorSizer);

	this->Layout();

	m_pollQuestionVertSizer = new wxBoxSizer(wxVERTICAL);

	m_mainPanel = new wxScrolledWindow(this, ID_POLLQUESTIONCTRL_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxVSCROLL);

	m_spinCtrlHorSizer = new wxBoxSizer(wxHORIZONTAL);

	m_lblNumberOfAnswers = new wxStaticText(m_mainPanel, wxID_ANY, POLLQUESTIONCTRL_LBL_NUMBEROFANSWERS_CAPTION);

	m_txtSpinCtrl = new wxTextCtrl( m_mainPanel, ID_POLLQUESTIONCTRL_TXT_SPINCTRL, wxEmptyString, wxPoint(0, 0), wxSize(50, -1), wxTE_READONLY );
	m_txtSpinCtrl->SetValue(wxString::Format(wxT("%d"), m_numberOfAnswers));
	m_statusAnswerRange = CheckAnswersRange();

	m_btnAddAnswer = new wxButton(m_mainPanel, ID_POLLQUESTIONCTRL_BTN_ADDANSWER, wxT("+"));
	m_btnRemoveAnswer = new wxButton(m_mainPanel, ID_POLLQUESTIONCTRL_BTN_REMOVEANSWER, wxT("-"));


	m_lblQuestion = new wxStaticText(m_mainPanel, wxID_ANY, POLLQUESTIONCTRL_LBL_QUESTION_CAPTION);
	m_txtQuestion = new wxTextCtrl( m_mainPanel, ID_POLLQUESTIONCTRL_TXT_QUESTION, wxEmptyString, wxPoint(50, 0), wxSize(380, -1), wxTE_MULTILINE );



	m_answerAreaVertSizer = new wxBoxSizer(wxVERTICAL);

/*------------------------------------Adds ctrls on sizers------------------------------------------------*/

	m_spinCtrlHorSizer->Add(m_lblNumberOfAnswers, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);
	m_spinCtrlHorSizer->Add(m_txtSpinCtrl, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);
	m_spinCtrlHorSizer->Add(m_btnAddAnswer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5);
	m_spinCtrlHorSizer->Add(m_btnRemoveAnswer, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5);
	m_spinCtrlHorSizer->Layout();
	
	m_pollQuestionVertSizer->Add(m_spinCtrlHorSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
	m_pollQuestionVertSizer->Add(m_lblQuestion, 0, wxALIGN_LEFT|wxLEFT|wxTOP, 5);
	m_pollQuestionVertSizer->Add(m_txtQuestion, 0, wxEXPAND|wxLEFT|wxTOP|wxBOTTOM, 5);
	m_pollQuestionVertSizer->Add(m_answerAreaVertSizer, 1, wxEXPAND|wxALL, 5);
	m_pollQuestionVertSizer->Layout();
	
	m_pollQuestionHorSizer->Add(m_mainPanel, 1, wxALL|wxEXPAND, 5);
	m_pollQuestionHorSizer->Layout();

	m_questionSize = m_txtSpinCtrl->GetSize() + wxSize(0, 50) + m_lblQuestion->GetSize() + m_txtQuestion->GetSize();


/*----------------------------------END Adds ctrls on sizers----------------------------------------------*/

	CreateAnswersArea(m_mode);

	if (m_mode == POLLQUESTIONCTRL_MODE_RECEIVEPOLL)
	{
		m_spinCtrlHorSizer->Show(false);
		m_txtQuestion->SetEditable(false);
	}

	m_mainPanel->SetSizer(m_pollQuestionVertSizer);
	m_mainPanel->EnableScrolling(false, true);
	//Ugly, but's works
	m_mainPanel->SetScrollbars(0, POLLQUESTIONCTRL_SCROLLBAR_SCROLL_UNITS, 0, m_numberOfAnswers,0,0, true);
	m_answerAreaVertSizer->Layout();
	m_mainPanel->SetScrollbars(0, POLLQUESTIONCTRL_SCROLLBAR_SCROLL_UNITS, 0, m_numberOfAnswers,0,0, true);
	m_answerAreaVertSizer->Layout();
	m_mainPanel->Layout();
	//END Ugly, but's works
}

/** Check if number of answers is in the range.
* @return Return POLLQUESTIONCTRL_MINIMAL_ANSWER_RANGE if number of answers is less than minAnswers choosed in instantiation.
* @return Return POLLQUESTIONCTRL_INSIDE_ANSWER_RANGE if number of answers is between minAnswers and maxAnswers choosed in instantiation.
* @return Return POLLQUESTIONCTRL_MAXIMUM_ANSWER_RANGE if number of answers is greater than maxAnswers choosed in instantiation.
* Private method.
*/
int PollQuestionCtrl::CheckAnswersRange()
{
	if (m_numberOfAnswers <= m_minAnswers)
		return POLLQUESTIONCTRL_MINIMAL_ANSWER_RANGE;
	
	if ((m_numberOfAnswers > m_minAnswers) && (m_numberOfAnswers < m_maxAnswers))
		return POLLQUESTIONCTRL_INSIDE_ANSWER_RANGE;

	if (m_numberOfAnswers >= m_maxAnswers)
		return POLLQUESTIONCTRL_MAXIMUM_ANSWER_RANGE;

	return -1;
}

/** Create the answers area with the number of answers choosed in instantiation.
* @param[in] mode. Choose if answers text can be edited. True for edit or false for nom edit.
* Private method.
*/
void PollQuestionCtrl::CreateAnswersArea(const bool &mode)
{
	int answersCount = m_numberOfAnswers;
	
/*-------------------Workaround to clear RadioButotns in Linux--------------------------------*/
	m_rdBtnErased = new wxRadioButton(m_mainPanel, wxID_ANY, wxEmptyString);
	m_rdBtnErased->Hide();
	m_rdBtnErased->SetValue(true);
	m_answerAreaVertSizer->Add(m_rdBtnErased, 0, wxEXPAND|wxALL, 5);
/*---------------END Workaround to clear RadioButotns in Linux--------------------------------*/

	for(int i = 0;i < answersCount;i++)
	{
		wxRadioButton *rdBtnAnswer = new wxRadioButton(m_mainPanel, m_itemAnswer, wxString::Format(_("%c)"), m_itemAnswer));
		wxTextCtrl *txtAnswer = new wxTextCtrl(m_mainPanel, 30 + m_itemAnswer, wxEmptyString, wxPoint(10, i*20+100), wxSize(300,-1));

		txtAnswer->SetMaxLength(m_maxAnswerInputLength);

		m_answerSize = txtAnswer->GetSize() + wxSize(0, 10);

		if (mode == POLLQUESTIONCTRL_MODE_RECEIVEPOLL)
			txtAnswer->SetEditable(false);

		wxBoxSizer *itemAnswerSizer = new wxBoxSizer(wxHORIZONTAL);

		m_itemAnswer++;

		m_vectorRdBtnAnswer.push_back(rdBtnAnswer);
		m_vectorTxtAnswer.push_back(txtAnswer);		
		
		itemAnswerSizer->Add(m_vectorRdBtnAnswer[i], 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 8);
		itemAnswerSizer->Add(m_vectorTxtAnswer[i], 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5);
		
		itemAnswerSizer->Layout();

		m_vectorAnswerItemSizer.push_back(itemAnswerSizer);

		m_answerAreaVertSizer->Add(m_vectorAnswerItemSizer[i], 0, wxEXPAND|wxALL, 5);
	}
	m_answerAreaVertSizer->Layout();
}

/** Returns the question text.
* @return strQuestion. Return the question text in wxString format.
*/
wxString PollQuestionCtrl::GetQuestionText()
{
	wxString strQuestion = m_txtQuestion->GetValue();
	return strQuestion;
}

/** Returns the index of the answer choosed.
* @return If a error ocurred return POLLQUESIONCTRL_GETANSWERCHOICE_ERROR, usually if no answer choice.
*/
int PollQuestionCtrl::GetAnswerChoice()
{
	if (m_rdBtnErased->GetValue())
		return POLLQUESIONCTRL_GETANSWERCHOICE_ERROR;

	int size = (int)m_vectorRdBtnAnswer.size();

	for(int i=0;i<size;i++)
	{
		if (m_vectorRdBtnAnswer[i]->GetValue())
			// return (65+i);
			// NOTE: Returning the index itself makes more sense and is easier to work
			// with the current App structure.
			return i;	
	}
	return POLLQUESIONCTRL_GETANSWERCHOICE_ERROR;
}

/** Returns the text of the answers.
* @return return a vector with all answers texts.
*/
std::vector<wxString> PollQuestionCtrl::GetStrAnswers()
{
	int size = (int)m_vectorTxtAnswer.size();
	std::vector<wxString> answersText;

	for(int i=0;i<size;i++)
	{
		answersText.push_back(m_vectorTxtAnswer[i]->GetValue());
	}
	return answersText;
}

/** Clear the question text area and write in this field the new question text.
* @pram[in] question. Question text thats must be fill this field.
*/
void PollQuestionCtrl::SetQuestionText(const wxString &question)
{
	ClearQuestionText();

	wxString strQuestion = question;
	m_txtQuestion->SetValue(strQuestion);
}

/** Clear the answers text area and write in these fields the new answers texts.
* @pram[in] vectorAnswers. Vector of answers texts thats must be fill these fields.
*/
void PollQuestionCtrl::SetAnswersText(const std::vector<wxString> &vectorAnswers)
{
	std::vector<wxString> vectorStrAnswers = vectorAnswers;
	int vectorStrAnswersSize = (int)vectorStrAnswers.size();
	int answerItemsToCreate = vectorStrAnswersSize - m_numberOfAnswers;

	if(answerItemsToCreate > 0)
		AddAnswerItem(answerItemsToCreate);
	else
		RemoveAnswerItem(-(answerItemsToCreate));
	
	m_numberOfAnswers = vectorStrAnswers.size();
	SetNumberOfAnswers(vectorStrAnswers.size());

	ClearAnswersText();

	if ((!vectorStrAnswers.empty()) && (vectorStrAnswersSize>=m_numberOfAnswers))
	{
		for(int i=0;i<m_numberOfAnswers;i++)
		{
			m_vectorTxtAnswer[i]->SetValue(vectorStrAnswers[i]);
		}
	}
}

/** Set the maximum length for the answers text controls.
* @para[in] length. Maximum length thats controls must be.
*/
void PollQuestionCtrl::SetAnswerInputLength(unsigned long length)
{
	m_maxAnswerInputLength = length;

	if(!m_vectorTxtAnswer.empty())
	{
		std::vector<wxTextCtrl *>::iterator it;
		for(it = m_vectorTxtAnswer.begin(); it != m_vectorTxtAnswer.end(); it++)
		{
			(*it)->SetMaxLength(length);
		}
	}
}

/** Clear the question text field.
*
*/
void PollQuestionCtrl::ClearQuestionText()
{
	m_txtQuestion->SetValue(wxEmptyString);
}

/** Clear the answers texts fields.
*
*/
void PollQuestionCtrl::ClearAnswersText()
{
	int size = (int)m_vectorTxtAnswer.size();
	for(int i=0;i<size;i++)
	{
		m_vectorTxtAnswer[i]->SetValue(wxEmptyString);
	}
}

/** Clear the answers choices fields.
*
*/
void PollQuestionCtrl::ClearAnswersChoice()
{
	/*int size = (int)m_vectorRdBtnAnswer.size();
	for(int i=0;i<size;i++)
	{
		m_vectorRdBtnAnswer[i]->SetValue(false);
	}*/

	m_rdBtnErased->SetValue(true);
}

/** Clear all fields of this control.
*
*/
void PollQuestionCtrl::ClearAll()
{
	ClearQuestionText();
	ClearAnswersChoice();
	ClearAnswersText();
}

/*----------------------------Begin event functions definitions-----------------------------------------*/
void PollQuestionCtrl::OnBtnAddAnswerClick(wxCommandEvent &event)
{
	int numberOfAnswer = m_numberOfAnswers;
	/*-------------------------------------------Checks if user can remove answer-----------------------------------------*/
	SetNumberOfAnswers(false);

	/*-------------------------------------------Remove a answser area-----------------------------------------*/
	if (m_statusAnswerRange < POLLQUESTIONCTRL_MAXIMUM_ANSWER_RANGE)
	{				
		AddAnswerItem();
		m_numberOfAnswers++;
		m_txtSpinCtrl->SetValue(wxString::Format(wxT("%d"), m_numberOfAnswers));
	}
	else
		wxMessageBox(wxString::Format(_("Cannot choose number of answers greater than %d"), m_maxAnswers), _("Information"),
		 wxCENTRE | wxICON_INFORMATION, this);


/*-----------------------------------------END Remove a answser area---------------------------------------*/
}

void PollQuestionCtrl::OnBtnRemoveAnswerClick(wxCommandEvent &event)
{
	int numberOfAnswer = m_numberOfAnswers;
	/*-------------------------------------------Checks if user can remove answer-----------------------------------------*/
	SetNumberOfAnswers(true);

	/*-------------------------------------------Remove a answser area-----------------------------------------*/
	if (m_statusAnswerRange > POLLQUESTIONCTRL_MINIMAL_ANSWER_RANGE)
	{				
		RemoveAnswerItem();
		m_numberOfAnswers--;
		m_txtSpinCtrl->SetValue(wxString::Format(wxT("%d"), m_numberOfAnswers));
	}
	else
		wxMessageBox(wxString::Format(_("Cannot choose number of answers less than %d"), m_minAnswers), 
		_("Information"), wxCENTRE | wxICON_INFORMATION, this);

/*-----------------------------------------END Remove a answser area---------------------------------------*/
}

void PollQuestionCtrl::OnSize(wxSizeEvent &event)
{
	
}
/*-----------------------------End event functions definitions------------------------------------------*/

/** Adds a number of answers for the control.
* @pram[in] count. Number of answers thats must be added.
* Private method.
*/
void PollQuestionCtrl::AddAnswerItem(unsigned count)
{
	for(unsigned i = 0; i < count; i++)
	{
		int item = m_numberOfAnswers + i;

		wxRadioButton *rdBtnAnswer = new wxRadioButton(m_mainPanel, m_itemAnswer, wxString::Format(_("%c)"), m_itemAnswer));
		wxTextCtrl *txtAnswer = new wxTextCtrl(m_mainPanel, 30 + m_itemAnswer, wxEmptyString, wxPoint(10, item*20+100), wxSize(300,-1));
		wxBoxSizer *itemAnswerSizer = new wxBoxSizer(wxHORIZONTAL);

		txtAnswer->SetMaxLength(m_maxAnswerInputLength);

		m_itemAnswer++;

		m_vectorRdBtnAnswer.push_back(rdBtnAnswer);
		m_vectorTxtAnswer.push_back(txtAnswer);

		itemAnswerSizer->Add(m_vectorRdBtnAnswer[item], 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP, 8);
		itemAnswerSizer->Add(m_vectorTxtAnswer[item], 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5);

		itemAnswerSizer->Layout();
		
		m_vectorAnswerItemSizer.push_back(itemAnswerSizer);

		m_answerAreaVertSizer->Add(m_vectorAnswerItemSizer[item], 0, wxEXPAND|wxALL, 5);

		m_answerAreaVertSizer->Layout();

		m_mainPanel->Layout();
	}
}

/** Removes a number of answers of the control.
* @pram[in] count. Number of answers thats must be removed.
* Private method.
*/
void PollQuestionCtrl::RemoveAnswerItem(unsigned count)
{
	for(unsigned i = 0; i < count; i++)
	{
		wxBoxSizer *itemAnswerSizer = m_vectorAnswerItemSizer[m_vectorAnswerItemSizer.size()-1];
		
		wxSize itemAnswerSizerSize = itemAnswerSizer->GetSize();

		itemAnswerSizer->Clear(true);

		bool removed = m_answerAreaVertSizer->Detach(itemAnswerSizer);

		//wxRadioButton *rdBtnAnswer = m_vectorRdBtnAnswer[m_vectorRdBtnAnswer.size()-1];
		m_vectorRdBtnAnswer.pop_back();
		//rdBtnAnswer->Destroy();

		//wxTextCtrl *txtAnswer = m_vectorTxtAnswer[m_vectorTxtAnswer.size()-1];
		m_vectorTxtAnswer.pop_back();

		//txtAnswer;
		m_vectorAnswerItemSizer.pop_back();
		delete itemAnswerSizer;

		m_answerAreaVertSizer->Layout();
		/*m_pollQuestionVertSizer->Layout();*/

		/*m_answerAreaVertSizer->Show(false);
		m_answerAreaVertSizer->Show(true);*/

		m_itemAnswer--;
	}
}

/** Show the number of answers selected in a text control.
* @pram[in] remove. If remove equal true, use formula to discover total area of control adding more one on number of answers.
* Else, use formula to discover total area of control subtracting minus one on number of answers.
* Private method.
*/
void PollQuestionCtrl::SetNumberOfAnswers(bool remove)
{
	m_txtSpinCtrl->SetValue(wxString::Format(wxT("%d"), m_numberOfAnswers));

	m_statusAnswerRange = CheckAnswersRange();

	int pollTotalArea;
	if (remove)
		pollTotalArea = m_questionSize.y + (m_numberOfAnswers -1)* m_answerSize.y;
	else
		pollTotalArea = m_questionSize.y + (m_numberOfAnswers +1)* m_answerSize.y;

	int controlArea = this->GetSize().y;
	
	if ((m_statusAnswerRange == POLLQUESTIONCTRL_INSIDE_ANSWER_RANGE) && (pollTotalArea > controlArea)) 
	{
		m_mainPanel->SetScrollbars(0, 60, 0, m_numberOfAnswers,0,0, true);
		m_answerAreaVertSizer->Layout();
		m_mainPanel->SetScrollbars(0, 60, 0, m_numberOfAnswers,0,0, true);
		m_answerAreaVertSizer->Layout();
	}
}
