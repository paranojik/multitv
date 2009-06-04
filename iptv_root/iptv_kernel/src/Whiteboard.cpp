#include "iptv_kernel/Whiteboard.h"
#include "iptv_kernel/WhiteboardNotificationParameters.h"
#include "chat/nOpCodes.h"

#include "iptv_kernel/LogManager.h"
#include "iptv_kernel/Base64.h"

using namespace br::com::sbVB::VBLib;

Whiteboard::Whiteboard()
{

}

void Whiteboard::OnIrmMessage(IrmMessage &msg, IrmUser &user, IrmConnection &conn)
{
	switch (msg.m_irmMessageCode)
	{
	//---------------------------------------------------------------------------------------------
	// White board events
	//---------------------------------------------------------------------------------------------
	case WBOARD_CHANNEL:
		OnChannelWhiteboard(msg.m_strSource, msg.m_strTarget, msg.m_strData);
		break;
	//---------------------------------------------------------------------------------------------
	}
}

void Whiteboard::OnChannelWhiteboard(VBString sender, VBString channel, VBString data)
{
	// Notifying the kernel.
	WhiteboardMessage param;
	param.m_moduleId = IPTV_MODULE_ID_WHITEBOARD;
	param.m_code = IMNC_MODULE_SPECIFIC;
	param.m_whiteboardCode = WBNC_MESSAGE;
	param.m_sender = sender;
	param.m_channel = channel;
	param.m_data = data;
	Notify(param);
}

void Whiteboard::SendChannelWhiteboard(VBString channel, VBString data)
{
	RequestChannelWhiteboard(channel, data);
}

void WhiteboardDebug(VBString sender, VBString channel, VBString data)
{
	LogManager::Instance()->AddLogHeader("Whiteboard Begin");
	VBString message = "sender:";
	message += sender;
	message += "\n";
	message += "channel:";
	message += channel;
	message += "\n";
	message += "encoded data:";
	message += data;
	message += "\n";
	message += "***BINARY DATA BEGIN***";
	Base64 base64;
	char *decodedData;
	int size = base64.Decode64(data, &decodedData);
	LogManager::Instance()->AddLogEntry(message);
	LogManager::Instance()->WriteLogBinary(decodedData, size);
	message = "\n***BINARY DATA END***\n";
	LogManager::Instance()->AddLogEntry(message);
	LogManager::Instance()->AddLogHeader("Whiteboard End");
}
