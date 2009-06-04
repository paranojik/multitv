#include "SettingsManager.h"
#include "wx/dir.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ProfileArray);


SettingsManager::SettingsManager()
{
	m_initialized = false;
	m_latestProfile = false;
	m_selectedProfileName = wxT("Latest");
	m_transmissionProfile = wxT("128 kbps");
	m_videoDevice = wxT("0");
	m_audioDevice = wxT("0");
	m_audioCodec = wxT("speex_hi");
	m_videoCodec = wxT("h264");
	m_recordingStatus = wxT("0");
	m_recordingPath = wxGetCwd();
}

SettingsManager::~SettingsManager()
{
}

bool SettingsManager::Initialize()
{
	if(wxFile::Exists(SETTINGS_FILENAME))
		if(m_settings.Load(SETTINGS_FILENAME))
			if (m_settings.GetRoot()->GetName() == wxT("iptv"))
			{
				// load program configurations
				if(!LoadConfiguration())
					return false;

				// load profiles list and cheks if a Latest profile exists
				LoadProfilesList();
				if(!m_latestProfile)
					return false;

				// load the last profile
				if(!SelectProfile(GetSelectedProfileName()))
					return false;

				m_initialized = true;
				return true;
			}

	return false;
}

bool SettingsManager::LoadConfiguration()
{
	wxXmlNode *node = m_settings.GetRoot()->GetChildren();

	// Search for configuration in XML
	while (node)
	{
		if (node->GetName() == wxT("config"))
		{
			wxXmlNode *config = node->GetChildren();

			// Load all the configurations
			bool skinConfigLoaded = false;
			bool languageConfigLoaded = false;
			bool transmissionProfileConfigLoaded = false;
			bool audioDeviceLoaded = false;
			bool audioCodecLoaded = false;
			bool videoDeviceLoaded = false;
			bool videoCodecLoaded = false;
			bool recordingStatusLoaded = false;
			bool recordingPathLoaded = false;
			while(config)
			{
				if(config->GetName() == wxT("skin"))
				{
					m_skin = config->GetNodeContent();
					skinConfigLoaded = true;
				}
				else if(config->GetName() == wxT("language"))
				{
					m_language = config->GetNodeContent();
					languageConfigLoaded = true;
				}
				else if(config->GetName() == wxT("transmissionProfile"))
				{
					m_transmissionProfile = config->GetNodeContent();
					transmissionProfileConfigLoaded = true;
				}
				else if(config->GetName() == wxT("audioDevice"))
				{
					m_audioDevice = config->GetNodeContent();
					audioDeviceLoaded = true;
				}
				else if(config->GetName() == wxT("audioCodec"))
				{
					m_audioCodec = config->GetNodeContent();
					audioCodecLoaded = true;
				}
				else if(config->GetName() == wxT("videoDevice"))
				{
					m_videoDevice = config->GetNodeContent();
					videoDeviceLoaded = true;
				}
				else if(config->GetName() == wxT("videoCodec"))
				{
					m_videoCodec = config->GetNodeContent();
					videoCodecLoaded = true;
				}
				else if(config->GetName() == wxT("recordingEnable"))
				{
					m_recordingStatus = config->GetNodeContent();
					recordingStatusLoaded = true;
				}
				else if(config->GetName() == wxT("recordingPath"))
				{
					if (wxDir::Exists(config->GetNodeContent()))
						m_recordingPath = config->GetNodeContent();
					recordingPathLoaded = true;
				}

				config = config->GetNext();
			}
			// check if all config were found
			if(skinConfigLoaded && languageConfigLoaded)
				return true;
			return false;
		}
		node = node->GetNext();
	}

	return false;
}

wxString SettingsManager::GetSkin()
{
	return m_skin;
}

wxString SettingsManager::GetLanguage()
{
	return m_language;
}

wxString SettingsManager::GetAudioDevice()
{
	return m_audioDevice;
}

wxString SettingsManager::GetVideoDevice()
{
	return m_videoDevice;
}

wxString SettingsManager::GetVideoCodec()
{
	return m_videoCodec;
}

wxString SettingsManager::GetAudioCodec()
{
	return m_audioCodec;
}

wxString SettingsManager::GetTransmissionProfile()
{
	return m_transmissionProfile;
}

wxString SettingsManager::GetRecordingStatus()
{
	return m_recordingStatus;
}

wxString SettingsManager::GetRecordingPath()
{
	return m_recordingPath;
}

wxString SettingsManager::GetSelectedProfileName()
{
	return m_selectedProfileName;
}

void SettingsManager::SetSkin(wxString skinName)
{
	m_skin = skinName;
	SaveXml();
}

void SettingsManager::SetLanguage(wxString language)
{
	m_language = language;
	SaveXml();
}

void SettingsManager::SetAudioDevice(const wxString &audioDevice)
{
	m_audioDevice = audioDevice;
	SaveXml();
}

void SettingsManager::SetVideoDevice(const wxString &videoDevice)
{
	m_videoDevice = videoDevice;
	SaveXml();
}

void SettingsManager::SetVideoCodec(const wxString &videoCodec)
{
	m_videoCodec = videoCodec;
	SaveXml();
}

void SettingsManager::SetAudioCodec(const wxString &audioCodec)
{
	m_audioCodec = audioCodec;
	SaveXml();
}

void SettingsManager::SetTransmissionProfile(const wxString &transmissionProfile)
{
	m_transmissionProfile = transmissionProfile;
	SaveXml();
}

void SettingsManager::SetRecordingStatus(const wxString &recordingStatus)
{
	m_recordingStatus = recordingStatus;
	SaveXml();
}

void SettingsManager::SetRecordingPath(const wxString &recordingPath)
{
	m_recordingPath = recordingPath;
	SaveXml();
}

void SettingsManager::SetProfile(wxString profileName)
{
	if(SelectProfile(profileName))
	{
		m_selectedProfileName = profileName;
	}
}

void SettingsManager::LoadProfilesList()
{
	wxXmlNode *node = m_settings.GetRoot()->GetChildren();

	// Search all the profiles in XML
	while (node)
	{
		if (node->GetName() == wxT("profile"))
		{
			wxXmlNode *profileSetting = node->GetChildren();

			// auxiliary strings
			wxString profileName;
			wxString userName;
			wxString userPassword;
			wxString serverName;
			wxString serverPassword;
			wxString channelName;
			wxString channelPassword;

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
					profileName = profileSetting->GetNodeContent();
					profileNameLoaded = true;
					if(profileName == wxT("Latest"))
						m_latestProfile = true;
				}
				else if(profileSetting->GetName() == wxT("userName"))
				{
					userName = profileSetting->GetNodeContent();
					userNameLoaded = true;
				}
				else if(profileSetting->GetName() == wxT("userPassword"))
				{
					userPassword = profileSetting->GetNodeContent();
					userPasswordLoaded = true;
				}
				else if(profileSetting->GetName() == wxT("serverName"))
				{
					serverName = profileSetting->GetNodeContent();
					serverNameLoaded = true;
				}
				else if(profileSetting->GetName() == wxT("serverPassword"))
				{
					serverPassword = profileSetting->GetNodeContent();
					serverPasswordLoaded = true;
				}
				else if(profileSetting->GetName() == wxT("channelName"))
				{
					channelName = profileSetting->GetNodeContent();
					channelNameLoaded = true;
				}
				else if(profileSetting->GetName() == wxT("channelPassword"))
				{
					channelPassword = profileSetting->GetNodeContent();
					channelPasswordLoaded = true;
				}
				profileSetting = profileSetting->GetNext();
			}

			// if all the settings could be loaded then save them
			if( profileNameLoaded && userNameLoaded &&
				userPasswordLoaded && serverNameLoaded &&
				serverPasswordLoaded && channelNameLoaded &&
				channelPasswordLoaded)
			{
				Profile profile = Profile(	profileName,
											userName,
											userPassword,
											serverName,
											serverPassword,
											channelName,
											channelPassword);
				m_profileArray.Add(profile);
			}
		}
		node = node->GetNext();
	}
}

wxArrayString SettingsManager::GetProfilesList()
{
	wxArrayString profileNameList;

	for(int i = 0; i < (int)m_profileArray.Count(); i++)
		profileNameList.Add(m_profileArray[i].GetName());

	return profileNameList;
}

bool SettingsManager::AddProfile(Profile profile)
{
	// check if the profile already exists
	wxArrayString profileNameList = GetProfilesList();
	for(int i = 0; i < (int)profileNameList.Count(); i++)
	{
		if(profileNameList[i] == profile.GetName())
			return false;
	}

	m_profileArray.Add(profile);
	SaveXml();
	return true;
}

bool SettingsManager::ModifyProfile(wxString profileName, Profile profile)
{
	for(int i = 0; i < (int)m_profileArray.Count(); i++)
	{
		if(m_profileArray[i].GetName() == profileName)
		{
			m_profileArray[i] = profile;

			// if modifying the Latest profile, make sure that it keep it's name
			if(profileName == wxT("Latest"))
				m_profileArray[i].SetName(wxT("Latest"));

			SaveXml();
			return true;
		}
	}

	return false;
}

void SettingsManager::DeleteProfile(wxString profileName)
{
	// cannot delete Latest
	if(profileName == wxT("Latest"))
		return;

	for(int i = 0; i < (int)m_profileArray.Count(); i++)
	{
		if(m_profileArray[i].GetName() == profileName)
		{
			m_profileArray.RemoveAt(i);
			SaveXml();
			return;
		}
	}
}

bool SettingsManager::SelectProfile(wxString profileName)
{
	for(int i = 0; i < (int)m_profileArray.Count(); i++)
	{
		if(m_profileArray[i].GetName() == profileName)
		{
			m_selectedProfile = m_profileArray[i];
			return true;
		}
	}

	return false;
}

Profile SettingsManager::GetSelectedProfile()
{
	return m_selectedProfile;
}

Profile SettingsManager::GetProfile(wxString profileName)
{
	Profile profile;

	for(int i = 0; i < (int)m_profileArray.Count(); i++)
	{
		if(m_profileArray[i].GetName() == profileName)
		{
			profile = m_profileArray[i];
			return(profile);
		}
	}

	return(profile);
}

wxString SettingsManager::GetConfigXmlString()
{
	wxString string;

	string = wxT("  <config>\n");
	string += wxT("    <skin>") + m_skin + wxT("</skin>\n");
	string += wxT("    <language>") + m_language + wxT("</language>\n");
	string += wxT("    <transmissionProfile>") + m_transmissionProfile + wxT("</transmissionProfile>\n");
	string += wxT("    <audioDevice>") + m_audioDevice + wxT("</audioDevice>\n");
	string += wxT("    <audioCodec>") + m_audioCodec + wxT("</audioCodec>\n");
	string += wxT("    <videoDevice>") + m_videoDevice + wxT("</videoDevice>\n");
	string += wxT("    <videoCodec>") + m_videoCodec + wxT("</videoCodec>\n");
	string += wxT("    <recordingEnable>") + m_recordingStatus + wxT("</recordingEnable>\n");
	string += wxT("    <recordingPath>") + m_recordingPath + wxT("</recordingPath>\n");
	string += wxT("    <profile>") + m_selectedProfileName + wxT("</profile>\n");
	string += wxT("  </config>\n");

	return string;
}

void SettingsManager::SaveXml()
{
	wxString string;

	string = wxT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<iptv>\n");
	string += GetConfigXmlString();
	for(int i = 0; i < (int)m_profileArray.Count(); i++)
	{
		string += m_profileArray[i].GetXmlString();
	}
	string += wxT("</iptv>");

	wxFile file(SETTINGS_FILENAME, wxFile::write);
	file.Write(string);
}
