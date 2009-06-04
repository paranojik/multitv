#include <iostream>
#include "testIrmQuery.h"

using namespace std;

TestIrmQuery::TestIrmQuery()
{
	m_connected = false;
	m_connecting = false;

	m_user = NULL;
	m_passwd = NULL;
	m_server = NULL;
	m_serverPort = -1;

	// Initialize IRMStub. Abort If failed.
	if ( !m_IrmStubProxy.Initialize(this) )
	{
		cout << "Cannot initialize network." << endl;
	}
}

TestIrmQuery::~TestIrmQuery()
{
}

bool TestIrmQuery::Connect(const char *user, const char * passwd, const char *server, unsigned port)
{
	IrmParam pServer(server);
	IrmParam pPort(port);
	IrmParam pUser(user);
	IrmParam pEmail("---");
    IrmParam pRealName("MultiTV Shell Client");
	IrmParam pClientVersion("1.0.0.0");
	IrmParam pPass(passwd);

	if ( !m_connected && !m_connecting )
	{
		if ( m_IrmStubProxy.Connect( pServer,
									 pPort,
									 pUser,
									 pEmail,
									 pRealName,
									 pClientVersion,
									 pPass ))
		{
			m_user = user;
			m_passwd = passwd;
			m_server = server;
			m_serverPort = port;

			m_connected	= false;
			m_connecting = true;

			return true;
		}
	}
	return false;
}

void TestIrmQuery::Disconnect()
{
	if ( m_connected || m_connecting )
	{
		m_connected	= false;
		m_connecting	= false;
		m_IrmStubProxy.Disconnect();

		//wxMutexLocker lock(m_mutex_request);
		//if ( lock.IsOk() )
		//	m_Queue_Request.clear();
		//else
		//	this->Answer_push( Answer_Error,
		//					   wxT("Erro ao desconectar.") );
	}
}

void TestIrmQuery::Fire_OnMessage( long _cmd, IrmParam& _source, IrmParam& _target, IrmParam& _data,
							  IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3,
							  IrmParam& _compl4, IrmParam& _compl5, IrmParam& _compl6,
							  IrmParam& _compl7)
{
	LPCSTR message = _compl7.GetString();

	switch (_cmd)
	{
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
	case APP_PING:
		OnAppPing();
		break;
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
		OnIdentifyError();
		break;
	//---------------------------------------------------------------------------------------------
	case CMD_ERROR:
		OnCmdError(message);
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIASTATS:
		//OnMediaStats( message );
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIAQUERY:
		//OnMediaQuery( message );
		break;
	//---------------------------------------------------------------------------------------------
	case MEDIAQUERYEND:
		//OnMediaQueryEnd();
		break;
	//---------------------------------------------------------------------------------------------
	case PRIVMSG:
		OnPrivateMessage(_data.GetString());
		break;
	//---------------------------------------------------------------------------------------------
	case GET_CHANNEL_LIST:
        OnChannelQueryItem(_target.GetString(), _compl1.GetString(), _data.GetString());
        break;
	//---------------------------------------------------------------------------------------------
	case ENDOF_CHANNEL_LIST :
        OnEndChannelList();
        break;
	//---------------------------------------------------------------------------------------------
	case JOIN_CHANNEL :
        OnJoinChannel(_target.GetString(), _source.GetString());
        break;
	//---------------------------------------------------------------------------------------------
	case PART_CHANNEL :
        OnPartChannel(_target.GetString(), _source.GetString());
        break;
	//---------------------------------------------------------------------------------------------
	case GET_USER_LIST :
        OnGetUserList(_source.GetString(), _data.GetString());
        break;
	//---------------------------------------------------------------------------------------------
	case ENDOF_USER_LIST :
        OnEndUserList(_source.GetString());
        break;
	//---------------------------------------------------------------------------------------------
	}
}

void TestIrmQuery::Authenticate(const char *pass)
{
	IrmParam pPass(pass);

	m_IrmStubProxy.SendIrmMessage(IDENTIFY, pPass);
}

void TestIrmQuery::GetChannelList()
{
	m_IrmStubProxy.SendIrmMessage(GET_CHANNEL_LIST);
}

void TestIrmQuery::GetChannelList(const char *channel)
{
	IrmParam pChannel(channel);

	m_IrmStubProxy.SendIrmMessage(GET_CHANNEL_LIST, pChannel);
}

void TestIrmQuery::JoinChannel(const char *channel)
{
	IrmParam pChannel(channel);

	m_IrmStubProxy.SendIrmMessage(JOIN_CHANNEL, pChannel);
}

void TestIrmQuery::PartChannel(const char *channel)
{
	IrmParam pChannel(channel);

	m_IrmStubProxy.SendIrmMessage(PART_CHANNEL, pChannel);
}

void TestIrmQuery::GetUserList(const char *channel)
{
	IrmParam pChannel(channel);

	m_IrmStubProxy.SendIrmMessage(GET_USER_LIST, pChannel);
}

void TestIrmQuery::SendPrivateMessage(const char *receiver, const char *message)
{
	IrmParam pReceiver(receiver);
	IrmParam pMessage(message);

	m_IrmStubProxy.SendIrmMessage(PRIVMSG, pReceiver, pMessage);
}

// =============================== EVENT HANDLERS ===============================

void TestIrmQuery::OnConnectedHost()
{
	cout << "Connected to host." << endl;
}

void TestIrmQuery::OnConnectedChat()
{
	cout << "Connected to chat." << endl;
}

void TestIrmQuery::OnDisconnectedChat()
{
	cout << "Disconnected from chat." << endl;
}

void TestIrmQuery::OnAppPing()
{
	cout << "Application ping." << endl;
	m_IrmStubProxy.AppPong();
	cout << "Send application pong." << endl;
}

void TestIrmQuery::OnIdentificationRequest()
{
	cout << "Identification requested (Authentication requested)." << endl;

	Authenticate(m_passwd);
	cout << "Authenticating ..." << endl;
}

void TestIrmQuery::OnIdentify()
{
	cout << "Authenticated." << endl;
}

void TestIrmQuery::OnIdentifyError()
{
	cout << "Authentication failed." << endl;
}

void TestIrmQuery::OnCmdError(LPCSTR message)
{
	cout << "Command error. Message: " << message << endl;
}

void TestIrmQuery::OnChannelQueryItem(LPCSTR szName, LPCSTR nUsers, LPCSTR szTopic)
{
	//cout << "\n***BEGIN CHANNEL ITEM***\n";
	cout << "* channel: " << szName << " | " << nUsers << " users | topic: " << szTopic << endl;
	//cout << "\n***END CHANNEL ITEM***\n";
}

void TestIrmQuery::OnEndChannelList()
{
	cout << "\n***END OF CHANNEL LIST***\n";
}

void TestIrmQuery::OnJoinChannel(LPCSTR channel, LPCSTR user)
{
	cout << "User " << user << " has joined channel " << channel << "." << endl;
}

void TestIrmQuery::OnPartChannel(LPCSTR channel, LPCSTR user)
{
	cout << "User " << user << " has left channel " << channel << "." << endl;
}

void TestIrmQuery::OnGetUserList(LPCSTR channel, LPCSTR user)
{
	cout << "user: " << user << endl << "channel: " << channel << endl;
}

void TestIrmQuery::OnEndUserList(LPCSTR channel)
{
	cout << "\n***END OF USER LIST***\n";
	cout << "channel = " << channel << endl;
}

void TestIrmQuery::OnPrivateMessage(LPCSTR message)
{
	cout << message << endl;
}
