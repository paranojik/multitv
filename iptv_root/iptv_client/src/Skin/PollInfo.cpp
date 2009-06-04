#include <wx/wx.h>
#include <vector>

#include "PollItem.h"
#include "PollInfo.h"

using namespace std;

/** Default constructor.
 */
PollInfo::PollInfo()
{
	m_blankAnswer.SetText(BLANK_ANSWER_TEXT);
	Clear();
}

/** Set user name of the poll starter.
 * @param[in] user	The user name.
 */
void PollInfo::SetPollStarter(const wxString &user)
{
	m_pollStarter = user;
}

/** Set poll question text.
 * @param[in] text	The poll question.
 */
void PollInfo::SetQuestionText(const wxString &text)
{
	m_questionText = text;
	Validate();
}

/** Sets the number of users the poll has been sent to.
 * @param[in] number	The number of users.
 */
void PollInfo::SetNumberOfUsersSentTo(unsigned number)
{
	m_usersSentTo = number;
}

/** Add an item to the poll answers list.
 * @param[in] item	Item to be added.
 */
void PollInfo::AddAnswerItem(const PollItem &item)
{
	m_answers.push_back(item);
	Validate();
}


/** Add an item to the poll answers list.
 * @param[in] itemText	Text of the item to be added.
 */
void PollInfo::AddAnswerItem(const wxString &itemText)
{
	PollItem item;
	item.SetText(itemText);
	AddAnswerItem(item);
}

/** Removes an item from the answers list.
 * @param[in] index	The index of the item to remove.
 */
void PollInfo::RemoveAnswerItem(int index)
{
	if((index >= 0) && (index < (int)m_answers.size()))	// Basic bounds check
	{
		m_answers.erase(m_answers.begin() + (index - 1));
		Validate();
	}
}

/** Add vote(s) to a given item's vote count.
 * @param[in] index	Item being voted.
 * @param[in] count	Number of votes to add. Default: 1
 */
void PollInfo::AddVote(int index, int count)
{
	if(index < 0)
	{
		m_blankAnswer.IncreaseAnswerCount(count);
		m_totalVotes += count;
	}
	else
	{
		if(index < (int)m_answers.size())
		{
			m_answers[index].IncreaseAnswerCount(count);
			m_totalVotes += count;
		}
	}
}

/** Resets the vote count for all items in the Poll.
 */
void PollInfo::ClearVotes()
{
	m_blankAnswer.ClearAnswerCount();
	for(unsigned i = 0; i < GetAnswerItemCount(); i++)
		m_answers[i].ClearAnswerCount();
	m_totalVotes = 0;
}
/** Returns the current vote count for a given item.
 * @param[in] index	Index of the item being queried.
 * @return The vote count.
 */
int PollInfo::GetNumberOfVotes(int index) const
{
	if(index < 0)
		return m_blankAnswer.GetAnswerCount();
	else
	{
		if(index < (int)m_answers.size())
			return m_answers[index].GetAnswerCount();
		
		return -1;
	}
}

/** Returns the number of users the Poll has been sent to.
 * @return The number of users the Poll has been sent to.
 */
unsigned PollInfo::GetNumberOfUsersSentTo() const
{
	return m_usersSentTo;
}

/** Computes the grand total of all votes in the Poll.
 */
void PollInfo::ComputeTotalVotes()
{
	m_totalVotes = 0;
	for(int i = -1; i < (int)GetAnswerItemCount(); i++)
		m_totalVotes += GetNumberOfVotes(i);
}

/** Returns the previously computed total of all votes in the Poll.
 * @return The total number of votes.
 */
int PollInfo::GetTotalVotes() const
{
	return m_totalVotes;
}

/** Returns the nickname of the poll starter.
 * @return The poll starter's nickname.
 */
const wxString &PollInfo::GetPollStarter() const
{
	return m_pollStarter;
}

/** Returns the question text.
 * @return The question text.
 */
const wxString &PollInfo::GetQuestionText() const
{
	return m_questionText;
}

/** Returns a reference to the PollItem at given index.
 * @param[in] Index of the desired item.
 * @return A const reference to the PollItem at given index.
 */
const PollItem &PollInfo::GetAnswerItem(int index) const
{
	if(index < 0)
		return m_blankAnswer;

	return m_answers[index];
}

/** Get the number of answer items of this Poll.
 * @return The number of answers.
 */
unsigned PollInfo::GetAnswerItemCount() const
{
	return (unsigned)m_answers.size();
}

/** Checks poll validity.
 * @return true if the Poll is valid.
 */
bool PollInfo::IsValid() const
{
	return m_valid;
}

/** Clears the whole PollInfo structure. Invalidates the Poll.
 */
void PollInfo::Clear()
{
	m_answers.clear();
	m_questionText.Clear();
	m_totalVotes = 0;
	m_usersSentTo = 0;
	m_valid = false;
}

// Private function that checks validity of the contents.
void PollInfo::Validate()
{
	if((m_answers.size() >= MIN_ANSWER_ITEMS) && (m_questionText != wxEmptyString))
	{
		m_valid = true;
		for(unsigned i = 0; i < m_answers.size(); i++)
		{
			if(m_answers[i].GetText() == wxEmptyString)
			{
				m_valid = false;
				break;
			}
		}
	}		
	else
		m_valid = false;
}
