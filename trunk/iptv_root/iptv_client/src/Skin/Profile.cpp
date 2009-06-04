#include "Profile.h"

Profile::Profile()
{
}

Profile::Profile(
				wxString profileName,
				wxString userName,
				wxString userPassword,
				wxString serverName,
				wxString serverPassword,
				wxString channelName,
				wxString channelPassword
				)
{
	m_name = profileName;
	m_userName = userName;
	m_userPassword = userPassword;
	m_serverName = serverName;
	m_serverPassword = serverPassword;
	m_channelName = channelName;
	m_channelPassword = channelPassword;
}

Profile::~Profile()
{
}

void Profile::SetName(const wxString &profileName)
{
	m_name = profileName;
}

void Profile::SetUserName(const wxString &userName)
{
	m_userName = userName;
}

void Profile::SetUserPassword(const wxString &userPassword)
{
	m_userPassword = userPassword;
}

void Profile::SetServerName(const wxString &serverName)
{
	m_serverName = serverName;
}

void Profile::SetServerPassword(const wxString &serverPassword)
{
	m_serverPassword = serverPassword;
}

void Profile::SetChannelName(const wxString &channelName)
{
	m_channelName = channelName;
}

void Profile::SetChannelPassword(const wxString &channelPassword)
{
	m_channelPassword = channelPassword;
}

const wxString &Profile::GetName()
{
	return m_name;
}

const wxString &Profile::GetUserName()
{
	return m_userName;
}

const wxString &Profile::GetUserPassword()
{
	return m_userPassword;
}

const wxString &Profile::GetServerName()
{
	return m_serverName;
}

const wxString &Profile::GetServerPassword()
{
	return m_serverPassword;
}

const wxString &Profile::GetChannelName()
{
	return m_channelName;
}

const wxString &Profile::GetChannelPassword()
{
	return m_channelPassword;
}

const wxString Profile::GetXmlString()
{
	wxString string;

	string = wxT("  <profile>\n");
	string += wxT("    <profileName>") + m_name + wxT("</profileName>\n");
	string += wxT("    <userName>") + m_userName + wxT("</userName>\n");
	string += wxT("    <userPassword>") + m_userPassword + wxT("</userPassword>\n");
	string += wxT("    <serverName>") + m_serverName + wxT("</serverName>\n");
	string += wxT("    <serverPassword>") + m_serverPassword + wxT("</serverPassword>\n");
	string += wxT("    <channelName>") + m_channelName + wxT("</channelName>\n");
	string += wxT("    <channelPassword>") + m_channelPassword + wxT("</channelPassword>\n");
	string += wxT("  </profile>\n");

	return string;
}

void Profile::ExportToXml(const wxString &path)
{
	// write xml in a string
	wxString string;
	string = wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	string += GetXmlString();

	// write profile in specified file
	wxFile file(path, wxFile::write);
	file.Write(string);
}

bool Profile::ImportFromXml(const wxString &path)
{
	if(!wxFile::Exists(path))
		return false;

	wxXmlDocument profile;
	if(!profile.Load(path))
		return false;

	wxXmlNode *node = profile.GetRoot();

	if (node->GetName() == wxT("profile"))
	{
		wxXmlNode *profileSetting = node->GetChildren();

		// Load all the profile settings
		bool profileNameLoaded = false;
		bool userNameLoaded = false;
		bool userPasswordLoaded = false;
		bool serverNameLoaded = false;
		bool serverPasswordLoaded = false;
		bool channelNameLoaded = false;
		bool channelPasswordLoaded = false;
		while(profileSetting)
		{
			if(profileSetting->GetName() == wxT("profileName"))
			{
				m_name = profileSetting->GetNodeContent();
				profileNameLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("userName"))
			{
				m_userName = profileSetting->GetNodeContent();
				userNameLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("userPassword"))
			{
				m_userPassword = profileSetting->GetNodeContent();
				userPasswordLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("serverName"))
			{
				m_serverName = profileSetting->GetNodeContent();
				serverNameLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("serverPassword"))
			{
				m_serverPassword = profileSetting->GetNodeContent();
				serverPasswordLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("channelName"))
			{
				m_channelName = profileSetting->GetNodeContent();
				channelNameLoaded = true;
			}
			else if(profileSetting->GetName() == wxT("channelPassword"))
			{
				m_channelPassword = profileSetting->GetNodeContent();
				channelPasswordLoaded = true;
			}
			profileSetting = profileSetting->GetNext();
		}

		// if all the settings could be loaded then return true
		if( profileNameLoaded && userNameLoaded &&
			userPasswordLoaded && serverNameLoaded &&
			serverPasswordLoaded && channelNameLoaded &&
			channelPasswordLoaded)
		{
			return true;
		}
	}

	return false;
}
