#ifndef POLLANSWERDIALOG_H
#define POLLANSWERDIALOG_H

#include <wx/wx.h>

#include "PollQuestionCtrl.h"
#include "PollInfo.h"
#include "AppInterface.h"

#define POLL_MIN_ANSWERS	2
#define POLL_MAX_ANSWERS	5

enum
{
	PAD_BUTTON_ID_BLANK	= wxID_HIGHEST + 4000,
	PAD_BUTTON_ID_VOTE
};

/** @brief Class thats implements poll answer dialog.
*
* When a user thats have voice send a poll, the others users receiving this dialog to answer.
*/
class PollAnswerDialog: public wxDialog
{
public:
	PollAnswerDialog();
	PollAnswerDialog(wxWindow *parent, wxWindowID id = wxID_ANY, 
		const wxString &title = _("Poll"), 
		const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxT("PollAnswerDialog"));

	bool Create(wxWindow *parent, wxWindowID id = wxID_ANY, 
		const wxString &title = _("Poll"), 
		const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString &name = wxT("PollAnswerDialog"));

	void SetAppInterface(AppInterface *iface);

	void OnBlankButtonClick(wxCommandEvent &event);
	void OnVoteButtonClick(wxCommandEvent &event);

private:
	PollQuestionCtrl *m_pollQuestionCtrl;
	AppInterface *m_appInterface;
	wxButton *m_btnBlank;
	wxButton *m_btnVote;

	void Init();
	void CreateControls();
	void UpdateAnswerArea();

	DECLARE_EVENT_TABLE()
};

#endif
