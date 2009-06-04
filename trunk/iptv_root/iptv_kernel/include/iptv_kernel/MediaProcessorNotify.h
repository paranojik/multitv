#ifndef MEDIA_PROCESSOR_NOTIFY_H
#define MEDIA_PROCESSOR_NOTIFY_H

enum MediaProcessorNotifyCode{	MPNC_READ_PACKET,				// Parameter class: MediaProcessorParam
								MPCN_WRITE_PACKET,				// Parameter class: Unknown
								MPCN_VIDEO_ALIVE,				// Parameter class: MediaProcessorParam
								MPCN_AUDIO_ALIVE,				// Parameter class: MediaProcessorParam
								MPCN_BUFFERING_START,			// Parameter class: Unknown
								MPCN_BUFFERING_STOP,			// Parameter class: Unknown

								MPCN_ERROR};					// Parameter class: Unknown

/** @brief Base structure to send MediaProcessor notifications.
 *
 */
struct MediaProcessorParam
{
    MediaProcessorNotifyCode m_code;
	unsigned long m_mediaID;
};

/** @brief Receives notifications from MediaProcessor class.
 *
 * Any class that needs to receive MediaProcessor notifications should
 * be derived of this class.
 *
 */
class MediaProcessorNotify
{
public:
	virtual bool OnMediaProcessorNotify(MediaProcessorParam &param) = 0;
};

#endif
