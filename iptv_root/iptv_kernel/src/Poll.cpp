#include "iptv_kernel/Poll.h"
#include "iptv_kernel/PollNotificationParameters.h"
#include "chat/nOpCodes.h"
#include <list>

using namespace std;
using namespace br::com::sbVB::VBLib;

Poll::Poll()
{

}

void Poll::OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn)
{
	switch (msg.m_irmMessageCode)
	{
	//---------------------------------------------------------------------------------------------
	// Private message events
	//---------------------------------------------------------------------------------------------
	case PRIVMSG:
		OnUserPrivateMessage(msg.m_strSource, msg.m_strData);
		break;
	//---------------------------------------------------------------------------------------------
	case PRIVMSG_CHANNEL:
		OnChannelPrivateMessage(msg.m_strSource, msg.m_strTarget, msg.m_strData);
		break;
	//---------------------------------------------------------------------------------------------
	}
}

bool Poll::PollSend(const PollData &pollData)
{
	// Preparing messages.
	PollCtcpMessage begin;
	begin.SetPollMessageCode(POLL_MESSAGE_CODE_BEGIN);
	VBString beginMessage = begin.GetPollCtcpMessage();

	PollCtcpMessage question;
	question.SetPollMessageCode(POLL_MESSAGE_CODE_QUESTION);
	question.SetText(pollData.question);
	VBString questionMessage = question.GetPollCtcpMessage();

	list<VBString> optionMessageList;

	list<PollAnswer>::const_iterator it;

	for(it = pollData.answerList.begin(); it != pollData.answerList.end(); it++)
	{
		PollCtcpMessage option;
		option.SetPollMessageCode(POLL_MESSAGE_CODE_OPTION);
		option.SetOptionIndex(it->index);
		option.SetText(it->text);
		VBString optionMessage = option.GetPollCtcpMessage();
		optionMessageList.push_back(optionMessage);
	}
	
	PollCtcpMessage end;
	end.SetPollMessageCode(POLL_MESSAGE_CODE_END);
	VBString endMessage = end.GetPollCtcpMessage();

	// Validating messages.
	if(beginMessage.IsEmpty())
		return false;

	if(questionMessage.IsEmpty())
		return false;

	list<VBString>::iterator it2;
	for(it2 = optionMessageList.begin(); it2 != optionMessageList.end(); it2++)
		if((*it2).IsEmpty())
			return false;

	if(endMessage.IsEmpty())
		return false;

	// Sending messages.
	SendChannelPrivateMessage(pollData.channel, beginMessage);
	SendChannelPrivateMessage(pollData.channel, questionMessage);
	for(it2 = optionMessageList.begin(); it2 != optionMessageList.end(); it2++)
		SendChannelPrivateMessage(pollData.channel, *it2);
	SendChannelPrivateMessage(pollData.channel, endMessage);

	return true;
}

bool Poll::PollSendAnswer(VBString recipient, int optionIndex)
{
	PollCtcpMessage msg;
	msg.SetPollMessageCode(POLL_MESSAGE_CODE_ANSWER);
	msg.SetOptionIndex(optionIndex);

	VBString message = msg.GetPollCtcpMessage();

	if(!message.IsEmpty())
	{
		SendPrivateMessage(recipient, message);
		return true;
	}

	return false;
}

bool Poll::PollSendStats(const PollData &pollData)
{
	// Preparing messages.
	PollCtcpMessage begin;
	begin.SetPollMessageCode(POLL_MESSAGE_CODE_STATS_BEGIN);
	VBString beginMessage = begin.GetPollCtcpMessage();

	PollCtcpMessage question;
	question.SetPollMessageCode(POLL_MESSAGE_CODE_STATS_QUESTION);
	question.SetText(pollData.question);
	VBString questionMessage = question.GetPollCtcpMessage();

	list<VBString> optionMessageList;

	list<PollAnswer>::const_iterator it;

	for(it = pollData.answerList.begin(); it != pollData.answerList.end(); it++)
	{
		PollCtcpMessage option;
		option.SetPollMessageCode(POLL_MESSAGE_CODE_STATS_OPTION);
		option.SetOptionIndex(it->index);
		option.SetAnswerCount(it->answerCount);
		option.SetText(it->text);
		VBString optionMessage = option.GetPollCtcpMessage();
		optionMessageList.push_back(optionMessage);
	}
	
	PollCtcpMessage end;
	end.SetPollMessageCode(POLL_MESSAGE_CODE_STATS_END);
	VBString endMessage = end.GetPollCtcpMessage();

	// Validating messages.
	if(beginMessage.IsEmpty())
		return false;

	if(questionMessage.IsEmpty())
		return false;

	list<VBString>::iterator it2;
	for(it2 = optionMessageList.begin(); it2 != optionMessageList.end(); it2++)
		if((*it2).IsEmpty())
			return false;

	if(endMessage.IsEmpty())
		return false;

	// Sending messages.
	SendChannelPrivateMessage(pollData.channel, beginMessage);
	SendChannelPrivateMessage(pollData.channel, questionMessage);
	for(it2 = optionMessageList.begin(); it2 != optionMessageList.end(); it2++)
		SendChannelPrivateMessage(pollData.channel, *it2);
	SendChannelPrivateMessage(pollData.channel, endMessage);

	return true;
}

void ShowPollData(PollCtcpMessage &msg)
{
	if(msg.IsPollBegin())
		cout << "Poll Begin" << endl;

	if(msg.IsPollQuestion())
		cout << "Poll Question|text=" << msg.GetText()<< endl;

	if(msg.IsPollOption())
		cout << "Poll Option|index=" << msg.GetOptionIndex() << "|text=" << msg.GetText() <<endl;

	if(msg.IsPollEnd())
		cout << "Poll End" << endl;

	if(msg.IsPollAnswer())
		cout << "Poll Answer|index=" << msg.GetOptionIndex() << endl;

	if(msg.IsPollStatsBegin())
		cout << "Poll Stats Begin" << endl;

	if(msg.IsPollStatsQuestion())
		cout << "Poll Stats Question|text=" << msg.GetText()<< endl;

	if(msg.IsPollStatsOption())
		cout << "Poll Stats Option|index=" << msg.GetOptionIndex() << "|count=" << msg.GetAnswerCount() << "|text=" << msg.GetText() <<endl;

	if(msg.IsPollStatsEnd())
		cout << "Poll Stats End" << endl;
}

void Poll::OnUserPrivateMessage(VBString sender, VBString message)
{
	PollCtcpMessage msg(message);

	if(msg.IsPollCtcpMessage())
	{
		//ShowPollData(msg);

		if(msg.IsPollAnswer())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_ANSWER;
			p.m_index = msg.GetOptionIndex();
			p.m_user = sender;
			Notify(p);	
		}
	}
}

void Poll::OnChannelPrivateMessage(VBString sender, VBString channel, VBString message)
{
	PollCtcpMessage msg(message);

	if(msg.IsPollCtcpMessage())
	{
		//ShowPollData(msg);

		if(msg.IsPollBegin())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_BEGIN;
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollQuestion())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_QUESTION;
			p.m_text = msg.GetText();
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollOption())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_OPTION;
			p.m_index = msg.GetOptionIndex();
			p.m_text = msg.GetText();
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollEnd())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_END;
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		if(msg.IsPollStatsBegin())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_STATS_BEGIN;
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollStatsQuestion())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_STATS_QUESTION;
			p.m_text = msg.GetText();
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollStatsOption())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_STATS_OPTION;
			p.m_index = msg.GetOptionIndex();
			p.m_answerCount = msg.GetAnswerCount();
			p.m_text = msg.GetText();
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
		else if(msg.IsPollStatsEnd())
		{
			PollParameter p;
			p.m_code = IMNC_MODULE_SPECIFIC;
			p.m_moduleId = IPTV_MODULE_ID_POLL;
			p.m_pollCode = POLLNC_STATS_END;
			p.m_user = sender;
			p.m_channel = channel;
			Notify(p);	
		}
	}
}
