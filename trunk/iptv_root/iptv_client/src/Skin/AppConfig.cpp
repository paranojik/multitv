#include <wx/wx.h>

#include "AppConfig.h"

/** Default constructor.
 */
AppConfig::AppConfig()
{
	m_mode = APP_MODE_NONE;
	// Conference mode parameters
	m_cfServer = wxEmptyString;
	m_cfServerPass = wxEmptyString;
	m_cfUser = wxEmptyString;
	m_cfPass = wxEmptyString;
	m_cfRealName = wxEmptyString;
	m_cfEmail = wxEmptyString;
	// Multicast mode parameters
	m_mcAddress = wxEmptyString;
	m_mcPort = 0;
	// File mode parameters
	m_fpFileName = wxEmptyString;
}

/** Returns the current application mode.
 *
 * This function returns the current application mode.
 * @return Returns one of: @n
 * @b APP_MODE_NONE				Idle/not connected.@n
 * @b APP_MODE_CONFERENCE		Unicast conference mode.@n
 * @b APP_MODE_MULTICAST		Multicast viewer mode.@n
 * @b APP_MODE_FILE				Conference file playback.@n
 * @b APP_MODE_INVALID			Invalid mode.@n
 */
int AppConfig::GetMode() const
{
	return m_mode;
}

/** Returns address of the server for CONFERENCE mode.
 * @return The server address (DNS or IPv4).
 */
const wxString &AppConfig::GetConfServer() const
{
	return m_cfServer;
}

/** Returns server password for CONFERENCE mode.
 * @return The server password.
 */
const wxString &AppConfig::GetConfServerPass() const
{
	return m_cfServerPass;
}

/** Returns user name for CONFERENCE mode.
 * @return The user name.
 */
const wxString &AppConfig::GetConfUser() const
{
	return m_cfUser;
}

/** Returns user password for CONFERENCE mode.
 * @return The user password.
 */
const wxString &AppConfig::GetConfPass() const
{
	return m_cfPass;
}

/** Returns user full name for CONFERENCE mode.
 * @return The user's full name.
 * @remarks Currently not used.
 */
const wxString &AppConfig::GetConfRealName() const
{
	return m_cfRealName;
}

/** Returns user's email address for CONFERENCE mode.
 * @return The user's email address.
 * @remarks Currently not used.
 */
const wxString &AppConfig::GetConfEmail() const
{
	return m_cfEmail;
}

/** Returns listen address for MULTICAST mode.
 * @return The listen address (IPv4).
 */
const wxString &AppConfig::GetMcastAddress() const
{
	return m_mcAddress;
}

/** Returns listen port for MULTICAST mode.
 * @return The listen port number.
 */
unsigned AppConfig::GetMcastPort() const
{
	return m_mcPort;
}

/** Returns file name for FILE mode.
 * @return The file name.
 */
const wxString &AppConfig::GetFileName() const
{
	return m_fpFileName;
}

/** Set application mode.
 * @param[in] mode	App mode to set.
 */
void AppConfig::SetMode(int mode)
{
	if((mode < APP_MODE_NONE) || (mode >= APP_MODE_INVALID))
		m_mode = APP_MODE_INVALID;
	else
		m_mode = mode;
}

/** Set CONFERENCE server.
 * @param[in] server	Server address (DNS or IPv4)
 */
void AppConfig::SetConfServer(const wxString &server)
{
	m_cfServer = server;
}

/** Set CONFERENCE server password.
 * @param[in] pass	Server password.
 */
void AppConfig::SetConfServerPass(const wxString &pass)
{
	m_cfServerPass = pass;
}

/** Set CONFERENCE user nickname.
 * @param[in] user	User nickname.
 */
void AppConfig::SetConfUser(const wxString &user)
{
	m_cfUser = user;
}

/** Set CONFERENCE user password.
 * @param[in] pass	User password.
 */
void AppConfig::SetConfPass(const wxString &pass)
{
	m_cfPass = pass;
}

/** Set CONFERENCE user real name.
 * @param[in] name	User real name.
 * @remarks Currently not used.
 */
void AppConfig::SetConfRealName(const wxString &name)
{
	m_cfRealName = name;
}

/** Set CONFERENCE user email.
 * @param[in] email	Email to set.
 * @remarks Currently not used.
 */
void AppConfig::SetConfEmail(const wxString &email)
{
	m_cfEmail = email;
}

/** Set MULTICAST listen address.
 * @param[in] address	IPv4 address to listen on.
 */
void AppConfig::SetMcastAddress(const wxString &address)
{
	m_mcAddress = address;
}

/** Set MULTICAST listen port.
 * @param[in] port	UDP port to listen on.
 */
void AppConfig::SetMcastPort(unsigned port)
{
	m_mcPort = port;
}

/** Set FILE name.
 * @param[in] filename	Full path to the file.
 */
void AppConfig::SetFileName(const wxString &filename)
{
	m_fpFileName = filename;
}
