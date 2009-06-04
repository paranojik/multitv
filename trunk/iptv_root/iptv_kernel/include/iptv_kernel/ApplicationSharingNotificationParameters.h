#ifndef APPLICATION_SHARING_NOTIFICATION_PARAMETERS_H
#define APPLICATION_SHARING_NOTIFICATION_PARAMETERS_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModuleNotificationSink.h"

enum AppSharingNotificationCode	{
									ASNC_INVALID,
									ASNC_RECEIVE_INIT,
									ASNC_RECEIVE_BEGIN,
									ASNC_RECEIVE_NEW_FRAME,
									ASNC_RECEIVE_END,
									ASNC_RECEIVE_ERROR,
									ASNC_RECEIVE_ABORT
								};

/** @brief Base structure to send ApplicationSharing notifications.
 *
 */
struct ASNotificationParameter : public IPTVModuleNotificationParameter
{
    AppSharingNotificationCode m_appSharingCode;
	long m_mediaId;
	br::com::sbVB::VBLib::VBString m_user;
	br::com::sbVB::VBLib::VBString m_channel;
};

/** @brief Application Sharing notification structure.
 *
 */
struct ASReceiveInit : public ASNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_mediaClass;
	br::com::sbVB::VBLib::VBString m_networkProtocol;
	br::com::sbVB::VBLib::VBString m_host;
	unsigned m_port;
};

/** @brief Application Sharing notification structure.
 *
 */
struct ASReceiveFrame : public ASNotificationParameter
{
	long m_width;
	long m_height;
	long m_bufferlength;
	int m_format;
	unsigned char* m_buffer;
};

#endif
