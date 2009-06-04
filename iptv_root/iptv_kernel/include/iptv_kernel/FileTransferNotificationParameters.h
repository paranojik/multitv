#ifndef FILE_TRANSFER_NOTIFICATION_PARAMETERS_H
#define FILE_TRANSFER_NOTIFICATION_PARAMETERS_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IPTVModuleNotificationSink.h"
#include "iptv_kernel/FileTransferOrigin.h"

enum FileTransferNotificationCode	{
										FTNC_INVALID,
										FTNC_RECEIVE_BEGIN,
										FTNC_RECEIVE_PROGRESS,
										FTNC_RECEIVE_LOST_PACKET,
										FTNC_RECEIVE_SUCCESS,
										FTNC_RECEIVE_ERROR,
										FTNC_SEND_BEGIN,
										FTNC_SEND_PROGRESS,
										FTNC_SEND_LOST_PACKET,
										FTNC_SEND_EOF,
										FTNC_SEND_SUCCESS,
										FTNC_SEND_ERROR,
										FTNC_SEND_RECEIVER_NEW,
										FTNC_SEND_RECEIVER_CANCEL,
										FTNC_SEND_RECEIVER_END
									};

/** @brief Base structure to send FileTransfer notifications.
 *
 */
struct FileTransferNotificationParameter : public IPTVModuleNotificationParameter
{
    FileTransferNotificationCode m_fileTransferCode;
	long m_mediaId;
};

/** @brief FileTransfer notification structure.
 *
 */
struct FileTransferBeginParameter : public FileTransferNotificationParameter
{
	br::com::sbVB::VBLib::VBString m_fileName;
	br::com::sbVB::VBLib::VBString m_tempFilePath;
	br::com::sbVB::VBLib::VBString m_senderNick;
	unsigned long m_fileSize;
	unsigned m_packetSize;
	FileTransferOrigin m_origin;
};

/** @brief FileTransfer notification structure.
 *
 */
struct FileTransferProgressParameter : public FileTransferNotificationParameter
{
	unsigned long m_currentPacketIndex;
	unsigned long m_lastPacketIndex;
};

/** @brief FileTransfer notification structure.
 *
 */
struct FileTransferLostPacketParameter : public FileTransferNotificationParameter
{
	unsigned long m_lostPackets;
	unsigned long m_lostPacketsTotal;
};

#endif
