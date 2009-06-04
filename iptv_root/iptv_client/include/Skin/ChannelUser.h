#ifndef CHANNEL_USER_H
#define CHANNEL_USER_H

#include "User.h"

/** @brief Specialization of User for CONFERENCE mode.
 *
 */
class ChannelUser: public User
{
private:
	bool m_isOperator;
	bool m_isCollaborator;
	bool m_hasVoice;
	bool m_isRequestingVoice;
	bool m_isInSeriesCollab;
	int m_collabIndex;
	int m_voiceReqIndex;

	static int sm_collab;
	static int sm_voiceReq;

public:
	ChannelUser();
	ChannelUser(const User &user);
	virtual ~ChannelUser();

	void SetChannelOperator(bool op);
	void SetChannelCollaborator(bool collab);
	void SetMediaVoice(bool hasVoice);
	void SetVoiceRequest(bool request);
	void SetSeriesCollaboration(bool value);

	bool IsChannelOperator() const;
	bool IsChannelCollaborator() const;
	bool HasMediaVoice() const;
	bool IsRequestingVoice() const;
	bool IsInSeriesCollaboration() const;

	int GetCollabIndex() const;
	int GetVoiceReqIndex() const;

	static void ResetOrderingCounters();

	bool operator < (const ChannelUser &rhs);
};

#endif
