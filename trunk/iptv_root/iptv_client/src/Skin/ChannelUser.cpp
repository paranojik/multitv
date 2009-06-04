#include "ChannelUser.h"

// These are for ordering collaborators and voice requesters
// according to the specified criteria (see operator <).
int ChannelUser::sm_collab = 0;
int ChannelUser::sm_voiceReq = 0;

/** Default constructor.
 */
ChannelUser::ChannelUser(): User()
{
	m_isOperator = false;
	m_isCollaborator = false;
	m_hasVoice = false;
	m_isRequestingVoice = false;
	m_collabIndex = -1;
	m_voiceReqIndex = -1;
}

/** Constructor from a base User.
 */
ChannelUser::ChannelUser(const User &user)
{
	m_isOperator = false;
	m_isCollaborator = false;
	m_hasVoice = false;
	m_isRequestingVoice = false;
	m_isInSeriesCollab = false;
	m_collabIndex = -1;
	m_voiceReqIndex = -1;

	m_nickName = user.GetNickName();
	m_modeList = user.GetModeList();
}

/** Destructor.
 */
ChannelUser::~ChannelUser()
{
}

/** Static method to reset the collaboration/voice request counters
 */
void ChannelUser::ResetOrderingCounters()
{
	sm_collab = 0;
	sm_voiceReq = 0;
}

/** Set the "operator" flag.
 * @param[in] op	Value to set.
 */
void ChannelUser::SetChannelOperator(bool op)
{
	m_isOperator = op;
}

/** Set "collaboration" flag.
 * @param[in] collab	Value to set.
 */
void ChannelUser::SetChannelCollaborator(bool collab)
{
	m_isCollaborator = collab;
	if(collab)
	{
		m_collabIndex = sm_collab;
		sm_collab++;
	}
	else
		m_collabIndex = -1;
}

/** Set "media voice" flag.
 * @param[in] hasVoice	Value to set.
 */
void ChannelUser::SetMediaVoice(bool hasVoice)
{
	m_hasVoice = hasVoice;
}

/** Set "media voice" flag.
 * @param[in] hasVoice	Value to set.
 */
void ChannelUser::SetVoiceRequest(bool request)
{
	m_isRequestingVoice = request;
	if(request)
	{
		m_voiceReqIndex = sm_voiceReq;
		sm_voiceReq++;
	}
	else
		m_voiceReqIndex = -1;
}

/** Set "series collaboration" flag.
 * @param[in] value	Value to set.
 */
void ChannelUser::SetSeriesCollaboration(bool value)
{
	m_isInSeriesCollab = value;
}

/** Query "operator" flag.
 * @return Value of the flag.
 */
bool ChannelUser::IsChannelOperator() const
{
	return m_isOperator;
}

/** Query "collaboration" flag.
 * @return Value of the flag.
 */
bool ChannelUser::IsChannelCollaborator() const
{
	return m_isCollaborator;
}

/** Query "media voice" flag.
 * @return Value of the flag.
 */
bool ChannelUser::HasMediaVoice() const
{
	return m_hasVoice;
}

/** Query "voice request" flag.
 * @return Value of the flag.
 */
bool ChannelUser::IsRequestingVoice() const
{
	return m_isRequestingVoice;
}

/** Query "series collaboration" flag.
 * @return Value of the flag.
 */
bool ChannelUser::IsInSeriesCollaboration() const
{
	return m_isInSeriesCollab;
}

/** Get collaboration order index.
 * @return The index.
 */
int ChannelUser::GetCollabIndex() const
{
	return m_collabIndex;
}

/** Get voice request order index.
 * @return The index.
 */
int ChannelUser::GetVoiceReqIndex() const
{
	return m_voiceReqIndex;
}

/** Comparison operator used for ordering users (in the list).
 *
 * This function is responsible for the ordering of the users list.
 * Ordering proceeds as follows:@n
 * 1. Users with media voice precede those who don't.@n
 * 2. Users with collaboration precede those who don't have it. Collaborators are sorted in request order.@n
 * 3. Users who are channel operators precede those who aren't. Operators are sorted in alphabetical order.@n
 * 4. Users who are requesting voice precede those who aren't. Requesters are sorted in request order.@n
 * 5. Last come the other users, by alphabetical order.@n
 *
 * @param[in] rhs	Right-hand side of the operator.
 * @return true if the left-hand side precedes the right-hand side, false otherwise.
 */
bool ChannelUser::operator <(const ChannelUser &rhs)
{
	// Here is where the user list ordering criteria are defined.
	
	// 1. Users with media-voice first
	if(HasMediaVoice())
	{
		if(rhs.HasMediaVoice())
			return ((User *)this)->operator <(rhs);
		return true;
	}
	else	// if the right-hand-side user has voice and this one doesn't, she comes first
		if(rhs.HasMediaVoice())
			return false;
	
	// 2. Then come the collaborators, in collaboration request order.
	if(IsChannelCollaborator())
	{
		if(!rhs.IsChannelCollaborator())	// Collaborators come before non-collaborators
			return true;
		else	// If both are collaborators, then sort by collaboration index (lowest first)
			return (GetCollabIndex() < rhs.GetCollabIndex());
	}
	else
	{
		if(rhs.IsChannelCollaborator())		// If I'm not a collaborator and the user in right-hand-side is, let her pass
			return false;
	}
	
	// 3. Then come the channel operators, in alphabetical order
	if(IsChannelOperator())
	{
		if(rhs.IsChannelOperator())
		{
			return ((User *)this)->operator <(rhs);		// If both are operators, order alphabetically
		}
		else
		{
			return true;		// Operators come before non-operators
		}
	}
	else
	{
		if(rhs.IsChannelOperator())		// As above, operators come before non-operators
			return false;
	}

	// 4. Then, the voice-requesting users, in order of request.
	if(IsRequestingVoice())
	{
		if(!rhs.IsRequestingVoice())	// Requesters come before non-requesters
			return true;
		else
			return(GetVoiceReqIndex() < rhs.GetVoiceReqIndex());	// Else, sort by request order.
	}
	else
	{
		if(rhs.IsRequestingVoice())		// As above, requesters come before non-requesters
			return false;
	}

	// 5. Then others, sorted alphabetically
	return ((User *)this)->operator <(rhs);
}
