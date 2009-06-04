#ifndef VOICE_REQUEST_CTCP_MESSAGE_H
#define VOICE_REQUEST_CTCP_MESSAGE_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/CtcpMessage.h"

class VoiceRequestCtcpMessage : public CtcpMessage
{
public:
	VoiceRequestCtcpMessage(br::com::sbVB::VBLib::VBString message);

	bool IsVoiceRequestCtcpMessage() const;
	bool IsRequest() const;
	bool IsCancel() const;
	bool IsRemoveAll() const;
	
private:
	void ParseParameters();

	bool m_isVoiceRequestCtcp;
	bool m_isRequest;
	bool m_isCancel;
	bool m_isRemoveAll;

	static const br::com::sbVB::VBLib::VBString m_ctcpVoiceRequest;
	static const br::com::sbVB::VBLib::VBString m_ctcpVoiceRequestCancel;
	static const br::com::sbVB::VBLib::VBString m_ctcpVoiceRequestRemoveAll;
};

#endif
