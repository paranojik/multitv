#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

#include "VBLib/VBLib.h"
#include "iptv_kernel/IrmMessage.h"
#include "iptv_kernel/IPTVMediaModule.h"
#include "iptv_kernel/FileTransferOrigin.h"

// disable warnings in MSVC when using shared lib
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)	// size_t to int conversion
#pragma warning(disable: 4005)	// FD_SETSIZE (winsock2.h) macro redefinition
#endif

#include "shared/compat.h"
#include "xfiletransfer/IXFileTransferNotify.h"
#include "xfiletransfer/XFileTransferDll.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

enum FileTransferState	{
							FT_STATE_IDLE,
							FT_STATE_RECEIVING,
							FT_STATE_TRANSMITTING
						};

struct FileTransferData
{
	FileTransferData();
	void Reset();

	long m_mediaId;
	MediaConnectionData m_connectionData;
	br::com::sbVB::VBLib::VBString m_channel;
	br::com::sbVB::VBLib::VBString m_user;
	br::com::sbVB::VBLib::VBString m_fileName;
	br::com::sbVB::VBLib::VBString m_filePath;
	unsigned long m_fileSize;
	unsigned long m_packetLength;
	unsigned m_bitRate;
};

/** @brief Implements the File Transfer module.
 *
 * This class responsibilities are handling File Transfer IRM messages and provinding a higher
 * level interface to the iptv_xfiletransfer library that implements the File Transfer rules.
 *
 */
class FileTransfer :	public IPTVMediaModule,
						public IXFTPNotify
{
public:
	FileTransfer();

	bool CancelReception(long mediaId);

	bool BeginTransmission(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString filePath,
							unsigned long fileSize,
							unsigned bitRate,
							FileTransferOrigin origin = FT_ORIGIN_SEND_FILES);

	bool CancelTransmission(long mediaId);

private:
	void OnFileTransferEvent(XFTP_CODE code, DWORD data1, DWORD data2);

	unsigned GetMediaPort() const;

	void Reset();

	void OnUserPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
								br::com::sbVB::VBLib::VBString message);

	void OnChannelPrivateMessage(	br::com::sbVB::VBLib::VBString sender,
									br::com::sbVB::VBLib::VBString channel,
									br::com::sbVB::VBLib::VBString message);

	void OnMediaNotify(	br::com::sbVB::VBLib::VBString notification,
						long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						br::com::sbVB::VBLib::VBString user);

	void OnMediaReceive(long mediaId,
						long port,
						br::com::sbVB::VBLib::VBString multiCastIp,
						long packetLength,
						long headerLength,
						br::com::sbVB::VBLib::VBString header,
						IrmUser user,
						IrmConnection conn);

	void OnMediaReceiveRun(long mediaId);
	void OnMediaReceiveStop(long mediaId);

	void OnMediaReceiveAll(	br::com::sbVB::VBLib::VBString channel,
							br::com::sbVB::VBLib::VBString mediaClass,
							long port,
							br::com::sbVB::VBLib::VBString multiCastIp,
							IrmUser user,
							IrmConnection conn);

	void OnMediaReceiveAllRun(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaReceiveAllStop(	br::com::sbVB::VBLib::VBString channel,
								br::com::sbVB::VBLib::VBString mediaClass);

	void OnMediaSend(	long mediaId,
						br::com::sbVB::VBLib::VBString mediaClass,
						br::com::sbVB::VBLib::VBString channel,
						long port,
						IrmUser &user,
						IrmConnection &conn);

	void OnMediaSendStop(long mediaId);

	void OnPacketRequest(	long mediaId,
							unsigned long initialPacketIndex,
							unsigned long finalPacketIndex);

	void OnFileTransferSendProgressStats(unsigned long currentPacketIndex, unsigned long lastPacketIndex);
	void OnFileTransferSendLostPacketStats(unsigned long lostPackets, unsigned long lostPacketsTotal);
	void OnFileTransferSendEof();
	void OnFileTransferSendSuccess();
	void OnFileTransferSendError();

	void OnFileTransferReceiveProgressStats(unsigned long currentPacketIndex, unsigned long lastPacketIndex);
	void OnFileTransferReceiveLostPacketStats(unsigned long lostPackets);
	void OnFileTransferReceivePacketRequest(unsigned long initialPacketIndex, unsigned long finalPacketIndex);
	void OnFileTransferReceiveSuccess();
	void OnFileTransferReceiveError();

	bool IsIdle() const;
	bool IsReceiving() const;
	bool IsTransmitting() const;
	void SetState(FileTransferState state);

	XFileTransferProxy m_fileTransferProxy;

	FileTransferState m_state;

	FileTransferData m_receptionData;
	FileTransferData m_transmissionData;

	static const unsigned m_fileTransferMediaPort;
};

#endif
