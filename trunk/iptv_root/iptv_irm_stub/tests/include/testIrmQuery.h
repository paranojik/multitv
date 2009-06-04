#ifndef TEST_IRM_QUERY_H
#define TEST_IRM_QUERY_H

#include "shared/compat.h"
#include "chat/nOpCodes.h"
#include "stub/IrmStubDll.h"

class TestIrmQuery : public IrmStubDataSink
{
private:
	// Atributes ----------------------------------------------------------------------------------

	// IRM Control Object
	ProxyIrmStub	m_IrmStubProxy;

	// Status flags
	bool m_connected;
	bool m_connecting;

	const char * m_user;
	const char * m_passwd;
	const char * m_server;
	int m_serverPort;

	// Connection data
	/*IrmParam *m_user,
			 *m_port,
			 *m_server,
			 *m_email,
			 *m_realName,
			 *m_clientVersion,
			 *m_pass;*/

	// Private Methods ----------------------------------------------------------------------------
	void Fire_OnMessage ( long _cmd, IrmParam& _source, IrmParam& _target, IrmParam& _data,
						  IrmParam& _compl1, IrmParam& _compl2, IrmParam& _compl3,
						  IrmParam& _compl4, IrmParam& _compl5, IrmParam& _compl6,
						  IrmParam& _compl7 );

public:
	// Public Methods -----------------------------------------------------------------------------
	TestIrmQuery();
	virtual ~TestIrmQuery();

	bool Connect (const char *user, const char * passwd, const char *server, unsigned port);
	void Disconnect();
	void Authenticate(const char *pass);
	void GetChannelList();
	void GetChannelList(const char *channel);
	void JoinChannel(const char *channel);
	void PartChannel(const char *channel);
	void GetUserList(const char *channel);
	void SendPrivateMessage(const char *receiver, const char *message);

	void OnConnectedHost();
	void OnConnectedChat();
	void OnDisconnectedChat();
	void OnAppPing();
	void OnIdentificationRequest();
	void OnIdentify();
	void OnIdentifyError();
	void OnCmdError(LPCSTR message);
	void OnChannelQueryItem(LPCSTR szName, LPCSTR nUsers, LPCSTR szTopic);
	void OnEndChannelList();
	void OnJoinChannel(LPCSTR channel, LPCSTR user);
	void OnPartChannel(LPCSTR channel, LPCSTR user);
	void OnGetUserList(LPCSTR channel, LPCSTR user);
	void OnEndUserList(LPCSTR channel);
	void OnPrivateMessage(LPCSTR message);
};

#endif
