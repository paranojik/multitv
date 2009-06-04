#ifndef MODE_H
#define MODE_H

#include <wx/wx.h>

enum
{
	IRM_MODE_NONE,
	IRM_MODE_USER_INVISIBLE,
	IRM_MODE_USER_AWAY,
	IRM_MODE_USER_REGISTERED,
	IRM_MODE_USER_CAMERA_ON,
	IRM_MODE_USER_MIC_MUTE,

	IRM_MODE_CHANNEL_OPERATOR = 50,
	IRM_MODE_CHANNEL_MEDIA_COLLABORATOR,
	IRM_MODE_CHANNEL_INVITE_ONLY,
	IRM_MODE_CHANNEL_PRIVATE,
	IRM_MODE_CHANNEL_SECRET,
	IRM_MODE_CHANNEL_MODERATED,
	IRM_MODE_CHANNEL_OP_ONLY_TOPIC,
	IRM_MODE_CHANNEL_NO_EXT_MSGS,
	IRM_MODE_CHANNEL_BAN,
	IRM_MODE_CHANNEL_PASSWORD_PROTECTED,
	IRM_MODE_CHANNEL_BITRATE,
	IRM_MODE_CHANNEL_AUTO_BITRATE,
	IRM_MODE_CHANNEL_MULTIPLE_TRANSMISSION,
	IRM_MODE_CHANNEL_ONE_AUDIO,
	IRM_MODE_CHANNEL_AUDIO_MUTE,
	IRM_MODE_CHANNEL_USER_LIMIT,
	IRM_MODE_CHANNEL_BAND_SHARE_LIMIT,
	IRM_MODE_CHANNEL_TRANSMISSION_LIMIT,

	IRM_MODE_MEDIA_VOICE = 100,
	IRM_MODE_MEDIA_VOICE_REQUEST
};

/** @brief Represents an IRM mode.
 *
 * This class represents an IRM mode. Can be channel or user mode, and there is a special
 * provision for media voice.
 *
 */
class Mode
{
private:
	int m_id;
	wxString m_parameter;
public:
	Mode(int id = IRM_MODE_NONE, const wxString &parameter = wxEmptyString);
	
	int GetID() const;
	const wxString &GetParameter() const;

	void SetID(int id);
	void SetParameter(const wxString &parameter);
};

extern const Mode g_NullMode;

#endif
