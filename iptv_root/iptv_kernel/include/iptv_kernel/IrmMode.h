#ifndef IRM_MODE_H
#define IRM_MODE_H

#include "VBLib/VBLib.h"
#include <list>
#include "iptv_kernel/IrmQueryNotify.h"

#define IRM_MODE_POSITIVE						'+'
#define IRM_MODE_NEGATIVE						'-'
#define IRM_MODE_INVALID						'\0'

#define IRM_CHANNEL_MODE_OPERATOR				'o'
#define IRM_CHANNEL_MODE_PRIVATE				'p'
#define IRM_CHANNEL_MODE_SECRET					's'
#define IRM_CHANNEL_MODE_INVITE_ONLY			'i'
#define IRM_CHANNEL_MODE_OP_CHANGE_TOPIC		't'
#define IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES	'n'
#define IRM_CHANNEL_MODE_MODERATED				'm'
#define IRM_CHANNEL_MODE_USER_LIMIT				'l'
#define IRM_CHANNEL_MODE_BAN					'b'
#define IRM_CHANNEL_MODE_MEDIA_COLLABORATOR		'v'
#define IRM_CHANNEL_MODE_KEY					'k'
#define IRM_CHANNEL_MODE_BIT_RATE				'x'
#define IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE	'B'
#define IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION	'V'
#define IRM_CHANNEL_MODE_ONE_AUDIO				'y'
#define IRM_CHANNEL_MODE_AUDIO_MUTE				'a'
#define IRM_CHANNEL_MODE_BAND_SHARE_LIMIT		'S'
#define IRM_CHANNEL_MODE_TRANSMISSION_LIMIT		'u'

#define IRM_CHANNEL_MODE_OWNER					'w'
#define IRM_CHANNEL_MODE_LOG					'g'
#define IRM_CHANNEL_MODE_NAMES_LIMIT			'h'

#define IRM_USER_MODE_INVISIBLE					'i'
#define IRM_USER_MODE_REGISTERED				'r'
#define IRM_USER_MODE_CAMERA_ON					'C'
#define IRM_USER_MODE_MIC_MUTE					'T'
#define IRM_USER_MODE_AWAY						'W'

class IrmMode;

// Mode tools
std::list<IrmMode> CreateModeList(	br::com::sbVB::VBLib::VBString target,
									br::com::sbVB::VBLib::VBString flags,
									br::com::sbVB::VBLib::VBString parameters);

/** @brief Represents an IRM mode.
 *
 */
class IrmMode
{
public:
	IrmMode(	char name,
				bool value,
				br::com::sbVB::VBLib::VBString target,
				br::com::sbVB::VBLib::VBString parameter = "");
	~IrmMode();
 
	char GetName();
	bool GetValue();
	br::com::sbVB::VBLib::VBString GetTarget();
	br::com::sbVB::VBLib::VBString GetParameter();

	void SetParameter(br::com::sbVB::VBLib::VBString parameter);
	bool HasParameter();

	IrmQueryNotifyCode GetChannelModeNotificationCode();
	IrmQueryNotifyCode GetUserModeNotificationCode();

private:
	char m_name;
	bool m_value;
	bool m_isChannelMode;
	br::com::sbVB::VBLib::VBString m_target;
	br::com::sbVB::VBLib::VBString m_parameter;
};

#endif
