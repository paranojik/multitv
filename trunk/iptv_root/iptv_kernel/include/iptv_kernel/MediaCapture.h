#ifndef MEDIA_CAPTURE_H
#define MEDIA_CAPTURE_H

#include "iptv_kernel/MediaCaptureNotify.h"

#include "AudioCapture.h"
#include "CVideoFrame.h"

#include "shared/ithread.h"
#include "shared/semaphore.h"

#include "iptv_video_capture/include/VideoCapture.h"

#define DEFAULT_FMT MONO16
#define DEFAULT_BUF_FREQUENCY 32000
#define DEFAULT_BUF_LEN_MS 100
// Andre Cotrim 2008-07-23
// NOTE: this must be smaller than the desired frame rate because
// usually cameras (especially USB cameras) take a rather long time
// (in terms of CPU time, even more when even the cheapest modern-day
// CPUs run at speeds near or above 2 GHz) to return a video frame.
#define DEFAULT_VIDEO_CAPTURE_TIME 10 /* in miliseconds*/
#define DEFAULT_VIDEO_WIDTH 320
#define DEFAULT_VIDEO_HEIGHT 240
#define ONE_HOUR 60 * 60 * 1000	/* one hour of miliseconds */

using namespace tv::ip;

/** @brief Captures media from local devices.
 *
 */
class MediaCapture
{
public:
	MediaCapture(MediaCaptureNotify *notify = NULL);
	virtual ~MediaCapture();

	// Notify methods
	void SetNotify(MediaCaptureNotify *notify);

	// Video capture methods
	bool InitializeVideoCapture(unsigned videoDevice = 0);
	void GetVideoDeviceList(VIDEODEVICEINFO_LIST& deviceList);
	void ReleaseVideoCapture();
	bool SetVideoCaptureConfigurations(	double framerate = DEFAULT_VIDEO_CAPTURE_TIME,
										unsigned long width = DEFAULT_VIDEO_WIDTH,
										unsigned long height = DEFAULT_VIDEO_HEIGHT);
	// Audio capture methods
	void GetAudioDeviceList(std::deque<std::string> &deviceList);
	bool InitializeAudioCapture(int audioDevice = 0);
	void ReleaseAudioCapture();
	bool SetAudioCaptureConfigurations(	unsigned frameSamples, unsigned framesInPacket,
										unsigned sampleRate, unsigned channels, unsigned sampleSize);

private:
	MediaCaptureNotify *m_notify;

	// Video
	VideoDevice* m_videoDevice;
	VideoDeviceEnumerator m_videoEnum;
	CVideoFrame m_videoFrame;
	unsigned long m_width, m_height, m_dataLenght;
	bool m_videoCaptureInitialized;
	IThread *m_videoCaptureThread;
	bool m_exitVideoCaptureThread;
	unsigned long m_videoCaptureInterval;
	unsigned m_deviceID;

	// Audio
	CAudioCapture m_audioCapture;
	CAudioCapDevice *m_audioCaptureDevice;
	sampleFmt m_fmt;
	unsigned m_frequency;
	unsigned m_bufferLengthMs;
	bool m_audioCaptureInitialized;
	IThread *m_audioCaptureThread;
	bool m_exitAudioCaptureThread;
	unsigned long m_audioCaptureInterval;
	int m_samplesToProcess;

	// Video capture methods
	//CVideoFrame* GetVideoData();
	static int VideoCaptureThread(IThread *thread, void *param);

	// Audio capture methods
	void GetAudioSamplesCaptured(unsigned &availableSamples);
	bool GetAudioFrame(CAudioFrame &audioFrame, unsigned samples);
	void GetAudioData(unsigned char *buffer, unsigned bufferSize, unsigned samples);
	void FreeAudioBuffer();
	static int AudioCaptureThread(IThread *thread, void *param);
};

#endif
