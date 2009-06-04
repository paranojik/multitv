#include "iptv_kernel/FileTransfer.h"
#include "iptv_kernel/FileTransferCtcpMessage.h"
#include "iptv_kernel/FileTransferHeader.h"
#include "iptv_kernel/LogManager.h"
#include "iptv_kernel/FileTransferNotificationParameters.h"
#include "stdlib.h"

#include "shared/XStr.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

const unsigned FileTransfer::m_fileTransferMediaPort = 8006;

FileTransfer::FileTransfer()
{
	// Initializing XFileTransferProxy.
	m_fileTransferProxy.Build();
	m_fileTransferProxy.Initialize(this);

	// Reseting the members.
	Reset();
}

void FileTransfer::OnFileTransferEvent(XFTP_CODE code, DWORD data1, DWORD data2)
{
	switch(code)
	{
		//---------------------------------------------------------------------------------------------
		// File transmission events
		//---------------------------------------------------------------------------------------------
		case XFTPC_SNDP1STATS:
			OnFileTransferSendProgressStats(data1, data2);
		break;
		case XFTPC_SNDP2STATS:
			OnFileTransferSendLostPacketStats(data1, data2);
		break;
		case XFTPC_EOF:
			OnFileTransferSendEof();
		break;
		case XFTPC_SNDEND:
			OnFileTransferSendSuccess();
		break;
		case XFTPC_SNDERR:
			OnFileTransferSendError();
		break;
		//---------------------------------------------------------------------------------------------
		// File reception events
		//---------------------------------------------------------------------------------------------
		case XFTPC_RCVP1STATS:
			OnFileTransferReceiveProgressStats(data1, data2);
		break;
		case XFTPC_RCVP2STATS:
			OnFileTransferReceiveLostPacketStats(data2);
		break;
		case XFTPC_PKTREQ:
			OnFileTransferReceivePacketRequest(data1, data2);
		break;
		case XFTPC_RCVSUCCESS:
			OnFileTransferReceiveSuccess();
		break;
		case XFTPC_RCVERR:
			OnFileTransferReceiveError();
		break;
	}
}

void FileTransfer::OnUserPrivateMessage(VBString sender, VBString message)
{
	FileTransferCtcpMessage msg(message);

	if(msg.IsFileTransferCtcpMessage())
	{
		if(msg.IsReceptionCancel())
		{
			// An user has canceled the file reception.
			if(IsTransmitting())
			{
				// Notifying the kernel.
				FileTransferNotificationParameter param;
				param.m_moduleId = IPTV_MODULE_ID_WHITEBOARD;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_fileTransferCode = FTNC_SEND_RECEIVER_CANCEL;
				param.m_mediaId = m_transmissionData.m_mediaId;
				Notify(param);
			}
		}
		else if(msg.IsReceptionEnd())
		{
			// An user has received the file successfully.
			if(IsTransmitting())
			{
				// Notifying the kernel.
				FileTransferNotificationParameter param;
				param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_fileTransferCode = FTNC_SEND_RECEIVER_END;
				param.m_mediaId = m_transmissionData.m_mediaId;
				Notify(param);
			}
		}
	}
}

void FileTransfer::OnChannelPrivateMessage(VBString sender, VBString channel, VBString message)
{
	FileTransferCtcpMessage msg(message);

	if(msg.IsFileTransferCtcpMessage())
	{
		if(msg.IsTransmissionBegin())
		{
			// An user began transmitting a file.
			// This does not have to be handled.
			// Nothing to do.
		}

		else if(msg.IsTransmissionEnd())
		{
			// An user finished sending a file.
			if(IsReceiving())
				m_fileTransferProxy.NotifyEOF();
		}
	}
}

void FileTransfer::OnMediaNotify(VBString notification, long mediaId, VBString mediaClass,
								 VBString channel, VBString user)
{
	if(IsMediaClassFileTransfer(mediaClass))
	{
		if(IsMediaNotificationSend(notification))
		{
			if(IsIdle())
			{
				Reset();
				SetState(FT_STATE_RECEIVING);

				MediaConnectionData connInfo;
				connInfo = GetReceptionData();

				RequestMediaReceive(mediaId, connInfo.GetNetworkProtocolName(), connInfo.GetPort());

				// Storing information about the last notified file transfer transmission.
				// This information will be used in XFileTransfer::BeginReceive.
				m_receptionData.m_mediaId = mediaId;
				m_receptionData.m_channel = channel;
				m_receptionData.m_user = user;
			}
		}

		else if(IsMediaNotificationStart(notification))
		{
			// An user accepted the file transmission.
			if(IsTransmitting())
			{
				// Notifying the kernel.
				FileTransferNotificationParameter param;
				param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
				param.m_code = IMNC_MODULE_SPECIFIC;
				param.m_fileTransferCode = FTNC_SEND_RECEIVER_NEW;
				param.m_mediaId = m_transmissionData.m_mediaId;
				Notify(param);
			}
		}

		else if(IsMediaNotificationStop(notification))
		{
			// An user stopped receiving a file transmission.
			if(IsTransmitting())
			{
				// Nothing to do.
			}
		}
	}
}

void FileTransfer::OnMediaReceive(	long mediaId, long port, VBString multiCastIp,
									long packetLength, long headerLength, VBString header,
									IrmUser user, IrmConnection conn)
{
	if(IsReceiving())
	{
		FileTransferHeader fileTransferHeader(header);

		if(mediaId == m_receptionData.m_mediaId)
		{
			MediaConnectionData connInfo;
			connInfo = GetReceptionData(conn.GetHost(), port, multiCastIp);

			VBString filePath;
#ifdef WIN32
// TODO: verify if the file already exists in the client temp path.
// If it does, rename it appending an index until we have an unique file path.
			char *winTempPath;
			winTempPath = getenv("TEMP");
			if(winTempPath != NULL)
			{
				filePath = winTempPath;
				filePath += "\\";
				filePath += fileTransferHeader.GetFileName();
			}
			else
			{
				filePath += fileTransferHeader.GetFileName();
			}
#endif
#ifdef _LINUX
// TODO: verify if the file already exists in the client temp path.
// If it does, rename it appending an index until we have an unique file path.
			filePath = "/var/tmp/";
			filePath += fileTransferHeader.GetFileName();
#endif

			FileTransferBeginParameter param;
			param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
			param.m_code = IMNC_MODULE_SPECIFIC;
			param.m_fileTransferCode = FTNC_RECEIVE_BEGIN;
			param.m_mediaId = m_receptionData.m_mediaId;
			param.m_fileName = fileTransferHeader.GetFileName();
			param.m_tempFilePath = filePath;
			param.m_senderNick = m_receptionData.m_user;
			param.m_fileSize = fileTransferHeader.GetFileSize();
			param.m_packetSize = fileTransferHeader.GetPacketSize();
			param.m_origin = fileTransferHeader.GetOrigin();
			
			Notify(param);

			if(m_fileTransferProxy.BeginReceive(	m_receptionData.m_mediaId,
													XStrDyn(connInfo.GetNetworkProtocolName().c_str()),
													connInfo.GetHost(),
													connInfo.GetPort(),
													user.GetNick(),
													m_receptionData.m_channel,
													filePath,
													fileTransferHeader.GetFileSize(),
													(USHORT)fileTransferHeader.GetPacketSize(),
													FALSE,
													FALSE))
			{
				m_fileTransferProxy.RunRX();

				if(!connInfo.IsNetworkProtocolTcp())
					RequestMediaReceiveRun(m_receptionData.m_mediaId);
			}

			else
			{
				// FT error receive.
				// Nothing to do.
			}
		}
	}
}

void FileTransfer::OnMediaReceiveRun(long mediaId)
{
	if(IsReceiving())
	{
		if(mediaId == m_receptionData.m_mediaId)
		{
			// Reception running.
			// Nothing to do.
		}
	}
}

void FileTransfer::OnMediaReceiveStop(long mediaId)
{
	if(IsReceiving())
	{
		if(mediaId == m_receptionData.m_mediaId)
		{
			Reset();
		}
	}
}

void FileTransfer::OnMediaReceiveAll(	VBString channel,
										VBString mediaClass,
										long port,
										VBString multiCastIp,
										IrmUser user,
										IrmConnection conn)
{
	// File Transfer does not handle this message.
}

void FileTransfer::OnMediaReceiveAllRun(	VBString channel,
											VBString mediaClass)
{
	// File Transfer does not handle this message.
}

void FileTransfer::OnMediaReceiveAllStop(	VBString channel,
											VBString mediaClass)
{
	// File Transfer does not handle this message.
}

void FileTransfer::OnMediaSend(	long mediaId,
								br::com::sbVB::VBLib::VBString mediaClass,
								br::com::sbVB::VBLib::VBString channel,
								long port,
								IrmUser &user,
								IrmConnection &conn)
{
	if(IsMediaClassFileTransfer(mediaClass))
	{
		if(IsTransmitting())
		{
			if(m_transmissionData.m_channel == channel)
			{
				m_transmissionData.m_mediaId = mediaId;
				m_transmissionData.m_connectionData.SetPort(port);

				MediaConnectionData connData;
				connData = GetTransmissionData(conn.GetHost(), m_transmissionData.m_connectionData.GetPort(), IRM_NETWORK_PROTOCOL_UDP);

				if(m_fileTransferProxy.BeginSend(	m_transmissionData.m_mediaId,
													connData.GetHost(),
													connData.GetPort(),
													m_transmissionData.m_filePath,
													m_transmissionData.m_bitRate,
													connData.GetNetworkProtocolName(),
													FALSE, FALSE))
				{
					m_fileTransferProxy.RunTX();

					// Notifying the kernel.
					FileTransferNotificationParameter param;
					param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
					param.m_code = IMNC_MODULE_SPECIFIC;
					param.m_fileTransferCode = FTNC_SEND_BEGIN;
					param.m_mediaId = m_transmissionData.m_mediaId;
					Notify(param);

					// CTCP notification. Notify the channel NOTIFYSND.
				}

				else
				{
					// Notifying the server.
					RequestMediaSendStop(m_transmissionData.m_mediaId);
					Reset();
				}
			}
		}
	}
}

void FileTransfer::OnMediaSendStop(long mediaId)
{
	if(IsTransmitting())
	{
		if(mediaId == m_transmissionData.m_mediaId)
		{
			Reset();
		}
	}
}

void FileTransfer::OnPacketRequest(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	if(IsTransmitting())
	{
		if(mediaId == m_transmissionData.m_mediaId)
		{
			m_fileTransferProxy.PktRequest(initialPacketIndex, finalPacketIndex);
		}
	}
}

void FileTransfer::OnFileTransferSendProgressStats(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	if(IsTransmitting())
	{
		// Notifying the kernel.
		FileTransferProgressParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_SEND_PROGRESS;
		param.m_currentPacketIndex = currentPacketIndex;
		param.m_lastPacketIndex = lastPacketIndex;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferSendLostPacketStats(unsigned long lostPackets, unsigned long lostPacketsTotal)
{
	if(IsTransmitting())
	{
		// Notifying the kernel.
		FileTransferLostPacketParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_SEND_LOST_PACKET;
		param.m_lostPackets = lostPackets;
		param.m_lostPacketsTotal = lostPacketsTotal;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferSendEof()
{
	if(IsTransmitting())
	{
		// Notifying channel users that the first phase of transmission has ended.
		FileTransferCtcpMessage msg;
		msg.SetFileTransferMessageCode(FT_MESSAGE_CODE_SEND_EOF);
		msg.SetMediaId(m_transmissionData.m_mediaId);
		SendChannelPrivateMessage(m_transmissionData.m_channel, msg.GetFileTransferCtcpMessage());

		// Notifying the kernel.
		FileTransferNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_SEND_EOF;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferSendSuccess()
{
	if(IsTransmitting())
	{
		// Notifying the server.
		RequestMediaSendStop(m_transmissionData.m_mediaId);

		// Notifying the kernel.
		FileTransferNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_SEND_SUCCESS;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferSendError()
{
	if(IsTransmitting())
	{
		// Notifying the server.
		RequestMediaSendStop(m_transmissionData.m_mediaId);

		// Notifying the kernel.
		FileTransferNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_SEND_ERROR;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferReceiveProgressStats(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	if(IsReceiving())
	{
		// Notifying the kernel.
		FileTransferProgressParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_RECEIVE_PROGRESS;
		param.m_currentPacketIndex = currentPacketIndex;
		param.m_lastPacketIndex = lastPacketIndex;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferReceiveLostPacketStats(unsigned long lostPackets)
{
	if(IsReceiving())
	{
		// Notifying the kernel.
		FileTransferLostPacketParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_RECEIVE_LOST_PACKET;
		param.m_lostPackets = lostPackets;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferReceivePacketRequest(unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	if(IsReceiving())
	{
		// Notifying the server.
		RequestMediaPacket(m_receptionData.m_mediaId, initialPacketIndex, finalPacketIndex);
	}
}

void FileTransfer::OnFileTransferReceiveSuccess()
{
	if(IsReceiving())
	{
		// Notifying the user who sent the file that the transmission was successful.
		FileTransferCtcpMessage msg;
		msg.SetFileTransferMessageCode(FT_MESSAGE_CODE_RECEIVE_END);
		msg.SetMediaId(m_receptionData.m_mediaId);
		VBString message =  msg.GetFileTransferCtcpMessage();
		SendPrivateMessage(m_receptionData.m_user, message);
		
		// Notifying the server.
		RequestMediaReceiveStop(m_receptionData.m_mediaId);

		// Notifying the kernel.
		FileTransferNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_RECEIVE_SUCCESS;
		Notify(param);
	}
}

void FileTransfer::OnFileTransferReceiveError()
{
	if(IsReceiving())
	{
		// Notifying the server.
		RequestMediaReceiveStop(m_receptionData.m_mediaId);

		// Notifying the kernel.
		FileTransferNotificationParameter param;
		param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
		param.m_code = IMNC_MODULE_SPECIFIC;
		param.m_fileTransferCode = FTNC_RECEIVE_ERROR;
		Notify(param);
	}
}

bool FileTransfer::CancelReception(long mediaId)
{
	if(IsReceiving())
	{
		// Notifying the user who sent the file that the reception was canceled.
		FileTransferCtcpMessage msg;
		msg.SetFileTransferMessageCode(FT_MESSAGE_CODE_RECEIVE_CANCEL);
		msg.SetMediaId(m_receptionData.m_mediaId);
		VBString message =  msg.GetFileTransferCtcpMessage();
		SendPrivateMessage(m_receptionData.m_user, message);

		// Notifying the server.
		RequestMediaReceiveStop(m_receptionData.m_mediaId);

		Reset();

		return true;
	}

	return false;
}

bool FileTransfer::BeginTransmission(VBString channel, VBString filePath, unsigned long fileSize, unsigned bitRate, FileTransferOrigin origin)
{
	if(IsIdle())
	{
		Reset();
		SetState(FT_STATE_TRANSMITTING);

		MediaConnectionData connData;
		connData = GetTransmissionData(IRM_NETWORK_PROTOCOL_UDP);

		m_transmissionData.m_channel = channel;
		m_transmissionData.m_filePath = filePath;
		m_transmissionData.m_fileSize = fileSize;
		m_transmissionData.m_packetLength = m_fileTransferProxy.GetSndDataLen();
		m_transmissionData.m_bitRate = bitRate;
		m_transmissionData.m_connectionData.SetNetworkProtocol(connData.GetNetworkProtocol());

		VBString header;
		unsigned int headerSize;
		header = FileTransferHeader::CreateHeader(	m_transmissionData.m_filePath, m_transmissionData.m_fileSize,
													m_transmissionData.m_packetLength, origin);
		headerSize = header.length();

		RequestMediaSend(	m_transmissionData.m_channel,
							m_irmMediaClassFileTransfer,
							m_transmissionData.m_connectionData.GetNetworkProtocolName(),
							m_transmissionData.m_bitRate,
							headerSize,
							header);

		return true;
	}

	return false;
}

bool FileTransfer::CancelTransmission(long mediaId)
{
	if(m_state == FT_STATE_TRANSMITTING)
	{
		// Notifying the server.
		RequestMediaSendStop(m_transmissionData.m_mediaId);
		Reset();

		return true;
	}

	return false;
}

bool FileTransfer::IsIdle() const
{
	if(m_state == FT_STATE_IDLE)
		return true;
	return false;
}

bool FileTransfer::IsReceiving() const
{
	if(m_state == FT_STATE_RECEIVING)
		return true;
	return false;
}

bool FileTransfer::IsTransmitting() const
{
	if(m_state == FT_STATE_TRANSMITTING)
		return true;
	return false;
}

void FileTransfer::SetState(FileTransferState state)
{
	m_state = state;
}

unsigned FileTransfer::GetMediaPort() const
{
	return m_fileTransferMediaPort;
}

void FileTransfer::Reset()
{
	m_fileTransferProxy.Reset();
	m_receptionData.Reset();
	m_transmissionData.Reset();
	SetState(FT_STATE_IDLE);
}

FileTransferData::FileTransferData()
{
	Reset();
}

void FileTransferData::Reset()
{
	m_mediaId = 0;
	m_connectionData.SetNetworkProtocol(IRM_NETWORK_PROTOCOL_INVALID);
	m_connectionData.SetHost("");
	m_connectionData.SetPort(0);
	m_channel = "";
	m_user = "";
	m_fileName = "";
	m_filePath = "";
	m_fileSize = 0;
	m_packetLength = 0;
	m_bitRate = 0;
}
