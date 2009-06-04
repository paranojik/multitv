#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/xml/xml.h>
#include <wx/dynarray.h>

#include "Constants.h"
#include "Profile.h"

WX_DECLARE_OBJARRAY(Profile, ProfileArray);

/** @brief Manages application profiles.
 *
 */
class SettingsManager
{
private:
	static SettingsManager *m_instance;
	wxXmlDocument m_settings;
	bool m_initialized;
	bool m_latestProfile;
	Profile m_selectedProfile;			// current profile
	ProfileArray m_profileArray;

	wxString m_skin;					// last skin used
	wxString m_language;				// last language used
	wxString m_audioDevice;				// last audio device used
	wxString m_videoDevice;				// last video device used
	wxString m_videoCodec;
	wxString m_audioCodec;
	wxString m_transmissionProfile;
	wxString m_recordingStatus;         //Enable or disable recording
	wxString m_recordingPath;			//Recording path
	wxString m_selectedProfileName;		// selected profile

	void LoadProfilesList();
	bool LoadConfiguration();
	bool SelectProfile(wxString profileName);
	wxString GetConfigXmlString();
	void SaveXml();

public:
	SettingsManager();
	virtual ~SettingsManager();
	bool Initialize();

	wxString GetSkin();
	wxString GetLanguage();
	wxString GetSelectedProfileName();
	wxString GetAudioDevice();
	wxString GetVideoDevice();
	wxString GetVideoCodec();
	wxString GetAudioCodec();
	wxString GetTransmissionProfile();
	wxString GetRecordingStatus();
	wxString GetRecordingPath();
	Profile GetSelectedProfile();
	Profile GetProfile(wxString profileName);
	void SetSkin(wxString skinName);
	void SetLanguage(wxString language);
	void SetProfile(wxString profileName);
	void SetAudioDevice(const wxString &audioDevice);
	void SetVideoDevice(const wxString &videoDevice);
	void SetVideoCodec(const wxString &videoCodec);
	void SetAudioCodec(const wxString &audioCodec);
	void SetTransmissionProfile(const wxString &transmissionProfile);
	void SetRecordingStatus(const wxString &recordingStatus);
	void SetRecordingPath(const wxString &recordingPath);

	wxArrayString GetProfilesList();
	bool AddProfile(Profile profile);
	bool ModifyProfile(wxString profileName, Profile profile);
	void DeleteProfile(wxString profileName);
};

#endif
