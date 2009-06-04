/////////////////////////////////////////////////////////////////////////////
// Name:        SendPollFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     26/05/2008 10:50:59
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _SENDPOLLFRAME_H_
#define _SENDPOLLFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
//#include "wx/spinctrl.h"
#include "PollQuestionCtrl.h"
#include "PollStatsDialog.h"
#include "AppInterface.h"

////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxSpinCtrl;
class wxGridSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
enum{
	ID_SENDPOLLFRAME = wxID_HIGHEST + 900,
	ID_SENDPOLLFRAME_MAINPANEL,
	//ID_SENDPOLLFRAME_SPINCTRL_TOTALANSWERS,
	//ID_SENDPOLLFRAME_TXT_QUESTION,
	ID_SENDPOLLFRAME_POLLQUESTIONCTRL,
	ID_SENDPOLLFRAME_BTN_ERASEPOLL,
	ID_SENDPOLLFRAME_BTN_SENDPOLL,
	ID_SENDPOLLFRAME_BTN_VISUALIZESTATISTIC,
	ID_SENDPOLLFRAME_BTN_SENDSTATISTIC
};
#define SYMBOL_SENDPOLLFRAME_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxFRAME_NO_TASKBAR
#define SYMBOL_SENDPOLLFRAME_TITLE _("Send Poll")
#define SYMBOL_SENDPOLLFRAME_IDNAME ID_SENDPOLLFRAME
#define SYMBOL_SENDPOLLFRAME_SIZE wxSize(520, 600)
#define SYMBOL_SENDPOLLFRAME_POSITION wxDefaultPosition
////@end control identifiers


/** @brief Class thats implements a new poll send window.
*
*/
class SendPollFrame: public wxFrame
{    
    DECLARE_CLASS( SendPollFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    SendPollFrame();
    SendPollFrame( wxWindow* parent, wxWindowID id = SYMBOL_SENDPOLLFRAME_IDNAME, const wxString& caption = SYMBOL_SENDPOLLFRAME_TITLE, const wxPoint& pos = SYMBOL_SENDPOLLFRAME_POSITION, const wxSize& size = SYMBOL_SENDPOLLFRAME_SIZE, long style = SYMBOL_SENDPOLLFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SENDPOLLFRAME_IDNAME, const wxString& caption = SYMBOL_SENDPOLLFRAME_TITLE, const wxPoint& pos = SYMBOL_SENDPOLLFRAME_POSITION, const wxSize& size = SYMBOL_SENDPOLLFRAME_SIZE, long style = SYMBOL_SENDPOLLFRAME_STYLE );

    /// Destructor
    ~SendPollFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	//Creates the controls for the answers
	void CreateAnswersCtrls();

	// Sets the App interface pointer.
	void SetAppInterface(AppInterface *iface);

////@begin SendPollFrame event handler declarations

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SENDPOLLFRAME_SPINBTN_TOTALANSWERS
    void OnSpinbtnTotalAnswersUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_ERASEPOLL
    void OnBtnErasePollClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_SENDPOLL
    void OnBtnSendPollClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_VISUALIZESTATISTIC
    void OnBtnVisualizeStatisticClick( wxCommandEvent& event );

	// Handles the PollStatsDialog Close event.
	void OnPollStatsDialogClose(wxCloseEvent &event);

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDPOLLFRAME_BTN_SENDSTATISTIC
    void OnBtnSendStatisticClick( wxCommandEvent& event );

////@end SendPollFrame event handler declarations

////@begin SendPollFrame member function declarations

	// Show or update poll statistics
	void ShowPollStats();
	void UpdateNumberOfVotes();

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SendPollFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin SendPollFrame member variables
    wxBoxSizer* m_mainVertSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_mainPanelVertSizer;
	wxBoxSizer* m_pollCtrlSizer;

	PollQuestionCtrl *m_pollQuestionCtrl;

	wxButton* m_btnErasePoll;
    wxButton* m_btnSendPoll;
    wxStaticBox* m_groupBoxStatisticSizer;
    wxGridSizer* m_statisticGridSizer;
    wxStaticText* m_lblSentQuestionsCaption;
    wxStaticText* m_lblSentQuestions;
    wxStaticText* m_lblReceivedAnswersCaption;
    wxStaticText* m_lblReceivedAnswers;
    wxBoxSizer* m_statisticCtrlSizer;
    wxButton* m_btnVisualizeStatistic;
    wxButton* m_btnSendStatistic;
	
	AppInterface *m_appInterface;
	PollStatsDialog *m_pollStatsDialog;
////@end SendPollFrame member variables
};

#endif
    // _SENDPOLLFRAME_H_
