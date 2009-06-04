#include "iptv_kernel/IPTVKernel.h"
#include "iptv_kernel/FileTransferNotificationParameters.h"
#include "iptv_kernel/FileTransferOrigin.h"
#include "iptv_kernel/PollNotificationParameters.h"
#include "iptv_kernel/WhiteboardNotificationParameters.h"
#include "iptv_kernel/ApplicationSharingNotificationParameters.h"

#include <iostream>
#include <locale.h>
using namespace std;

using namespace br::com::sbVB::VBLib;

IPTVKernel::IPTVKernel(KernelNotify *notify) :	m_fileMediaWriteConference(NULL),
												m_conferenceRecordingEnabled(false),
												m_videoEncoderEnabled(false),
												m_movie(this, this)
{
	m_notify = notify;
	m_irmQuery.SetNotify(this);
	m_mediaTransceiver.SetNotify(this);
	m_mediaCapture.SetNotify(this);
	m_fileTransfer.SetNotificationSink(this);
	m_poll.SetNotificationSink(this);
	m_whiteboard.SetNotificationSink(this);
	m_mediaConference.SetNotificationSink(this);
	m_appSharing.SetNotificationSink(this);
	m_movie.SetNotificationSink(this);

#ifdef IPTV_USING_MEDIA
	m_mediaProcessor.SetNotify(this);
#endif

	m_isConferenceViewerRunning = false;
	m_videoCaptureEnable = false;
	m_mediaColaborationEnable = false;

	m_handleOwnTransmission = false;
	m_mediaConferenceMediaId = 0;
}

IPTVKernel::~IPTVKernel()
{

}

void IPTVKernel::SetNotify(KernelNotify *notify)
{
	m_notify = notify;

	// Workaround for the locale problem with Lua and decimal separators.
	// The first call to setlocale() queries the system locale for numbers.
	// It then sets it to the POSIX locale "C" before initializing Lua,
	// initializes it, and sets the locale back to the value obtained in the
	// first setlocale() call.
	char *currentLocale = setlocale(LC_NUMERIC, NULL);
	std::cout << "System locale setting for numbers: " << currentLocale << std::endl;
	setlocale(LC_NUMERIC, "C");
	deque<string> dummy;
	m_mediaProcessor.GetAudioEncoders(dummy);
	setlocale(LC_NUMERIC, currentLocale);
}

VBString IPTVKernel::GetCoreVersion()
{
	return VBString(CORE_VERSION_ID);
}

void IPTVKernel::NotifyIrmMode(IrmQueryParam &param, KernelNotifyCode notificationCode)
{
	KernelModeParam kernelParam;
	IrmQueryModeParam &modeParam = (IrmQueryModeParam &)param;
	kernelParam.m_code = notificationCode;
	kernelParam.m_name = modeParam.m_name;
	kernelParam.m_value = modeParam.m_value;
	kernelParam.m_target = modeParam.m_target;
	kernelParam.m_parameter = modeParam.m_parameter;

	m_notify->OnKernelNotify(kernelParam);
}

void IPTVKernel::OnIPTVModuleNotification(IPTVModuleNotificationParameter &parameter)
{
	switch(parameter.m_code)
	{
		case IMNC_SEND_PRIVATE_MESSAGE:
		{
			IMNPPrivateMessage *p = (IMNPPrivateMessage *) &parameter;
			m_irmQuery.SendPrivateMessage(p->m_recipient, p->m_message);
		}
		break;
		case IMNC_SEND_CHANNEL_PRIVATE_MESSAGE:
		{
			IMNPPrivateMessage *p = (IMNPPrivateMessage *) &parameter;
			m_irmQuery.SendChannelPrivateMessage(p->m_recipient, p->m_message);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE:
		{
			IMNPMediaReceive *p = (IMNPMediaReceive *) &parameter;
			m_irmQuery.RequestMediaReceive(p->m_mediaId, p->m_networkProtocol, p->m_port);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE_RUN:
		{
			IMNPMediaId *p = (IMNPMediaId *) &parameter;
			m_irmQuery.RequestMediaReceiveRun(p->m_mediaId);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE_STOP:
		{
			IMNPMediaId *p = (IMNPMediaId *) &parameter;
			m_irmQuery.RequestMediaReceiveStop(p->m_mediaId);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE_ALL:
		{	
			IMNPMediaReceiveAll *p = (IMNPMediaReceiveAll *) &parameter;
			m_irmQuery.RequestMediaReceiveAll(p->m_channel, p->m_mediaClass, p->m_networkProtocol, p->m_port);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE_ALL_RUN:
		{	
			IMNPMediaReceiveAll *p = (IMNPMediaReceiveAll *) &parameter;
			m_irmQuery.RequestMediaReceiveAllRun(p->m_channel, p->m_mediaClass);
		}
		break;
		case IMNC_REQUEST_MEDIA_RECEIVE_ALL_STOP:
		{	
			IMNPMediaReceiveAll *p = (IMNPMediaReceiveAll *) &parameter;
			m_irmQuery.RequestMediaReceiveAllStop(p->m_channel, p->m_mediaClass);			
		}
		break;
		case IMNC_REQUEST_MEDIA_SEND:
		{
			IMNPMediaSend *p = (IMNPMediaSend *) &parameter;
			m_irmQuery.RequestMediaSend(p->m_channel, p->m_mediaClass, p->m_networkProtocol,
										p->m_bitRate, p->m_headerSize, p->m_header);
		}
		break;
		case IMNC_REQUEST_MEDIA_SEND_STOP:
		{
			IMNPMediaId *p = (IMNPMediaId *) &parameter;
			m_irmQuery.RequestMediaSendStop(p->m_mediaId);
		}
		break;
		case IMNC_REQUEST_MEDIA_PACKET:
		{	
			IMNPMediaPacket *p = (IMNPMediaPacket *) &parameter;
			m_irmQuery.RequestMediaPacket(p->m_mediaId, p->m_initialPacketIndex, p->m_finalPacketIndex);
		}
		break;
		case IMNC_REQUEST_CHANNEL_WHITEBOARD:
		{	
			IMNPWhiteboard *p = (IMNPWhiteboard *) &parameter;
			m_irmQuery.RequestChannelWhiteboard(p->m_channel, p->m_data);
		}
		break;
		case IMNC_MOVIE_SEND_BEGIN:
		{	
			IMNPMediaId *p = (IMNPMediaId *) &parameter;
			KernelMediaIDParam kernelParam;

			kernelParam.m_code = KNC_MOVIE_SEND_BEGIN;
			kernelParam.m_mediaID = p->m_mediaId;
			kernelParam.m_channelName = p->m_channel;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IMNC_MOVIE_SEND_EOF:
		{	

			IMNPMediaId *p = (IMNPMediaId *) &parameter;
			KernelMediaIDParam kernelParam;

			kernelParam.m_code = KNC_MOVIE_SEND_EOF;
			kernelParam.m_mediaID = p->m_mediaId;
			kernelParam.m_channelName = p->m_channel;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IMNC_REPLY_MEDIA_SEND:
		{	
			IMNPMediaSendReply *p = (IMNPMediaSendReply *) &parameter;
			long mediaId = p->m_mediaId;

			// The member m_mediaConferenceMediaId is used to block the own tranmission in musticast.
			m_mediaConferenceMediaId = mediaId;

			// Set colaboration mediaID
			m_mediaProcessor.SetMediaID(mediaId);

			// Get video encoder configurations and set video capture configurations
			double framerate;
			unsigned width;
			unsigned height;
			m_mediaProcessor.GetVideoConfigurations(framerate, width, height);
			if(!m_mediaCapture.SetVideoCaptureConfigurations(framerate, width, height))
			{
				// TODO: call IRM method to stop media send
				return;
			}

			// Get audio encoder configurations and set audio capture configurations
			unsigned frameSamples;
			unsigned framesInPacket;
			unsigned sampleRate;
			unsigned channels;
			unsigned sampleSize;
			m_mediaProcessor.GetAudioConfigurations(frameSamples, framesInPacket, sampleRate, channels, sampleSize);
			if(!m_mediaCapture.SetAudioCaptureConfigurations(frameSamples, framesInPacket, sampleRate, channels, sampleSize))
			{
				// TODO: call IRM method to stop media send
				return;
			}

			// Initialize Transmission
			if(!m_mediaTransceiver.InitializeTransmission(p->m_networkProtocol, p->m_host, p->m_port, NULL, FALSE))
			{
				// TODO: call IRM method to stop media send
				return;
			}

			StartConferenceRecording();

			// Initialize audio capture device. Video capture is independent and controled by preview button.
			if(!m_mediaCapture.InitializeAudioCapture(m_colaborationAudioDevice))
			{
				// TODO: call IRM method to stop media send

				// Stop and destroy transmission
				m_mediaTransceiver.StopTransmission();
				m_mediaTransceiver.DestroyTransmission();
				return;
			}

			// Enable media colaboration
			m_mediaColaborationEnable = true;
		}
		break;
		case IMNC_MODULE_SPECIFIC:
		{
			switch(parameter.m_moduleId)
			{
				case IPTV_MODULE_ID_FILE_TRANSFER:
				{
					FileTransferNotificationParameter *f = (FileTransferNotificationParameter *) &parameter;
					switch(f->m_fileTransferCode)
					{
						case FTNC_RECEIVE_BEGIN:
						{
							FileTransferBeginParameter *b = (FileTransferBeginParameter *) &parameter;
							KernelFTReceiveBeginParam fileTransferParam;
							if(b->m_origin == FT_ORIGIN_WHITE_BOARD)
								fileTransferParam.m_code = KNC_FT_RECEIVE_WB_BEGIN;
							else if(b->m_origin == FT_ORIGIN_SEND_FILES)
								fileTransferParam.m_code = KNC_FT_RECEIVE_BEGIN;
							fileTransferParam.m_mediaId = b->m_mediaId;
							fileTransferParam.m_fileName = b->m_fileName;
							fileTransferParam.m_tempFilePath = b->m_tempFilePath;
							fileTransferParam.m_senderNick = b->m_senderNick;
							fileTransferParam.m_fileSize = b->m_fileSize;
							fileTransferParam.m_packetSize = b->m_packetSize;
							m_notify->OnKernelNotify(fileTransferParam);
						}
						break;
						case FTNC_RECEIVE_PROGRESS:
						{
							FileTransferProgressParameter *b = (FileTransferProgressParameter *) &parameter;
							KernelFTProgressParam fileTransferParam;
							fileTransferParam.m_code = KNC_FT_RECEIVE_PROGRESS;
							fileTransferParam.m_currentPacketIndex= b->m_currentPacketIndex;
							fileTransferParam.m_lastPacketIndex = b->m_lastPacketIndex;
							m_notify->OnKernelNotify(fileTransferParam);
						}
						break;
						case FTNC_RECEIVE_LOST_PACKET:
						{
							FileTransferLostPacketParameter *b = (FileTransferLostPacketParameter *) &parameter;
							KernelFTLostPacketParam fileTransferParam;
							fileTransferParam.m_code = KNC_FT_RECEIVE_LOST_PACKET;
							fileTransferParam.m_lostPackets = b->m_lostPackets;
							m_notify->OnKernelNotify(fileTransferParam);
						}
						break;
						case FTNC_RECEIVE_SUCCESS:
						{
							KernelParam kParam;
							kParam.m_code = KNC_FT_RECEIVE_SUCCESS;	
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_RECEIVE_ERROR:
						{
							KernelParam kParam;
							kParam.m_code = KNC_FT_RECEIVE_ERROR;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_BEGIN:
						{
							FileTransferNotificationParameter *b = (FileTransferNotificationParameter *) &parameter;
							KernelMediaIDParam kParam;
							kParam.m_code = KNC_FT_SEND_BEGIN;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_PROGRESS:
						{
							FileTransferProgressParameter *b = (FileTransferProgressParameter *) &parameter;
							KernelFTProgressParam fileTransferParam;
							fileTransferParam.m_code = KNC_FT_SEND_PROGRESS;
							fileTransferParam.m_currentPacketIndex= b->m_currentPacketIndex;
							fileTransferParam.m_lastPacketIndex = b->m_lastPacketIndex;
							m_notify->OnKernelNotify(fileTransferParam);
						}
						break;
						case FTNC_SEND_LOST_PACKET:
						{
							FileTransferLostPacketParameter *b = (FileTransferLostPacketParameter *) &parameter;
							KernelFTLostPacketParam fileTransferParam;
							fileTransferParam.m_code = KNC_FT_SEND_LOST_PACKET;
							fileTransferParam.m_lostPackets = b->m_lostPackets;
							fileTransferParam.m_lostPacketsTotal = b->m_lostPacketsTotal;
							m_notify->OnKernelNotify(fileTransferParam);
						}
						break;
						case FTNC_SEND_EOF:
						{
							KernelParam kParam;
							kParam.m_code = KNC_FT_SEND_EOF;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_SUCCESS:
						{
							KernelParam kParam;
							kParam.m_code = KNC_FT_SEND_SUCCESS;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_ERROR:
						{
							KernelParam kParam;
							kParam.m_code = KNC_FT_SEND_ERROR;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_RECEIVER_NEW:
						{
							FileTransferNotificationParameter *b = (FileTransferNotificationParameter *) &parameter;
							KernelMediaIDParam kParam;
							kParam.m_code = KNC_FT_SEND_RECEIVER_NEW;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_RECEIVER_CANCEL:
						{
							FileTransferNotificationParameter *b = (FileTransferNotificationParameter *) &parameter;
							KernelMediaIDParam kParam;
							kParam.m_code = KNC_FT_SEND_RECEIVER_CANCEL;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case FTNC_SEND_RECEIVER_END:
						{
							FileTransferNotificationParameter *b = (FileTransferNotificationParameter *) &parameter;
							KernelMediaIDParam kParam;
							kParam.m_code = KNC_FT_SEND_RECEIVER_END;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
					}
				}
				break;
				case IPTV_MODULE_ID_POLL:
				{
					PollNotificationParameter *a = (PollNotificationParameter *) &parameter;
					switch(a->m_pollCode)
					{
						case POLLNC_BEGIN:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_BEGIN;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_QUESTION:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_QUESTION;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_text = b->m_text;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_OPTION:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_OPTION;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_index = b->m_index;
							kParam.m_text = b->m_text;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_END:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_END;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_ANSWER:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_ANSWER;
							kParam.m_userName = b->m_user;
							kParam.m_index = b->m_index;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_STATS_BEGIN:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_STATS_BEGIN;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_STATS_QUESTION:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_STATS_QUESTION;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_text = b->m_text;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_STATS_OPTION:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_STATS_OPTION;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_index = b->m_index;
							kParam.m_answerCount = b->m_answerCount;
							kParam.m_text = b->m_text;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case POLLNC_STATS_END:
						{
							PollParameter *b = (PollParameter *) &parameter;
							KernelPollParam kParam;
							kParam.m_code = KNC_POLL_STATS_END;
							kParam.m_userName = b->m_user;
							kParam.m_channelName = b->m_channel;
							m_notify->OnKernelNotify(kParam);
						}
						break;
					}
				}
				break;
				case IPTV_MODULE_ID_WHITEBOARD:
				{
					WhiteboardNotificationParameter *a = (WhiteboardNotificationParameter *) &parameter;
					switch(a->m_whiteboardCode)
					{
						case WBNC_MESSAGE:
						{
							WhiteboardMessage *b = (WhiteboardMessage *) &parameter;
							KernelWBMessageParam kParam;
							kParam.m_code = KNC_WB_MESSAGE;
							kParam.m_userName = b->m_sender;
							kParam.m_channelName = b->m_channel;
							kParam.m_data = b->m_data;
							m_notify->OnKernelNotify(kParam);
						}
						break;
					}
				}
				break;
				case IPTV_MODULE_ID_APP_SHARING:
				{
					ASNotificationParameter *a = (ASNotificationParameter *) &parameter;
					switch(a->m_appSharingCode)
					{
						case ASNC_RECEIVE_INIT:
						{
							ASReceiveInit *b = (ASReceiveInit *) &parameter;

							bool listen = false;

							// only listen if it uses UDP protocol
							if(b->m_networkProtocol != "TCP")
								listen = true;

							// concatenate connStr parameters
							VBString connStr = "MEDIARCVALLRUN #";
							connStr += b->m_channel;
							connStr += " ";
							connStr += b->m_mediaClass;
							connStr += " ";
							connStr += b->m_user;

							m_appSharingTransceiver.InitializeReception(b->m_networkProtocol.c_str(), b->m_host.c_str(), b->m_port, connStr.c_str(), listen);
							StartConferenceRecording();
							ISockBuff *buff;
							buff = m_appSharingTransceiver.CreateAppSharingBuffer(b->m_mediaId);
							if(buff != NULL)
								m_appSharing.InitializeReception(b->m_mediaId, buff);
							else
								m_appSharing.StopReception();
						}
						break;
						case ASNC_RECEIVE_BEGIN:
						{
							ASNotificationParameter *b = (ASNotificationParameter *) &parameter;

							KernelMediaIDParam kParam;
							kParam.m_code = KNC_AS_RECEIVE_BEGIN;
							kParam.m_userNickName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case ASNC_RECEIVE_END:
						{
							m_appSharingTransceiver.AbortReception();
							m_appSharing.StopReception();
							m_appSharingTransceiver.DestroyReception();

							ASNotificationParameter *b = (ASNotificationParameter *) &parameter;

							KernelMediaIDParam kParam;
							kParam.m_code = KNC_AS_RECEIVE_END;
							kParam.m_userNickName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case ASNC_RECEIVE_ERROR:
						{
							ASNotificationParameter *b = (ASNotificationParameter *) &parameter;

							KernelMediaIDParam kParam;
							kParam.m_code = KNC_AS_RECEIVE_ERROR;
							kParam.m_userNickName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_mediaID = b->m_mediaId;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case ASNC_RECEIVE_NEW_FRAME:
						{
							ASReceiveFrame *b = (ASReceiveFrame *) &parameter;

							KernelASFrameParam kParam;
							kParam.m_code = KNC_AS_RECEIVE_NEW_FRAME;
							kParam.m_userNickName = b->m_user;
							kParam.m_channelName = b->m_channel;
							kParam.m_mediaID = b->m_mediaId;
							kParam.m_buffer = b->m_buffer;
							kParam.m_bufferlength = b->m_bufferlength;
							kParam.m_width = b->m_width;
							kParam.m_height = b->m_height;
							kParam.m_format = b->m_format;
							m_notify->OnKernelNotify(kParam);
						}
						break;
						case ASNC_RECEIVE_ABORT:
						{
							m_appSharingTransceiver.AbortReception();
						}
						break;
					}
				}
				break;
			}
		}
		break;
	}
}

void IPTVKernel::OnIrmQueryNotify(IrmQueryParam &param)
{
	switch (param.m_code)
	{
		case IQNC_IRM_MESSAGE:
		{
			IrmQueryIrmMessageParam &msgParam = *(IrmQueryIrmMessageParam *)&param;

			if(m_mediaConference.IsEnabled())
				m_mediaConference.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);

			if(m_fileTransfer.IsEnabled())
				m_fileTransfer.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);

			if(m_poll.IsEnabled())
				m_poll.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);

			if(m_whiteboard.IsEnabled())
				m_whiteboard.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);

			m_appSharing.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);

			m_movie.OnIrmMessage(msgParam.m_message, msgParam.m_user, msgParam.m_connection);
		}
		break;
		case IQNC_CONNECTED_HOST:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_CONNECTED_HOST;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		                        
		case IQNC_CONNECTED_CHAT:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_CONNECTED_CHAT;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		                        
		case IQNC_DISCONNECTED:
		{
			IPTVMediaModule::SetHttpTunnelingOff();
			IPTVMediaModule::SetUdpConnectionUnavailable();

			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_DISCONNECTED;
			kernelParam.m_kernelMessage = param.m_irmMessage;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_UDP_CONNECTION_AVAILABLE:
		{
			IPTVMediaModule::SetUdpConnectionAvailable();
		}
		break;	
		case IQNC_AUTHENTICATED:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_AUTHENTICATED;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		     
		case IQNC_AUTHENTICATION_REQUESTED:
		{
			m_irmQuery.RequestAuthentication();
		}
		break;
		case IQNC_AUTHENTICATION_ERROR:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_AUTHENTICATION_ERROR;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_USER_QUIT:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_USER_QUIT;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;	
		case IQNC_CHANNEL_JOIN:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_JOIN;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		                        
		case IQNC_CHANNEL_PART:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_PART;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_CHANNEL_QUERY_ITEM:
		{
			KernelChannelParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_QUERY_ITEM;
			kernelParam.m_channelName = (*(IrmQueryChannelParam *)&param).m_channelName;
			kernelParam.m_channelTopic = (*(IrmQueryChannelParam *)&param).m_channelTopic;
			kernelParam.m_userCount = (*(IrmQueryChannelParam *)&param).m_userCount;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		                        
		case IQNC_CHANNEL_QUERY_END:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_QUERY_END;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_CHANNEL_PASSWD_INVALID:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_PASSWD_INVALID;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_USER_QUERY_ITEM:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_USER_QUERY_ITEM;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;		                        
		case IQNC_USER_QUERY_END:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_USER_QUERY_END;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = "";
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_PRIV_MSG:
		{
			KernelMessageParam kernelParam;
			kernelParam.m_code = KNC_IRM_PRIV_MSG;
			kernelParam.m_sender = (*(IrmQueryMessageParam *)&param).m_sender;
			kernelParam.m_channelName = (*(IrmQueryMessageParam *)&param).m_channelName;
			kernelParam.m_message = (*(IrmQueryMessageParam *)&param).m_message;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_CHANNEL_PRIV_MSG:
		{
			KernelMessageParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_PRIV_MSG;
			kernelParam.m_sender = (*(IrmQueryMessageParam *)&param).m_sender;
			kernelParam.m_channelName = (*(IrmQueryMessageParam *)&param).m_channelName;
			kernelParam.m_message = (*(IrmQueryMessageParam *)&param).m_message;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_CHANNEL_VOICE:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_CHANNEL_VOICE;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_VOICE_REQUEST:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_VOICE_REQUEST;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_VOICE_REQUEST_CANCEL:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_VOICE_REQUEST_CANCEL;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_VOICE_REQUEST_REMOVE_ALL:
		{
			KernelUserParam kernelParam;
			kernelParam.m_code = KNC_IRM_VOICE_REQUEST_REMOVE_ALL;
			kernelParam.m_channelName = (*(IrmQueryUserParam *)&param).m_channelName;
			kernelParam.m_userName = (*(IrmQueryUserParam *)&param).m_userName;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_CHANNEL_MODE_OPERATOR:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_OPERATOR);
			break;
		case IQNC_CHANNEL_MODE_PRIVATE:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_PRIVATE);
			break;
		case IQNC_CHANNEL_MODE_SECRET:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_SECRET);
			break;
		case IQNC_CHANNEL_MODE_INVITE_ONLY:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_INVITE_ONLY);
			break;
		case IQNC_CHANNEL_MODE_OP_CHANGE_TOPIC:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_OP_CHANGE_TOPIC);
			break;
		case IQNC_CHANNEL_MODE_NO_EXTERNAL_MESSAGES:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES);
			break;
		case IQNC_CHANNEL_MODE_MODERATED:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_MODERATED);
			break;
		case IQNC_CHANNEL_MODE_USER_LIMIT:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_USER_LIMIT);
			break;
		case IQNC_CHANNEL_MODE_BAN:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_BAN);
			break;
		case IQNC_CHANNEL_MODE_MEDIA_COLLABORATOR:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_MEDIA_COLLABORATOR);
			break;
		case IQNC_CHANNEL_MODE_KEY:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_KEY);
			break;
		case IQNC_CHANNEL_MODE_BIT_RATE:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_BIT_RATE);
			break;
		case IQNC_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE);
			break;
		case IQNC_CHANNEL_MODE_MULTIPLE_TRANSMISSION:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION);
			break;
		case IQNC_CHANNEL_MODE_ONE_AUDIO:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_ONE_AUDIO);
			break;
		case IQNC_CHANNEL_MODE_AUDIO_MUTE:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_AUDIO_MUTE);
			break;
		case IQNC_CHANNEL_MODE_BAND_SHARE_LIMIT:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_BAND_SHARE_LIMIT);
			break;
		case IQNC_CHANNEL_MODE_TRANSMISSION_LIMIT:
			NotifyIrmMode(param, KNC_IRM_CHANNEL_MODE_TRANSMISSION_LIMIT);
			break;                                                                    
		case IQNC_USER_MODE_INVISIBLE:
			NotifyIrmMode(param, KNC_IRM_USER_MODE_INVISIBLE);
			break;
		case IQNC_USER_MODE_REGISTERED:
			NotifyIrmMode(param, KNC_IRM_USER_MODE_REGISTERED);
			break;
		case IQNC_USER_MODE_CAMERA_ON:
			NotifyIrmMode(param, KNC_IRM_USER_MODE_CAMERA_ON);
			break;
		case IQNC_USER_MODE_MIC_MUTE:
			NotifyIrmMode(param, KNC_IRM_USER_MODE_MIC_MUTE);
			break;
		case IQNC_USER_MODE_AWAY:
			NotifyIrmMode(param, KNC_IRM_USER_MODE_AWAY);
			break;
		case IQNC_NEW_MEDIA_TRANSMISSION:
		{
			VBString mediaClass = (*(IrmQueryMediaParam *)&param).m_mediaClass;

			// if(mediaClass == IRM_MEDIACLASS_AVCONF)
			{
				KernelMediaIDParam kernelParam;
				kernelParam.m_code = KNC_NEW_CONFERENCE_TRANSMISSION;
				kernelParam.m_mediaID = (*(IrmQueryMediaParam *)&param).m_mediaId;
				kernelParam.m_channelName = (*(IrmQueryMediaParam *)&param).m_channelName;
				kernelParam.m_userNickName = (*(IrmQueryMediaParam *)&param).m_userNickName;
				m_notify->OnKernelNotify(kernelParam);	
			}
		}
		break;
		case IQNC_MEDIA_CONFERENCE_READY:
		{
			VBString host = (*(IrmQueryMediaParam *)&param).m_host;
			VBString netProtocol = (*(IrmQueryMediaParam *)&param).m_netProtocol;
			unsigned port = (*(IrmQueryMediaParam *)&param).m_port;
			bool listen = false;

			// only listen if it uses UDP protocol
			if(netProtocol != "TCP")
				listen = true;

			// concatenate connStr parameters
			VBString connStr = "MEDIARCVALLRUN #";
			connStr += (*(IrmQueryMediaParam *)&param).m_channelName;
			connStr += " ";
			connStr += (*(IrmQueryMediaParam *)&param).m_mediaClass;
			connStr += " ";
			connStr += (*(IrmQueryMediaParam *)&param).m_userNickName;

#ifdef IPTV_USING_MEDIA
			// This is only done if the media library is being used, which is done by
			// defining the IPTV_USING_MEDIA macro.

			// Initialize network and MediaProcessor.
			m_mediaProcessor.Initialize();
			if(!m_mediaTransceiver.InitializeReception(netProtocol.c_str(), host.c_str(), port, connStr,listen))
				m_mediaProcessor.Destroy();

			StartConferenceRecording();
#else
			// Initialize network.
			m_mediaTransceiver.OpenTransmission(netProtocol.c_str(), host.c_str(), port, connStr,listen);
#endif
		}
		break;
		case IQNC_MEDIA_VIEWER_READY:
		{
			VBString mediaClass = (*(IrmQueryMediaParam *)&param).m_mediaClass;

			if(m_irmQuery.IsMediaClassConference(mediaClass))
			{
				if(!m_isConferenceViewerRunning)
				{
					m_isConferenceViewerRunning = true;

					#ifdef IPTV_USING_MEDIA
					// This is only done if the media library is being used, which is
					// accomplished by defining the IPTV_USING_MEDIA macro.
					m_mediaProcessor.Initialize();
					#endif

					VBString host = (*(IrmQueryMediaParam *)&param).m_host;
					VBString netProtocol = (*(IrmQueryMediaParam *)&param).m_netProtocol;
					unsigned port = (*(IrmQueryMediaParam *)&param).m_port;

					if(!(m_mediaTransceiver.InitializeReception(netProtocol.c_str(), host.c_str(), port, NULL, TRUE)))
					{
						#ifdef IPTV_USING_MEDIA
						// This is only done if the media library is being used, which is done by
						// defining the IPTV_USING_MEDIA macro.
						m_mediaProcessor.Destroy();
						#endif
					}
				}
			}
		}
		break;
		case IQNC_ERROR_IRM_STUB_INIT:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_ERROR_IRM_STUB_INIT;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_ERROR_CONN_PARAM_BLANK:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_ERROR_IRM_CONN_PARAM_BLANK;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_ERROR_CONN_QUERY:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_ERROR_IRM_CONN_QUERY;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
		case IQNC_ERROR_CONN_STATE:
		{
			KernelParam kernelParam;
			kernelParam.m_code = KNC_ERROR_IRM_CONN_STATE;
			m_notify->OnKernelNotify(kernelParam);
		}
		break;
	}
}

/** Receives notifications from Media Transceiver.
*
*/
bool IPTVKernel::OnMediaTransceiverNotify(MediaTransceiverParam &param)
{
	switch(param.m_code)
	{
		case MTNC_NEW_TRANSMISSION:
		{
			unsigned long id = (*(MediaTransceiverIDParam *)&param).m_mediaID;
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_TRCV_NEW_TRANSMISSION;
			kernelMediaIDParam.m_mediaID = id;

#ifdef IPTV_USING_MEDIA
			// This is only done if the media library is being used, which is done by
			// defining the IPTV_USING_MEDIA macro. Both the MediaProcessor and the
			// application are notified of the MediaTransceiver new transmission event.

			if(!m_handleOwnTransmission)
			{
				// Blocking own transmission.
				if(id == m_mediaConferenceMediaId)
					return true;
			}

			if(id == m_appSharing.GetReceptionMediaId())
			{
				return true;
			}

			// Notify MediaProcessor that new transmission arrived in MediaTransceiver.
			if(m_mediaProcessor.OnKernelNotify(kernelMediaIDParam))
			{
				// Notify the application that new transmission arrived in MediaTransceiver.
				if(m_notify->OnKernelNotify(kernelMediaIDParam))
					return(true);
			}

			// The return value should always be true, as seen in the IP.TV-VAT client code.
			// Return false causes strange behaviour.
			return(true);
#else
			// If the media library is not being used, only the application
			// is notified of the MediaTransceiver new transmission event.

			if(!m_handleOwnTransmission)
			{
				// Blocking own transmission.
				if(id == m_mediaConferenceMediaId)
					return true;
			}

			// Notify the application that new transmission arrived in MediaTransceiver.
			if(m_notify->OnKernelNotify(kernelMediaIDParam))
				return(true);
			return(true);
#endif
		}
		break;
		case MTNC_DATA_READ_MEDIA:
		{
#ifdef IPTV_USING_MEDIA
			// This is only done if the media library is being used, which is done by
			// defining the IPTV_USING_MEDIA macro.

			KernelMediaDataParam kernelMediaDataParam;

			kernelMediaDataParam.m_code = KNC_TRCV_DATA_READ_MEDIA;
			kernelMediaDataParam.m_data = (*(MediaTransceiverDataParam *)&param).m_data;
			kernelMediaDataParam.m_dataLength = (*(MediaTransceiverDataParam *)&param).m_dataLength;

			// Notify MediaProcessor that a data packet was read in MediaTransceiver.
			if(m_mediaProcessor.OnKernelNotify(kernelMediaDataParam))
				return(true);
#endif
			return(false);
		}
		break;
		case MTNC_DATA_READ_APP:
		{
			KernelMediaDataParam kernelMediaDataParam;

			kernelMediaDataParam.m_code = KNC_TRCV_DATA_READ_APP;
			kernelMediaDataParam.m_data = (*(MediaTransceiverDataParam *)&param).m_data;
			kernelMediaDataParam.m_dataLength = (*(MediaTransceiverDataParam *)&param).m_dataLength;

			// Notify the application that a data packet was read in MediaTransceiver.
			if(m_notify->OnKernelNotify(kernelMediaDataParam))
				return(true);

			return(false);
		}
		break;
		case MTNC_NOTIFY_PROGRESS:
		{
			KernelMediaProgressParam kernelMediaProgressParam;

			kernelMediaProgressParam.m_code = KNC_TRCV_NOTIFY_PROGRESS;
			kernelMediaProgressParam.m_mediaID = (*(MediaTransceiverProgressParam *)&param).m_mediaID;
			kernelMediaProgressParam.m_percent = (*(MediaTransceiverProgressParam *)&param).m_percent;
			kernelMediaProgressParam.m_flags = (*(MediaTransceiverProgressParam *)&param).m_flags;

			// Notify the application that a data packet was read in MediaTransceiver.
			if(m_notify->OnKernelNotify(kernelMediaProgressParam))
				return(true);

			return(false);
		}
		break;
		default:
		{
			return(false);
		}
		break;
	}
}

#ifdef IPTV_USING_MEDIA
// This is only done if the media library is being used, which is done by
// defining the IPTV_USING_MEDIA macro.

/** Receives notifications from Media Processor.
*
*/
bool IPTVKernel::OnMediaProcessorNotify(MediaProcessorParam &param)
{
	bool returnValue = false;

	switch(param.m_code)
	{
		case MPNC_READ_PACKET:
		{
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_PRCS_READ_PACKET;
			kernelMediaIDParam.m_mediaID = param.m_mediaID;

			// notify MediaTransceiver that MediaProcessor wants to read a packet
			if(m_mediaTransceiver.OnKernelNotify(kernelMediaIDParam))
				returnValue = true;
		}
		break;
		case MPCN_WRITE_PACKET:
		{
		}
		break;
		case MPCN_VIDEO_ALIVE:
		{
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_PRCS_VIDEO_ALIVE;
			kernelMediaIDParam.m_mediaID = param.m_mediaID;

			// notify Application that video is alive
			if(m_notify->OnKernelNotify(kernelMediaIDParam))
				returnValue = true;
		}
		break;
		case MPCN_AUDIO_ALIVE:
		{
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_PRCS_AUDIO_ALIVE;
			kernelMediaIDParam.m_mediaID = param.m_mediaID;

			// notify Application that audio is alive
			if(m_notify->OnKernelNotify(kernelMediaIDParam))
				returnValue = true;
		}
		break;
		case MPCN_BUFFERING_START:
		{
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_PRCS_BUFFERING_START;
			kernelMediaIDParam.m_mediaID = param.m_mediaID;

			// notify Application that buffering starts
			if(m_notify->OnKernelNotify(kernelMediaIDParam))
				returnValue = true;
		}
		break;
		case MPCN_BUFFERING_STOP:
		{
			KernelMediaIDParam kernelMediaIDParam;

			kernelMediaIDParam.m_code = KNC_PRCS_BUFFERING_STOP;
			kernelMediaIDParam.m_mediaID = param.m_mediaID;

			// notify Application that buffering stops
			if(m_notify->OnKernelNotify(kernelMediaIDParam))
				returnValue = true;
		}
		break;
		case MPCN_ERROR:
		{
		}
		break;
	}
	
	return(returnValue);
}
#endif

void IPTVKernel::OnMediaCaptureNotify(MediaCaptureParam &param)
{
	switch(param.m_code)
	{
		case MCNC_CAPTURE_VIDEO_DATA:
		{
			unsigned char *data = (*(MediaCaptureVideoDataParam *)&param).m_data;
			unsigned width = (*(MediaCaptureVideoDataParam *)&param).m_width;
			unsigned height = (*(MediaCaptureVideoDataParam *)&param).m_height;

			KernelVideoCaptureDataParam kernelVideoCaptureDataParam;

			kernelVideoCaptureDataParam.m_code = KNC_CAPTURE_VIDEO_DATA;
			kernelVideoCaptureDataParam.m_data = data;
			kernelVideoCaptureDataParam.m_dataLength = (*(MediaCaptureVideoDataParam *)&param).m_dataLength;
			kernelVideoCaptureDataParam.m_width = width;
			kernelVideoCaptureDataParam.m_height = height;

			// Notify the application that a video data was captured by MediaCapture.
			m_notify->OnKernelNotify(kernelVideoCaptureDataParam);

			// Test if colaboration and video capture are enable then encode data and send it to MediaTransceiver
			if(m_mediaColaborationEnable && m_videoCaptureEnable && m_videoEncoderEnabled)
			{
				unsigned char *encodedData;
				unsigned encodedDataLength;
				m_mediaProcessor.EncodeVideo(data, width, height, encodedData, encodedDataLength);
				m_mediaTransceiver.WriteToTransmission(encodedData, encodedDataLength);
			}
		}
		break;
		case MCNC_CAPTURE_AUDIO_FRAME:
		{
			// Test if colaboration is enable then encode data and send it to MediaTransceiver
			if(m_mediaColaborationEnable)
			{
				CAudioFrame *audioFrame = (*(MediaCaptureAudioFrameParam *)&param).m_audioFrame;
				unsigned char *encodedData;
				unsigned encodedDataLength;
				m_mediaProcessor.EncodeAudio(*audioFrame, encodedData, encodedDataLength);
				m_mediaTransceiver.WriteToTransmission(encodedData, encodedDataLength);
			}
		}
		break;
	}
}

void IPTVKernel::TerminateAllMediaTransmissions()
{
	m_mediaTransceiver.AbortReception();
	m_appSharingTransceiver.AbortReception();

#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	m_mediaProcessor.EndAllTransmissions();
#endif

	m_movie.CancelReception();

	m_appSharing.StopReception();

	m_mediaTransceiver.DestroyReception();
	m_appSharingTransceiver.DestroyReception();

	StopMediaColaboration();
	DestroyConferenceRecording();

	m_irmQuery.CloseViewer();

#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	m_mediaProcessor.Destroy();
#endif

	m_isConferenceViewerRunning = false;

	m_movie.CancelTransmission();
}

/** Initialize a multicast transmission.
*
* @param[in] host Multicast server IP or host name.
* @param[in] port Multicast port.
* @return Return true if it works and false if an error occurred.
*/
bool IPTVKernel::InitializeMulticast(VBString host, unsigned port)
{
//#ifdef IPTV_USING_MEDIA
//	// This is only done if the media library is being used, which is done by
//	// defining the IPTV_USING_MEDIA macro.
//	m_mediaProcessor.Initialize();
//#endif

	if(m_irmQuery.InitializeViewer(host, port))
		return true;

	return false;

//	if(!(m_mediaTransceiver.OpenTransmission("MCAST", host.c_str(), port, NULL, TRUE)))
//	{
//		#ifdef IPTV_USING_MEDIA
//		// This is only done if the media library is being used, which is done by
//		// defining the IPTV_USING_MEDIA macro.
//		m_mediaProcessor.Destroy();
//		#endif
//
//		return false;
//	}
//
//	return true;
//

}

/** Initialize a conference that could be multicast, TCP or UDP.
*
* @return Return true if it works and false if an error occurred.
*/
bool IPTVKernel::InitializeConference(VBString channel)
{
	m_irmQuery.RequestConferenceMediaReceive(channel);

	return(true);
}

/** Initialize a *.imc file playback.
*
* @param[in] fileName Full name of file.
*/
bool IPTVKernel::InitializeFilePlayback(VBString fileName)
{
#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	m_mediaProcessor.Initialize();
#endif

	if(m_mediaTransceiver.InitializeFilePlayback(fileName.c_str()))
		return(true);

#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	m_mediaProcessor.Destroy();
#endif

	return(false);
}

void IPTVKernel::SeekFilePlayback(double percent)
{
	m_mediaTransceiver.SeekFilePlayback(percent);
}

void IPTVKernel::PlayFilePlayback()
{
	m_mediaTransceiver.PlayFilePlayback();
}

void IPTVKernel::PauseFilePlayback()
{
	m_mediaTransceiver.PauseFilePlayback();
}

/** Get video frame from a colaboration.
*
* @param[in] id Colaboration Media ID.
* @param[out] videoFrame Frame data.
* @param[out] width Frame width.
* @param[out] height Frame height.
* @return Return true if it works and false if an error occurred.
*/
bool IPTVKernel::GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
											unsigned long *width, unsigned long *height)
{
#ifdef IPTV_USING_MEDIA
	// This is only done if the media library is being used, which is done by
	// defining the IPTV_USING_MEDIA macro.
	if(m_movie.IsMovieTransmission(id))
	{
		if(m_movie.GetColaborationVideoFrame( id, videoFrame, width, height))
			return(true);
		else
			return(false);
	}

	else
	{
		if(m_mediaProcessor.GetColaborationVideoFrame( id, videoFrame, width, height))
			return(true);
		else
			return(false);
	}
#else
	return(false);
#endif
}

IRMUdpListeningState IPTVKernel::GetUdpListeningState()
{
	return m_irmQuery.GetUdpListeningState();
}

IRMConnectionState IPTVKernel::GetConnectionState()
{
	return m_irmQuery.GetConnectionState();
}

IRMAuthenticationState IPTVKernel::GetAuthenticationState()
{
	return m_irmQuery.GetAuthenticationState();
}

IRMChannelJoinState IPTVKernel::GetChannelJoinState()
{
	return m_irmQuery.GetChannelJoinState();
}

IRMChannelPartState IPTVKernel::GetChannelPartState()
{
	return m_irmQuery.GetChannelPartState();
}

IRMChannelQueryState IPTVKernel::GetChannelQueryState()
{
	return m_irmQuery.GetChannelQueryState();
}

IRMUserQueryState IPTVKernel::GetUserQueryState()
{
	return m_irmQuery.GetUserQueryState();
}

bool IPTVKernel::IsUdpConnectionAvailable()
{
	return IPTVMediaModule::IsUdpConnectionAvailable();
}

bool IPTVKernel::IsHttpTunnelingOn()
{
	return IPTVMediaModule::IsHttpTunnelingOn();
}

void IPTVKernel::EnableHttpTunneling()
{
	IPTVMediaModule::EnableHttpTunneling();
}

void IPTVKernel::DisableHttpTunneling()
{
	IPTVMediaModule::DisableHttpTunneling();
}

void IPTVKernel::EnableMediaConference()
{
	m_mediaConference.Enable();
}

void IPTVKernel::DisableMediaConference()
{
	m_mediaConference.Disable();
}

void IPTVKernel::EnableFileTransfer()
{
	m_fileTransfer.Enable();
}

void IPTVKernel::DisableFileTransfer()
{
	m_fileTransfer.Disable();
}

void IPTVKernel::EnablePoll()
{
	m_poll.Enable();
}

void IPTVKernel::DisablePoll()
{
	m_poll.Disable();
}

void IPTVKernel::EnableWhiteboard()
{
	m_whiteboard.Enable();
}

void IPTVKernel::DisableWhiteboard()
{
	m_whiteboard.Disable();
}

/**	Request connection to the specified server.
*
*/
void IPTVKernel::RequestConnection(	VBString server, unsigned port, VBString user,
									VBString passwd, VBString realName, VBString email, VBString serverPasswd)
{
	m_irmQuery.RequestConnection(server, port, user, passwd, realName, email, serverPasswd, GetCoreVersion());
}

/**	Disconnect from server.
*
*/
void IPTVKernel::Disconnect()
{
	m_irmQuery.Disconnect();

	IPTVMediaModule::SetHttpTunnelingOff();
	IPTVMediaModule::SetUdpConnectionUnavailable();
}

/**	Request to join the specified channel.
*
*/
void IPTVKernel::RequestChannelJoin(VBString channel, VBString passwd)
{
	m_irmQuery.RequestChannelJoin(channel, passwd);
	InitializeConferenceRecording();
}

/**	Request to part from the specified channel.
*
*/
void IPTVKernel::RequestChannelPart(VBString channel)
{
	m_irmQuery.RequestChannelPart(channel);
}

void IPTVKernel::RequestSetMediaVoice(VBString channel, VBString user)
{
	m_irmQuery.RequestSetMediaVoice(channel, user);
}

void IPTVKernel::RequestUnsetMediaVoice(VBString channel)
{
	// Calling RequestSetMediaVoice without the user parameter
	// unsets the media voice in the specified channel.
	m_irmQuery.RequestSetMediaVoice(channel);
}

void IPTVKernel::RequestSetTopic(VBString channel, VBString topic)
{
	m_irmQuery.RequestSetTopic(channel, topic);
}

void IPTVKernel::RequestSetChannelOperator(VBString channel, VBString user)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_OPERATOR, true, channel, user);
}

void IPTVKernel::RequestUnsetChannelOperator(VBString channel, VBString user)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_OPERATOR, false, channel, user);
}

void IPTVKernel::RequestSetChannelPrivate(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_PRIVATE, true, channel);
}

void IPTVKernel::RequestUnsetChannelPrivate(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_PRIVATE, false, channel);
}

void IPTVKernel::RequestSetChannelSecret(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_SECRET, true, channel);
}

void IPTVKernel::RequestUnsetChannelSecret(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_SECRET, false, channel);
}

void IPTVKernel::RequestSetChannelInviteOnly(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_INVITE_ONLY, true, channel);
}

void IPTVKernel::RequestUnsetChannelInviteOnly(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_INVITE_ONLY, false, channel);
}

void IPTVKernel::RequestSetChannelOpChangeTopic(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_OP_CHANGE_TOPIC, true, channel);
}

void IPTVKernel::RequestUnsetChannelOpChangeTopic(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_OP_CHANGE_TOPIC, false, channel);
}

void IPTVKernel::RequestSetChannelNoExternalMessages(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES, true, channel);
}

void IPTVKernel::RequestUnsetChannelNoExternalMessages(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_NO_EXTERNAL_MESSAGES, false, channel);
}

void IPTVKernel::RequestSetChannelModerated(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MODERATED, true, channel);
}

void IPTVKernel::RequestUnsetChannelModerated(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MODERATED, false, channel);
}

void IPTVKernel::RequestSetChannelUserLimit(VBString channel, int limit)
{
	VBString parameter;
	parameter = limit;
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_USER_LIMIT, true, channel, parameter);
}

void IPTVKernel::RequestUnsetChannelUserLimit(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_USER_LIMIT, false, channel);
}

void IPTVKernel::RequestSetChannelMediaCollabotator(VBString channel, VBString user)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MEDIA_COLLABORATOR, true, channel, user);
}

void IPTVKernel::RequestUnsetChannelMediaCollabotator(VBString channel, VBString user)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MEDIA_COLLABORATOR, false, channel, user);
}

void IPTVKernel::RequestSetChannelKey(VBString channel, VBString passwd)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_KEY, true, channel, passwd);
}

void IPTVKernel::RequestUnsetChannelKey(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_KEY, false, channel);
}

void IPTVKernel::RequestSetChannelBitRate(VBString channel, int bitRate)
{
	VBString parameter;
	parameter = bitRate;
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_BIT_RATE, true, channel, parameter);
}

void IPTVKernel::RequestUnsetChannelBitRate(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_BIT_RATE, false, channel);
}

void IPTVKernel::RequestSetChannelAutoChangeBitRate(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE, true, channel);
}

void IPTVKernel::RequestUnsetChannelAutoChangeBitRate(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_AUTO_CHANGE_BIT_RATE, false, channel);
}

void IPTVKernel::RequestSetChannelMultipleTransmission(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION, true, channel);
}

void IPTVKernel::RequestUnsetChannelMultipleTransmission(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_MULTIPLE_TRANSMISSION, false, channel);
}

void IPTVKernel::RequestSetChannelOneAudio(VBString channel, int audioSwitchTime)
{
	VBString parameter;
	parameter = audioSwitchTime;
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_ONE_AUDIO, true, channel, parameter);
}

void IPTVKernel::RequestUnsetChannelOneAudio(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_ONE_AUDIO, false, channel);
}

void IPTVKernel::RequestSetChannelAudioMute(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_AUDIO_MUTE, true, channel);
}

void IPTVKernel::RequestUnsetChannelAudioMute(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_AUDIO_MUTE, false, channel);
}

void IPTVKernel::RequestSetChannelBandShareLimit(VBString channel, int limit)
{
	VBString parameter;
	parameter = limit;
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_BAND_SHARE_LIMIT, true, channel, parameter);
}

void IPTVKernel::RequestUnsetChannelBandShareLimit(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_BAND_SHARE_LIMIT, false, channel);
}

void IPTVKernel::RequestSetChannelTransmissionLimit(VBString channel, int limit)
{
	VBString parameter;
	parameter = limit;
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_TRANSMISSION_LIMIT, true, channel, parameter);
}

void IPTVKernel::RequestUnsetChannelTransmissionLimit(VBString channel)
{
	m_irmQuery.RequestChannelModeChange(IRM_CHANNEL_MODE_TRANSMISSION_LIMIT, false, channel);
}

void IPTVKernel::RequestSetUserInvisible()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_INVISIBLE, true);
}

void IPTVKernel::RequestUnsetUserInvisible()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_INVISIBLE, false);
}

void IPTVKernel::RequestSetUserCameraOn()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_CAMERA_ON, true);
}

void IPTVKernel::RequestUnsetUserCameraOn()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_CAMERA_ON, false);
}

void IPTVKernel::RequestSetUserMicMute()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_MIC_MUTE, true);
}

void IPTVKernel::RequestUnsetUserMicMute()
{
	m_irmQuery.RequestUserModeChange(IRM_USER_MODE_MIC_MUTE, false);
}

void IPTVKernel::RequestSetUserAway()
{
	m_irmQuery.RequestSetUserAway();
}

void IPTVKernel::RequestUnsetUserAway()
{
	m_irmQuery.RequestUnsetUserAway();
}

void IPTVKernel::RequestInviteUser(VBString user, VBString channel)
{
	m_irmQuery.RequestInviteUser(user, channel);
}

/**	Query the channel list.
*
*/
void IPTVKernel::QueryChannel(VBString channel)
{
	m_irmQuery.QueryChannel(channel);
}

/**	Query the user list from the specified channel.
*
*/
void IPTVKernel::QueryUser(VBString channel)
{
	m_irmQuery.QueryUser(channel);
}

/**	Send private message.
*
*/
void IPTVKernel::SendPrivateMessage(VBString recipient, VBString message)
{
	m_irmQuery.SendPrivateMessage(recipient, message);
}

/**	Send channel private message.
*
*/
void IPTVKernel::SendChannelPrivateMessage(VBString channel, VBString message)
{
	m_irmQuery.SendChannelPrivateMessage(channel, message);
}


void IPTVKernel::SendMediaVoiceRequest(VBString channel)
{
	m_irmQuery.SendMediaVoiceRequest(channel);
}

void IPTVKernel::SendMediaVoiceRequestCancel(VBString channel)
{
	m_irmQuery.SendMediaVoiceRequestCancel(channel);
}

void IPTVKernel::SendMediaVoiceRequestRemoveAll(VBString channel)
{
	m_irmQuery.SendMediaVoiceRequestRemoveAll(channel);
}

/** Request media data.
*
*/
bool IPTVKernel::RequestMediaData(unsigned long mediaId)
{
	KernelMediaIDParam kernelMediaIDParam;

	kernelMediaIDParam.m_code = KNC_READ_PACKET;
	kernelMediaIDParam.m_mediaID = mediaId;

	// Notify MediaTransceiver that the kernel wants to read a packet.
	if(m_mediaTransceiver.OnKernelNotify(kernelMediaIDParam))
		return true;

	return false;
}

bool IPTVKernel::InitializeVideoCapture(unsigned videoDevice)
{
	if(m_mediaCapture.InitializeVideoCapture(videoDevice))
	{
		m_videoCaptureEnable = true;
		return true;
	}

	return false;
}

void IPTVKernel::ReleaseVideoCapture()
{
	m_mediaCapture.ReleaseVideoCapture();
	m_videoCaptureEnable = false;
}

bool IPTVKernel::FileTransferCancelReception(long mediaId)
{
	return m_fileTransfer.CancelReception(mediaId);
}

bool IPTVKernel::FileTransferBeginTransmission(VBString channel, VBString filePath, unsigned long fileSize, unsigned bitRate)
{
	return m_fileTransfer.BeginTransmission(channel, filePath, fileSize, bitRate);
}

bool IPTVKernel::FileTransferBeginWhiteboardTransmission(VBString channel, VBString filePath, unsigned long fileSize, unsigned bitRate)
{
	return m_fileTransfer.BeginTransmission(channel, filePath, fileSize, bitRate, FT_ORIGIN_WHITE_BOARD);
}

bool IPTVKernel::FileTransferCancelTransmission(long mediaId)
{
	return m_fileTransfer.CancelTransmission(mediaId);
}

bool IPTVKernel::MovieBeginTransmission(VBString channel,
										VBString filePath,
										unsigned bitRate)
{
	MovieCancelTransmission(0);
	return m_movie.BeginTransmission(channel, filePath, bitRate);
}

bool IPTVKernel::MovieCancelTransmission(long mediaId)
{
	return m_movie.CancelTransmission();
}

bool IPTVKernel::PollSend(const PollData &pollData)
{
	return m_poll.PollSend(pollData);
}

bool IPTVKernel::PollSendAnswer(VBString recipient, int optionIndex)
{
	return m_poll.PollSendAnswer(recipient, optionIndex);
}

bool IPTVKernel::PollSendStats(const PollData &pollData)
{
	return m_poll.PollSendStats(pollData);
}

void IPTVKernel::SendChannelWhiteboard(	VBString channel,
										VBString data)
{
	m_whiteboard.SendChannelWhiteboard(channel, data);
}

void IPTVKernel::InitializeVideoEncoder(VBString videoCodec,
										VBString videoBitrate)
{
	m_videoEncoderEnabled = false;
	// Initialize video encoder
	// transmission, network, bitrate, codec
	m_mediaProcessor.InitializeVideoEncoder("conf", "internet", videoBitrate, videoCodec);
	m_videoEncoderEnabled = true;
}

void IPTVKernel::InitializeMediaColaboration(	VBString channel, int audioDevice,
												VBString audioCodec, VBString audioBitrate,
												VBString videoCodec, VBString videoBitrate,
												unsigned transmissionBitrate)
{
	// Store audio device
	m_colaborationAudioDevice = audioDevice;

	// Initialize video encoder
	// transmission, network, bitrate, codec
	m_mediaProcessor.InitializeVideoEncoder("conf", "internet", videoBitrate, videoCodec);
	m_videoEncoderEnabled = true;

	// Initialize audio encoder
	// transmission, network, bitrate, codec
	m_mediaProcessor.InitializeAudioEncoder("conf", "internet", audioBitrate, audioCodec);

	// Request to the server a mediaID to start the transmission
	m_mediaConference.StartTransmission(channel, transmissionBitrate);
}

void IPTVKernel::StopMediaColaboration()
{
	// Disable media colaboration
	m_mediaColaborationEnable = false;

	// Disable video encoder
	m_videoEncoderEnabled = false;

	// Release audio capture. Video capture is controled by preview button.
	m_mediaCapture.ReleaseAudioCapture();

	// Change video capture configuration back to default
	m_mediaCapture.SetVideoCaptureConfigurations();
	
	// Stop and destroy transmission
	m_mediaTransceiver.StopTransmission();
	m_mediaTransceiver.DestroyTransmission();

	// TODO: call IRM method to stop media send
}

void IPTVKernel::EnableConferenceRecording(br::com::sbVB::VBLib::VBString filePath)
{
	if(!filePath.IsEmpty())
	{
		m_conferenceRecordingEnabled = true;
		m_conferenceRecordingFilePath = filePath;
	}

	else
		DisableConferenceRecording();
}

void IPTVKernel::DisableConferenceRecording()
{
	m_conferenceRecordingEnabled = false;
	m_conferenceRecordingFilePath = "";
}

void IPTVKernel::InitializeConferenceRecording()
{
	DestroyConferenceRecording();

	if(m_conferenceRecordingEnabled)
	{
		m_fileMediaWriteConference = new FileMW(TRUE, TRUE);
		if(m_fileMediaWriteConference->SetUrl(m_conferenceRecordingFilePath.c_str()))
			m_fileMediaWriteConference->OpenUrl();
	}
}

void IPTVKernel::StartConferenceRecording()
{
	if(m_conferenceRecordingEnabled && m_fileMediaWriteConference)
	{
		m_mediaTransceiver.StartRecording(m_fileMediaWriteConference);
		m_appSharingTransceiver.StartRecording(m_fileMediaWriteConference);
	}
}

void IPTVKernel::DestroyConferenceRecording()
{
	if(m_fileMediaWriteConference)
	{
		delete m_fileMediaWriteConference;
		m_fileMediaWriteConference = NULL;
	}
}

void IPTVKernel::GetVideoDeviceList(std::deque<VideoDeviceInfo>& deviceList)
{
	m_mediaCapture.GetVideoDeviceList(deviceList);
}

void IPTVKernel::GetAudioDeviceList(std::deque<std::string> &deviceList)
{
	m_mediaCapture.GetAudioDeviceList(deviceList);
}

void IPTVKernel::GetVideoEncoders(deque<string> &videoEncoders, std::deque<std::string> &videoBitrates)
{
	m_mediaProcessor.GetVideoEncoders(videoEncoders, videoBitrates);
}

void IPTVKernel::GetAudioEncoders(deque<string> &audioEncoders)
{
	m_mediaProcessor.GetAudioEncoders(audioEncoders);
}
