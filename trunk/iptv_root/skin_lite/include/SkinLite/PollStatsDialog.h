#ifndef POLLSTATSWINDOW_H
#define POLLSTATSWINDOW_H

#include <wx/wx.h>
#include <vector>

#include "PollInfo.h"

#define POLL_STATS_DIALOG_MAX_SIZE		wxSize(600,400)
#define POLL_STATS_DIALOG_PNG_PATH		wxT("PollGraphicResult.png")

enum{
	ID_POLL_STATS_DIALOG_BTN_GRAPHIC = wxID_HIGHEST + 10
};

/** @brief Class thats implements dialog with results statistics of poll. 
*
*/
class PollStatsDialog: public wxDialog
{
private:
	PollInfo m_info;

	wxStaticText *m_lblQuestionText;
	std::vector<wxStaticText *> m_lblOptions;
	std::vector<wxStaticText *> m_lblStatValues;
	wxFlexGridSizer *m_optionsGridSizer;
	wxStaticText *m_lblBlankVotes, *m_lblBlankVoteText;
	wxStaticText *m_lblTotalVotes;
	wxButton *m_btnClose;
	wxButton *m_btnGraphic;
	bool m_controlsCreated;

	void CreateControls();

public:
	PollStatsDialog();
	PollStatsDialog(const PollInfo &info);
	PollStatsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxString& title = _("Poll Statistics"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));
	PollStatsDialog(const PollInfo &info,
		wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxString& title = _("Poll Statistics"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));
	
	void SetPollInfo(const PollInfo &info);
	void UpdateInfo();
	
	bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxString& title = _("Poll Statistics"), 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxDEFAULT_DIALOG_STYLE, const wxString& name = wxT("dialogBox"));

	void OnCloseButtonClick(wxCommandEvent &event);
	void OnGraphicButtonClick(wxCommandEvent &event);
};

#endif
