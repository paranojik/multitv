#ifndef POLL_H
#define POLL_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVModule.h"
#include "iptv_kernel/PollCtcpMessage.h"
#include <list>

struct PollAnswer
{
	br::com::sbVB::VBLib::VBString text;
	int index;
	unsigned answerCount;
	bool operator < (const PollAnswer &that){ return ((*this).index < that.index); }
};

struct PollData
{
	br::com::sbVB::VBLib::VBString pollOwner;
	br::com::sbVB::VBLib::VBString channel;
	br::com::sbVB::VBLib::VBString question;
	std::list<PollAnswer> answerList;
};

/** @brief Implements the Poll module.
 *
 * This class responsibilities are handling Poll IRM messages and implementing
 * the Poll rules.
 *
 */
class Poll : public IPTVModule
{
public:
	Poll();
	void OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn);

	bool PollSend(const PollData &pollData);
	bool PollSendAnswer(br::com::sbVB::VBLib::VBString recipient, int optionIndex);
	bool PollSendStats(const PollData &pollData);

private:
	// IRM messages handling
	void OnUserPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
								br::com::sbVB::VBLib::VBString message);

	void OnChannelPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);
};

#endif
