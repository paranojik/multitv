#include <wx/wx.h>
#include "PollItem.h"

/** Default constructor.
 */
PollItem::PollItem()
{
	m_answerCount = 0;
}

/** Returns the item text.
 * @return The item text.
 */
const wxString &PollItem::GetText() const
{
	return m_text;
}

/** Returns the number of votes for this item.
 * @return The number of votes.
 */
int PollItem::GetAnswerCount() const
{
	return m_answerCount;
}

/** Sets item text.
 * @param[in] text	The text to set.
 */
void PollItem::SetText(const wxString &text)
{
	m_text = text;
}

/** Increase vote count.
 * @param[in] increment	Number by which to increase the count.
 */
void PollItem::IncreaseAnswerCount(int increment)
{
	m_answerCount += increment;
}

/** Resets the vote count.
 */
void PollItem::ClearAnswerCount()
{
	m_answerCount = 0;
}
