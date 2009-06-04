#include <string>

// disable warnings in MSVC when using shared lib
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)	// size_t to int conversion
#pragma warning(disable: 4005)	// FD_SETSIZE (winsock2.h) macro redefinition
#endif

#include "shared/compat.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "iptv_kernel/MediaCapture.h"
#include "global_error.h"



MediaCapture::MediaCapture(MediaCaptureNotify *notify)
{
	m_notify = notify;

	m_videoCaptureInitialized = false;
	m_audioCaptureInitialized = false;
	m_width = DEFAULT_VIDEO_WIDTH;
	m_height = DEFAULT_VIDEO_HEIGHT;

	m_audioCaptureDevice = NULL;
	m_frequency = DEFAULT_BUF_FREQUENCY;
	m_bufferLengthMs = DEFAULT_BUF_LEN_MS;

	m_videoDevice        = NULL;

	m_videoCaptureThread = NULL;
	m_audioCaptureThread = NULL;
	m_videoCaptureInterval = DEFAULT_VIDEO_CAPTURE_TIME;
}

MediaCapture::~MediaCapture()
{
	ReleaseVideoCapture();
	ReleaseAudioCapture();
}

void MediaCapture::SetNotify(MediaCaptureNotify *notify)
{
	m_notify = notify;
}

// ====================================== Video Capture ======================================

void MediaCapture::GetVideoDeviceList(VIDEODEVICEINFO_LIST& deviceList)
{
	m_videoEnum.GetVideoDeviceList(deviceList, true, true); // Cameras and screens
}

bool MediaCapture::InitializeVideoCapture(unsigned videoDevice)
{
	// If already capturing then release capture to start again
	if(m_videoCaptureInitialized)
		ReleaseVideoCapture();

	// Save new device id
	m_deviceID = videoDevice;

	// Initialize capture device
	m_videoDevice = m_videoEnum.GetVideoDevice(m_deviceID);

	if(m_videoDevice)
	{
		// Fixed bug caused by the capture thread if the video capture
		// started during the transmission. The XCapture::SetCaptureFormat
		// method was always being called with the default width and height
		// values. This works if the capture is initiated before the transmission,
		// but not otherwise if the dimensions for the current bitrate are
		// different from the default values, in which case it crashes.
		//
		// The solution is to always call XCapture::SetCaptureFormat with the
		// current dimensions. They are initialized with default values in the ctor,
		// guaranteeing they always have a valid value. The dimensions can be changed
		// only by MediaCapture::SetVideoCaptureConfigurations.
		m_videoDevice->SelectFormat(m_width, m_height);

		if(m_videoDevice->StartCapture() != RET_OK)
		{
			// Error if StartCapture returns != 0.
			m_videoDevice->Close();
			delete m_videoDevice;
			m_videoDevice = NULL;
			return false;
		}
	}		
	else
		return false;

	// Start video capture thread
	m_exitVideoCaptureThread = false;
	m_videoCaptureThread = CreateIThread(VideoCaptureThread, this, TRUE);

	bool ret = true;

	if(!m_videoCaptureThread)
		ret = false;
	else
	{
		if(!m_videoCaptureThread->IsThreadOk())
			ret = false;
	}

	if(!ret)
	{
		m_videoDevice->StopCapture();
		m_videoDevice->Close();
		delete m_videoDevice;
		m_videoDevice = NULL;
	}

	return ret;
}

void MediaCapture::ReleaseVideoCapture()
{
	if(m_videoCaptureInitialized)
	{
		// Stop video capture thread
		m_exitVideoCaptureThread = true;
		m_videoCaptureThread->WaitForMe(ONE_HOUR);
		delete m_videoCaptureThread;
		m_videoCaptureThread = NULL;

		m_videoDevice->StopCapture();
		m_videoDevice->Close();
		delete m_videoDevice;
		m_videoDevice = NULL;
	}

	m_videoCaptureInitialized = false;
}

bool MediaCapture::SetVideoCaptureConfigurations(double framerate, unsigned long width, unsigned long height)
{
	if(!m_videoDevice)
		return false;

	if((framerate <= 0.0) || (width == 0) || (height == 0))
		return false;

	m_width = width;
	m_height = height;

	m_videoDevice->SelectFormat(m_width, m_height);

	// Capture interval unit is milliseconds.
	m_videoCaptureInterval = (unsigned long)(1000.0 / framerate);

	return true;
}
/*
CVideoFrame* MediaCapture::GetVideoData()
{
	CVideoFrame* pVideoFrame = new CVideoFrame();
	m_videoDevice->GrabFrame(*pVideoFrame);
	return pVideoFrame;

	// TODO: frame format conversion in win32.

	// Old code using OpenCV that does said conversion:
	// Get the current frame.
    //IplImage *frame = cvRetrieveFrame(m_capture);
	// unsigned char *rawData;
	//int cvtFlags = 0;
	//CvSize size;

	//size.width = frame->width;
	//size.height = frame->height;
	//m_width = frame->width;
	//m_height = frame->height;
	
	//// check origin
	//if(frame->origin == 1)
	//	cvtFlags |= CV_CVTIMG_FLIP;
	//// check color order
	//if(std::string(frame->channelSeq) == "BGR")
	//	cvtFlags |= CV_CVTIMG_SWAP_RB;

	// Convert the frame properly.
	//if(cvtFlags)
	//{
	//	if(!m_currentFrame)
	//		m_currentFrame = cvCreateImage(size, frame->depth, frame->nChannels);
	//	cvConvertImage(frame, m_currentFrame, cvtFlags);
	//	cvGetRawData(m_currentFrame, &rawData, NULL, NULL);
	//}
	//else
	//{
	//	cvGetRawData(frame, &rawData, NULL, NULL);
	//}
}
*/
int MediaCapture::VideoCaptureThread(IThread *thread, void *param)
{
	// Now this should only update the current frame and generate notifications.

	// Cast param to MediaCapture.
	MediaCapture *mediaCapture = (MediaCapture *)param;

	// Capture thread initialized
	mediaCapture->m_videoCaptureInitialized = true;

	// Run the thread
	while(!mediaCapture->m_exitVideoCaptureThread)
	{
		if(mediaCapture->m_videoDevice->NewFrame())
		{
			// Try to grab a frame.
			mediaCapture->m_videoDevice->GrabFrame(mediaCapture->m_videoFrame);
			//frame.m_bSelfDestruct = false;
			
			MediaCaptureVideoDataParam mediaCaptureVideoDataParam;

			mediaCapture->m_videoFrame.GetFrameData(&mediaCaptureVideoDataParam.m_data,
							                        &mediaCaptureVideoDataParam.m_dataLength);

			mediaCapture->m_videoFrame.GetFrameDimensions(&mediaCaptureVideoDataParam.m_width,
									                      &mediaCaptureVideoDataParam.m_height);
			
			mediaCaptureVideoDataParam.m_code = MCNC_CAPTURE_VIDEO_DATA;

			if(mediaCapture->m_notify != NULL)
				mediaCapture->m_notify->OnMediaCaptureNotify(mediaCaptureVideoDataParam);
		}

		Wait(mediaCapture->m_videoCaptureInterval);
	}

	return 0;
}

// ====================================== Audio Capture ======================================

void MediaCapture::GetAudioDeviceList(std::deque<std::string> &deviceList)
{
	m_audioCapture.EnumDevices(deviceList);
}

bool MediaCapture::InitializeAudioCapture(int audioDevice)
{
	// If already capturing then release capture to start again
	if(m_audioCaptureInitialized)
		ReleaseAudioCapture();

	// Get device list
	std::deque<std::string> deviceList;
	GetAudioDeviceList(deviceList);

	// Check if it is out of range
	if(audioDevice >= (int)deviceList.size())
		return false;

	// Get capture device
	m_audioCapture.GetDevice(deviceList[audioDevice], m_fmt, m_frequency, m_bufferLengthMs, &m_audioCaptureDevice);

	// Check if device is ok
	if(!m_audioCaptureDevice->DeviceOK())
		return false;

	// Start audio capture
	m_audioCaptureDevice->StartCapture();

	// Start audio capture thread
	m_exitAudioCaptureThread = false;
	m_audioCaptureThread = CreateIThread(AudioCaptureThread, this, TRUE);

	if(!m_audioCaptureThread)
		return false;

	if(!m_audioCaptureThread->IsThreadOk())
		return false;

	return true;
}

void MediaCapture::GetAudioSamplesCaptured(unsigned &availableSamples)
{
	m_audioCaptureDevice->GetSamplesCaptured(availableSamples);
}

bool MediaCapture::GetAudioFrame(CAudioFrame &audioFrame, unsigned samples)
{
	if(m_audioCaptureDevice->GetData(audioFrame, samples) == RET_OK)
		return true;
	return false;
}

void MediaCapture::GetAudioData(unsigned char *buffer, unsigned bufferSize, unsigned samples)
{
	m_audioCaptureDevice->GetData(buffer, bufferSize, samples);
}

void MediaCapture::FreeAudioBuffer()
{
	m_audioCaptureDevice->FreeBuffer();
}

void MediaCapture::ReleaseAudioCapture()
{
	if(m_audioCaptureInitialized)
	{
		// Stop audio capture thread
		m_exitAudioCaptureThread = true;
		m_audioCaptureThread->WaitForMe(ONE_HOUR);
		delete m_audioCaptureThread;
		m_audioCaptureThread = NULL;

		if(m_audioCaptureDevice)
		{
			m_audioCaptureDevice->StopCapture();
			m_audioCapture.DestroyDevice(m_audioCaptureDevice);
		}
	}

	m_audioCaptureInitialized = false;
}

bool MediaCapture::SetAudioCaptureConfigurations(unsigned frameSamples, unsigned framesInPacket,
												 unsigned sampleRate, unsigned channels, unsigned sampleSize)
{
	if((frameSamples == 0) || (framesInPacket == 0) || (sampleRate == 0) || (channels == 0) || (sampleSize == 0))
		return false;

	// Set audio capture interval
	// Andre Cotrim 2008-07-25
	// Nyquist's criterion: If one wants something that is sampled to be perfectly recovered, one must sample it
	// with a frequency at least double the sampled thing's maximum frequency. 
	// In short: if the wait interval is too close to the frame/packet time, there may be a gap between 
	// the capture and the encoding, causing "popping" at the receiving side.
	m_audioCaptureInterval = (unsigned long)((frameSamples * 1000.0 * framesInPacket / sampleRate) / 2.0);
	// m_audioCaptureInterval = (frameSamples * framesInPacket * (1000.0 / sampleRate)) / 2;

	// Set samples to process
	m_samplesToProcess = frameSamples*framesInPacket;

	m_frequency = sampleRate;

	// If mono
	if(channels == 1)
	{
		// If 1 byte = 8 bits
		if(sampleSize == 1)
		{
			m_fmt = MONO8;
		}
		// If 2 bytes = 16 bits
		else if(sampleSize == 2)
		{
			m_fmt = MONO16;
		}
		// Error if more than 2 bytes = 16 bits
		else
		{
			return false;
		}
	}
	// If stereo
	else if(channels == 2)
	{
		// If 1 byte = 8 bits
		if(sampleSize == 1)
		{
			m_fmt = STEREO8;
		}
		// If 2 bytes = 16 bits
		else if(sampleSize == 2)
		{
			m_fmt = STEREO16;
		}
		// Error if more than 2 bytes = 16 bits
		else
		{
			return false;
		}
	}
	// Error if more than 2 channels
	else
	{
		return false;
	}

	return true;
}

int MediaCapture::AudioCaptureThread(IThread *thread, void *param)
{
	// Cast param to MediaCapture
	MediaCapture *mediaCapture = (MediaCapture *)param;

	// Capture thread initialized
	mediaCapture->m_audioCaptureInitialized = true;

	// Auxiliary variables
	unsigned samplesCaptured;
    CAudioFrame audioFrame;

	// Run the thread
	while(!mediaCapture->m_exitAudioCaptureThread)
	{
		mediaCapture->GetAudioSamplesCaptured(samplesCaptured);

		// Check if there is enough samples
		if(samplesCaptured >= (unsigned)mediaCapture->m_samplesToProcess)
		{
			// Try to get an audio frame
			if (mediaCapture->GetAudioFrame(audioFrame, mediaCapture->m_samplesToProcess))
			{
				MediaCaptureAudioFrameParam mediaCaptureAudioFrameParam;

				mediaCaptureAudioFrameParam.m_code = MCNC_CAPTURE_AUDIO_FRAME;
				mediaCaptureAudioFrameParam.m_audioFrame = &audioFrame;

				if(mediaCapture->m_notify != NULL)
					mediaCapture->m_notify->OnMediaCaptureNotify(mediaCaptureAudioFrameParam);
			}
		}

		Wait(mediaCapture->m_audioCaptureInterval);
	}

	return 0;
}
