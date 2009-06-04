#ifndef PROFILE_H
#define PROFILE_H

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/xml/xml.h>

/** @brief Contains an application profile. These are managed by SettingsManager.
 *
 */
class Profile
{
private:
	wxString m_name;
	wxString m_userName;
	wxString m_userPassword;
	wxString m_serverName;
	wxString m_serverPassword;
	wxString m_channelName;
	wxString m_channelPassword;

public:
	Profile();
	Profile	(
			wxString profileName,
			wxString userName,
			wxString userPassword,
			wxString serverName,
			wxString serverPassword,
			wxString channelName,
			wxString channelPassword
			);
	virtual ~Profile();

	void SetName(const wxString &profileName);
	void SetUserName(const wxString &userName);
	void SetUserPassword(const wxString &userPassword);
	void SetServerName(const wxString &serverName);
	void SetServerPassword(const wxString &serverPassword);
	void SetChannelName(const wxString &channelName);
	void SetChannelPassword(const wxString &channelPassword);

	const wxString &GetName();
	const wxString &GetUserName();
	const wxString &GetUserPassword();
	const wxString &GetServerName();
	const wxString &GetServerPassword();
	const wxString &GetChannelName();
	const wxString &GetChannelPassword();
	const wxString GetXmlString();
	void ExportToXml(const wxString &path);
	bool ImportFromXml(const wxString &path);

};

#endif
