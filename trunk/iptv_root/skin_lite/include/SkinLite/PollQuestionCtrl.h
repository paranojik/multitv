#ifndef _POLLQUESTIONCTRL_H_
#define _POLLQUESTIONCTRL_H_

#include <wx/wx.h>
#include <wx/spinbutt.h>
#include <vector>


#define POLLQUESTIONCTRL_MODE_SENDPOLL true
#define POLLQUESTIONCTRL_MODE_RECEIVEPOLL false
#define POLLQUESIONCTRL_GETANSWERCHOICE_ERROR -1

#define POLLQUESTIONCTRL_DEFAULT_MAX_INPUT_LENGTH	128

enum{
	ID_POLLQUESTIONCTRL_MAINPANEL = 1000,
	ID_POLLQUESTIONCTRL_BTN_ADDANSWER,
	ID_POLLQUESTIONCTRL_BTN_REMOVEANSWER,
	ID_POLLQUESTIONCTRL_TXT_SPINCTRL,
	ID_POLLQUESTIONCTRL_TXT_QUESTION
};

/** @brief Control created to auxiliary creation of poll send.
*
* This class implements the area where user selects the number of answers and fill them and the question.
*/
class PollQuestionCtrl: public wxControl
{
private:
	//wxPanel *m_mainPanel;

	wxScrolledWindow *m_mainPanel;

	//wxPanel *m_mainPanel;

	wxStaticText *m_lblNumberOfAnswers;
	wxTextCtrl *m_txtSpinCtrl;
	wxButton *m_btnAddAnswer;
	wxButton *m_btnRemoveAnswer;
	
	wxStaticText *m_lblQuestion;
	wxTextCtrl *m_txtQuestion;

	wxBoxSizer* m_spinCtrlHorSizer;
	wxBoxSizer* m_pollQuestionVertSizer;
	wxBoxSizer* m_pollQuestionHorSizer;
	wxBoxSizer* m_answerAreaVertSizer;

	int m_minAnswers;
	int m_maxAnswers;
	int m_defaultAnswers;
	int m_numberOfAnswers;
	int m_itemAnswer;

	bool m_mode;
	unsigned long m_maxAnswerInputLength;

	int m_statusAnswerRange;

	wxSize m_controlSize;

	wxSize m_answerSize;
	wxSize m_questionSize;
	
	wxRadioButton *m_rdBtnErased;

	std::vector<wxRadioButton *> m_vectorRdBtnAnswer;
	std::vector<wxTextCtrl *> m_vectorTxtAnswer;
	std::vector<wxBoxSizer *> m_vectorAnswerItemSizer;

	wxString m_strQuestion;
	int m_answersChoice;
	std::vector<wxString> m_vectorStrAnswers;

	void CreateAnswersArea(const bool &mode);
	int CheckAnswersRange();

	void AddAnswerItem(unsigned count = 1);
	void RemoveAnswerItem(unsigned count = 1);
	void SetNumberOfAnswers(bool remove);

//	bool HasScrollbar(int orient) const{return true;}

public:
	PollQuestionCtrl(){Initialize();}
	PollQuestionCtrl(wxWindow *parent, wxWindowID id, int minAnswers, int maxAnswers,
													int defaultAnswers,
													bool mode = POLLQUESTIONCTRL_MODE_SENDPOLL,
													const wxPoint& pos = wxDefaultPosition,
													const wxSize& size = wxDefaultSize,
													long style = wxSUNKEN_BORDER|wxTAB_TRAVERSAL);
	~PollQuestionCtrl();

	bool Create(wxWindow *parent, wxWindowID id, int minAnswers, int maxAnswers,
													int defaultAnswers, 
													bool mode = POLLQUESTIONCTRL_MODE_SENDPOLL,
													const wxPoint& pos = wxDefaultPosition,
													const wxSize& size = wxDefaultSize,
													long style = wxSUNKEN_BORDER|wxTAB_TRAVERSAL/*|wxVSCROLL*/);
	wxSize DoGetBestSize() const;

	void Initialize();
	void CreateControls();
	void SetPollCtrlLayout();
	void ClearQuestionText();
	void ClearAnswersText();
	void ClearAnswersChoice();
	void ClearAll();
	
	wxString GetQuestionText();
	int GetNumberOfAnswers(){return m_numberOfAnswers;}
	int GetAnswerChoice();
	std::vector<wxString> GetStrAnswers();

	void SetQuestionText(const wxString &question);
	void SetAnswersText(const std::vector<wxString> &vectorAnswers);

	void SetAnswerInputLength(unsigned long length);

	//Begin event functions
	void OnSize(wxSizeEvent &event);
	void OnBtnAddAnswerClick(wxCommandEvent &event);
	void OnBtnRemoveAnswerClick(wxCommandEvent &event);
	//End event functions
	
	DECLARE_DYNAMIC_CLASS(PollQuestionCtrl)
	DECLARE_EVENT_TABLE()
};

#endif
