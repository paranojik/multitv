/////////////////////////////////////////////////////////////////////////////
// Name:        SendPollFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     26/05/2008 10:50:59
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "SkinLiteConstants.h"
////@end includes

#include "SendPollFrame.h"
#include "AppInterface.h"
#include "PollInfo.h"

////@begin XPM images
////@end XPM images

#define SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_SIZE wxSize(50, -1)
#define SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_MINANSWERS 2
#define SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_MAXANSWERS 5		// Compatibility with existing Win32 client
#define SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_DEFAULTANSWERS 2

/*!
 * SendPollFrame type definition
 */

IMPLEMENT_CLASS( SendPollFrame, wxFrame )


/*!
 * SendPollFrame event table definition
 */

BEGIN_EVENT_TABLE( SendPollFrame, wxFrame )

////@begin SendPollFrame event table entries
    //EVT_SPINCTRL( ID_SENDPOLLFRAME_SPINCTRL_TOTALANSWERS, SendPollFrame::OnSpinbtnTotalAnswersUpdated )
//	EVT_SPIN_UP( ID_SENDPOLLFRAME_SPINCTRL_TOTALANSWERS, SendPollFrame::OnSpinbtnTotalAnswersUpdated )
//	EVT_SPIN_DOWN( ID_SENDPOLLFRAME_SPINCTRL_TOTALANSWERS, SendPollFrame::OnSpinbtnTotalAnswersUpdated )

    EVT_BUTTON( ID_SENDPOLLFRAME_BTN_ERASEPOLL, SendPollFrame::OnBtnErasePollClick )

    EVT_BUTTON( ID_SENDPOLLFRAME_BTN_SENDPOLL, SendPollFrame::OnBtnSendPollClick )

    EVT_BUTTON( ID_SENDPOLLFRAME_BTN_VISUALIZESTATISTIC, SendPollFrame::OnBtnVisualizeStatisticClick )

    EVT_BUTTON( ID_SENDPOLLFRAME_BTN_SENDSTATISTIC, SendPollFrame::OnBtnSendStatisticClick )

////@end SendPollFrame event table entries

END_EVENT_TABLE()


/** SendPollFrame default constructor.
 * 
 */
SendPollFrame::SendPollFrame()
{
    Init();
}

/** SendPollFrame constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_SENDPOLLFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SENDPOLLFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SENDPOLLFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SENDPOLLFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SENDPOLLFRAME_STYLE.
*/
SendPollFrame::SendPollFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
	SetMinSize(SYMBOL_SENDPOLLFRAME_SIZE);
}

/** SendPollFrame creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_SENDPOLLFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SENDPOLLFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SENDPOLLFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SENDPOLLFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SENDPOLLFRAME_STYLE.
*/
bool SendPollFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin SendPollFrame creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
////@end SendPollFrame creation
    return true;
}


/** SendPollFrame destructor.
 *
 */
SendPollFrame::~SendPollFrame()
{
////@begin SendPollFrame destruction
////@end SendPollFrame destruction
}


/*!
 * Member initialisation
 */

void SendPollFrame::Init()
{
////@begin SendPollFrame member initialisation
    m_mainVertSizer = NULL;
    m_mainPanel = NULL;
    m_mainPanelVertSizer = NULL;
//    m_totalAnswersSizer = NULL;
//    m_lblTotalAnswers = NULL;
//    m_spinCtrlTotalAnswers = NULL;
//    m_lblQuestion = NULL;
//    m_txtQuestion = NULL;
//    m_PollCtrlSizer = NULL;
    m_btnErasePoll = NULL;
    m_btnSendPoll = NULL;
    m_groupBoxStatisticSizer = NULL;
    m_statisticGridSizer = NULL;
    m_lblSentQuestionsCaption = NULL;
    m_lblSentQuestions = NULL;
    m_lblReceivedAnswersCaption = NULL;
    m_lblReceivedAnswers = NULL;
    m_statisticCtrlSizer = NULL;
    m_btnVisualizeStatistic = NULL;
    m_btnSendStatistic = NULL;
	m_pollStatsDialog = NULL;
	m_appInterface = NULL;
////@end SendPollFrame member initialisation
}

/** Sets linkage with application.
* @param[in] appInterface. Application interface.
*/
void SendPollFrame::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
	if(m_appInterface)
		m_appInterface->ResetActivePoll();
}

/*!
 * Control creation for SendPollFrame
 */

void SendPollFrame::CreateControls()
{    
////@begin SendPollFrame content construction
    SendPollFrame* itemFrame1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_SENDPOLLFRAME_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertSizer->Add(m_mainPanel, 1, wxGROW, 5);

    m_mainPanelVertSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(m_mainPanelVertSizer);

	m_pollQuestionCtrl = new PollQuestionCtrl(m_mainPanel, ID_SENDPOLLFRAME_POLLQUESTIONCTRL, SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_MINANSWERS, SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_MAXANSWERS, SENDPOLLFRAME_SPINCTRL_ANSWERSTOTAL_DEFAULTANSWERS, POLLQUESTIONCTRL_MODE_SENDPOLL, wxDefaultPosition, wxSize(500, 400));
	m_mainPanelVertSizer->Add(m_pollQuestionCtrl, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_pollCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanelVertSizer->Add(m_pollCtrlSizer, 0, wxALIGN_RIGHT|wxALL, 5);

    m_btnErasePoll = new wxButton( m_mainPanel, ID_SENDPOLLFRAME_BTN_ERASEPOLL, SENDPOLLFRAME_BTN_ERASEPOLL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_pollCtrlSizer->Add(m_btnErasePoll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnSendPoll = new wxButton( m_mainPanel, ID_SENDPOLLFRAME_BTN_SENDPOLL, SENDPOLLFRAME_BTN_SENDPOLL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_pollCtrlSizer->Add(m_btnSendPoll, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_groupBoxStatisticSizer = new wxStaticBox(m_mainPanel, wxID_ANY, _("Statistics"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(m_groupBoxStatisticSizer, wxHORIZONTAL);
    m_mainPanelVertSizer->Add(itemStaticBoxSizer14, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_statisticGridSizer = new wxGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer14->Add(m_statisticGridSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentQuestionsCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDPOLLFRAME_LBL_SENTQUESTIONS_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblSentQuestionsCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentQuestions = new wxStaticText( m_mainPanel, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblSentQuestions, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblReceivedAnswersCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDPOLLFRAME_LBL_RECEIVEDANSWERS_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblReceivedAnswersCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblReceivedAnswers = new wxStaticText( m_mainPanel, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblReceivedAnswers, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_statisticCtrlSizer = new wxBoxSizer(wxVERTICAL);
    itemStaticBoxSizer14->Add(m_statisticCtrlSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnVisualizeStatistic = new wxButton( m_mainPanel, ID_SENDPOLLFRAME_BTN_VISUALIZESTATISTIC, SENDPOLLFRAME_BTN_VISUALIZESTATISTIC_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticCtrlSizer->Add(m_btnVisualizeStatistic, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_btnSendStatistic = new wxButton( m_mainPanel, ID_SENDPOLLFRAME_BTN_SENDSTATISTIC, SENDPOLLFRAME_BTN_SENDSTATISTIC_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticCtrlSizer->Add(m_btnSendStatistic, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end SendPollFrame content construction
}

void SendPollFrame::CreateAnswersCtrls()
{
	/*wxRadioButton *rdBtnAnswer;
	wxTextCtrl *txtAnswer;

	m_answersGridSizer->Clear(true);
	m_mainPanelVertSizer->Layout();
	this->Fit();
	SetMinSize(this->GetSize());

	int totalAnswers = m_spinCtrlTotalAnswers->GetValue();
	for(int i = 0;i<totalAnswers;i++)
	{
		rdBtnAnswer = new wxRadioButton( m_mainPanel, wxID_ANY, wxString::Format(_("%c)"),65 + i), wxDefaultPosition, wxDefaultSize, 0 );
		rdBtnAnswer->SetValue(false);
		m_answersGridSizer->Add(rdBtnAnswer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

		txtAnswer = new wxTextCtrl( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		m_answersGridSizer->Add(txtAnswer, 0, wxALIGN_LEFT|wxGROW|wxALL, 5);
	}

	m_answersGridSizer->Layout();
	m_mainPanelVertSizer->Layout();
	this->Fit();
	SetMinSize(this->GetSize());
	this->Refresh();*/
}

/**Show the statistics for the current poll, or updates the information in the dialog if it is already present.
*
*/
void SendPollFrame::ShowPollStats()
{
	if(!m_appInterface)		// Can't do without it!!
		return;

	if(!m_pollStatsDialog)
	{
		const PollInfo *activePoll = m_appInterface->GetActivePoll();
		if(activePoll)
		{
			m_pollStatsDialog = new PollStatsDialog(*activePoll, this);
			m_pollStatsDialog->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(SendPollFrame::OnPollStatsDialogClose), NULL, this);
		}
		else
		{
			wxMessageBox(_("No poll statistics to show."), _("Error"), wxICON_ERROR, this);
			return;		// error: nothing to show!
		}
	}
	if(!m_pollStatsDialog->IsShown())
		m_pollStatsDialog->Show();
}

/** Updates the number of votes.
*
*/
void SendPollFrame::UpdateNumberOfVotes()
{
	if(!m_appInterface)
		return;

	const PollInfo *activePoll = m_appInterface->GetActivePoll();
	if(activePoll)
	{
		m_lblSentQuestions->SetLabel(wxString() << activePoll->GetNumberOfUsersSentTo());
		m_lblReceivedAnswers->SetLabel(wxString() << activePoll->GetTotalVotes());
	}
}

/*!
 * Should we show tooltips?
 */

bool SendPollFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap SendPollFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin SendPollFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end SendPollFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon SendPollFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin SendPollFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end SendPollFrame icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_ERASEPOLL
 */

void SendPollFrame::OnBtnErasePollClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_ERASEPOLL in SendPollFrame.
    // Before editing this code, remove the block markers.
    // event.Skip();
	m_pollQuestionCtrl->ClearAll();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_ERASEPOLL in SendPollFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_SENDPOLL
 */

void SendPollFrame::OnBtnSendPollClick( wxCommandEvent& event )
{
	if(!m_appInterface)
		return;

	wxString question = m_pollQuestionCtrl->GetQuestionText();
	int numOptions = m_pollQuestionCtrl->GetNumberOfAnswers();
	std::vector<wxString> options = m_pollQuestionCtrl->GetStrAnswers();
	PollInfo *pollInfo = new PollInfo;

	pollInfo->SetPollStarter(m_appInterface->GetCurrentUser()->GetNickName());
	pollInfo->SetQuestionText(question);
	for(int i = 0; i < numOptions; i++)
		pollInfo->AddAnswerItem(options[i]);
	if(!pollInfo->IsValid())
	{
		wxMessageBox(_("Invalid data."), _("Warning"), wxICON_EXCLAMATION, this);
		delete pollInfo;
	}
	else
	{
		if(!m_appInterface->SendPoll(pollInfo))
		{
			wxMessageBox(_("Couldn't send poll."), _("Error"), wxICON_ERROR, this);
			delete pollInfo;
		}
		else
		{
			UpdateNumberOfVotes();
			wxMessageBox(_("Poll sent successfully."), _("Success"), wxICON_INFORMATION, this);
		}
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_VISUALIZESTATISTIC
 */

void SendPollFrame::OnBtnVisualizeStatisticClick( wxCommandEvent& event )
{
	ShowPollStats();
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_SENDSTATISTIC
 */

void SendPollFrame::OnBtnSendStatisticClick( wxCommandEvent& event )
{
	if(!m_appInterface)
		return;
		
	m_appInterface->SendPollStats();
}

void SendPollFrame::OnPollStatsDialogClose(wxCloseEvent &event)
{
	m_pollStatsDialog->Destroy();
	m_pollStatsDialog = NULL;
}

/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SENDPOLLFRAME_SPINBTN_TOTALANSWERS
 */

void SendPollFrame::OnSpinbtnTotalAnswersUpdated( wxSpinEvent& event )
{
//	wxMessageBox(wxString::Format(wxT("Value = %d\nPosition = %d"), m_spinCtrlTotalAnswers->GetValue(), event.GetPosition()));

	CreateAnswersCtrls();
////@begin wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SENDPOLLFRAME_SPINBTN_TOTALANSWERS in SendPollFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SENDPOLLFRAME_SPINBTN_TOTALANSWERS in SendPollFrame. 
}
