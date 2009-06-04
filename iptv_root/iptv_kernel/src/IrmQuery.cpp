#include "iptv_kernel/IrmQuery.h"
#include "shared/irm/srv_defs.h"
#include "iptv_kernel/LogManager.h"
#include "iptv_kernel/CtcpMessage.h"
#include "iptv_kernel/VoiceRequestCtcpMessage.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

IrmQuery::IrmQuery(IrmQueryNotify *notify) :	m_isBeingDeleted(false),
												m_IrmStubProxy(NULL),
												m_irmMutex(NULL)
{

	m_irmMutex = new _SEMAPHORE(SEMAPH_MUTEX);
	m_IrmStubProxy = new ProxyIrmStub();

	// Set the pointer to the object that will be notified of IrmQuery events.
	m_notify = notify;

	// IRMQuery state initialization.
	ResetStateMachine();

	m_proxyIrmStubInitialized = false;

	// ProxyIrmStub initialization.
	if (!m_IrmStubProxy->Initialize(this))
	{
		// Error: ProxyIrmStub initialization not successful.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_IRM_STUB_INIT;

		m_notify->OnIrmQueryNotify(param);
	}

	else
	{
		// ProxyIrmStub initialization successful.
		m_proxyIrmStubInitialized = true;
	}
}

IrmQuery::~IrmQuery()
{
	// Note: dtor order is important.
	if(m_IrmStubProxy)
		delete m_IrmStubProxy;

	if(m_irmMutex)
		delete m_irmMutex;

	m_isBeingDeleted = true;
}

void IrmQuery::ResetStateMachine()
{
	m_udpListeningState = IRMULS_NOT_AVAILABLE;
	m_viewerState = IRMVS_NOT_INITIALIZED;
	m_connectionState= IRMCS_DISCONNECTED;
	m_authenticationState = IRMAS_NOT_AUTHENTICATED;
	m_channelJoinState = IRMCJS_NOT_JOINING;
	m_channelPartState = IRMCPS_NOT_PARTING;
	m_channelQueryState = IRMCQS_NOT_QUERYING;
	m_userQueryState = IRMUQS_NOT_QUERYING;
}

void IrmQuery::LockMutex()
{
	m_irmMutex->Wait();
}

void IrmQuery::UnlockMutex()
{
	m_irmMutex->Signal();
}

void IrmQuery::SetNotify(IrmQueryNotify *notify)
{
	LockMutex();
	m_notify = notify;
	UnlockMutex();
}

IRMUdpListeningState IrmQuery::GetUdpListeningState()
{
	LockMutex();
	IRMUdpListeningState state = m_udpListeningState;
	UnlockMutex();

	return state;
}

IRMViewerState IrmQuery::GetViewerState()
{
	LockMutex();
	IRMViewerState state = m_viewerState;
	UnlockMutex();

	return state;	
}

IRMConnectionState IrmQuery::GetConnectionState()
{
	LockMutex();
	IRMConnectionState state = m_connectionState;
	UnlockMutex();

	return state;
}

IRMAuthenticationState IrmQuery::GetAuthenticationState()
{
	LockMutex();
	IRMAuthenticationState state = m_authenticationState;
	UnlockMutex();

	return state;
}

IRMChannelJoinState IrmQuery::GetChannelJoinState()
{
	LockMutex();
	IRMChannelJoinState state = m_channelJoinState;
	UnlockMutex();

	return state;
}

IRMChannelPartState IrmQuery::GetChannelPartState()
{
	LockMutex();
	IRMChannelPartState state = m_channelPartState;
	UnlockMutex();

	return state;
}

IRMChannelQueryState IrmQuery::GetChannelQueryState()
{
	LockMutex();
	IRMChannelQueryState state = m_channelQueryState;
	UnlockMutex();

	return state;
}

IRMUserQueryState IrmQuery::GetUserQueryState()
{
	LockMutex();
	IRMUserQueryState state = m_userQueryState;
	UnlockMutex();

	return state;
}

IrmConnection IrmQuery::GetConnection()
{
	LockMutex();
	IrmConnection conn = m_connection;
	UnlockMutex();

	return conn;
}

IrmUser IrmQuery::GetUser()
{
	LockMutex();
	IrmUser user = m_user;
	UnlockMutex();

	return user;
}

bool IrmQuery::IsUdpListeningAvailable()
{
	LockMutex();
	IRMUdpListeningState udpState = GetUdpListeningState();
	UnlockMutex();

	if(udpState == IRMULS_AVAILABLE)
		return true;

	return false;
}

void PrintDataStringValue(VBString _data, VBString _string, long _value)
{
	VBString message = _data;
	message +=  " string = ";
	message += _string;
	message += " value = ";
	message += _value;
	LogManager::Instance()->AddLogEntry(message);
}

void PrintAllMessageParameters(long _cmd, IrmParam& _source, IrmParam& _target, IrmParam& _data,
							  IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3,
							  IrmParam& _compl4, IrmParam& _compl5, IrmParam& _compl6,
							  IrmParam& _compl7)
{
	VBString message;
	message =  "New message";
	LogManager::Instance()->AddLogHeader(message);

	message =  "Command code = ";
	message += _cmd;
	LogManager::Instance()->AddLogEntry(message);
	
	PrintDataStringValue("Source", _source.GetString(), _source.GetValue());
	PrintDataStringValue("Target", _target.GetString(), _target.GetValue());
	PrintDataStringValue("Data", _data.GetString(), _data.GetValue());
	PrintDataStringValue("Complement 1", _compl1.GetString(), _compl1.GetValue());
	PrintDataStringValue("Complement 2", _compl2.GetString(), _compl2.GetValue());
	PrintDataStringValue("Complement 3", _compl3.GetString(), _compl3.GetValue());
	PrintDataStringValue("Complement 4", _compl4.GetString(), _compl4.GetValue());
	PrintDataStringValue("Complement 5", _compl5.GetString(), _compl5.GetValue());
	PrintDataStringValue("Complement 6", _compl6.GetString(), _compl6.GetValue());
	PrintDataStringValue("Complement 7", _compl7.GetString(), _compl7.GetValue());

	message =  "End of message parameters";
	LogManager::Instance()->AddLogHeader(message);
}


void IrmQuery::Fire_OnMessage(long _cmd, IrmParam& _source, IrmParam& _target, IrmParam& _data,
							  IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3,
							  IrmParam& _compl4, IrmParam& _compl5, IrmParam& _compl6,
							  IrmParam& _compl7)
{
	if(!m_isBeingDeleted)
	{
		// Notifying the IRM message to the kernel.
		IrmQueryIrmMessageParam param;

		param.m_code = IQNC_IRM_MESSAGE;

		param.m_user = m_user;
		param.m_connection = m_connection;

		param.m_message.m_irmMessageCode = _cmd;

		param.m_message.m_strSource = _source.GetString();
		param.m_message.m_strTarget = _target.GetString();
		param.m_message.m_strData = _data.GetString();
		param.m_message.m_strComplement1 = _compl1.GetString();
		param.m_message.m_strComplement2 = _compl2.GetString();
		param.m_message.m_strComplement3 = _compl3.GetString();
		param.m_message.m_strComplement4 = _compl4.GetString();
		param.m_message.m_strComplement5 = _compl5.GetString();
		param.m_message.m_strComplement6 = _compl6.GetString();
		param.m_message.m_strComplement7 = _compl7.GetString();

		param.m_message.m_longSource = _source.GetValue();
		param.m_message.m_longTarget = _target.GetValue();
		param.m_message.m_longData = _data.GetValue();
		param.m_message.m_longComplement1 = _compl1.GetValue();
		param.m_message.m_longComplement2 = _compl2.GetValue();
		param.m_message.m_longComplement3 = _compl3.GetValue();
		param.m_message.m_longComplement4 = _compl4.GetValue();
		param.m_message.m_longComplement5 = _compl5.GetValue();
		param.m_message.m_longComplement6 = _compl6.GetValue();
		param.m_message.m_longComplement7 = _compl7.GetValue();

		m_notify->OnIrmQueryNotify(param);

		LockMutex();

		LPCSTR message = _compl7.GetString();

		switch (_cmd)
		{
		//---------------------------------------------------------------------------------------------
		// Error events
		//---------------------------------------------------------------------------------------------
		case CMD_ERROR:
			OnCommandError(_data.GetString(), _source.GetString(), _target.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		// Connection events
		//---------------------------------------------------------------------------------------------
		case CONNECTED_HOST:
			OnConnectedHost();
			break;
		//---------------------------------------------------------------------------------------------
		case CONNECTED_CHAT:
			OnConnectedChat();
			break;
		//---------------------------------------------------------------------------------------------
		case DISCONNECTED_CHAT:
			OnDisconnectedChat();
			break;
		//---------------------------------------------------------------------------------------------
		// Acknowledge events
		//---------------------------------------------------------------------------------------------
		case APP_PING:
			OnAppPing();
			break;
		//---------------------------------------------------------------------------------------------
		case UDP_PING:
			OnUdpPing(_source.GetString(), _target.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case UDP_PONG:
			OnUdpPong();
			break;
		//---------------------------------------------------------------------------------------------
		// Authentication events
		//---------------------------------------------------------------------------------------------
		case IDENTIFY_REQUEST:
			OnIdentificationRequest();
			break;
		//---------------------------------------------------------------------------------------------
		case IDENTIFY:
			OnIdentify();
			break;
		//---------------------------------------------------------------------------------------------
		case IDENTIFY_ERROR:
			OnIdentificationError();
			break;
		//---------------------------------------------------------------------------------------------
		// Server events
		//---------------------------------------------------------------------------------------------
		case SERVER_MSG:
			OnServerMessage(message);
			break;
		//---------------------------------------------------------------------------------------------
		case SERVER_REDIR:
			OnServerRedir(_source.GetString(), _target.GetString(), _data.GetValue());
			break;
		//---------------------------------------------------------------------------------------------
		// Channel and user events
		//---------------------------------------------------------------------------------------------
		case JOIN_CHANNEL:
			OnChannelJoin(_target.GetString(), _source.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case PART_CHANNEL:
			OnChannelPart(_target.GetString(), _source.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case MODE:
				OnMode(_target.GetString(), _data.GetString(), _source.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case UNAWAY:
				OnUserNotAway();
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIAVOICE:
			OnChannelMediaVoice(_source.GetString(), _target.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case CHANGE_NICK:
			OnUserChangeNick(_source.GetString(), _target.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case QUIT_CHAT:
			OnUserQuit(_source.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		// Query events
		//---------------------------------------------------------------------------------------------
		case GET_CHANNEL_LIST:
			OnChannelQueryItem(_target.GetString(), _data.GetString(), _compl1.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case ENDOF_CHANNEL_LIST:
			OnChannelQueryEnd();
			break;
		//---------------------------------------------------------------------------------------------
		case GET_USER_LIST :
			OnUserQueryItem(_source.GetString(),_data.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case ENDOF_USER_LIST :
			OnUserQueryEnd(_source.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIAQUERY:
			// ch, user, id, class
			//OnMediaQueryItem(_compl1.GetString(), _target.GetString(), _data.GetValue(), _compl2.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIAQUERYEND:
			//OnMediaQueryEnd();
			break;
		//---------------------------------------------------------------------------------------------
		// Private message events
		//---------------------------------------------------------------------------------------------
		case PRIVMSG:
			OnPrivateMessage(_source.GetString(), _data.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		case PRIVMSG_CHANNEL:
			OnChannelPrivateMessage(_source.GetString(), _target.GetString(), _data.GetString());
			break;
		//---------------------------------------------------------------------------------------------
		// Media events
		//---------------------------------------------------------------------------------------------
		case MEDIA:
			{
				OnMedia(_data.GetValue(), _compl1.GetString(), _target.GetString(), _source.GetString(), _compl2.GetString(), _compl3.GetValue(), _compl4.GetValue());
			}
			break;
		//---------------------------------------------------------------------------------------------
			case MEDIASNDSTOP:
			{
			}
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIANOTIFY:
			{
				OnMediaNotify(_compl1.GetString(), _data.GetValue(), _compl2.GetString(), _compl3.GetString(), _target.GetString());
			}
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIARCV:
			{
				PrintAllMessageParameters(_cmd, _source, _target, _data,
						  _compl1, _compl2, _compl3,
						  _compl4, _compl5, _compl6,
						  _compl7);
				// host, id, packetlen, port, headerlen, header
				//OnMediaReceive(_target.GetString(), _data.GetValue(), _compl1.GetValue(), _compl2.GetValue(), _compl3.GetValue(), _compl4.GetString() );
				//break;
			}
			break;
		//---------------------------------------------------------------------------------------------
		case MEDIARCVALL:
			{
				OnMediaReceiveAll(_data.GetString(), _compl1.GetString(), _compl2.GetValue(), _compl3.GetString());
			}
			break;
		//---------------------------------------------------------------------------------------------
		default:
			OnUnknownMessage(_cmd);
		}

		UnlockMutex();
	}
}

bool IrmQuery::IsCtcpMessage(VBString message)
{
	if(message.getChar(0) == IRM_QUERY_CTCP_PREFIX)
		return true;

	return false;
}

bool IrmQuery::InitializeViewer(VBString server, unsigned port)
{
	LockMutex();

	if(server.length() == 0)
	{
		// Error: viewer initialization parameter blank.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_VIEWER_PARAM_BLANK;
		m_notify->OnIrmQueryNotify(param);

		UnlockMutex();
		return false;
	}

	if((m_proxyIrmStubInitialized == true) && (m_connectionState == IRMCS_DISCONNECTED) && (m_viewerState == IRMVS_NOT_INITIALIZED))
	{
		IrmParam irmServer(server.c_str());

		if(m_IrmStubProxy->InitViewer(port, irmServer))
		{
			// Viewer initialization successful.
			m_viewerState = IRMVS_INITIALIZED;

			UnlockMutex();
			return true;		
		}

		else
		{
			// Error: viewer initialization not successful.
			IrmQueryParam param;
			param.m_code = IQNC_ERROR_VIEWER_INIT;
			m_notify->OnIrmQueryNotify(param);

			UnlockMutex();
			return false;
		}	
	}

	else
	{
		// Error: viewer initialization is not allowed unless ProxyIrmStub is initialized, the IRMQuery is disconnected and the viewer not initialized.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_VIEWER_STATE;
		m_notify->OnIrmQueryNotify(param);

		UnlockMutex();
		return false;
	}
}

void IrmQuery::CloseViewer()
{
	LockMutex();

	if(m_viewerState == IRMVS_INITIALIZED)
	{
		m_IrmStubProxy->CloseViewer();
		m_viewerState = IRMVS_NOT_INITIALIZED;
	}

	UnlockMutex();
}

void IrmQuery::RequestConnection(
									VBString server, unsigned port, VBString user,
									VBString passwd, VBString realName, VBString email,
									VBString serverPasswd,
									VBString clientVersion
								)
{
	LockMutex();

	if(	server.length() == 0 || user.length() == 0 || passwd.length() == 0 ||
		realName.length() == 0 || email.length() == 0 || clientVersion.length() == 0)
	{
		// Error: connection parameter blank.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_CONN_PARAM_BLANK;
		m_notify->OnIrmQueryNotify(param);

		UnlockMutex();
		return;
	}

	if(	(m_proxyIrmStubInitialized == true) &&
		(m_connectionState == IRMCS_DISCONNECTED) &&
		(m_viewerState == IRMVS_NOT_INITIALIZED))
	{
		IrmParam irmServer(server.c_str());
		IrmParam irmPort(port);
		IrmParam irmUser(user.c_str());
		IrmParam irmEmail(email.c_str());
		IrmParam irmRealName(realName.c_str());
		IrmParam irmClientVersion(clientVersion.c_str());
		IrmParam irmPass(passwd.c_str());
		IrmParam irmServerPasswd(serverPasswd.c_str());

		if (m_IrmStubProxy->Connect( irmServer,
									irmPort,
									irmUser,
									irmEmail,
									irmRealName,
									irmClientVersion,
									irmServerPasswd ))
		{
			// Conenction query successful.
			m_connectionState = IRMCS_CONNECTING;

			m_connection.SetHost(server);
			m_connection.SetPort(port);

			m_user.SetNick(user);
			m_user.SetPasswd(passwd);
			m_user.SetRealName(realName);
			m_user.SetEmail(email);

			UnlockMutex();
			return;
		}

		// Error: connection query not successful.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_CONN_QUERY;
		m_notify->OnIrmQueryNotify(param);

		UnlockMutex();
		return;
	}

	else
	{
		// Error: connection is not allowed unless ProxyIrmStub is initialized, the IRMQuery is disconnected and the viewer not initialized.
		IrmQueryParam param;
		param.m_code = IQNC_ERROR_CONN_STATE;
		m_notify->OnIrmQueryNotify(param);

		UnlockMutex();
		return;
	}

	UnlockMutex();
}

void IrmQuery::DoDisconnect()
{
	// m_IrmStubProxy->Disconnect();
	ResetStateMachine();

	m_user.Reset();
	m_connection.Reset();
}

void IrmQuery::Disconnect()
{
	LockMutex();

	DoDisconnect();

	UnlockMutex();
}

void IrmQuery::RequestAuthentication()
{
	LockMutex();

	if((m_connectionState == IRMCS_CONNECTED)&&(m_authenticationState != IRMAS_AUTHENTICATING))
	{
		IrmParam irmPasswd(m_user.GetPasswd().c_str());
		m_IrmStubProxy->SendIrmMessage(IDENTIFY, irmPasswd);

		// The password is reset because it will no longer be needed.
		// The authentication is attempted only once per connection.
		m_user.SetPasswd("");

		m_authenticationState = IRMAS_AUTHENTICATING;
	}

	UnlockMutex();
}

void IrmQuery::RequestChannelJoin(VBString channel, VBString passwd)
{
	LockMutex();

	if((m_connectionState == IRMCS_CONNECTED)&&(m_channelJoinState == IRMCJS_NOT_JOINING))
	{
		IrmParam irmChannel(channel.c_str());
		IrmParam irmPasswd(passwd.c_str());
		m_IrmStubProxy->SendIrmMessage(JOIN_CHANNEL, irmChannel, irmPasswd);

		m_channelJoinState = IRMCJS_JOINING;
		m_userQueryState = IRMUQS_QUERYING;
	}

	UnlockMutex();
}

void IrmQuery::RequestChannelPart(VBString channel)
{
	LockMutex();

	if((m_connectionState == IRMCS_CONNECTED)&&(m_channelPartState == IRMCPS_NOT_PARTING))
	{
		IrmParam irmChannel(channel.c_str());
		m_IrmStubProxy->SendIrmMessage(PART_CHANNEL, irmChannel);

		m_channelPartState = IRMCPS_PARTING;
	}

	UnlockMutex();
}

void IrmQuery::RequestSetMediaVoice(VBString channel, VBString user)
{
	VBString command = "MEDIAVOICE ";
	command += AddChannelPrefix(channel);

	command += " ";
	
	if(user.IsEmpty())
		user = "\"\"";

	command += user;

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::RequestSetTopic(VBString channel, VBString topic)
{
	VBString command = "TOPIC ";
	command += AddChannelPrefix(channel);
	command += " \"";
	command += topic;
	command += "\"";

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::RequestChannelModeChange(	char name,
											bool value,
											VBString channel,
											VBString parameter)
{
	channel = AddChannelPrefix(channel);

	RequestModeChange(name, value, channel, parameter);
}

void IrmQuery::RequestUserModeChange(	char name,
										bool value,
										VBString user)
{
	// If the parameter user is not provided,
	// the current user is assumed.
	if(user.IsEmpty())
		user = m_user.GetNick();

	RequestModeChange(name, value, user);
}

void IrmQuery::RequestModeChange(	char name,
									bool value,
									VBString target,
									VBString parameter)
{
	VBString command = "MODE ";
	command += target;
	command += " ";
	value?(command += IRM_MODE_POSITIVE):(command += IRM_MODE_NEGATIVE);
	command += name;
	if(!parameter.IsEmpty())
	{
		command += " ";
		command += parameter;
	}

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);	
}

void IrmQuery::RequestSetUserAway()
{
	VBString command = "AWAY : ";
	command += m_user.GetNick();

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::RequestUnsetUserAway()
{
	VBString command = "AWAY";

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::RequestInviteUser(VBString user, VBString channel)
{
	VBString command = "INVITE ";
	command += user;
	command += AddChannelPrefix(channel);

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::QueryChannel(VBString channel)
{
	LockMutex();

	if((m_connectionState == IRMCS_CONNECTED)&&(m_channelQueryState == IRMCQS_NOT_QUERYING))
	{
		if(channel.IsEmpty())
		{
			// Query information from all channels.
			m_IrmStubProxy->SendIrmMessage(GET_CHANNEL_LIST);
		}

		else
		{
			// Query information from a single channel.
			IrmParam irmChannel(channel.c_str());
			m_IrmStubProxy->SendIrmMessage(GET_CHANNEL_LIST, irmChannel);
		}

		m_channelQueryState = IRMCQS_QUERYING;
	}

	UnlockMutex();
}

void IrmQuery::QueryChannelModes(VBString channel)
{
	VBString command = "MODE ";
	command += AddChannelPrefix(channel);

	IrmParam irmCommand(command.c_str());
	m_IrmStubProxy->SendIrmMessage(USER_COMMAND, irmCommand);
}

void IrmQuery::QueryUser(VBString channel)
{
	LockMutex();

	if((m_connectionState == IRMCS_CONNECTED)&&(m_userQueryState == IRMUQS_NOT_QUERYING))
	{
		IrmParam irmChannel(channel.c_str());
		m_IrmStubProxy->SendIrmMessage(GET_USER_LIST, irmChannel);

		m_userQueryState = IRMUQS_QUERYING;
	}

	UnlockMutex();
}

void IrmQuery::SendPrivateMessage(VBString recipient, VBString message)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmRecipient(recipient.c_str());
		IrmParam irmMessage(message.c_str());

		m_IrmStubProxy->SendIrmMessage(PRIVMSG, irmRecipient, irmMessage);
	}

	UnlockMutex();
}

void IrmQuery::SendChannelPrivateMessage(VBString channel, VBString message)
{
	SendPrivateMessage(AddChannelPrefix(channel), message);
}

void IrmQuery::SendMediaVoiceRequest(VBString channel)
{
	SendChannelPrivateMessage(channel, "\01MEDIAVOICEREQUEST\01");
}

void IrmQuery::SendMediaVoiceRequestCancel(VBString channel)
{
	SendChannelPrivateMessage(channel, "\01MEDIAVOICECANCEL\01");
}

void IrmQuery::SendMediaVoiceRequestRemoveAll(VBString channel)
{
	SendChannelPrivateMessage(channel, "\01MEDIAVOICEREMOVE\01");
}

void IrmQuery::RequestMediaReceive(long mediaId, VBString networkProtocol, unsigned port)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmMediaId(mediaId);
		IrmParam irmNetworkProtocol(networkProtocol.c_str());
		IrmParam irmPort(port);

		m_IrmStubProxy->SendIrmMessage(MEDIARCV, irmMediaId, irmNetworkProtocol, irmPort);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaReceiveRun(long mediaId)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmMediaId(mediaId);

		m_IrmStubProxy->SendIrmMessage(MEDIARCVRUN, irmMediaId);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaReceiveStop(long mediaId)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmMediaId(mediaId);

		m_IrmStubProxy->SendIrmMessage(MEDIARCVSTOP, irmMediaId);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaReceiveAll(	VBString channel,
										VBString mediaClass,
										VBString networkProtocol,
										unsigned port)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmChannel(AddChannelPrefix(channel));
		IrmParam irmMediaClass(mediaClass);
		IrmParam irmNetworkProtocol(networkProtocol);
		IrmParam irmPort(port);

		m_IrmStubProxy->SendIrmMessage(	MEDIARCVALL, irmChannel, irmMediaClass,
										irmNetworkProtocol, irmPort);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaReceiveAllRun(	VBString channel,
											VBString mediaClass)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmChannel(AddChannelPrefix(channel));
		IrmParam irmMediaClass(mediaClass);

		m_IrmStubProxy->SendIrmMessage(MEDIARCVALLRUN, irmChannel, irmMediaClass);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaReceiveAllStop(	VBString channel,
											VBString mediaClass)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmChannel(AddChannelPrefix(channel));
		IrmParam irmMediaClass(mediaClass);

		m_IrmStubProxy->SendIrmMessage(	MEDIARCVALLSTOP, irmChannel, irmMediaClass);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaSend(	VBString channel,
									VBString mediaClass,
									VBString networkProtocol,
									unsigned bitRate,
									unsigned headerSize,
									VBString header)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmChannel(AddChannelPrefix(channel));
		IrmParam irmMediaClass(mediaClass);
		IrmParam irmNetworkProtocol(networkProtocol);
		IrmParam irmBitRate(bitRate);
		IrmParam irmHeaderSize(headerSize);
		IrmParam irmHeader(header);

		cout	<< "Requesting media send" << endl
				<< "Channel = " << AddChannelPrefix(channel)
				<< "|Media Class = " << mediaClass
				<< "|Network Protocol = " << networkProtocol
				<< "|Bit Rate = " << bitRate
				<< "|Header Size = " << headerSize
				<< "|Header = " << header << endl;

		if(header.length() == 0)
			m_IrmStubProxy->SendIrmMessage(	MEDIASND, irmChannel, irmMediaClass,
											irmNetworkProtocol, irmBitRate);
		else
			m_IrmStubProxy->SendIrmMessage(	MEDIASND, irmChannel, irmMediaClass,
											irmNetworkProtocol, irmBitRate, irmHeaderSize,
											irmHeader);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaSendStop(long mediaId)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmMediaId(mediaId);

		m_IrmStubProxy->SendIrmMessage(MEDIASNDSTOP, irmMediaId);
	}

	UnlockMutex();
}

void IrmQuery::RequestMediaPacket(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmMediaId(mediaId);
		IrmParam irmInitialPacket(initialPacketIndex);
		IrmParam irmFinalPacket(finalPacketIndex);

		m_IrmStubProxy->SendIrmMessage(MEDIAPKTREQUEST, irmMediaId, irmInitialPacket, irmFinalPacket);
	}

	UnlockMutex();
}

void IrmQuery::RequestChannelWhiteboard(VBString channel, VBString data)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		IrmParam irmChannel(AddChannelPrefix(channel));
		IrmParam irmData(data);

		m_IrmStubProxy->SendIrmMessage(WBOARD_CHANNEL, irmChannel, irmData);
	}

	UnlockMutex();
}

void IrmQuery::RequestConferenceMediaReceive(VBString channel)
{
	LockMutex();

	if(m_connectionState == IRMCS_CONNECTED)
	{
		VBString mediaClass = IRM_MEDIACLASS_AVCONF;
		VBString netProtocol;
		unsigned port;

		if(IsUdpListeningAvailable())
		{
			netProtocol = IRM_NET_PROTOCOL_UDP;
			port = IRM_AVCONF_UDP_PORT;
		}
		else
		{
			netProtocol = IRM_NET_PROTOCOL_TCP;
			// The port is meaningless in this request if the protocol is TCP.
			port = 0;
		}
		
		RequestMediaReceiveAll(channel, mediaClass, netProtocol, port);
	}

	UnlockMutex();
}

//---------------------------------------------------------------------------------------------
// IRM message handlers
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// Error events
//---------------------------------------------------------------------------------------------

/** Command error.
*
*/
void IrmQuery::OnCommandError(LPCSTR x, LPCSTR y, LPCSTR z)
{
	int ircError = atoi(x);
	VBString msg1 = y;
	VBString msg2 = z;

	switch(ircError)
	{
		case ERR_NICKNAMEINUSE :
			{
				IrmQueryParam param;
				param.m_code = IQNC_DISCONNECTED;
				param.m_irmMessage = msg1;
				m_notify->OnIrmQueryNotify(param);
				DoDisconnect();
			}
			break;
		case ERR_BADCHANNELKEY:
			{
				IrmQueryParam param;
				param.m_code = IQNC_CHANNEL_PASSWD_INVALID;
				param.m_irmMessage = msg1;
				m_notify->OnIrmQueryNotify(param);

				m_channelJoinState = IRMCJS_NOT_JOINING;
			}
			break;
	}

	std::cout << "IRM Command error message: "<< ircError << "-" << msg1 << "-" << msg2 << std::endl;
}

//---------------------------------------------------------------------------------------------
// Connection events
//---------------------------------------------------------------------------------------------

/** Connected to host.
*
*/
void IrmQuery::OnConnectedHost()
{
	if(m_connectionState == IRMCS_CONNECTING)
	{
		IrmQueryParam param;
		param.m_code = IQNC_CONNECTED_HOST;
		m_notify->OnIrmQueryNotify(param);
	}
}

/** Connected to chat.
*
*/
void IrmQuery::OnConnectedChat()
{
	if(m_connectionState == IRMCS_CONNECTING)
	{
		IrmQueryParam param;
		param.m_code = IQNC_CONNECTED_CHAT;
		m_notify->OnIrmQueryNotify(param);

		m_connectionState = IRMCS_CONNECTED;
	}
}

/** Disconnected.
*
*/
void IrmQuery::OnDisconnectedChat()
{
	if(m_connectionState != IRMCS_DISCONNECTED)
	{
		IrmQueryParam param;
		param.m_code = IQNC_DISCONNECTED;
		m_notify->OnIrmQueryNotify(param);
	}

	DoDisconnect();
}

//---------------------------------------------------------------------------------------------
// Acknowledge events
//---------------------------------------------------------------------------------------------

/** Server ping.
*
*/
void IrmQuery::OnAppPing()
{
	m_IrmStubProxy->AppPong();
}

/** Udp ping.
*
*/
void IrmQuery::OnUdpPing(LPCSTR user, LPCSTR ip)
{
	// std::cout << "OnUdpPing User: " << user << " - " <<"IP: " << ip << std::endl;
}

/** UDP listening is available.
*	The UDP Pong message is the server answer to the client's UDP Ping message.
*	This means that the client is able to receive UDP packets.
*/
void IrmQuery::OnUdpPong()
{
	m_udpListeningState = IRMULS_AVAILABLE;

	IrmQueryParam param;
	param.m_code = IQNC_UDP_CONNECTION_AVAILABLE;
	m_notify->OnIrmQueryNotify(param);
}

//---------------------------------------------------------------------------------------------
// Authentication events
//---------------------------------------------------------------------------------------------

/** Authentication requested.
*
*/
void IrmQuery::OnIdentificationRequest()
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_authenticationState != IRMAS_AUTHENTICATING))
	{
		IrmQueryParam param;
		param.m_code = IQNC_AUTHENTICATION_REQUESTED;

		m_notify->OnIrmQueryNotify(param);
	}
}

/** Authenticated.
*
*/
void IrmQuery::OnIdentify()
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_authenticationState == IRMAS_AUTHENTICATING))
	{
		IrmQueryParam param;
		param.m_code = IQNC_AUTHENTICATED;
		m_notify->OnIrmQueryNotify(param);

		m_authenticationState = IRMAS_AUTHENTICATED;
	}
}

/** Authentication failed.
*
*/
void IrmQuery::OnIdentificationError()
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_authenticationState == IRMAS_AUTHENTICATING))
	{
		IrmQueryParam param;
		param.m_code = IQNC_AUTHENTICATION_ERROR;
		m_notify->OnIrmQueryNotify(param);

		m_authenticationState = IRMAS_NOT_AUTHENTICATED;
	}
}

//---------------------------------------------------------------------------------------------
// Server events
//---------------------------------------------------------------------------------------------

void IrmQuery::OnServerMessage(LPCSTR message)
{
	// std::cout << "Server message: " << message << std::endl;
}

void IrmQuery::OnServerRedir(LPCSTR source, LPCSTR server, long port)
{
	// std::cout << "Server redir source = " << source << " server = " << server << " port = " << port << std::endl;
}

//---------------------------------------------------------------------------------------------
// Channel and user events
//---------------------------------------------------------------------------------------------

/** Channel joining process initiated.
*
*/
void IrmQuery::OnChannelJoin(LPCSTR channel, LPCSTR user)
{
	if((m_connectionState == IRMCS_CONNECTED))
	{
		IrmUser irmUser(user);

		IrmQueryUserParam param;
		param.m_code = IQNC_CHANNEL_JOIN;
		param.m_channelName = RemoveChannelPrefix(channel);
		param.m_userName = irmUser.GetNick();
		
		m_notify->OnIrmQueryNotify(param);

		NotifyIrmModeFromUserPrefix(irmUser, channel);
	}
}

/** Left channel.
*
*/
void IrmQuery::OnChannelPart(LPCSTR channel, LPCSTR user)
{
	if((m_connectionState == IRMCS_CONNECTED))
	{
		IrmQueryUserParam param;
		param.m_code = IQNC_CHANNEL_PART;
		param.m_channelName = RemoveChannelPrefix(channel);
		param.m_userName = user;	
		m_notify->OnIrmQueryNotify(param);

		if((m_channelPartState == IRMCPS_PARTING)&&(m_user.GetNick() == param.m_userName))
		{
			m_channelPartState = IRMCPS_NOT_PARTING;
		}
	}
}

/** Notifies the kernel of changes in channel or user modes.
* @param[in] target	The channel or user who receives the mode.
* @param[in] flags The list of mode flags.
* @param[in] parameters The list of mode parameters.
*/
void IrmQuery::OnMode(LPCSTR target, LPCSTR flags, LPCSTR parameters)
{
	// Creating the mode list
	list<IrmMode> modeList = CreateModeList(target, flags, parameters);

	// Parsing the mode list
	while(modeList.size() > 0)
	{
		// Getting the current mode
		IrmMode mode = modeList.front();

		IrmQueryNotifyCode notificationCode;

		// Setting channel notification code.
		if(IsChannel(mode.GetTarget()))
			notificationCode = mode.GetChannelModeNotificationCode();

		// Setting user notification code.
		else
			notificationCode = mode.GetUserModeNotificationCode();

		IrmQueryModeParam param;
		param.m_code = notificationCode;

		if(param.IsNotificationCodeValid())
		{
			// Sending mode notification
			param.m_name = mode.GetName();
			param.m_value = mode.GetValue();
			param.m_target = RemoveChannelPrefix(mode.GetTarget());
			param.m_parameter = mode.GetParameter();		
			m_notify->OnIrmQueryNotify(param);
		}

		// Popping the mode list
		modeList.pop_front();
	}
}

void IrmQuery::OnUserNotAway()
{
	// Sending mode notification
	char userModeName;
	IrmQueryModeParam modeParam;

	userModeName = IRM_USER_MODE_AWAY;
	modeParam.m_code = IQNC_USER_MODE_AWAY;

	modeParam.m_name = userModeName;
	modeParam.m_value = false;
	modeParam.m_target = m_user.GetNick();
	m_notify->OnIrmQueryNotify(modeParam);
}

void IrmQuery::OnChannelMediaVoice(LPCSTR channel, LPCSTR user)
{
	// Sending media voice notification
	IrmQueryUserParam param;

	param.m_code = IQNC_CHANNEL_VOICE;
	param.m_channelName = RemoveChannelPrefix(channel);
	param.m_userName = user;

	m_notify->OnIrmQueryNotify(param);	
}

void IrmQuery::OnUserChangeNick(LPCSTR oldNick, LPCSTR newNick)
{
	// std::cout << "OnUserChangeNick old nick = " << oldNick << " new nick = " << newNick << std::endl;
}

/** User quit.
*
*/
void IrmQuery::OnUserQuit(LPCSTR user)
{
	if((m_connectionState == IRMCS_CONNECTED))
	{
		IrmQueryUserParam param;
		param.m_code = IQNC_USER_QUIT;
		param.m_userName = user;
		param.m_channelName = "";
		
		m_notify->OnIrmQueryNotify(param);
	}
}

//---------------------------------------------------------------------------------------------
// Query events
//---------------------------------------------------------------------------------------------

/** Item from the channel query.
*
*/
void IrmQuery::OnChannelQueryItem(LPCSTR channel, LPCSTR topic, LPCSTR userCount)
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_channelQueryState == IRMCQS_QUERYING))
	{
		IrmQueryChannelParam param;
		param.m_code = IQNC_CHANNEL_QUERY_ITEM;
		param.m_channelName = RemoveChannelPrefix(channel);
		param.m_channelTopic = topic;
		param.m_userCount = atoi(userCount);
		m_notify->OnIrmQueryNotify(param);
	}
}

/** End of the channel query.
*
*/
void IrmQuery::OnChannelQueryEnd()
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_channelQueryState == IRMCQS_QUERYING))
	{
		IrmQueryParam param;
		param.m_code = IQNC_CHANNEL_QUERY_END;
		m_notify->OnIrmQueryNotify(param);

		m_channelQueryState = IRMCQS_NOT_QUERYING;
	}
}

/** Item from the channel user query.
*
*/
void IrmQuery::OnUserQueryItem(LPCSTR channel, LPCSTR user)
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_userQueryState == IRMUQS_QUERYING))
	{
		IrmUser irmUser(user);

		IrmQueryUserParam param;
		param.m_code = IQNC_USER_QUERY_ITEM;
		param.m_channelName = RemoveChannelPrefix(channel);
		param.m_userName = irmUser.GetNick();
		m_notify->OnIrmQueryNotify(param);

		NotifyIrmModeFromUserPrefix(irmUser, channel);
	}
}

void IrmQuery::NotifyIrmModeFromUserPrefix(IrmUser irmUser, VBString channel)
{
	if(irmUser.HasValidPrefix())
	{
		// Sending mode notification
		IrmQueryModeParam modeParam;

		modeParam.m_value = true;
		modeParam.m_target = RemoveChannelPrefix(channel);
		modeParam.m_parameter = irmUser.GetNick();

		if(irmUser.IsOperator())
		{
			modeParam.m_code = IQNC_CHANNEL_MODE_OPERATOR;
			modeParam.m_name = IRM_CHANNEL_MODE_OPERATOR;

			m_notify->OnIrmQueryNotify(modeParam);
		}

		if (irmUser.IsMediaCollaborator())
		{
			modeParam.m_code = IQNC_CHANNEL_MODE_MEDIA_COLLABORATOR;
			modeParam.m_name = IRM_CHANNEL_MODE_MEDIA_COLLABORATOR;

			m_notify->OnIrmQueryNotify(modeParam);
		}
	}
}

/** End of the channel user query.
*
*/
void IrmQuery::OnUserQueryEnd(LPCSTR channel)
{
	if((m_connectionState == IRMCS_CONNECTED)&&(m_userQueryState == IRMUQS_QUERYING))
	{
		IrmQueryUserParam param;
		param.m_code = IQNC_USER_QUERY_END;
		param.m_userName = "";
		param.m_channelName = RemoveChannelPrefix(channel);
		
		m_notify->OnIrmQueryNotify(param);

		m_userQueryState = IRMUQS_NOT_QUERYING;

		if(m_channelJoinState == IRMCJS_JOINING)
		{
			// Channel join complete.
			QueryChannelModes(channel);
			m_channelJoinState = IRMCJS_NOT_JOINING;
		}
	}
}

//---------------------------------------------------------------------------------------------
// Private message events
//---------------------------------------------------------------------------------------------

/** Private message received.
*
*/
void IrmQuery::OnPrivateMessage(LPCSTR sender, LPCSTR message)
{
	if(m_connectionState == IRMCS_CONNECTED)
	{
		if(!IsCtcpMessage(VBString(message)))
		{
			IrmUser irmUser(sender);

			IrmQueryMessageParam param;
			param.m_code = IQNC_PRIV_MSG;
			param.m_sender = irmUser.GetNick();
			param.m_channelName = "";
			param.m_message = message;
			
			m_notify->OnIrmQueryNotify(param);
		}

		else
			cout << "Private CTCP: " << message << endl;
	}
}

/** Channel private message received.
*
*/
void IrmQuery::OnChannelPrivateMessage(LPCSTR sender, LPCSTR channel, LPCSTR message)
{
	if(m_connectionState == IRMCS_CONNECTED)
	{
		if(!IsCtcpMessage(VBString(message)))
		{
			IrmUser irmUser(sender);

			IrmQueryMessageParam param;
			param.m_code = IQNC_CHANNEL_PRIV_MSG;
			param.m_sender = irmUser.GetNick();
			param.m_channelName = RemoveChannelPrefix(channel);
			param.m_message = message;
			
			m_notify->OnIrmQueryNotify(param);
		}

		else
		{
			VoiceRequestCtcpMessage voiceRequestMessage(message);

			if(voiceRequestMessage.IsVoiceRequestCtcpMessage())
			{
				IrmQueryUserParam param;

				if(voiceRequestMessage.IsRequest())
					param.m_code = IQNC_VOICE_REQUEST;
				else if(voiceRequestMessage.IsCancel())
					param.m_code = IQNC_VOICE_REQUEST_CANCEL;
				else if(voiceRequestMessage.IsRemoveAll())
					param.m_code = IQNC_VOICE_REQUEST_REMOVE_ALL;
				else
					param.m_code = IQNC_INVALID_CODE;

				param.m_channelName = RemoveChannelPrefix(channel);
				IrmUser irmUser(sender);
				param.m_userName = irmUser.GetNick();
				
				m_notify->OnIrmQueryNotify(param);
			}

			cout << "Channel CTCP: " << message << endl;
		}
	}
}

//---------------------------------------------------------------------------------------------
// Media events
//---------------------------------------------------------------------------------------------

/** Media.
*
*/
void IrmQuery::OnMedia(long mediaId, LPCSTR mediaClass, LPCSTR channel, LPCSTR user, LPCSTR multicastIp, long multiastPort, long bitRate)
{
	VBString strMediaClass = mediaClass;

	if(m_connectionState == IRMCS_CONNECTED)
	{
		if(IsMediaClassConference(strMediaClass))
		{
			std::cout << "Media Conference connected to server" << std::endl;
		}

		else if(IsMediaClassExecutiveTv(strMediaClass))
		{
			std::cout << "Media Executive TV connected to server" << std::endl;
		}
	}

	if(m_viewerState == IRMVS_INITIALIZED)
	{
		if(IsMediaClassConference(strMediaClass))
		{
			IrmQueryMediaParam param;

			param.m_code = IQNC_MEDIA_VIEWER_READY;
			param.m_mediaId = mediaId;
			param.m_mediaClass = mediaClass;
			param.m_channelName = RemoveChannelPrefix(channel);
			param.m_userNickName = user;
			param.m_host = multicastIp;
			param.m_port = multiastPort;
			param.m_netProtocol = "MCAST";
			
			m_notify->OnIrmQueryNotify(param);

			param.m_code = IQNC_NEW_MEDIA_TRANSMISSION;
			param.m_mediaId = mediaId;
			param.m_mediaClass = mediaClass;
			param.m_channelName = RemoveChannelPrefix(channel);
			param.m_userNickName = user;
			
			m_notify->OnIrmQueryNotify(param);
		}

		else if(IsMediaClassExecutiveTv(strMediaClass))
		{
			std::cout << "Executive TV viewer" << std::endl;
		}	
	}
}

/** Media notify.
*
*/
void IrmQuery::OnMediaNotify(LPCSTR notification, long mediaId, LPCSTR mediaClass, LPCSTR channel, LPCSTR user)
{
	LogManager::Instance()->AddLogHeader("Begin OnMediaNotify");
	VBString message = "Notification = ";
	message += notification;
	LogManager::Instance()->AddLogEntry(message);
	message = "Media ID = ";
	message += mediaId;
	LogManager::Instance()->AddLogEntry(message);
	message = "Media class = ";
	message += mediaClass;
	LogManager::Instance()->AddLogEntry(message);
	message = "Channel = ";
	message += RemoveChannelPrefix(channel);
	LogManager::Instance()->AddLogEntry(message);
	message = "User = ";
	message += user;
	LogManager::Instance()->AddLogEntry(message);

	//if(m_connectionState == IRMCS_CONNECTED)
	{
		VBString strNotification = notification;

		if(IsMediaClassExecutiveTv(mediaClass))
		{
			//LogManager::Instance()->AddLogEntry("Media class = Executive TV");
			//VBString netProtocol = IRM_NET_PROTOCOL_TCP;
			//unsigned port = 0;

			//IrmParam irmMediaId(mediaId);
			//IrmParam irmNetProtocol(netProtocol.c_str());
			//IrmParam irmPort(port);

			//m_IrmStubProxy->SendIrmMessage(MEDIARCV, irmMediaId, irmNetProtocol, irmPort);

			//LogManager::Instance()->AddLogEntry("MEDIARCV sent");
		}
		
		if(IsMediaClassConference(mediaClass))
		{
			if(strNotification == IRM_MEDIANOTIFY_MEDIASND)
			{
				IrmQueryMediaParam param;

				param.m_code = IQNC_NEW_MEDIA_TRANSMISSION;
				param.m_mediaId = mediaId;
				param.m_mediaClass = mediaClass;
				param.m_channelName = RemoveChannelPrefix(channel);
				param.m_userNickName = user;
				
				m_notify->OnIrmQueryNotify(param);
			}
		}
	}

	LogManager::Instance()->AddLogHeader("End OnMediaNotify");
}

/** Media receive all.
*
*/
void IrmQuery::OnMediaReceiveAll(LPCSTR channel, LPCSTR mediaClass, long port, LPCSTR multiCastIp)
{
	if(m_connectionState == IRMCS_CONNECTED)
	{
		VBString mClass = mediaClass;
		if(mClass != "AVCONF")
			return;

		VBString mCastIp = multiCastIp;
		VBString netProtocol;
		VBString host;
		unsigned connectionPort;

		if(mCastIp != "")
		{
			// Since the server returned a valid multi cast ip address, the multi cast protocol must be used.
			netProtocol = IRM_NET_PROTOCOL_MCAST;
			host = mCastIp;
			connectionPort = unsigned(port);
		}

		else
		{
			if(GetUdpListeningState() == IRMULS_NOT_AVAILABLE)
			{
				// The TCP protocol must be used because UDP listening is not available.
				netProtocol = IRM_NET_PROTOCOL_TCP;
				host = m_connection.GetHost();
				connectionPort = unsigned(port);
			}

			else
			{
				// The UDP protocol must be used because UDP listening is available.
				netProtocol = IRM_NET_PROTOCOL_UDP;
				host = "";
				// For now we're only dealing with AVCONF media class, so
				// the port is always set to the AVCONF port.
				// Probably the port should be set by the module that implements the
				// media class in question.
				connectionPort = IRM_AVCONF_UDP_PORT;
			}
		}

		IrmQueryMediaParam param;
		param.m_code = IQNC_MEDIA_CONFERENCE_READY;
		param.m_userNickName = m_user.GetNick();
		param.m_channelName = RemoveChannelPrefix(channel);
		param.m_mediaClass = mediaClass;
		param.m_netProtocol = netProtocol;
		param.m_host = host;
		param.m_port = connectionPort;
		
		m_notify->OnIrmQueryNotify(param);

		IrmParam irmChannel(RemoveChannelPrefix(channel));
		IrmParam irmMediaClass(mediaClass);
		IrmParam irmNick(m_user.GetNick());

		if(netProtocol != IRM_NET_PROTOCOL_TCP)
			m_IrmStubProxy->SendIrmMessage(MEDIARCVALLRUN, irmChannel, irmMediaClass, irmNick);
	}
}

/** Unknown message.
*
*/
void IrmQuery::OnUnknownMessage(long messageId)
{
	std::cout << "Unknown message. ID = " << messageId << std::endl;
}

bool IrmQuery::IsMediaClassConference(VBString mediaClass)
{
	if(mediaClass == IRM_MEDIACLASS_AVCONF)
		return true;

	return false;
}

bool IrmQuery::IsMediaClassExecutiveTv(VBString mediaClass)
{
	if(mediaClass == IRM_MEDIACLASS_SIMPOSIUM_VIDEO)
		return true;

	if(mediaClass == IRM_MEDIACLASS_SIMPOSIUM_SCREEN)
		return true;

	if(mediaClass == IRM_MEDIACLASS_SIMPOSIUM_MOVIE)
		return true;

	return false;
}

bool IrmQuery::IsMediaClassAppSharing(VBString mediaClass)
{
	if(mediaClass == IRM_MEDIACLASS_APP)
		return true;

	return false;
}

VBString IrmQuery::AddChannelPrefix(VBString channel)
{
	if(channel.getChar(0) != IRM_CHANNEL_PREFIX)
	{
		VBString channelPrefix;
		channelPrefix = IRM_CHANNEL_PREFIX;
		channel = channelPrefix + channel;
	}

	return channel;
}

VBString IrmQuery::RemoveChannelPrefix(VBString channel)
{
	if(channel.getChar(0) == IRM_CHANNEL_PREFIX)
		channel = channel.strAfterPosition(1);

	return channel;
}

bool IrmQuery::IsChannel(VBString str)
{
	if(str.getChar(0) == IRM_CHANNEL_PREFIX)
		return true;

	return false;
}
