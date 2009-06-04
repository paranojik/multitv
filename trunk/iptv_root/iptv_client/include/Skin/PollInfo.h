#ifndef POLLINFO_H
#define POLLINFO_H

#include <wx/wx.h>
#include <vector>

#include "PollItem.h"

#define MIN_ANSWER_ITEMS	2
#define BLANK_ANSWER_TEXT	_("No answer")

/** @brief Holds Poll information.
 *
 */
class PollInfo
{
private:
	wxString m_pollStarter;
	wxString m_questionText;
	PollItem m_blankAnswer;
	std::vector<PollItem> m_answers;
	int m_totalVotes;
	unsigned m_usersSentTo;
	bool m_valid;

	void Validate();
public:
	PollInfo();

	void SetPollStarter(const wxString &user);
	void SetQuestionText(const wxString &text);
	void SetNumberOfUsersSentTo(unsigned number);
	void AddAnswerItem(const PollItem &item);
	void AddAnswerItem(const wxString &itemText);
	void RemoveAnswerItem(int index);

	void Clear();

	bool IsValid() const;

	void AddVote(int index, int count = 1);
	void ClearVotes();
	void ComputeTotalVotes();
	int GetNumberOfVotes(int index) const;
	int GetTotalVotes() const;
	unsigned GetNumberOfUsersSentTo() const;

	const wxString &GetPollStarter() const;
	const wxString &GetQuestionText() const;
	const PollItem &GetAnswerItem(int index) const;
	unsigned GetAnswerItemCount() const;
};

#endif
