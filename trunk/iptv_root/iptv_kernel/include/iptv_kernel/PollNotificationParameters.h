#ifndef POLL_NOTIFICATION_PARAMETERS_H
#define POLL_NOTIFICATION_PARAMETERS_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModuleNotificationSink.h"

enum PollNotificationCode	{
								POLLNC_INVALID,
								POLLNC_BEGIN,
								POLLNC_QUESTION,
								POLLNC_OPTION,
								POLLNC_END,
								POLLNC_ANSWER,
								POLLNC_STATS_BEGIN,
								POLLNC_STATS_QUESTION,
								POLLNC_STATS_OPTION,
								POLLNC_STATS_END
							};

struct PollNotificationParameter : public IPTVModuleNotificationParameter
{
	PollNotificationCode m_pollCode;
};

struct PollParameter : public PollNotificationParameter
{
	int m_index;
	int m_answerCount;
	br::com::sbVB::VBLib::VBString m_text;
	br::com::sbVB::VBLib::VBString m_user;
	br::com::sbVB::VBLib::VBString m_channel;
};

#endif
