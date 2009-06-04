#ifndef POLLITEM_H
#define POLLITEM_H

#include <wx/wx.h>
/** @brief Represents one item (option) of the Poll.
 *
 */
class PollItem
{
private:
	wxString m_text;
	int m_answerCount;
public:
	PollItem();

	const wxString &GetText() const;
	int GetAnswerCount() const;

	void SetText(const wxString &text);
	void IncreaseAnswerCount(int increment = 1);
	void ClearAnswerCount();
};

#endif
