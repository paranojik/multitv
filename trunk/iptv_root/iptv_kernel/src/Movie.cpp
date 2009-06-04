#include "iptv_kernel/Movie.h"
#include "global_error.h"
#include "Media_const.h"

#include "shared/socket/sockbuffs.h"
#include "shared/socket/sockmwbuffs.h"
#include "SimposiumNetwork.h"
#include "CMedia.h"
#include "CVideoFrame.h"
#include "iptv_kernel/MediaProcessorNotify.h"
#include "iptv_kernel/MediaTransceiverNotify.h"

typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

using namespace br::com::sbVB::VBLib;
using namespace std;

const unsigned Movie::m_movieMediaPort = 8006;

Movie::Movie(MediaProcessorNotify *processorNotify,
			 MediaTransceiverNotify *transceiverNotify)
			 :	m_writeBuffer(NULL),
				m_readBufferContainer(NULL),
				m_simposiumNetwork(NULL),
				m_media(NULL),
				m_mediaVideoFrame(NULL),
				m_processorNotify(processorNotify),
				m_transceiverNotify(transceiverNotify)
{
	// Reseting the members.
	Reset();
}

Movie::~Movie()
{
	if(m_readBufferContainer)
		m_readBufferContainer->Abort();
	if(m_media)
		m_media->EndAllTransmissions();

	if(m_writeBuffer)
		delete m_writeBuffer;

	DestroyReadBufferContainer();

	if(m_simposiumNetwork)
		delete m_simposiumNetwork;

	if(m_media)
		delete m_media;

	if(m_mediaVideoFrame)
		delete m_mediaVideoFrame;
}

void Movie::DestroyReadBufferContainer()
{
	if(m_readBufferContainer)
	{
		m_readBufferContainer->Abort();
		delete m_readBufferContainer;
		m_readBufferContainer = NULL;
	}
}

void Movie::OnUserPrivateMessage(VBString sender, VBString message)
{
	//MovieCtcpMessage msg(message);

	//if(msg.IsMovieCtcpMessage())
	//{
	//	if(msg.IsReceptionCancel())
	//	{
	//		// An user has canceled the file reception.
	//		if(IsTransmitting())
	//		{
	//			// Notifying the kernel.
	//			MovieNotificationParameter param;
	//			param.m_moduleId = IPTV_MODULE_ID_WHITEBOARD;
	//			param.m_code = IMNC_MODULE_SPECIFIC;
	//			param.m_fileTransferCode = FTNC_SEND_RECEIVER_CANCEL;
	//			param.m_mediaId = m_transmissionData.m_mediaId;
	//			Notify(param);
	//		}
	//	}
	//	else if(msg.IsReceptionEnd())
	//	{
	//		// An user has received the file successfully.
	//		if(IsTransmitting())
	//		{
	//			// Notifying the kernel.
	//			MovieNotificationParameter param;
	//			param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
	//			param.m_code = IMNC_MODULE_SPECIFIC;
	//			param.m_fileTransferCode = FTNC_SEND_RECEIVER_END;
	//			param.m_mediaId = m_transmissionData.m_mediaId;
	//			Notify(param);
	//		}
	//	}
	//}
}

void Movie::OnChannelPrivateMessage(VBString sender, VBString channel, VBString message)
{
	//MovieCtcpMessage msg(message);

	//if(msg.IsMovieCtcpMessage())
	//{
	//	if(msg.IsTransmissionBegin())
	//	{
	//		// An user began transmitting a file.
	//		// This does not have to be handled.
	//		// Nothing to do.
	//	}

	//	else if(msg.IsTransmissionEnd())
	//	{
	//		// An user finished sending a file.
	//		if(IsReceiving())
	//			m_fileTransferProxy.NotifyEOF();
	//	}
	//}
}

void Movie::OnMediaNotify(VBString notification, long mediaId, VBString mediaClass,
								 VBString channel, VBString user)
{
	if(IsMediaClassMovie(mediaClass))
	{
		if(IsMediaNotificationSend(notification))
		{
			if(IsIdle())
			{
				Reset();
				SetState(MOVIE_STATE_RECEIVING);

				MediaConnectionData connInfo;
				connInfo = GetReceptionData();

				RequestMediaReceive(mediaId, connInfo.GetNetworkProtocolName(), connInfo.GetPort());

				// Storing information about the last notified file transfer transmission.
				m_receptionData.m_mediaId = mediaId;
				m_receptionData.m_channel = channel;
				m_receptionData.m_user = user;
			}
		}

		else if(IsMediaNotificationStart(notification))
		{
			// An user accepted the movie transmission.
			if(IsTransmitting())
			{
	//			// Notifying the kernel.
	//			MovieNotificationParameter param;
	//			param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
	//			param.m_code = IMNC_MODULE_SPECIFIC;
	//			param.m_fileTransferCode = FTNC_SEND_RECEIVER_NEW;
	//			param.m_mediaId = m_transmissionData.m_mediaId;
	//			Notify(param);
			}
		}

		else if(IsMediaNotificationStop(notification))
		{
			// An user stopped receiving a movie transmission.
			if(IsTransmitting())
			{
				// Nothing to do.
			}
		}
	}
}

void Movie::OnMediaReceive(	long mediaId, long port, VBString multiCastIp,
							long packetLength, long headerLength, VBString header,
							IrmUser user, IrmConnection conn)
{
	if(IsReceiving())
	{
//		MovieHeader fileTransferHeader(header);
//
		if(mediaId == m_receptionData.m_mediaId)
		{
			if(m_readBufferContainer)
				m_readBufferContainer->Abort();
			if(m_media)
				m_media->EndAllTransmissions();

			DestroyReadBufferContainer();
			m_readBufferContainer = new SockBuffs(1, MAX_PKT_COUNT * MAX_SEGMENT_SIZE);

			MediaConnectionData connInfo;
			m_receptionData.m_connectionData = GetReceptionData(conn.GetHost(), port, multiCastIp);

			m_readBufferContainer->SetUrl(	m_receptionData.m_connectionData.GetNetworkProtocolName(),
											m_receptionData.m_connectionData.GetHost(),
											m_receptionData.m_connectionData.GetPort(),
											NULL,
											FALSE,
											NULL);

			m_readBufferContainer->CreateWnd(m_receptionData.m_mediaId);
			m_readBufferContainer->SetWndMediaId(m_receptionData.m_mediaId, m_receptionData.m_mediaId);

			if(m_media)
				delete m_media;

			m_media = new CMedia();
			m_media->SetKernelNotify(this);

			m_readBufferContainer->OpenUrl();

			m_media->AddTransmission(m_receptionData.m_mediaId, TRUE, TRUE, (BYTE*)header.c_str(), headerLength);

			if(!connInfo.IsNetworkProtocolTcp())
				RequestMediaReceiveRun(m_receptionData.m_mediaId);

//
//			MovieBeginParameter param;
//			param.m_moduleId = IPTV_MODULE_ID_FILE_TRANSFER;
//			param.m_code = IMNC_MODULE_SPECIFIC;
//			param.m_fileTransferCode = FTNC_RECEIVE_BEGIN;
//			param.m_mediaId = m_receptionData.m_mediaId;
//			param.m_fileName = fileTransferHeader.GetFileName();
//			param.m_tempFilePath = filePath;
//			param.m_senderNick = m_receptionData.m_user;
//			param.m_fileSize = fileTransferHeader.GetFileSize();
//			param.m_packetSize = fileTransferHeader.GetPacketSize();
//			param.m_origin = fileTransferHeader.GetOrigin();
//			
//			Notify(param);
//
//			if(m_fileTransferProxy.BeginReceive(	m_receptionData.m_mediaId,
//													XStrDyn(connInfo.GetNetworkProtocolName().c_str()),
//													connInfo.GetHost(),
//													connInfo.GetPort(),
//													user.GetNick(),
//													m_receptionData.m_channel,
//													filePath,
//													fileTransferHeader.GetFileSize(),
//													(USHORT)fileTransferHeader.GetPacketSize(),
//													FALSE,
//													FALSE))
//			{
//				m_fileTransferProxy.RunRX();
//
//				if(!connInfo.IsNetworkProtocolTcp())
//					RequestMediaReceiveRun(m_receptionData.m_mediaId);
//			}
//
//			else
//			{
//				// FT error receive.
//				// Nothing to do.
//			}
		}
	}
}

void Movie::OnMediaReceiveRun(long mediaId)
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

void Movie::OnMediaReceiveStop(long mediaId)
{
	if(IsReceiving())
	{
		if(mediaId == m_receptionData.m_mediaId)
		{
			Reset();
		}
	}
}

void Movie::OnMediaReceiveAll(	VBString channel,
										VBString mediaClass,
										long port,
										VBString multiCastIp,
										IrmUser user,
										IrmConnection conn)
{
	// Movie does not handle this message.
}

void Movie::OnMediaReceiveAllRun(	VBString channel,
											VBString mediaClass)
{
	// Movie does not handle this message.
}

void Movie::OnMediaReceiveAllStop(	VBString channel,
											VBString mediaClass)
{
	// Movie does not handle this message.
}

void Movie::OnMediaSend(	long mediaId,
							br::com::sbVB::VBLib::VBString mediaClass,
							br::com::sbVB::VBLib::VBString channel,
							long port,
							IrmUser &user,
							IrmConnection &conn)
{
	if(IsMediaClassMovie(mediaClass))
	{
		if(IsTransmitting())
		{
			if(m_transmissionData.m_channel == channel)
			{
				// TODO: Initialize preview.
				//if(m_media)
				//	m_media->EndAllTransmissions();
	

				MediaConnectionData connData;
				connData = GetTransmissionData(conn.GetHost(), port, IRM_NETWORK_PROTOCOL_UDP);

				m_transmissionData.m_mediaId = mediaId;
				m_transmissionData.m_connectionData.SetNetworkProtocol(connData.GetNetworkProtocol());
				m_transmissionData.m_connectionData.SetHost(connData.GetHost());
				m_transmissionData.m_connectionData.SetPort(connData.GetPort());
				
				m_simposiumNetwork->SetId(m_transmissionData.m_mediaId);

				if(m_writeBuffer)
					delete m_writeBuffer;

				m_writeBuffer = new SockMWBuffs(0,
												0,
												6000,
												false,
												true,
												false);

				m_writeBuffer->SetUrl(	m_transmissionData.m_connectionData.GetNetworkProtocolName().c_str(),
										m_transmissionData.m_connectionData.GetHost().c_str(),
										m_transmissionData.m_connectionData.GetPort(),
										NULL,
										false,
										NULL);

				if(m_writeBuffer->OpenUrl())
				{
					if(m_simposiumNetwork->StartTransmission() == RET_OK)
					{
						IMNPMediaId parameter;
						parameter.m_code = IMNC_MOVIE_SEND_BEGIN;
						parameter.m_mediaId = m_transmissionData.m_mediaId;
						parameter.m_channel = m_transmissionData.m_channel;
						Notify(parameter);
					}

					else
					{
						// Notifying the server.
						RequestMediaSendStop(m_transmissionData.m_mediaId);
					}
				}

				else
				{
					// Notifying the server.
					RequestMediaSendStop(m_transmissionData.m_mediaId);
				}
			}
		}
	}
}

void Movie::OnMediaSendStop(long mediaId)
{
	if(IsTransmitting())
	{
		if(mediaId == m_transmissionData.m_mediaId)
		{
			m_simposiumNetwork->Terminate();
			Reset();

			IMNPMediaId parameter;
			parameter.m_code = IMNC_MOVIE_SEND_EOF;
			parameter.m_mediaId = m_transmissionData.m_mediaId;
			parameter.m_channel = m_transmissionData.m_channel;
			Notify(parameter);
		}
	}
}

void Movie::OnPacketRequest(long mediaId, unsigned long initialPacketIndex, unsigned long finalPacketIndex)
{
	// Movie does not handle this message.
}

ULONG Movie::OnHeader(BYTE *_pHeader, ULONG _ulHeaderLen)
{
	return 0;
}

ULONG Movie::OnPacket(BYTE *_pPacket, ULONG _ulPktLen)
{
	if(m_state == MOVIE_STATE_TRANSMITTING)
	{
		if(_ulPktLen > 0)
		{
			unsigned long dummy;
			m_writeBuffer->Write(_pPacket, _ulPktLen, &dummy);
		}
	}

	return 0;
}

ULONG Movie::OnEOF()
{
	// Notifying the server.
	RequestMediaSendStop(m_transmissionData.m_mediaId);

	return 0;
}

BOOL Movie::OnMediaNotify(MediaParam *_pParameters)
{
	if(IsIdle())
		return false;

    if (!_pParameters)
        return(FALSE);

	BOOL returnValue = FALSE;

    switch(_pParameters->code)
    {
        case MN_READ_PACKET:
		{
			if(IsReceiving())
			{
				unsigned long id = _pParameters->id;

				ISockBuff *sockReg = m_readBufferContainer->GetWndObj(id);

				if (sockReg)
				{
					unsigned char  data[MAX_PKT_SIZE];
					unsigned long dataLength = MAX_PKT_SIZE;

					if (sockReg->Read(data, dataLength, &dataLength) >= 0)
					{
						m_media->SetPacket(data, dataLength);
					}
				}
			}
        }
		break;

        case MN_VIDEO_ALIVE:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_VIDEO_ALIVE;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_processorNotify != NULL)
				if(m_processorNotify->OnMediaProcessorNotify(mediaProcessorParam))
					returnValue = TRUE;
		}
		break;

		case MN_AUDIO_ALIVE:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_AUDIO_ALIVE;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_processorNotify != NULL)
				if(m_processorNotify->OnMediaProcessorNotify(mediaProcessorParam))
					returnValue = TRUE;
		}
		break;
	}

	return(returnValue);
}

bool Movie::IsMovieTransmission(long mediaId)
{
	if(mediaId > 0)
		if(	(m_receptionData.m_mediaId == mediaId) ||
			(m_transmissionData.m_mediaId == mediaId))
			return true;

	return false;
}

bool Movie::BeginTransmission(VBString channel, VBString filePath, unsigned bitRate)
{
	if(IsIdle())
	{
		//if(m_simposiumNetwork)
		//	delete m_simposiumNetwork;

		//m_simposiumNetwork = new SimposiumNetwork();

		if(!m_simposiumNetwork)
			m_simposiumNetwork = new SimposiumNetwork();
		m_simposiumNetwork->SetSink(this);

		m_simposiumNetwork->SetFilename(filePath.c_str());
		BYTE* pHeader;
		ULONG uHeaderLen;
		
		if(m_simposiumNetwork->GetHeader(&pHeader, uHeaderLen) != RET_OK)
			return false;
		long t = strlen((const char *)pHeader);

		Reset();
		SetState(MOVIE_STATE_TRANSMITTING);

		MediaConnectionData connData;
		connData = GetTransmissionData(IRM_NETWORK_PROTOCOL_UDP);

		m_transmissionData.m_channel = channel;
		m_transmissionData.m_filePath = filePath;
		m_transmissionData.m_bitRate = bitRate;
		m_transmissionData.m_connectionData.SetNetworkProtocol(connData.GetNetworkProtocol());

		RequestMediaSend(	m_transmissionData.m_channel,
							m_irmMediaClassMovie,
							m_transmissionData.m_connectionData.GetNetworkProtocolName(),
							m_transmissionData.m_bitRate,
							uHeaderLen,
							(const char *)pHeader);
		return true;
	}

	return false;
}

bool Movie::CancelTransmission()
{
	if(m_state == MOVIE_STATE_TRANSMITTING)
	{
		// Notifying the server.
		RequestMediaSendStop(m_transmissionData.m_mediaId);

		return true;
	}

	return false;
}

bool Movie::CancelReception()
{
	if(m_readBufferContainer)
		m_readBufferContainer->Abort();
	if(m_media)
		m_media->EndAllTransmissions();

	return true;
}

bool Movie::GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
										unsigned long *width, unsigned long *height)
{
	if(IsIdle())
		return false;
	// ATTENTION : this method don't need to be locked because it is only called from the application
	// thread and the application only calls it once at a time.
	if(m_media)
	{
		VideoFrame_reg * videoFrameReg;
		if(m_media->GetVideoReg(id, &videoFrameReg) == RET_OK)
		{
			if(!videoFrameReg)
				return false;	// Return false if we don't have the frame reg
	
			if(!(videoFrameReg->NewData()))
				return false; // Only update if there is a new frame
	
			// CVideoFrame should exists until application asks for another frame because when
			// CVideoFrame is destroyed the data pointer becomes invalid
			if(m_mediaVideoFrame)
				delete m_mediaVideoFrame;

			m_mediaVideoFrame = new CVideoFrame();
	
			// Read video frame data
			videoFrameReg->ReadData(*m_mediaVideoFrame);
	
			// Bail out if our frame isn't OK
			if(!m_mediaVideoFrame->FrameOk())
				return false;
			// Else, all went OK, get the video data and dimensions and return true
			m_mediaVideoFrame->GetFrameDimensions(width, height);
			m_mediaVideoFrame->GetFrameData(videoFrame, NULL);

			return true;
		}
	}
	return false;
}

bool Movie::IsIdle() const
{
	if(m_state == MOVIE_STATE_IDLE)
		return true;
	return false;
}

bool Movie::IsReceiving() const
{
	if(m_state == MOVIE_STATE_RECEIVING)
		return true;
	return false;
}

bool Movie::IsTransmitting() const
{
	if(m_state == MOVIE_STATE_TRANSMITTING)
		return true;
	return false;
}

void Movie::SetState(MovieState state)
{
	m_state = state;
}

unsigned Movie::GetMediaPort() const
{
	return m_movieMediaPort;
}

void Movie::Reset()
{
	m_receptionData.Reset();
	m_transmissionData.Reset();
	SetState(MOVIE_STATE_IDLE);
}

MovieData::MovieData()
{
	Reset();
}

void MovieData::Reset()
{
	m_mediaId = 0;
	m_connectionData.SetNetworkProtocol(IRM_NETWORK_PROTOCOL_INVALID);
	m_connectionData.SetHost("");
	m_connectionData.SetPort(0);
	m_channel = "";
	m_user = "";
	m_filePath = "";
	m_bitRate = 0;
}
