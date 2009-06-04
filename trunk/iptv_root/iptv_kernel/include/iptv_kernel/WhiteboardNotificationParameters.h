#ifndef WHITEBOARD_NOTIFICATION_PARAMETERS_H
#define WHITEBOARD_NOTIFICATION_PARAMETERS_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModuleNotificationSink.h"

enum WhiteboardNotificationCode	{
									WBNC_INVALID,
									WBNC_MESSAGE
								};

struct WhiteboardNotificationParameter : public IPTVModuleNotificationParameter
{
	WhiteboardNotificationCode m_whiteboardCode;
};

struct WhiteboardMessage : public WhiteboardNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_sender;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_data;
};

#endif
