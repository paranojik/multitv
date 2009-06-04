#include "iptv_kernel/IrmMode.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

IrmMode::IrmMode(	char name,
					bool value,
					VBString target,
					VBString parameter)
{
	m_name = name;
	m_value = value;
	m_target = target;
	m_parameter = parameter;
}

IrmMode::~IrmMode()
{

}

char IrmMode::GetName()
{
	return m_name;
}

bool IrmMode::GetValue()
{
	return m_value;
}

VBString IrmMode::GetTarget()
{
	return m_target;
}

VBString IrmMode::GetParameter()
{
	return m_parameter;
}

void IrmMode::SetParameter(br::com::sbVB::VBLib::VBString parameter)
{
	m_parameter = parameter;
}

IrmQueryNotifyCode IrmMode::GetChannelModeNotificationCode()
{
	int modeFlag = m_name;
	IrmQueryNotifyCode notificationCode = IQNC_INVALID_CODE;

	switch(modeFlag)
	{
		case IRM_CHANNEL_MODE_OPERATOR:
			notificationCode = IQNC_CHANNEL_MODE_OPERATOR;
			break;
		case IRM_CHANNEL_MODE_PRIVATE:
			notificationCode = IQNC_CHANNEL_MODE_PRIVATE;
			break;
		case IRM_CHANNEL_MODE_SECRET:
			notificationCode = IQNC_CHANNEL_MODE_SECRET;
			break;
		case IRM_CHANNEL_MODE_INVITE_ONLY:
			notificationCode = IQNC_CHANNEL_MODE_INVITE_ONLY;
			break;
		case IRM_CHANNEL_MODE_OP_CHANGE_TOPIC:
			notificationCode = IQNC_CHANNEL_MODE_OP_CHANGE_TOPIC;
			break;
		case IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES:
			notificationCode = IQNC_CHANNEL_MODE_NO_EXTERNAL_MESSAGES;
			break;
		case IRM_CHANNEL_MODE_MODERATED:
			notificationCode = IQNC_CHANNEL_MODE_MODERATED;
			break;
		case IRM_CHANNEL_MODE_USER_LIMIT:
			notificationCode = IQNC_CHANNEL_MODE_USER_LIMIT;
			break;
		case IRM_CHANNEL_MODE_BAN:
			notificationCode = IQNC_CHANNEL_MODE_BAN;
			break;
		case IRM_CHANNEL_MODE_MEDIA_COLLABORATOR:
			notificationCode = IQNC_CHANNEL_MODE_MEDIA_COLLABORATOR;
			break;
		case IRM_CHANNEL_MODE_KEY:
			notificationCode = IQNC_CHANNEL_MODE_KEY;
			break;
		case IRM_CHANNEL_MODE_BIT_RATE:
			notificationCode = IQNC_CHANNEL_MODE_BIT_RATE;
			break;
		case IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE:
			notificationCode = IQNC_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE;
			break;
		case IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION:
			notificationCode = IQNC_CHANNEL_MODE_MULTIPLE_TRANSMISSION;
			break;
		case IRM_CHANNEL_MODE_ONE_AUDIO:
			notificationCode = IQNC_CHANNEL_MODE_ONE_AUDIO;
			break;
		case IRM_CHANNEL_MODE_AUDIO_MUTE:
			notificationCode = IQNC_CHANNEL_MODE_AUDIO_MUTE;
			break;
		case IRM_CHANNEL_MODE_BAND_SHARE_LIMIT:
			notificationCode = IQNC_CHANNEL_MODE_BAND_SHARE_LIMIT;
			break;
		case IRM_CHANNEL_MODE_TRANSMISSION_LIMIT:
			notificationCode = IQNC_CHANNEL_MODE_TRANSMISSION_LIMIT;
			break;
		default:
			notificationCode = IQNC_INVALID_CODE;
	}

	return notificationCode;
}

IrmQueryNotifyCode IrmMode::GetUserModeNotificationCode()
{
	int modeFlag = m_name;
	IrmQueryNotifyCode notificationCode = IQNC_INVALID_CODE;

	switch(modeFlag)
	{
		case IRM_USER_MODE_INVISIBLE:
			notificationCode = IQNC_USER_MODE_INVISIBLE;
			break;
		case IRM_USER_MODE_REGISTERED:
			notificationCode = IQNC_USER_MODE_REGISTERED;
			break;
		case IRM_USER_MODE_CAMERA_ON:
			notificationCode = IQNC_USER_MODE_CAMERA_ON;
			break;
		case IRM_USER_MODE_MIC_MUTE:
			notificationCode = IQNC_USER_MODE_MIC_MUTE;
			break;
		case IRM_USER_MODE_AWAY:
			notificationCode = IQNC_USER_MODE_AWAY;
			break;
		default:
			notificationCode = IQNC_INVALID_CODE;
	}

	return notificationCode;
}

bool IrmMode::HasParameter()
{
	int modeFlag = m_name;

	switch(modeFlag)
	{
		case IRM_CHANNEL_MODE_OPERATOR:
		case IRM_CHANNEL_MODE_USER_LIMIT:
		case IRM_CHANNEL_MODE_MEDIA_COLLABORATOR:
		case IRM_CHANNEL_MODE_KEY:
		case IRM_CHANNEL_MODE_BIT_RATE:
		case IRM_CHANNEL_MODE_ONE_AUDIO:
		case IRM_CHANNEL_MODE_BAND_SHARE_LIMIT:
		case IRM_CHANNEL_MODE_TRANSMISSION_LIMIT:
		case IRM_CHANNEL_MODE_OWNER:
		case IRM_CHANNEL_MODE_LOG:
		case IRM_CHANNEL_MODE_NAMES_LIMIT:
			return true;
	}

	return false;
}

list<IrmMode> CreateModeList(VBString target, VBString flags, VBString parameters)
{
	// Creating the flag and parameter list
	list<char> flagList;
	list<VBString> parameterList;

	for(unsigned i = 0; i < flags.size(); i++)
	{
		char ch = flags.getChar(i);
		if((ch != ' '))
			flagList.push_back(ch);
	}

	for(unsigned i = 0; i <= parameters.tokCount(' '); i++)
	{
			VBString str = parameters.strtok(' ', i);
			if((str != " ") && (str != ""))
				parameterList.push_back(str);
	}

	// Creating the mode list
	list<IrmMode> modeList;
	bool modeValue = true;
	while(flagList.size() > 0)
	{
		int currentFlag = flagList.front();

		if(currentFlag == IRM_MODE_POSITIVE)
			modeValue = true;

		else if(currentFlag == IRM_MODE_NEGATIVE)
			modeValue = false;

		else
		{
			char modeName = flagList.front();
			IrmMode mode(modeName, modeValue, target);

			if(parameterList.size() != 0)
			{
				if(mode.HasParameter())
				{
					mode.SetParameter(parameterList.front());
					parameterList.pop_front();
				}
			}

			modeList.push_back(mode);
		}

		flagList.pop_front();
	}

	return modeList;
}
