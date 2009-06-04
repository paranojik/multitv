#include "iptv_kernel/MediaTransceiver.h"

#ifdef IPTV_USING_MEDIA

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif

#include "Media_const.h"
#include "global_error.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#else
#define MAX_PKT_SIZE 65536
#define MAX_PKT_COUNT 1000
#define MAX_TRANSMISSIONS 10
#endif

using namespace br::com::sbVB::VBLib;

MediaTransceiver::MediaTransceiver(MediaTransceiverNotify *notify)
{
	m_notify = notify;
	m_sockBuffs = NULL;
	m_sockBuffsIsAborted = false;
	m_sockMWBuffs = NULL;
}

MediaTransceiver::~MediaTransceiver()
{
	delete m_sockBuffs;
}

void MediaTransceiver::SetNotify(MediaTransceiverNotify *notify)
{
	m_notify = notify;
}

void MediaTransceiver::NotifyPkt(DWORD notifyId, ULONG id, double fPercent, BYTE flags)
{
	if(m_notify != NULL)
	{
		MediaTransceiverProgressParam mediaTransceiverProgressParam;

		mediaTransceiverProgressParam.m_code = MTNC_NOTIFY_PROGRESS;
		mediaTransceiverProgressParam.m_mediaID = id;
		mediaTransceiverProgressParam.m_percent = fPercent;
		mediaTransceiverProgressParam.m_flags = flags;

		// notify kernel the progress of file playback
		if(m_notify != NULL)
			m_notify->OnMediaTransceiverNotify(mediaTransceiverProgressParam);
	}
}

void MediaTransceiver::NotifyEOF(void)
{
	// not implemented yet
}

/** Receives new transmission notifications from Sockbuffs lib.
*
*/
BOOL MediaTransceiver::NewID(ULONG id, BYTE mediaType)
{
	BOOL returnValue = FALSE;

	if(m_notify != NULL)
	{
			if (m_sockBuffs->CreateWnd(id, MAX_PKT_SIZE, 10, TRUE, SB_MEDIAPKTTYPE_EXT, id))         
			{
				MediaTransceiverIDParam mediaTransceiverIDParam;

				mediaTransceiverIDParam.m_code = MTNC_NEW_TRANSMISSION;
				mediaTransceiverIDParam.m_mediaID = id;

				// notify kernel that new transmission arrives
				if(m_notify != NULL)
					if(m_notify->OnMediaTransceiverNotify(mediaTransceiverIDParam))
						returnValue = TRUE;
			}
	}

    return(returnValue);
}

ISockBuff *MediaTransceiver::GetISockBuff(long mediaId)
{
	return m_sockBuffs->GetWndObj(mediaId);
}

ISockBuff *MediaTransceiver::CreateAppSharingBuffer(long mediaId)
{
	ISockBuff *buff = NULL;

	if (m_sockBuffs->CreateWnd(	mediaId, APP_SHARING_MAX_PKT_SIZE, APP_SHARING_PKT_WAIT_FOR_DELAYED,
								TRUE, SB_MEDIAPKTTYPE_LPKT, mediaId))         
	{
		buff = GetISockBuff(mediaId);
	}

	return buff;
}

/** Receives notifications from Kernel.
*
*/
bool MediaTransceiver::OnKernelNotify(KernelParam &param)
{
	if(!m_sockBuffs)
		return false;
	if(m_sockBuffsIsAborted)
		return false;

	bool returnValue = false;

	if(m_notify != NULL)
	{
		switch(param.m_code)
		{
			case KNC_READ_PACKET:
			case KNC_PRCS_READ_PACKET:
			{
				unsigned long id = (*(KernelMediaIDParam *)&param).m_mediaID;

				ISockBuff *sockReg = m_sockBuffs->GetWndObj(id);

				if (sockReg)
				{
					unsigned char  data[MAX_PKT_SIZE];
					unsigned long dataLength = MAX_PKT_SIZE;

					if (sockReg->Read(data, dataLength, &dataLength) >= 0)
					{
						MediaTransceiverDataParam mediaTransceiverDataParam;

						if(param.m_code == KNC_READ_PACKET)
							mediaTransceiverDataParam.m_code = MTNC_DATA_READ_APP;
						else if(param.m_code == KNC_PRCS_READ_PACKET)
							mediaTransceiverDataParam.m_code = MTNC_DATA_READ_MEDIA;

						mediaTransceiverDataParam.m_data = data;
						mediaTransceiverDataParam.m_dataLength = dataLength;

						// notify Kernel that data packet was read
						if(m_notify != NULL)
							if(m_notify->OnMediaTransceiverNotify(mediaTransceiverDataParam))
								returnValue = true;
					}
				}
			}
			break;
			case KNC_PRCS_WRITE_PACKET:
			{
			}
			break;
		}
	}

	return(returnValue);
}

bool MediaTransceiver::InitializeFilePlayback(LPCSTR filename)
{
	if(m_notify != NULL)
	{
		if(!m_sockBuffs)
			m_sockBuffs = new SockBuffs(MAX_TRANSMISSIONS, MAX_PKT_SIZE * MAX_PKT_COUNT);

		if ( m_sockBuffs->SetUrl(filename) && m_sockBuffs->SetINotify(this, 1)
			 && m_sockBuffs->OpenUrl() )
		{
			m_sockBuffsIsAborted = false;
			return(true);
		}
	}
	
	return(false);
}

void MediaTransceiver::SeekFilePlayback(double percent)
{
	if(m_notify != NULL)
	{
		if(m_sockBuffs)
			m_sockBuffs->SetReadOffset(percent);
	}
}

void MediaTransceiver::PlayFilePlayback()
{
	if(m_notify != NULL)
	{
		if(m_sockBuffs)
			m_sockBuffs->Play();
	}
}

void MediaTransceiver::PauseFilePlayback()
{
	if(m_notify != NULL)
	{
		if(m_sockBuffs)
			m_sockBuffs->Pause(TRUE);
	}
}

bool MediaTransceiver::InitializeReception(LPCSTR protocol, LPCSTR host, DWORD port, LPCSTR connStr, BOOL listen)
{
	DestroyReception();

	if(m_notify != NULL)
	{
		if(!m_sockBuffs)
			m_sockBuffs = new SockBuffs(MAX_TRANSMISSIONS, MAX_PKT_SIZE * MAX_PKT_COUNT);
	}
	else
	{
		if(!m_sockBuffs)
			m_sockBuffs = new SockBuffs(APP_SHARING_MAX_CONNECTIONS, APP_SHARING_MAX_PKT_SIZE * APP_SHARING_MAX_PKT);
	}

    if ( m_sockBuffs->SetUrl(protocol, host ,port, connStr, listen, NULL) && 
           m_sockBuffs->SetINotify(this, 0) && m_sockBuffs->OpenUrl() )
	{
		m_sockBuffsIsAborted = false;
		return(true);
	}
	
	return(false);
}

void MediaTransceiver::StopReception()
{
	if(m_sockBuffs)
	{
		m_sockBuffs->CloseUrl();
	}
}

void MediaTransceiver::AbortReception()
{
	if(m_sockBuffs)
	{
		m_sockBuffs->Abort();
		m_sockBuffsIsAborted = true;

		// Note
		// Guilherme Lello, 2008-11-04.
		// Calling SockBuffs::DestroyAllWnds at this moment
		// crashes the app with iptv_shared revision 32.
		// m_sockBuffs->DestroyAllWnds();
	}
}

void MediaTransceiver::DestroyReception()
{
	if(m_sockBuffs)
	{
		delete m_sockBuffs;
		m_sockBuffs = NULL;
	}
}

bool MediaTransceiver::InitializeTransmission(LPCSTR protocol, LPCSTR host, DWORD port, LPCSTR connStr, BOOL listen)
{
	if(!m_sockMWBuffs)
		m_sockMWBuffs = new SockMWBuffs(0, 0, 6000, FALSE, FALSE, MW_DEFRAGMODE_PKT | MW_DEFRAGMODE_PKTEXT);
	
	m_sockMWBuffs->SetUrl(protocol, host, port, connStr, listen, NULL);
	
	if (m_sockMWBuffs->OpenUrl())
		return(true);
	
	return(false);
}

void MediaTransceiver::WriteToTransmission(unsigned char *data, unsigned dataLength)
{
	unsigned long dummy;
	m_sockMWBuffs->Write(data, dataLength, &dummy);
}

void MediaTransceiver::StopTransmission()
{
	if(m_sockMWBuffs)
	{
		m_sockMWBuffs->CloseUrl();
	}
}

void MediaTransceiver::DestroyTransmission()
{
	if(m_sockMWBuffs)
	{
		delete m_sockMWBuffs;
		m_sockMWBuffs = NULL;
	}
}

void MediaTransceiver::StartRecording(ISockBuff *fileMediaWrite)
{
	if(m_sockBuffs && fileMediaWrite)
		m_sockBuffs->SetFileMWObj(fileMediaWrite);

	if(m_sockMWBuffs && fileMediaWrite)
		m_sockMWBuffs->SetFileMWObj(fileMediaWrite);
}
