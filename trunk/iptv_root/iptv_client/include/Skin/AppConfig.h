#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <wx/wx.h>

enum
{
	APP_MODE_NONE = 0,
	APP_MODE_CONFERENCE,
	APP_MODE_MULTICAST,
	APP_MODE_FILE,
	APP_MODE_INVALID
};

/** @brief Stores the running parameters of the application.
 *
 * This class stores the running parameters of the application, comprised
 * of user and connection information. It also keeps track of the current
 * application connection mode: CONFERENCE (unicast video-conference),
 * MULTICAST (multicast viewer) and FILE (conference file playback).
 * The get/set methods are categorized: Conf (conference), Mcast(multicast)
 * and File (file playback).
 */
class AppConfig
{
private:
	int m_mode;
	// Conference mode parameters
	wxString m_cfServer;
	wxString m_cfServerPass;
	wxString m_cfUser;
	wxString m_cfPass;
	wxString m_cfRealName;
	wxString m_cfEmail;
	// Multicast mode parameters
	wxString m_mcAddress;
	unsigned m_mcPort;
	// File mode parameters
	wxString m_fpFileName;
public:
	AppConfig();

	int GetMode() const;
	const wxString &GetConfServer() const;
	const wxString &GetConfServerPass() const;
	const wxString &GetConfUser() const;
	const wxString &GetConfPass() const;
	const wxString &GetConfRealName() const;
	const wxString &GetConfEmail() const;

	const wxString &GetMcastAddress() const;
	unsigned GetMcastPort() const;

	const wxString &GetFileName() const;

	void SetMode(int mode);
	void SetConfServer(const wxString &server);
	void SetConfServerPass(const wxString &pass);
	void SetConfUser(const wxString &user);
	void SetConfPass(const wxString &pass);
	void SetConfRealName(const wxString &name);
	void SetConfEmail(const wxString &email);

	void SetMcastAddress(const wxString &address);
	void SetMcastPort(unsigned port);

	void SetFileName(const wxString &filename);
};

#endif // APPCONFIG_H
