#ifndef MEDIA_PROCESSOR_H
#define MEDIA_PROCESSOR_H

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267)	// disable MSVC's size_t-to-int warning
#endif

#include "shared/compat.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "VBLib/VBLib.h"

#include "IKernelNotify.h"
#include "iptv_kernel/MediaProcessorNotify.h"
#include "iptv_kernel/KernelNotify.h"

#include "CMedia.h"
#include "CVideoFrame.h"
#include "CAudioFrame.h"
#include "AudioEncoder.h"
#include "VideoEncoder.h"
#include "MediaNetwork.h"

typedef CReadData_reg<CVideoFrame> VideoFrame_reg;

/** @brief Manipulates media.
 *
 * This class responsibilities are encoding/decoding audio and video IP.TV streams.
 *
 */
class MediaProcessor :	public IMediaKernelNotify,
						public KernelNotify
{
private:
	MediaProcessorNotify *m_notify;
	CMedia *m_media;
	CVideoFrame *m_mediaVideoFrame;
	VideoEncoder *m_videoEncoder;
	AudioEncoder *m_audioEncoder;
	AudioNetworkEncoder m_audioNetwork;
	VideoNetworkEncoder m_videoNetwork;
	long m_mediaID;
	unsigned m_videoSubSequential;	// packet identification
	unsigned m_audioSubSequential;	// packet identification
    unsigned char m_encodedVideoData[MAX_ENCODED_VIDEO_SIZE];
    unsigned char m_encodedAudioData[MAX_ENCODED_AUDIO_SIZE];

public:
	MediaProcessor(MediaProcessorNotify *notify = NULL);
	~MediaProcessor();

	// Codec methods
	bool GetColaborationVideoFrame(	unsigned long id, unsigned char ** videoFrame,
									unsigned long *width, unsigned long *height);
	bool EndAllTransmissions();
	void Initialize();
	void Destroy();

	// Encoder methods
	void SetMediaID(long mediaID);

	// Video encoder methods
	void InitializeVideoEncoder(br::com::sbVB::VBLib::VBString transmission,
								br::com::sbVB::VBLib::VBString network,
								br::com::sbVB::VBLib::VBString bitrate,
								br::com::sbVB::VBLib::VBString codec);
	void GetVideoConfigurations(double &framerate, unsigned &width, unsigned &height);
	void GetVideoEncoders(std::deque<std::string> &videoEncoders, std::deque<std::string> &videoBitrates);
	bool EncodeVideo(	unsigned char *data, unsigned width, unsigned height,
						unsigned char *&encodedData, unsigned &encodedDataLength);

	// Audio encoder methods
	void InitializeAudioEncoder(br::com::sbVB::VBLib::VBString transmission,
								br::com::sbVB::VBLib::VBString network,
								br::com::sbVB::VBLib::VBString bitrate,
								br::com::sbVB::VBLib::VBString codec);
	void GetAudioConfigurations(unsigned &frameSamples, unsigned &framesInPacket,
								unsigned &sampleRate, unsigned &channels, unsigned &sampleSize);
	void GetAudioEncoders(std::deque<std::string> &audioEncoders);
	bool EncodeAudio(CAudioFrame &audioFrame, unsigned char *&encodedData, unsigned &encodedDataLength);

	// Notification methods
	void SetNotify(MediaProcessorNotify *notify);
	BOOL OnMediaNotify(MediaParam *_pParameters);
	bool OnKernelNotify(KernelParam &param);
};

#endif
