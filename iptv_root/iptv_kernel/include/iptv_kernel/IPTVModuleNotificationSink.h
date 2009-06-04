#ifndef IPTV_MODULE_NOTIFICATION_SINK_H
#define IPTV_MODULE_NOTIFICATION_SINK_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"

enum IPTVModuleNotificationCode	{
									IMNC_INVALID,
									IMNC_MODULE_SPECIFIC,
									IMNC_SEND_PRIVATE_MESSAGE,
									IMNC_SEND_CHANNEL_PRIVATE_MESSAGE,
									IMNC_REQUEST_MEDIA_RECEIVE,
									IMNC_REQUEST_MEDIA_RECEIVE_RUN,
									IMNC_REQUEST_MEDIA_RECEIVE_STOP,
									IMNC_REQUEST_MEDIA_RECEIVE_ALL,
									IMNC_REQUEST_MEDIA_RECEIVE_ALL_RUN,
									IMNC_REQUEST_MEDIA_RECEIVE_ALL_STOP,
									IMNC_REQUEST_MEDIA_SEND,
									IMNC_REQUEST_MEDIA_SEND_STOP,
									IMNC_REQUEST_MEDIA_PACKET,
									IMNC_REQUEST_CHANNEL_WHITEBOARD,
									IMNC_MOVIE_SEND_BEGIN,
									IMNC_MOVIE_SEND_EOF,
									IMNC_MOVIE_SEND_ERROR,
									IMNC_REPLY_MEDIA_SEND
								};

enum IPTVModuleId				{
									IPTV_MODULE_ID_AV_CONFERENCE,
									IPTV_MODULE_ID_FILE_TRANSFER,
									IPTV_MODULE_ID_POLL,
									IPTV_MODULE_ID_APP_SHARING,
									IPTV_MODULE_ID_WHITEBOARD
								};

/** @brief Base structure to send an IP.TV Module notification.
 *
 */
struct IPTVModuleNotificationParameter
{
    IPTVModuleNotificationCode m_code;
	IPTVModuleId m_moduleId;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPPrivateMessage : public IPTVModuleNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_recipient;
	br::com::sbVB::VBLib::VBString m_message;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaReceive : public IPTVModuleNotificationParameter
{
	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_networkProtocol;
	unsigned m_port;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaId : public IPTVModuleNotificationParameter
{
	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_user;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaReceiveAll : public IPTVModuleNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_mediaClass;
	br::com::sbVB::VBLib::VBString m_networkProtocol;
	unsigned m_port;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaSend : public IPTVModuleNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_mediaClass;
	br::com::sbVB::VBLib::VBString m_networkProtocol;
	unsigned m_bitRate;
	unsigned m_headerSize;
	br::com::sbVB::VBLib::VBString m_header;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaPacket : public IPTVModuleNotificationParameter
{
	long m_mediaId;
	unsigned long m_initialPacketIndex;
	unsigned long m_finalPacketIndex;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPWhiteboard : public IPTVModuleNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_data;
};

/** @brief IP.TV Module notification structure.
 *
 */
struct IMNPMediaSendReply : public IPTVModuleNotificationParameter
{
	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_networkProtocol;
	br::com::sbVB::VBLib::VBString m_host;
	unsigned m_port;
};

/** @brief Receives notifications from an IP.TV Module.
 *
 * Any class that needs to receive IP.TV module notifications should
 * be derived of this class.
 *
 */
class IPTVModuleNotificationSink
{
public:
	virtual void OnIPTVModuleNotification(IPTVModuleNotificationParameter &parameter) = 0;
};

#endif
