#ifdef IPTV_USING_MEDIA

#include "iptv_kernel/MediaProcessor.h"

#ifdef _LINUX
#include "shared/compat.h"
#endif

using namespace br::com::sbVB::VBLib;

MediaProcessor::MediaProcessor(MediaProcessorNotify *notify)
{
	m_notify = notify;
	m_media = NULL;
	m_mediaVideoFrame = NULL;
	m_videoEncoder = NULL;
	m_audioEncoder = NULL;
}

MediaProcessor::~MediaProcessor()
{
	if(m_media)
		delete m_media;
	if(m_mediaVideoFrame)
		delete m_mediaVideoFrame;
}

void MediaProcessor::SetNotify(MediaProcessorNotify *notify)
{
	m_notify = notify;
}

/** Receives notifications from IPTV_Media lib.
*
*/
BOOL MediaProcessor::OnMediaNotify(MediaParam *_pParameters)
{
    if (!_pParameters)
        return(FALSE);

	BOOL returnValue = FALSE;

    switch(_pParameters->code)
    {
        case MN_READ_PACKET:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPNC_READ_PACKET;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel that media wants to read a packet
			if(m_notify->OnMediaProcessorNotify(mediaProcessorParam))
				returnValue = TRUE;
        }
		break;

		case MN_WRITE_PACKET:
		{
		}
		break;

        case MN_VIDEO_ALIVE:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_VIDEO_ALIVE;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_notify->OnMediaProcessorNotify(mediaProcessorParam))
				returnValue = TRUE;
		}
		break;

		case MN_AUDIO_ALIVE:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_AUDIO_ALIVE;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_notify->OnMediaProcessorNotify(mediaProcessorParam))
				returnValue = TRUE;
		}
		break;
	
		case MN_BUFFERING_START:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_BUFFERING_START;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_notify->OnMediaProcessorNotify(mediaProcessorParam))
				returnValue = TRUE;
		}
		break;

		case MN_BUFFERING_STOP:
		{
			MediaProcessorParam mediaProcessorParam;
			mediaProcessorParam.m_code = MPCN_BUFFERING_STOP;
			mediaProcessorParam.m_mediaID = _pParameters->id;

			// notify kernel
			if(m_notify->OnMediaProcessorNotify(mediaProcessorParam))
				returnValue = TRUE;
		}
		break;

		//case MN_LOCALE_SET_ENGLISH:
		//{
		//	KernelParam p;
		//	p.m_code = KNC_LOCALE_SET_ENGLISH;
		//	g_kernelNotify->OnKernelNotify(p);
		//}
		//break;

		//case MN_LOCALE_RESTORE:
		//{
		//	KernelParam p;
		//	p.m_code = KNC_LOCALE_RESTORE;
		//	g_kernelNotify->OnKernelNotify(p);
		//}
		//break;
    
		case MN_ERROR:
		{
		}
		break;
	}

	return(returnValue);
}

/** Receives notifications from Kernel.
*
*/
bool MediaProcessor::OnKernelNotify(KernelParam &param)
{
	bool returnValue = false;

	switch(param.m_code)
	{
		case KNC_TRCV_NEW_TRANSMISSION:
		{
			unsigned long id = (*(KernelMediaIDParam *)&param).m_mediaID;
			if(m_media->AddTransmission(id) == RET_OK)
				returnValue = true;
		}
		break;
		case KNC_TRCV_DATA_READ_MEDIA:
		{
			unsigned char  *data = (*(KernelMediaDataParam *)&param).m_data;
			unsigned long dataLength = (*(KernelMediaDataParam *)&param).m_dataLength;

			if(m_media->SetPacket(data, dataLength) == RET_OK)
				returnValue = true;
		}
		break;
	}

	return(returnValue);
}

void MediaProcessor::Initialize()
{
	if(!m_media)
	{
		m_media = new CMedia();
		m_media->SetKernelNotify(this);
	}
}

bool MediaProcessor::GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
												unsigned long *width, unsigned long *height)
{
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

bool MediaProcessor::EndAllTransmissions()
{
	if(m_media)
		if(m_media->EndAllTransmissions() == RET_OK)
			return(true);

	return(false);
}


void MediaProcessor::Destroy()
{
	if(m_media)
	{
		delete m_media;
		m_media = NULL;
	}
}

// ================================================== Encoding ==================================================

void MediaProcessor::SetMediaID(long mediaID)
{
	m_mediaID = mediaID;
}

void MediaProcessor::InitializeVideoEncoder(VBString transmission, VBString network, VBString bitrate, VBString codec)
{
	m_videoNetwork.Init(transmission.c_str(), network.c_str(), bitrate.c_str(), codec.c_str());
	m_videoEncoder = (VideoEncoder *) m_videoNetwork.GetCodec();

	m_videoSubSequential = 0;
}

void MediaProcessor::GetVideoConfigurations(double &framerate, unsigned &width, unsigned &height)
{
	framerate = m_videoEncoder->GetFramerate();
	width = m_videoEncoder->GetWidth();
	height = m_videoEncoder->GetHeight();
}

void MediaProcessor::GetVideoEncoders(std::deque<std::string> &videoEncoders, std::deque<std::string> &videoBitrates)
{
	MediaCodecsList codecEnumerator;
	codecEnumerator.GetVideoCodecs(videoEncoders, videoBitrates);
}

bool MediaProcessor::EncodeVideo(unsigned char *data, unsigned width, unsigned height, unsigned char *&encodedData, unsigned &encodedDataLength)
{
	// Create the video frame
	CVideoFrame videoFrame;
	videoFrame.SetData(data, width*height*3, GetTickCount(), width, height, RGB24);

	// Set max encoded video size
	encodedDataLength = MAX_ENCODED_VIDEO_SIZE;

	// Encode video
	unsigned headerLen;
	if (m_videoNetwork.EncodePkt(videoFrame, m_mediaID, m_videoSubSequential, m_encodedVideoData, encodedDataLength, headerLen) == RET_OK)
	{
		m_videoSubSequential++;
		encodedData = m_encodedVideoData;
		return true;
	}

	// If cannot encode
	encodedData = NULL;
	encodedDataLength = 0;
	return false;
}

void MediaProcessor::InitializeAudioEncoder(VBString transmission, VBString network, VBString bitrate, VBString codec)
{
	m_audioNetwork.Init(transmission.c_str(), network.c_str(), bitrate.c_str(), codec.c_str());
	m_audioEncoder = (AudioEncoder *) m_audioNetwork.GetCodec();

	m_audioSubSequential = 0;
}

void MediaProcessor::GetAudioConfigurations(unsigned &frameSamples, unsigned &framesInPacket,
											unsigned &sampleRate, unsigned &channels, unsigned &sampleSize)
{
	frameSamples = m_audioEncoder->GetFrameSamples();
	framesInPacket = m_audioEncoder->GetFramesPkt();
	sampleRate = m_audioEncoder->GetSampleRate();
	channels = m_audioEncoder->GetChannels();
	sampleSize = m_audioEncoder->GetSampleSize();
}

void MediaProcessor::GetAudioEncoders(std::deque<std::string> &audioEncoders)
{
	MediaCodecsList codecEnumerator;
	codecEnumerator.GetAudioCodecs(audioEncoders);
}

bool MediaProcessor::EncodeAudio(CAudioFrame &audioFrame, unsigned char *&encodedData, unsigned &encodedDataLength)
{
	// Set max encoded audio size
	encodedDataLength = MAX_ENCODED_AUDIO_SIZE;

	// Encode audio
	unsigned headerLen;
	if (m_audioNetwork.EncodePkt(audioFrame, m_mediaID, m_audioSubSequential, m_encodedAudioData, encodedDataLength, headerLen) == RET_OK)
	{   
		m_audioSubSequential++;
		encodedData = m_encodedAudioData;
		return true;
	}

	// If cannot encode
	encodedData = NULL;
	encodedDataLength = 0;
	return false;
}

#endif
