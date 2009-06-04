#ifndef POLL_CTCP_MESSAGE_H
#define POLL_CTCP_MESSAGE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/CtcpMessage.h"

enum PollMessageCode
{
	POLL_MESSAGE_CODE_BEGIN,
	POLL_MESSAGE_CODE_QUESTION,
	POLL_MESSAGE_CODE_OPTION,
	POLL_MESSAGE_CODE_END,
	POLL_MESSAGE_CODE_ANSWER,
	POLL_MESSAGE_CODE_STATS_BEGIN,
	POLL_MESSAGE_CODE_STATS_QUESTION,
	POLL_MESSAGE_CODE_STATS_OPTION,
	POLL_MESSAGE_CODE_STATS_END
};

class PollCtcpMessage : public CtcpMessage
{
public:
	PollCtcpMessage();
	PollCtcpMessage(br::com::sbVB::VBLib::VBString message);

	bool IsPollCtcpMessage() const;
	bool IsPollBegin() const;
	bool IsPollQuestion() const;
	bool IsPollOption() const;
	bool IsPollEnd() const;
	bool IsPollAnswer() const;
	bool IsPollStatsBegin() const;
	bool IsPollStatsQuestion() const;
	bool IsPollStatsOption() const;
	bool IsPollStatsEnd() const;

	br::com::sbVB::VBLib::VBString GetText() const;
	long GetOptionIndex() const;
	long GetAnswerCount() const;

	void SetText(br::com::sbVB::VBLib::VBString text);
	void SetOptionIndex(long optionId);
	void SetAnswerCount(long answerCount);
	void SetPollMessageCode(PollMessageCode code);

	br::com::sbVB::VBLib::VBString GetPollCtcpMessage() const;
	
private:
	void ParseParameters();
	void Reset();

	br::com::sbVB::VBLib::VBString m_text;
	long m_optionIndex;
	long m_answerCount;

	bool m_isPollCtcp;
	bool m_isPollBegin;
	bool m_isPollQuestion;
	bool m_isPollOption;
	bool m_isPollEnd;

	bool m_isPollAnswer;

	bool m_isPollStatsBegin;
	bool m_isPollStatsQuestion;
	bool m_isPollStatsOption;
	bool m_isPollStatsEnd;

	static const int m_firstAnswerId;
	static const int m_noAnswerId;
	static const int m_noAnswerIndex;
	static const int m_answerCountFieldSize;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollBegin;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollQuestion;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollOption;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollEnd;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollAnswer;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollStatsBegin;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollStatsQuestion;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollStatsOption;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollStatsOptionNoAnswer;
	static const br::com::sbVB::VBLib::VBString m_ctcpPollStatsEnd;
};

#endif
