#ifndef IRM_QUERY_NOTIFY_H
#define IRM_QUERY_NOTIFY_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IrmUser.h"
#include "iptv_kernel/IrmConnection.h"

enum IrmQueryNotifyCode	{
							IQNC_INVALID_CODE,
							IQNC_IRM_MESSAGE,								// Parameter class: IrmQueryIrmMessageParam
							IQNC_CONNECTED_HOST,							// Parameter class: IrmQueryParam
							IQNC_CONNECTED_CHAT,							// Parameter class: IrmQueryParam
							IQNC_DISCONNECTED,								// Parameter class: IrmQueryParam
							IQNC_UDP_CONNECTION_AVAILABLE,					// Parameter class: IrmQueryParam
							IQNC_AUTHENTICATED,								// Parameter class: IrmQueryParam
							IQNC_AUTHENTICATION_REQUESTED,					// Parameter class: IrmQueryParam
							IQNC_AUTHENTICATION_ERROR,						// Parameter class: IrmQueryParam
							IQNC_USER_QUIT,									// Parameter class: IrmQueryUserParam
							IQNC_CHANNEL_JOIN,								// Parameter class: IrmQueryUserParam
							IQNC_CHANNEL_PART,								// Parameter class: IrmQueryUserParam
							IQNC_CHANNEL_QUERY_ITEM,						// Parameter class: IrmQueryChannelParam
							IQNC_CHANNEL_QUERY_END,							// Parameter class: IrmQueryParam
							IQNC_CHANNEL_PASSWD_INVALID,					// Parameter class: IrmQueryParam
							IQNC_USER_QUERY_ITEM,							// Parameter class: IrmQueryUserParam
							IQNC_USER_QUERY_END,							// Parameter class: IrmQueryChannelParam
							IQNC_PRIV_MSG,									// Parameter class: IrmQueryMessageParam
							IQNC_CHANNEL_PRIV_MSG,							// Parameter class: IrmQueryMessageParam
							IQNC_NEW_MEDIA_TRANSMISSION,					// Parameter class: IrmQueryMediaParam
							IQNC_MEDIA_CONFERENCE_READY,					// Parameter class: IrmQueryMediaParam
							IQNC_MEDIA_VIEWER_READY,						// Parameter class: IrmQueryMediaParam
                                                                    		
							IQNC_CHANNEL_VOICE,								// Parameter class: IrmQueryUserParam

							IQNC_VOICE_REQUEST,								// Parameter class: IrmQueryUserParam
							IQNC_VOICE_REQUEST_CANCEL,						// Parameter class: IrmQueryUserParam
							IQNC_VOICE_REQUEST_REMOVE_ALL,					// Parameter class: IrmQueryUserParam
                                                                    		
							IQNC_CHANNEL_MODE_OPERATOR,						// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_PRIVATE,						// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_SECRET,						// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_INVITE_ONLY,					// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_OP_CHANGE_TOPIC,				// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_NO_EXTERNAL_MESSAGES,			// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_MODERATED,					// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_USER_LIMIT,					// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_BAN,							// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_MEDIA_COLLABORATOR,			// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_KEY,							// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_BIT_RATE,						// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE,			// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_MULTIPLE_TRANSMISSION,		// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_ONE_AUDIO,					// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_AUDIO_MUTE,					// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_BAND_SHARE_LIMIT,				// Parameter class: IrmQueryModeParam
							IQNC_CHANNEL_MODE_TRANSMISSION_LIMIT,			// Parameter class: IrmQueryModeParam
                                                                    		
							IQNC_USER_MODE_INVISIBLE,						// Parameter class: IrmQueryModeParam
							IQNC_USER_MODE_REGISTERED,						// Parameter class: IrmQueryModeParam
							IQNC_USER_MODE_CAMERA_ON,						// Parameter class: IrmQueryModeParam
							IQNC_USER_MODE_MIC_MUTE,						// Parameter class: IrmQueryModeParam
							IQNC_USER_MODE_AWAY,							// Parameter class: IrmQueryModeParam	
                                                                			
							IQNC_ERROR_IRM_STUB_INIT,						// Parameter class: IrmQueryParam
							IQNC_ERROR_CONN_STATE,							// Parameter class: IrmQueryParam
							IQNC_ERROR_CONN_QUERY,							// Parameter class: IrmQueryParam
							IQNC_ERROR_CONN_PARAM_BLANK,					// Parameter class: IrmQueryParam
							IQNC_ERROR_VIEWER_PARAM_BLANK,					// Parameter class: IrmQueryParam
							IQNC_ERROR_VIEWER_STATE,						// Parameter class: IrmQueryParam
							IQNC_ERROR_VIEWER_INIT,							// Parameter class: IrmQueryParam
						};

/** @brief Base structure to send an IRM Query notification.
 *
 */
struct IrmQueryParam
{
    IrmQueryNotifyCode m_code;

	// Should be removed. The App shouldn't display this message as error information.
	br::com::sbVB::VBLib::VBString m_irmMessage;

	bool IsNotificationCodeValid();
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryIrmMessageParam : public IrmQueryParam
{
	IrmMessage m_message;
	IrmUser m_user;
	IrmConnection m_connection;
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryChannelParam : public IrmQueryParam
{
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_channelTopic;
	int m_userCount;
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryUserParam : public IrmQueryParam
{
	br::com::sbVB::VBLib::VBString m_userName;
	br::com::sbVB::VBLib::VBString m_channelName;
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryMessageParam : public IrmQueryParam
{
	br::com::sbVB::VBLib::VBString m_sender;
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_message;
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryModeParam : public IrmQueryParam
{
	char m_name;
	bool m_value;
	br::com::sbVB::VBLib::VBString m_target;
	br::com::sbVB::VBLib::VBString m_parameter;
};

/** @brief IRM Query notification structure.
 *
 */
struct IrmQueryMediaParam : public IrmQueryParam
{
	unsigned m_mediaId;
	br::com::sbVB::VBLib::VBString m_userNickName;
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_mediaClass;
	br::com::sbVB::VBLib::VBString m_netProtocol;
	br::com::sbVB::VBLib::VBString m_host;
	unsigned m_port;
};

/** @brief Receives notifications from IRM Query.
 *
 * Any class that needs to receive IRM Query notifications should
 * be derived of this class.
 *
 */
class IrmQueryNotify
{
public:
	virtual void OnIrmQueryNotify(IrmQueryParam &param) = 0;
};

#endif
