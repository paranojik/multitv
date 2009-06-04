#include "iptv_kernel/PollCtcpMessage.h"

using namespace br::com::sbVB::VBLib;

const int PollCtcpMessage::m_firstAnswerId						= 3;
const int PollCtcpMessage::m_noAnswerId							= 0;
const int PollCtcpMessage::m_noAnswerIndex						= -1;
const int PollCtcpMessage::m_answerCountFieldSize				= 5;

const VBString PollCtcpMessage::m_ctcpPollBegin					= "SNDRESET";
const VBString PollCtcpMessage::m_ctcpPollQuestion				= "SNDQUESTION";
const VBString PollCtcpMessage::m_ctcpPollOption				= "SNDOPTION";
const VBString PollCtcpMessage::m_ctcpPollEnd					= "SNDEND";

const VBString PollCtcpMessage::m_ctcpPollAnswer				= "SENDANSWER";

const VBString PollCtcpMessage::m_ctcpPollStatsBegin			= "RCVRESET";
const VBString PollCtcpMessage::m_ctcpPollStatsQuestion			= "RCVQUESTION";
const VBString PollCtcpMessage::m_ctcpPollStatsOption			= "RCVOPTION";
const VBString PollCtcpMessage::m_ctcpPollStatsOptionNoAnswer	= "RCVNOANSWER";
const VBString PollCtcpMessage::m_ctcpPollStatsEnd				= "RCVEND";

PollCtcpMessage::PollCtcpMessage() : CtcpMessage("")
{
	Reset();
}

PollCtcpMessage::PollCtcpMessage(VBString message) : CtcpMessage(message)
{
	Reset();
	ParseParameters();
}

bool PollCtcpMessage::IsPollCtcpMessage() const
{
	return m_isPollCtcp;
}

bool PollCtcpMessage::IsPollBegin() const
{
	return m_isPollBegin;
}

bool PollCtcpMessage::IsPollQuestion() const
{
	return m_isPollQuestion;
}

bool PollCtcpMessage::IsPollOption() const
{
	return m_isPollOption;
}

bool PollCtcpMessage::IsPollEnd() const
{
	return m_isPollEnd;
}

bool PollCtcpMessage::IsPollAnswer() const
{
	return m_isPollAnswer;
}

bool PollCtcpMessage::IsPollStatsBegin() const
{
	return m_isPollStatsBegin;
}

bool PollCtcpMessage::IsPollStatsQuestion() const
{
	return m_isPollStatsQuestion;
}

bool PollCtcpMessage::IsPollStatsOption() const
{
	return m_isPollStatsOption;
}

bool PollCtcpMessage::IsPollStatsEnd() const
{
	return m_isPollStatsEnd;
}

VBString PollCtcpMessage::GetText() const
{
	return m_text;
}

long PollCtcpMessage::GetOptionIndex() const
{
	return  m_optionIndex;
}

long PollCtcpMessage::GetAnswerCount() const
{
	return m_answerCount;
}

void PollCtcpMessage::SetText(br::com::sbVB::VBLib::VBString text)
{
	m_text = text;
}

void PollCtcpMessage::SetOptionIndex(long optionId)
{
	 m_optionIndex = optionId;
}

void PollCtcpMessage::SetAnswerCount(long answerCount)
{
	m_answerCount = answerCount;
}

void PollCtcpMessage::SetPollMessageCode(PollMessageCode code)
{
	Reset();
	m_isPollCtcp = true;

	switch(code)
	{
		case POLL_MESSAGE_CODE_BEGIN:
			m_isPollBegin = true;
		break;
		case POLL_MESSAGE_CODE_QUESTION:
			m_isPollQuestion = true;
		break;
		case POLL_MESSAGE_CODE_OPTION:
			m_isPollOption = true;
		break;
		case POLL_MESSAGE_CODE_END:
			m_isPollEnd = true;
		break;
		case POLL_MESSAGE_CODE_ANSWER:
			m_isPollAnswer = true;
		break;
		case POLL_MESSAGE_CODE_STATS_BEGIN:
			m_isPollStatsBegin = true;
		break;
		case POLL_MESSAGE_CODE_STATS_QUESTION:
			m_isPollStatsQuestion = true;
		break;
		case POLL_MESSAGE_CODE_STATS_OPTION:
			m_isPollStatsOption = true;
		break;
		case POLL_MESSAGE_CODE_STATS_END:
			m_isPollStatsEnd = true;
		break;
	}
}

VBString PollCtcpMessage::GetPollCtcpMessage() const
{
	VBString ctcpMessage;

	if(m_isPollBegin)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollBegin;
		ctcpMessage += m_ctcpPrefix;
	}
	else if(m_isPollQuestion)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollQuestion;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_text;
	}
	else if(m_isPollOption)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollOption;
		if(( m_optionIndex < 0))
			return "";
		char option;
		option = 'A' + (char) m_optionIndex;
		if(option > 'Z')
			return "";
		ctcpMessage += option;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_text;
	}
	else if(m_isPollEnd)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollEnd;
		ctcpMessage += m_ctcpPrefix;
	}
	else if(m_isPollAnswer)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollAnswer;
		ctcpMessage += m_ctcpPrefix;
		if( m_optionIndex == m_noAnswerIndex)
			ctcpMessage += m_noAnswerId;
		else
		{
			if( m_optionIndex < 0)
				return "";
			char option;
			option = 'A' + (char) m_optionIndex;
			if(option > 'Z')
				return "";
			int optionIndex =  m_optionIndex + m_firstAnswerId;
			VBString optionString;
			optionString += optionIndex;
			ctcpMessage += optionString;
		}
	}
	else if(m_isPollStatsBegin)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollStatsBegin;
		ctcpMessage += m_ctcpPrefix;
	}
	else if(m_isPollStatsQuestion)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollStatsQuestion;
		ctcpMessage += m_ctcpPrefix;
		ctcpMessage += m_text;
	}
	else if(m_isPollStatsOption)
	{
		ctcpMessage = m_ctcpPrefix;
		if( m_optionIndex == m_noAnswerIndex)
			ctcpMessage += m_ctcpPollStatsOptionNoAnswer;
		else
		{
			if( m_optionIndex < 0)
				return "";
			ctcpMessage += m_ctcpPollStatsOption;
			char option;
			option = 'A' + (char) m_optionIndex;
			if(option > 'Z')
				return "";
			ctcpMessage += option;
		}
		ctcpMessage += m_ctcpPrefix;
		char count[6];
		int c = 0;
		if(m_answerCount < pow((double)10, (double)m_answerCountFieldSize))
			c = m_answerCount;
		sprintf(count, "%5d", c);
		ctcpMessage += count;
		ctcpMessage += m_text;
	}
	else if(m_isPollStatsEnd)
	{
		ctcpMessage = m_ctcpPrefix;
		ctcpMessage += m_ctcpPollStatsEnd;
		ctcpMessage += m_ctcpPrefix;
	}

	return ctcpMessage;
}

void PollCtcpMessage::ParseParameters()
{
	if(GetCtcpMessage() == m_ctcpPollBegin)
	{
		m_isPollBegin = true;
		m_isPollCtcp = true;	
	}

	if(GetCtcpMessage() == m_ctcpPollQuestion)
	{
		m_isPollQuestion = true;
		m_isPollCtcp = true;
		m_text = GetParameters();
	}

	if(GetCtcpMessage().strUntilPosition(m_ctcpPollOption.size() - 1) == m_ctcpPollOption)
	{
		m_isPollOption = true;
		m_isPollCtcp = true;
		m_text = GetParameters();

		// Reseting  m_optionIndex value.
		 m_optionIndex = -1;

		// Getting  m_optionIndex value.
		char option = GetCtcpMessage().getChar(m_ctcpPollOption.size());
		if(isalpha(option))
		{
			if((option >= 'A') && (option) <= 'Z')
			{
				 m_optionIndex = option - 'A';
			}
		}
	}

	if(GetCtcpMessage() == m_ctcpPollEnd)
	{
		m_isPollEnd = true;
		m_isPollCtcp = true;	
	}

	if(GetCtcpMessage() == m_ctcpPollAnswer)
	{
		m_isPollAnswer = true;
		m_isPollCtcp = true;

		// Reseting  m_optionIndex value.
		 m_optionIndex = -1;

		VBString option = GetParameters();

		 m_optionIndex = (long) atoi(option.c_str());
		 m_optionIndex -= m_firstAnswerId;

		if( m_optionIndex < 0)
			 m_optionIndex = -1;
	}

	if(GetCtcpMessage() == m_ctcpPollStatsBegin)
	{
		m_isPollStatsBegin = true;
		m_isPollCtcp = true;	
	}

	if(GetCtcpMessage() == m_ctcpPollStatsQuestion)
	{
		m_isPollStatsQuestion = true;
		m_isPollCtcp = true;
		m_text = GetParameters();
	}

	if(GetCtcpMessage().strUntilPosition(m_ctcpPollStatsOption.size() - 1) == m_ctcpPollStatsOption)
	{
		m_isPollStatsOption = true;
		m_isPollCtcp = true;

		VBString parameters = GetParameters();

		m_answerCount = atoi(parameters.strUntilPosition(m_answerCountFieldSize - 1).c_str());
		m_text = parameters.strAfterPosition(m_answerCountFieldSize);

		// Reseting  m_optionIndex value.
		 m_optionIndex = -1;

		// Getting  m_optionIndex value.
		char option = GetCtcpMessage().getChar(m_ctcpPollStatsOption.size());
		if(isalpha(option))
		{
			if((option >= 'A') && (option) <= 'Z')
			{
				 m_optionIndex = option - 'A';
			}
		}
	}

	if(GetCtcpMessage() == m_ctcpPollStatsOptionNoAnswer)
	{
		m_isPollStatsOption = true;
		m_isPollCtcp = true;
		 m_optionIndex = -1;

		VBString parameters = GetParameters();
		m_answerCount = atoi(parameters.strUntilPosition(m_answerCountFieldSize - 1).c_str());
	}

	if(GetCtcpMessage() == m_ctcpPollStatsEnd)
	{
		m_isPollStatsEnd = true;
		m_isPollCtcp = true;	
	}
}

void PollCtcpMessage::Reset()
{
	m_isPollCtcp = false;
	m_isPollBegin = false;
	m_isPollQuestion = false;
	m_isPollOption = false;
	m_isPollEnd = false;
	m_isPollAnswer = false;
	m_isPollStatsBegin = false;
	m_isPollStatsQuestion = false;
	m_isPollStatsOption = false;
	m_isPollStatsEnd = false;
	m_optionIndex = -1;
	m_answerCount = 0;
}
