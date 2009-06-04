//==========================================================
//	IRM Shell Client
//	Written by the MultiTV team
//==========================================================

//==========================================================
//	Headers
//==========================================================
#include "iptv_kernel/IPTVKernel.h"
#include "iptv_kernel/LogManager.h"
#include "VBLib/VBLib.h"

// Old stuff. Now it's part of iptv_video_capture.
// #include "iptv_kernel/VideoDeviceEnumerator.h"

#include <iostream>
#include <deque>

//==========================================================
//	Defines
//==========================================================
#define BUFFER_SIZE	1024

//==========================================================
//	Namespaces
//==========================================================
using namespace std;
using namespace br::com::sbVB::VBLib;

//==========================================================
//	Global declarations
//==========================================================

// The kernel does not need to be a global variable.
// I did it here to simplify the interfaces of the shell client's global functions,
// since I'm using only one kernel.
IPTVKernel kernel;
VBString currentChannel;
PollData *pollData;
bool silentShell = false;

const VBString KernelMessages[] =
{
	"Kernel: Read packet",
	"Kernel: New conference transmission",
	"IRM: Connected to host",				    
	"IRM: Connected to chat",				    
	"IRM: Disconnected from chat",		
	"IRM: Authenticated",				            
	"IRM: Authentication failure",                              
	"IRM: User quit",	                                    
	"IRM: Channel join",			                    
	"IRM: Channel part",			                    
	"IRM: New channel query item",	                            
	"IRM: End of the channel query",
	"IRM: Channel password invalid",
	"IRM: New user query item",	                            
	"IRM: End of the user query",                               
	"IRM: Private message",                                     
	"IRM: Channel private message",
	"IRM: Channel voice",
	"IRM: Voice request",
	"IRM: Voice request cancel",
	"IRM: Voice request remove all",
	"IRM: Channel mode operator",
	"IRM: Channel mode private",
	"IRM: Channel mode secret",
	"IRM: Channel mode invite only",
	"IRM: Channel mode only operator can change topic",
	"IRM: Channel mode no external messages",
	"IRM: Channel mode moderated",
	"IRM: Channel mode user limit",
	"IRM: Channel mode ban",
	"IRM: Channel mode media collaborator",
	"IRM: Channel mode key",
	"IRM: Channel mode bit rate",
	"IRM: Channel mode auto change bit rate",
	"IRM: Channel mode multitple transmission",
	"IRM: Channel mode one audio",
	"IRM: Channel mode audio mute",
	"IRM: Channel mode band share limit",
	"IRM: Channel mode transmission limit",
	"IRM: User mode invisible",
	"IRM: User mode registered",
	"IRM: User mode camera on",
	"IRM: User mode microphone mute",
	"IRM: User mode away",
	"Media Transceiver data read by Media Processor",
	"Media Transceiver data read by the Application",
	"Media Transceiver: new transmission",
	"Media Transceiver: notify progress",
	"Media Processor: read packet",
	"Media Processor: write packet",
	"Media Processor: video alive",
	"Media Processor: audio alive",
	"Media Processor: buffering start",
	"Media Processor: buffering stop",
	"Media Capture: video data",
	"File Transfer: begin receive",
	"File Transfer: receive progress",
	"File Transfer: lost packet",
	"File Transfer: receive success",
	"File Transfer: receive error",
	"File Transfer: send begin",
	"File Transfer: send progress",
	"File Transfer: send lost packet",
	"File Transfer: send eof",
	"File Transfer: send success",
	"File Transfer: send error",
	"File Transfer: send new receiver",
	"File Transfer: send receiver transmission canceled",
	"File Transfer: send receiver transmission end",
	"Poll: Begin",
	"Poll: Question",
	"Poll: Option",
	"Poll: End",
	"Poll: Answer",
	"Poll: Stats Begin",
	"Poll: Stats Question",
	"Poll: Stats Option",
	"Poll: Stats End",
	"Whiteboard: Message",
	"IRM error: IRMStub initialization",
	"IRM error: attempt to connect in invalid connection state",
	"IRM error: connection query unsuccessful",
	"IRM error: connection parameters blank"
};

void ShowIRMState()
{
	// Used only for debug purposes.

	//cout << "***IRM state***" << endl;
	//cout << "UDP listening = " << kernel.GetUdpListeningState() << endl;
	//cout << "connection = " << kernel.GetConnectionState() << endl;
	//cout << "authentication = " << kernel.GetAuthenticationState() << endl;
	//cout << "channel join = " << kernel.GetChannelJoinState() << endl;
	//cout << "channel part = " << kernel.GetChannelPartState() << endl;
	//cout << "query channel= " << kernel.GetChannelQueryState() << endl;
	//cout << "query user = " << kernel.GetUserQueryState() << endl;
}

void ShowKernelMessage(KernelParam& param)
{
	cout << "Kernel message: " << KernelMessages[param.m_code];
	cout << ". " << param.m_kernelMessage << endl;
}

void ShowChannelInfo(KernelParam& param)
{
	cout << "Channel: " << (*(IrmQueryChannelParam *)&param).m_channelName;
	cout << " * Users: " << (*(IrmQueryChannelParam *)&param).m_userCount;
	cout << " * Topic: " << (*(IrmQueryChannelParam *)&param).m_channelTopic;
	cout << endl;
}

void ShowUserInfo(KernelParam& param)
{
	cout << "User: " << (*(IrmQueryUserParam *)&param).m_userName;
	cout << " * Channel: " << (*(IrmQueryUserParam *)&param).m_channelName;
	cout << endl;
}

void ShowPrivMessage(KernelParam& param)
{
	cout << "Message from " << (*(KernelMessageParam *)&param).m_sender;
	if((*(KernelMessageParam *)&param).m_channelName.length() != 0)
		cout << " to channel " << (*(KernelMessageParam *)&param).m_channelName;
	cout << ": " << (*(KernelMessageParam *)&param).m_message;
	cout << endl;
}

void ShowMode(KernelParam& param)
{
	KernelModeParam* modeParam;
	modeParam = (KernelModeParam*) &param;
	char value;
	(modeParam->m_value)?(value = '+'):(value = '-');

	cout	<< "* " << modeParam->m_target <<
			" sets mode: " << value << modeParam->m_name;

	if(modeParam->m_parameter != "")
		cout << " " << modeParam->m_parameter;

	cout << endl;
}

void ShowMediaIdInfo(KernelParam& param)
{
	cout << "Media ID: " << (*(KernelMediaIDParam *)&param).m_mediaID;
	cout << " * User: " << (*(KernelMediaIDParam *)&param).m_userNickName;
	cout << " * Channel: " << (*(KernelMediaIDParam *)&param).m_channelName;
	cout << endl;
}

void SendCtcpMessage(VBString recipient, VBString ctcpCode, VBString parameter)
{
	VBString msg;
	msg = '\01';
	msg += ctcpCode;
	msg += '\01';
	msg += parameter;

	kernel.SendPrivateMessage(recipient, msg);
}

//==========================================================
//	Kernel notification class declaration and definition
//==========================================================
class ShellKernelNotify : public KernelNotify
{
	bool OnKernelNotify(KernelParam& param);
};

/** This method receives the notifications from the kernel.
*	The type of notification is defined by KernelParam::m_code.
*	The actions to each notification should be placed in the
*	scope of it's corresponding case in the switch statement.
*/
bool ShellKernelNotify::OnKernelNotify(KernelParam& param)
{
	//if((param.m_code >= 21) && (param.m_code <= 26))
	//	return true;

	if(!silentShell)
		ShowKernelMessage(param);

	switch (param.m_code)
	{
		case KNC_NEW_CONFERENCE_TRANSMISSION:
			{
				ShowMediaIdInfo(param);
			}
			break;
		case KNC_IRM_CONNECTED_HOST:
			{
				
			}
			break;		                        
		case KNC_IRM_CONNECTED_CHAT:
			{

			}
			break;		                        
		case KNC_IRM_DISCONNECTED:
			{

			}
			break;		                        
		case KNC_IRM_AUTHENTICATED:
			{

			}
			break;		     
		case KNC_IRM_AUTHENTICATION_ERROR:
			{

			}
			break;
		case KNC_IRM_USER_QUIT:
			{
				ShowUserInfo(param);
			}
			break;	
		case KNC_IRM_CHANNEL_JOIN:
			{
				ShowUserInfo(param);
				currentChannel = (*(IrmQueryUserParam *)&param).m_channelName;
				// kernel.InitializeConference((*(IrmQueryUserParam *)&param).m_channelName);
			}
			break;		                        
		case KNC_IRM_CHANNEL_PART:
			{
				ShowUserInfo(param);
				kernel.TerminateAllMediaTransmissions();
			}
			break;
		case KNC_IRM_CHANNEL_QUERY_ITEM:
			{
				ShowChannelInfo(param);
			}
			break;		                        
		case KNC_IRM_CHANNEL_QUERY_END:
			{

			}
			break;
		case KNC_IRM_CHANNEL_PASSWD_INVALID:
			{

			}
			break;	
		case KNC_IRM_USER_QUERY_ITEM:
			{
				ShowUserInfo(param);
			}
			break;		                        
		case KNC_IRM_USER_QUERY_END:
			{
				ShowUserInfo(param);
			}
			break;

		case KNC_IRM_PRIV_MSG:
			{
				ShowPrivMessage(param);
			}
			break;
		case KNC_IRM_CHANNEL_PRIV_MSG:
			{
				ShowPrivMessage(param);
			}
			break;
		case KNC_IRM_CHANNEL_VOICE:
		case KNC_IRM_VOICE_REQUEST:
		case KNC_IRM_VOICE_REQUEST_CANCEL:
		case KNC_IRM_VOICE_REQUEST_REMOVE_ALL:
			{
				ShowUserInfo(param);
			}
			break;
		case KNC_IRM_CHANNEL_MODE_OPERATOR:
		case KNC_IRM_CHANNEL_MODE_PRIVATE:
		case KNC_IRM_CHANNEL_MODE_SECRET:
		case KNC_IRM_CHANNEL_MODE_INVITE_ONLY:
		case KNC_IRM_CHANNEL_MODE_OP_CHANGE_TOPIC:
		case KNC_IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES:
		case KNC_IRM_CHANNEL_MODE_MODERATED:
		case KNC_IRM_CHANNEL_MODE_USER_LIMIT:
		case KNC_IRM_CHANNEL_MODE_BAN:
		case KNC_IRM_CHANNEL_MODE_MEDIA_COLLABORATOR:
		case KNC_IRM_CHANNEL_MODE_KEY:
		case KNC_IRM_CHANNEL_MODE_BIT_RATE:
		case KNC_IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE:
		case KNC_IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION:
		case KNC_IRM_CHANNEL_MODE_ONE_AUDIO:
		case KNC_IRM_CHANNEL_MODE_AUDIO_MUTE:
		case KNC_IRM_CHANNEL_MODE_BAND_SHARE_LIMIT:
		case KNC_IRM_CHANNEL_MODE_TRANSMISSION_LIMIT:
		case KNC_IRM_USER_MODE_INVISIBLE:
		case KNC_IRM_USER_MODE_REGISTERED:
		case KNC_IRM_USER_MODE_CAMERA_ON:
		case KNC_IRM_USER_MODE_MIC_MUTE:
		case KNC_IRM_USER_MODE_AWAY:
			ShowMode(param);
			break;
		case KNC_TRCV_NEW_TRANSMISSION:
			{
				cout << "Media ID: " << (*(KernelMediaIDParam *)&param).m_mediaID << endl;
			}
			break;
		case KNC_TRCV_DATA_READ_APP:
			{
				(*(KernelMediaDataParam *)&param).m_dataLength;
				cout << "Data address: " << (void *)(*(KernelMediaDataParam *)&param).m_data << " - " << endl;
				cout << "Data length: " << (*(KernelMediaDataParam *)&param).m_dataLength << endl;
			}
			break;
		case KNC_TRCV_NOTIFY_PROGRESS:
			{
				printf("Playback progress = %0.02f%%\n", (*(KernelMediaProgressParam *)&param).m_percent);
			}
			break;
		case KNC_FT_RECEIVE_BEGIN:
			{
				KernelFTReceiveBeginParam *p = (KernelFTReceiveBeginParam *) &param;
				cout << "Media ID = " << p->m_mediaId << endl;
				cout << "File name = " << p->m_fileName << endl;
				cout << "Temp path = " << p->m_tempFilePath << endl;
				cout << "Sender = " << p->m_senderNick << endl;
				cout << "File size = " << p->m_fileSize << endl;
				cout << "Packet size = " << p->m_packetSize << endl;

			}
			break;
		case KNC_FT_RECEIVE_PROGRESS:
			{
				KernelFTProgressParam *p = (KernelFTProgressParam *) &param;
				cout << "Packet " << p->m_currentPacketIndex << "/" << p->m_lastPacketIndex << endl;
			}
			break;
		case KNC_FT_RECEIVE_LOST_PACKET:
			{
				KernelFTLostPacketParam *p = (KernelFTLostPacketParam *) &param;
				cout << "Number of packets to recover = " << p->m_lostPackets << endl;
			}
			break;
		case KNC_FT_RECEIVE_SUCCESS:
			{
			}
			break;
		case KNC_FT_RECEIVE_ERROR:
			{
			}
			break;
		case KNC_FT_SEND_BEGIN:
			{
				KernelMediaIDParam *p = (KernelMediaIDParam *) &param;
				cout << "FT Media ID = " << p->m_mediaID << endl;
			}
			break;
		case KNC_POLL_BEGIN:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_QUESTION:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "Question: " << p->m_text << endl;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_OPTION:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "Option index: " << p->m_index << endl;
				cout << "Option text: " << p->m_text << endl;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_END:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_ANSWER:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout	<< "The user " << p->m_userName
						<<" has chosen option " << p->m_index << endl;

				if(pollData != NULL)
				{
					list<PollAnswer>::iterator it;
					for(it = pollData->answerList.begin(); it != pollData->answerList.end(); it++)
						if(it->index == p->m_index)
							it->answerCount++;
				}
			}
			break;
		case KNC_POLL_STATS_BEGIN:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_STATS_QUESTION:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "Question: " << p->m_text << endl;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_STATS_OPTION:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "Option index: " << p->m_index << endl;
				cout << "Option count: " << p->m_answerCount << endl;
				cout << "Option text: " << p->m_text << endl;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
		case KNC_POLL_STATS_END:
			{
				KernelPollParam *p = (KernelPollParam *) &param;
				cout << "User: " << p->m_userName << endl;
				cout << "Channel: " << p->m_channelName << endl;
			}
			break;
	}
	
	ShowIRMState();
	return true;
}

//==========================================================
//	Main function
//==========================================================
int main(int argc, char **argv)
{
//==========================================================
//	Welcome messages
//==========================================================
	cout << "\n*** IRM Shell Client ***\n\n\n";
	cout << "type 'h' for available commands\n\n";
	LogManager::Instance()->AddLogHeader("IRM Shell Client");
//==========================================================

//==========================================================
//	Kernel notify declaration and initialization
//==========================================================
	ShellKernelNotify notify;
	kernel.SetNotify(&notify);
	pollData = NULL;

//==========================================================
//	Local variables declaration
//==========================================================
	char cmd[BUFFER_SIZE];
	char server[BUFFER_SIZE];
	unsigned port;
	char user[BUFFER_SIZE];
	char passwd[BUFFER_SIZE];
	char realName[BUFFER_SIZE];
	char email[BUFFER_SIZE];
	char channel[BUFFER_SIZE];
	char recipient[BUFFER_SIZE];
	char mode[BUFFER_SIZE];
	char target[BUFFER_SIZE];
	char parameter[BUFFER_SIZE];
	char filename[BUFFER_SIZE];
	char message[BUFFER_SIZE];
	char mediaId[BUFFER_SIZE];
	char percent[BUFFER_SIZE];
	char topic[BUFFER_SIZE];
	char ctcpCode[BUFFER_SIZE];

	char input = '\0';

	//cout << "> ";
	//cin.getline(cmd, BUFFER_SIZE);

	VBString command;

//==========================================================
//	Main loop
//==========================================================
	while(command != "/q")
	{
		// cout << "> "; // input mark
		//cin >> input;

		//// Clear input buffer.
		//char commandDummy;
		//cin.getline(&commandDummy, 1);

		cout << "> ";
		cin.getline(cmd, BUFFER_SIZE);
		command = cmd;

		cout << endl;

		ShowIRMState();


//==========================================================
//	Help
//==========================================================
		if(command == "h")
		{
			cout << endl << "*** Command list ***" << endl;
			cout << "h - help" << endl;
			cout << "c - connect to IRM server" << endl;
			cout << "d - disconnect from IRM server" << endl;
			cout << "v - initialize multicast viewer" << endl;
			cout << "j - join channel" << endl;
			cout << "p - part channel" << endl;
			cout << "l - list channels" << endl;
			cout << "u - list users in channel " << endl;
			cout << "m - send a private message" << endl;
			cout << "u - set media voice" << endl;
			cout << "t - set topic" << endl;
			cout << "x - set channel mode" << endl;
			cout << "z - set user mode" << endl;
			cout << "b - invite user" << endl;
			cout << "r - read media packet" << endl;
			cout << "f - file playback" << endl;
			cout << "s - seek in file playback" << endl;
			cout << "i - play in file playback" << endl;
			cout << "o - pause in file playback" << endl;
			cout << "/vdev - list video devices" << endl;
			cout << "w - initialize media capture" << endl;
			cout << "g - get media capture frame" << endl;
			cout << "e - release media capture" << endl;
			cout << "q - quit" << endl;
			cout << endl;
		}
//==========================================================
//	Connect to IRM server
//==========================================================
		if(command == "c")
		{
			LogManager::Instance()->AddLogEntry("Connecting...");

			// Clear input buffer.
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Host: ";
			cin.getline(server, BUFFER_SIZE);
			//cin.getline(&dummy, 1);
			//cout << "Port: ";
			//cin >> port;
			//cin.getline(&dummy, 1);
			cout << "Username: ";
			cin.getline(user, BUFFER_SIZE);
			cout << "Password: ";
			cin.getline(passwd, BUFFER_SIZE);

			// strncpy(server, "iptv.poli.ufrj.br", BUFFER_SIZE);
			port = 8000;
			strncpy(realName, "myNick", BUFFER_SIZE);
			strncpy(email, "myNick@myHost.com", BUFFER_SIZE);

			kernel.RequestConnection(server, port, user, passwd, realName, email,"");

			cout << endl;

			LogManager::Instance()->AddLogEntry("Connection done.");
		}
//==========================================================
//	Disconnect from IRM server
//==========================================================
		if(command == "d")
		{
			LogManager::Instance()->AddLogEntry("Disconnecting...");

			kernel.TerminateAllMediaTransmissions();
			cout << "Disconnecting from IRM server...";
			kernel.Disconnect();

			LogManager::Instance()->AddLogEntry("Disconnection done.");
		}
//==========================================================
//	Initialize multicast viewer
//==========================================================
		if(command == "v")
		{
			cout << "*** Initialize multicast viewer ***" << endl;
			// Clear input buffer.
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Host: ";
			cin.getline(server, BUFFER_SIZE);
			cout << "Port: ";
			cin >> port;

			if(!kernel.InitializeMulticast(server, port))
				cout << "Multicast initialization error" << endl;
		}
//==========================================================
//	Join channel
//==========================================================
		if(command == "j")
		{
			char dummy;
			cin.getline(&dummy, 1);
			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);
			cout << "Password: ";
			cin.getline(passwd, BUFFER_SIZE);
			kernel.RequestChannelJoin(channel, passwd);
		}
//==========================================================
//	Part channel
//==========================================================
		if(command == "p")
		{
			char dummy;
			cin.getline(&dummy, 1);
			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);
			kernel.RequestChannelPart(channel);
		}
//==========================================================
//	List channels
//==========================================================
		if(command == "l")
		{
			kernel.QueryChannel();
		}
//==========================================================
//	List users in channel
//==========================================================
		if(command == "u")
		{
			char dummy;
			cin.getline(&dummy, 1);
			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);
			kernel.QueryUser(channel);
		}
//==========================================================
//	Send private message
//==========================================================
		if(command == "m")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Recipient: ";
			cin.getline(recipient, BUFFER_SIZE);
			cout << "Message: ";
			cin.getline(message, BUFFER_SIZE);

			kernel.SendPrivateMessage(recipient, message);
		}
//==========================================================
//	Set media voice
//==========================================================
		if(command == "/media voice")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "channel: ";
			cin.getline(channel, BUFFER_SIZE);
			cout << "user: ";
			cin.getline(user, BUFFER_SIZE);

			kernel.RequestSetMediaVoice(channel, user);
		}
//==========================================================
//	Set topic
//==========================================================
		if(command == "t")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "channel: ";
			cin.getline(channel, BUFFER_SIZE);
			cout << "topic: ";
			cin.getline(topic, BUFFER_SIZE);

			kernel.RequestSetTopic(channel, topic);
		}
//==========================================================
//	Set channel mode
//==========================================================
		if(command == "x")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "mode: ";
			cin.getline(mode, BUFFER_SIZE);
			cout << "channel: ";
			cin.getline(target, BUFFER_SIZE);
			cout << "parameter: ";
			cin.getline(parameter, BUFFER_SIZE);

			bool value = true;
			if(mode[0] == '-')
				value = false;
			char modeName = mode[1];

			int intParameter = atoi(parameter);

			switch(modeName)
			{
				case IRM_CHANNEL_MODE_OPERATOR:
					if(value)
						kernel.RequestSetChannelOperator(target, parameter);
					else
						kernel.RequestUnsetChannelOperator(target, parameter);
					break;
				case IRM_CHANNEL_MODE_PRIVATE:
					if(value)
						kernel.RequestSetChannelPrivate(target);
					else
						kernel.RequestUnsetChannelPrivate(target);
					break;
				case IRM_CHANNEL_MODE_SECRET:
					if(value)
						kernel.RequestSetChannelSecret(target);
					else
						kernel.RequestUnsetChannelSecret(target);
					break;
				case IRM_CHANNEL_MODE_INVITE_ONLY:
					if(value)
						kernel.RequestSetChannelInviteOnly(target);
					else
						kernel.RequestUnsetChannelInviteOnly(target);
					break;
				case IRM_CHANNEL_MODE_OP_CHANGE_TOPIC:
					if(value)
						kernel.RequestSetChannelOpChangeTopic(target);
					else
						kernel.RequestUnsetChannelOpChangeTopic(target);
					break;
				case IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES:
					if(value)
						kernel.RequestSetChannelNoExternalMessages(target);
					else
						kernel.RequestUnsetChannelNoExternalMessages(target);
					break;
				case IRM_CHANNEL_MODE_MODERATED:
					if(value)
						kernel.RequestSetChannelModerated(target);
					else
						kernel.RequestUnsetChannelModerated(target);
					break;
				case IRM_CHANNEL_MODE_USER_LIMIT:
					if(value)
						kernel.RequestSetChannelUserLimit(target, intParameter);
					else
						kernel.RequestUnsetChannelUserLimit(target);
					break;
				case IRM_CHANNEL_MODE_MEDIA_COLLABORATOR:
					if(value)
						kernel.RequestSetChannelMediaCollabotator(target, parameter);
					else
						kernel.RequestUnsetChannelMediaCollabotator(target, parameter);
					break;
				case IRM_CHANNEL_MODE_KEY:
					if(value)
						kernel.RequestSetChannelKey(target, parameter);
					else
						kernel.RequestUnsetChannelKey(target);
					break;
				case IRM_CHANNEL_MODE_BIT_RATE:
					if(value)
						kernel.RequestSetChannelBitRate(target, intParameter);
					else
						kernel.RequestUnsetChannelBitRate(target);
					break;
				case IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE:
					if(value)
						kernel.RequestSetChannelAutoChangeBitRate(target);
					else
						kernel.RequestUnsetChannelAutoChangeBitRate(target);
					break;
				case IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION:
					if(value)
						kernel.RequestSetChannelMultipleTransmission(target);
					else
						kernel.RequestUnsetChannelMultipleTransmission(target);
					break;
				case IRM_CHANNEL_MODE_ONE_AUDIO:
					if(value)
						kernel.RequestSetChannelOneAudio(target, intParameter);
					else
						kernel.RequestUnsetChannelOneAudio(target);
					break;
				case IRM_CHANNEL_MODE_AUDIO_MUTE:
					if(value)
						kernel.RequestSetChannelAudioMute(target);
					else
						kernel.RequestUnsetChannelAudioMute(target);
					break;
				case IRM_CHANNEL_MODE_BAND_SHARE_LIMIT:
					if(value)
						kernel.RequestSetChannelBandShareLimit(target, intParameter);
					else
						kernel.RequestUnsetChannelBandShareLimit(target);
					break;
				case IRM_CHANNEL_MODE_TRANSMISSION_LIMIT:
					if(value)
						kernel.RequestSetChannelTransmissionLimit(target, intParameter);
					else
						kernel.RequestUnsetChannelTransmissionLimit(target);
					break;
			}
		}
//==========================================================
//	Set user mode
//==========================================================
		if(command == "z")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "mode: ";
			cin.getline(mode, BUFFER_SIZE);

			bool value = true;
			if(mode[0] == '-')
				value = false;
			char modeName = mode[1];

			switch(modeName)
			{
				case IRM_USER_MODE_INVISIBLE:
					if(value)
						kernel.RequestSetUserInvisible();
					else
						kernel.RequestUnsetUserInvisible();
					break;
				case IRM_USER_MODE_CAMERA_ON:
					if(value)
						kernel.RequestSetUserCameraOn();
					else
						kernel.RequestUnsetUserCameraOn();
					break;
				case IRM_USER_MODE_MIC_MUTE:
					if(value)
						kernel.RequestSetUserMicMute();
					else
						kernel.RequestUnsetUserMicMute();
					break;
				case IRM_USER_MODE_AWAY:
					if(value)
						kernel.RequestSetUserAway();
					else
						kernel.RequestUnsetUserAway();
					break;
				default:
					cout << "Invalid user mode" << endl;
			}
		}
//==========================================================
//	Invite user
//==========================================================
		if(command == "b")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "user: ";
			cin.getline(user, BUFFER_SIZE);
			cout << "channel: ";
			cin.getline(channel, BUFFER_SIZE);

			kernel.RequestInviteUser(user, channel);
		}
//==========================================================
//	Read media packet
//==========================================================
		if(command == "r")
		{
			silentShell = true;

			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Media ID: ";
			cin.getline(mediaId, BUFFER_SIZE);

			//unsigned long x(268532472);
			kernel.RequestMediaData((unsigned long)atoi(mediaId));

			silentShell = false;
		}
//==========================================================
//	File playback
//==========================================================
		if(command == "f")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Filename: ";
			cin.getline(filename, BUFFER_SIZE);

			kernel.InitializeFilePlayback(filename);
		}
//==========================================================
//	Seek in file playback
//==========================================================
		if(command == "s")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Position (in %): ";
			cin.getline(percent, BUFFER_SIZE);

			kernel.SeekFilePlayback((double)atof(percent));
		}
//==========================================================
//	Play in file playback
//==========================================================
		if(command == "i")
		{
			kernel.PlayFilePlayback();
		}
//==========================================================
//	Pause in file playback
//==========================================================
		if(command == "o")
		{
			kernel.PauseFilePlayback();
		}
//==========================================================
//	List video devices
//==========================================================
		if(command == "/vdev")
		{
			cout << "This command is currently not available";
			// Old stuff. Now it's part of iptv_video_capture.
			//VideoDeviceEnumerator v;
			//list<VideoDeviceInfo> deviceList;
			//VideoDeviceInfo device;
			//
			//v.GetVideoDeviceList(deviceList);
			//
			//while(!deviceList.empty())
			//{
			//	device = deviceList.front();
			//	deviceList.pop_front();
			//	
			//	cout << "Video Device "
			//		<< device.index
			//		<< ": "
			//		<< device.name
			//		<< endl;
			//}
		}
//==========================================================
//	Initialize media capture
//==========================================================
		if(command == "w")
		{
			//if(kernel.InitializeVideoCapture(0))
			//	cout << "Media capture initialized." << endl;
			//else
			//	cout << "Media capture initialization failed." << endl;
		}
//==========================================================
//	Get media capture frame
//==========================================================
		if(command == "g")
		{
			//unsigned char *data;
			//int width, height;
			//if(kernel.GetVideoCaptureFrame(&data, &width, &height))
			//{
			//	cout << "*** Frame captured ***" << endl;
			//	cout << "Data address: " << (void *)data << endl;
			//	cout << "Width: " << width << endl;
			//	cout << "Height: " << height << endl;
			//	cout << "**********************" << endl;
			//}
			//else
			//	cout << "Can't capture frame." << endl;
		}
//==========================================================
//	Release media capture
//==========================================================
		if(command == "e")
		{
			//kernel.ReleaseVideoCapture();
			//cout << "Media capture released." << endl;
		}
//==========================================================
//	Transmit file
//==========================================================
		if(command == "/ft transmit")
		{
			kernel.FileTransferBeginTransmission("multitv", "c:\\iptv_viewer_multiple_colaborations_on_linux.png", 463416, 128);
			// kernel.FileTransferBeginTransmission("multitv", "c:\\hello ft.txt", 4, 128);
		}
//==========================================================
//	Request set media voice
//==========================================================
		if(command == "/voice")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);

			cout << "User: ";
			cin.getline(user, BUFFER_SIZE);

			kernel.RequestSetMediaVoice(channel, user);
		}
//==========================================================
//	Send request media voice
//==========================================================
		if(command == "/voice request")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);

			kernel.SendMediaVoiceRequest(channel);
		}
//==========================================================
//	Send request media voice cancel
//==========================================================
		if(command == "/voice cancel")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);

			kernel.SendMediaVoiceRequestCancel(channel);
		}
//==========================================================
//	Send request media voice remove all
//==========================================================
		if(command == "/voice remove")
		{
			// Clear input buffer
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Channel: ";
			cin.getline(channel, BUFFER_SIZE);

			kernel.SendMediaVoiceRequestRemoveAll(channel);
		}
//==========================================================
//	Profile poli
//==========================================================
		if(command == "/poli")
		{
			kernel.RequestConnection("iptv.poli.ufrj.br", 8000, "mtguilherme1", "guilherme", "Guilherme", "g@e","");
		}
//==========================================================
//	Profile alpha
//==========================================================
		if(command == "/alpha")
		{
			kernel.RequestConnection("alpha.ip.tv", 8000, "mtguilherme1", "guilherme", "Guilherme", "g@e","");
		}
//==========================================================
//	UDP Availability
//==========================================================
		if(command == "/udp")
		{
			if(kernel.IsUdpConnectionAvailable())
				cout << "UDP available" << endl;
			else
				cout << "UDP unavailable" << endl;
		}
//==========================================================
//	File Transfer Enable
//==========================================================
		if(command == "/ft")
		{
			kernel.EnableFileTransfer();
			cout << "File Transfer Enabled" << endl;
		}
//==========================================================
//	Poll Enable
//==========================================================
		if(command == "/poll")
		{
			kernel.EnablePoll();
			cout << "Poll Enabled" << endl;
		}
//==========================================================
//	Whiteboard Enable
//==========================================================
		if(command == "/whiteboard")
		{
			kernel.EnableWhiteboard();
			cout << "Whiteboard Enabled" << endl;
		}
//==========================================================
//	REQUEST AV Conference transmission
//==========================================================
		if(command == "/avconf")
		{
			//kernel.EnableMediaConference();
			//kernel.InitializeMediaColaboration("multitv-m", 128);
		}
//==========================================================
//	Send CTCP message
//==========================================================
		if(command == "/ctcp")
		{
			char dummy;
			cin.getline(&dummy, 1);

			cout << "Recipient: ";
			cin.getline(recipient, BUFFER_SIZE);
			cout << "Ctcp code: ";
			cin.getline(ctcpCode, BUFFER_SIZE);
			cout << "Parameter: ";
			cin.getline(parameter, BUFFER_SIZE);

			SendCtcpMessage(recipient, ctcpCode, parameter);
		}
//==========================================================
//	Send poll
//==========================================================
		if(command == "/poll send")
		{
			if(pollData != NULL)
				delete pollData;
			pollData = new PollData();

			pollData->channel = currentChannel;

			cout << "Question: ";
			cin.getline(parameter, BUFFER_SIZE);
			pollData->question = parameter;

			int i = 0;
			VBString option;

			do
			{
				cout << "Option " << i << ": ";
				cin.getline(parameter, BUFFER_SIZE);
				option = parameter;

				PollAnswer pollAnswer;
				pollAnswer.index = i;
				pollAnswer.answerCount = 0;
				pollAnswer.text = option;

				if(!option.IsEmpty())
					pollData->answerList.push_back(pollAnswer);
				i++;
			} while(!option.IsEmpty());

			if(kernel.PollSend(*pollData))
				cout << "Poll send succesfull" << endl;
			else
				cout << "Poll send failure" << endl;

			PollAnswer noAnswer;
			noAnswer.index = -1;
			noAnswer.answerCount = 0;
			noAnswer.text = "No answer";
			pollData->answerList.push_back(noAnswer);
		}
//==========================================================
//	Send poll answer
//==========================================================
		if(command == "/poll answer")
		{
			cout << "Poll owner: ";
			cin.getline(recipient, BUFFER_SIZE);
			cout << "Option index: ";
			cin.getline(parameter, BUFFER_SIZE);

			kernel.PollSendAnswer(recipient, atoi(parameter));
		}
//==========================================================
//	View poll stats
//==========================================================
		if(command == "/poll stats")
		{
			if(pollData != NULL)
			{
				if(pollData->answerList.size() == 0)
					cout << "There are no answers to the current poll" << endl;
				list<PollAnswer>::iterator it;
				for(it = pollData->answerList.begin(); it != pollData->answerList.end(); it++)
					cout << "The option " << it->index << " had " << it->answerCount << " votes" << endl;
			}
			else
				cout << "There is no active poll" << endl;
		}
//==========================================================
//	Send poll stats
//==========================================================
		if(command == "/poll send stats")
		{
			if(pollData != NULL)
				if(kernel.PollSendStats(*pollData))
					cout << "Poll send stats succesfull" << endl;
				else
					cout << "Poll send stats failure" << endl;
			else
				cout << "There is no acitive poll" << endl;
		}
	}
//==========================================================
//	Farewell messages
//==========================================================
	cout << "Terminating connections..." << endl << endl;
	LogManager::Instance()->AddLogEntry("Terminating connections...");
//==========================================================
//	Terminating connection
//==========================================================
	kernel.TerminateAllMediaTransmissions();
	kernel.Disconnect();
	LogManager::Instance()->AddLogEntry("Connections terminated.");
//==========================================================
//	Exit
//==========================================================
	exit(0);
}
