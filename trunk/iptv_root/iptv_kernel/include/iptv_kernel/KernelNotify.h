#ifndef KERNEL_NOTIFY_H
#define KERNEL_NOTIFY_H

#include "VBLib/VBLib.h"

enum KernelNotifyCode{
						KNC_READ_PACKET,							// Parameter class: KernelMediaIDParam
						KNC_NEW_CONFERENCE_TRANSMISSION,			// Parameter class: KernelMediaIDParam
						KNC_IRM_CONNECTED_HOST,						// Parameter class: KernelParam
						KNC_IRM_CONNECTED_CHAT,						// Parameter class: KernelParam
						KNC_IRM_DISCONNECTED,						// Parameter class: KernelParam
						KNC_IRM_AUTHENTICATED,						// Parameter class: KernelParam
						KNC_IRM_AUTHENTICATION_ERROR,				// Parameter class: KernelParam

						KNC_IRM_USER_QUIT,							// Parameter class: KernelUserParam
						KNC_IRM_CHANNEL_JOIN,						// Parameter class: KernelUserParam
						KNC_IRM_CHANNEL_PART,						// Parameter class: KernelUserParam
						KNC_IRM_CHANNEL_QUERY_ITEM,					// Parameter class: KernelChannelParam
						KNC_IRM_CHANNEL_QUERY_END,					// Parameter class: KernelParam
						KNC_IRM_CHANNEL_PASSWD_INVALID,				// Parameter class: KernelParam
						KNC_IRM_USER_QUERY_ITEM,					// Parameter class: KernelUserParam
						KNC_IRM_USER_QUERY_END,						// Parameter class: KernelChannelParam
						KNC_IRM_PRIV_MSG,							// Parameter class: KernelMessageParam
						KNC_IRM_CHANNEL_PRIV_MSG,					// Parameter class: KernelMessageParam

						KNC_IRM_CHANNEL_VOICE,						// Parameter class: KernelUserParam

						KNC_IRM_VOICE_REQUEST,						// Parameter class: KernelUserParam
						KNC_IRM_VOICE_REQUEST_CANCEL,				// Parameter class: KernelUserParam
						KNC_IRM_VOICE_REQUEST_REMOVE_ALL,			// Parameter class: KernelUserParam

						KNC_IRM_CHANNEL_MODE_OPERATOR,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_PRIVATE,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_SECRET,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_INVITE_ONLY,			// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_OP_CHANGE_TOPIC,		// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES,	// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_MODERATED,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_USER_LIMIT,			// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_BAN,					// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_MEDIA_COLLABORATOR,	// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_KEY,					// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_BIT_RATE,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE,	// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION,	// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_ONE_AUDIO,				// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_AUDIO_MUTE,			// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_BAND_SHARE_LIMIT,		// Parameter class: KernelModeParam
						KNC_IRM_CHANNEL_MODE_TRANSMISSION_LIMIT,	// Parameter class: KernelModeParam

						KNC_IRM_USER_MODE_INVISIBLE,				// Parameter class: KernelModeParam
						KNC_IRM_USER_MODE_REGISTERED,				// Parameter class: KernelModeParam
						KNC_IRM_USER_MODE_CAMERA_ON,				// Parameter class: KernelModeParam
						KNC_IRM_USER_MODE_MIC_MUTE,					// Parameter class: KernelModeParam
						KNC_IRM_USER_MODE_AWAY,						// Parameter class: KernelModeParam			

						KNC_TRCV_DATA_READ_MEDIA,					// Parameter class: KernelMediaDataParam
						KNC_TRCV_DATA_READ_APP,						// Parameter class: KernelMediaDataParam
						KNC_TRCV_NEW_TRANSMISSION,					// Parameter class: KernelMediaIDParam
						KNC_TRCV_NOTIFY_PROGRESS,					// Parameter class: KernelMediaProgressParam
						KNC_PRCS_READ_PACKET,						// Parameter class: KernelMediaIDParam
						KNC_PRCS_WRITE_PACKET,						// Parameter class: Unknown (not implemented)
						KNC_PRCS_VIDEO_ALIVE,						// Parameter class: KernelMediaIDParam
						KNC_PRCS_AUDIO_ALIVE,						// Parameter class: KernelMediaIDParam
						KNC_PRCS_BUFFERING_START,					// Parameter class: KernelMediaIDParam
						KNC_PRCS_BUFFERING_STOP,					// Parameter class: KernelMediaIDParam

						KNC_CAPTURE_VIDEO_DATA,						// Parameter class: KernelVideoCaptureDataParam

						KNC_FT_RECEIVE_BEGIN,						// Parameter class: KernelFTReceiveBeginParam
						KNC_FT_RECEIVE_WB_BEGIN,					// Parameter class: KernelFTReceiveBeginWBParam
						KNC_FT_RECEIVE_PROGRESS,					// Parameter class: KernelFTReceiveProgressParam
						KNC_FT_RECEIVE_LOST_PACKET,					// Parameter class: KernelFTReceiveLostPacketParam
						KNC_FT_RECEIVE_SUCCESS,						// Parameter class: KernelMediaIDParam
						KNC_FT_RECEIVE_ERROR,						// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_BEGIN,							// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_PROGRESS,						// Parameter class: KernelFTReceiveProgressParam
						KNC_FT_SEND_LOST_PACKET,					// Parameter class: KernelFTReceiveLostPacketParam
						KNC_FT_SEND_EOF,							// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_SUCCESS,						// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_ERROR,							// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_RECEIVER_NEW,					// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_RECEIVER_CANCEL,				// Parameter class: KernelMediaIDParam
						KNC_FT_SEND_RECEIVER_END,					// Parameter class: KernelMediaIDParam

						KNC_MOVIE_SEND_BEGIN,						// Parameter class: KernelMediaIDParam
						KNC_MOVIE_SEND_EOF,							// Parameter class: KernelMediaIDParam
						KNC_MOVIE_SEND_ERROR,						// Parameter class: KernelMediaIDParam

						KNC_POLL_BEGIN,								// Parameter class: KernelPollParam
						KNC_POLL_QUESTION,							// Parameter class: KernelPollParam
						KNC_POLL_OPTION,							// Parameter class: KernelPollParam
						KNC_POLL_END,								// Parameter class: KernelPollParam
						KNC_POLL_ANSWER,							// Parameter class: KernelPollParam
						KNC_POLL_STATS_BEGIN,						// Parameter class: KernelPollParam
						KNC_POLL_STATS_QUESTION,					// Parameter class: KernelPollParam
						KNC_POLL_STATS_OPTION,						// Parameter class: KernelPollParam
						KNC_POLL_STATS_END,							// Parameter class: KernelPollParam

						KNC_WB_MESSAGE,								// Parameter class: KernelWBMessageParam

						KNC_AS_RECEIVE_BEGIN,						// Parameter class: KernelMediaIDParam	
						KNC_AS_RECEIVE_NEW_FRAME,					// Parameter class: KernelASFrameParam
						KNC_AS_RECEIVE_END,							// Parameter class: KernelMediaIDParam
						KNC_AS_RECEIVE_ERROR,						// Parameter class: KernelMediaIDParam

						KNC_ERROR_IRM_STUB_INIT,					// Parameter class: KernelParam
						KNC_ERROR_IRM_CONN_STATE,					// Parameter class: KernelParam
						KNC_ERROR_IRM_CONN_QUERY,					// Parameter class: KernelParam
						KNC_ERROR_IRM_CONN_PARAM_BLANK,				// Parameter class: KernelParam

						KNC_DEBUG,									// Parameter class: KernelParam
						KNC_LOCALE_SET_ENGLISH,						// Parameter class: KernelParam
						KNC_LOCALE_RESTORE,							// Parameter class: KernelParam
					};

/** @brief Base structure to send an IP.TV Kernel notification.
 *
 */
struct KernelParam
{
    KernelNotifyCode m_code;
	br::com::sbVB::VBLib::VBString m_kernelMessage;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelChannelParam : public KernelParam
{
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_channelTopic;
	int m_userCount;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelUserParam : public KernelParam
{
	br::com::sbVB::VBLib::VBString m_userName;
	br::com::sbVB::VBLib::VBString m_channelName;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelMessageParam : public KernelParam
{
	br::com::sbVB::VBLib::VBString m_sender;
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_message;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelModeParam : public KernelParam
{
	char m_name;
	bool m_value;
	br::com::sbVB::VBLib::VBString m_target;
	br::com::sbVB::VBLib::VBString m_parameter;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelMediaIDParam : public KernelParam
{
	unsigned long m_mediaID;
	br::com::sbVB::VBLib::VBString m_channelName;
	br::com::sbVB::VBLib::VBString m_userNickName;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelMediaDataParam : public KernelParam
{
	unsigned char  *m_data;
	unsigned long m_dataLength;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelMediaProgressParam : public KernelParam
{
	unsigned long m_mediaID;
	double m_percent;
	unsigned char m_flags;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelFTReceiveBeginParam : public KernelParam
{
	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_fileName;
	br::com::sbVB::VBLib::VBString m_tempFilePath;
	br::com::sbVB::VBLib::VBString m_senderNick;
	unsigned long m_fileSize;
	unsigned m_packetSize;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelFTProgressParam : public KernelParam
{
	unsigned long m_currentPacketIndex;
	unsigned long m_lastPacketIndex;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelFTLostPacketParam : public KernelParam
{
	unsigned long m_lostPackets;
	unsigned long m_lostPacketsTotal;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelPollParam : public KernelUserParam
{
	br::com::sbVB::VBLib::VBString m_text;
	int m_index;
	int m_answerCount;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelWBMessageParam : public KernelUserParam
{
	br::com::sbVB::VBLib::VBString m_data;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelASFrameParam : public KernelMediaIDParam
{
	long m_width;
	long m_height;
	long m_bufferlength;
	int m_format;
	unsigned char* m_buffer;
};

/** @brief IP.TV Kernel notification structure.
 *
 */
struct KernelVideoCaptureDataParam : public KernelMediaDataParam
{
	unsigned m_width;
	unsigned m_height;
};

/** @brief Receives notifications from IP.TV Kernel.
 *
 * Any class that needs to receive IP.TV Kernel notifications should
 * be derived of this class.
 *
 */
class KernelNotify
{
public:
	virtual bool OnKernelNotify(KernelParam &param) = 0;
};

#endif
