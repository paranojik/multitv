#ifndef MEDIA_CAPTURE_NOTIFY_H
#define MEDIA_CAPTURE_NOTIFY_H

#include "CAudioFrame.h"

enum MediaCaptureNotifyCode	{	
									MCNC_CAPTURE_VIDEO_DATA,	// Parameter class: MediaCaptureVideoDataParam
									MCNC_CAPTURE_AUDIO_FRAME,	// Parameter class: MediaCaptureAudioDataParam
							};			
														

/** @brief Base structure to send MediaCapture notifications.
 *
 */
struct MediaCaptureParam
{
    MediaCaptureNotifyCode m_code;
};

/** @brief MediaCapture notification structure.
 *
 */
struct MediaCaptureVideoDataParam : public MediaCaptureParam
{
	unsigned char  *m_data;
	unsigned long m_dataLength;
	unsigned long m_width;
	unsigned long m_height;
};

/** @brief MediaCapture notification structure.
 *
 */
struct MediaCaptureAudioFrameParam : public MediaCaptureParam
{
	CAudioFrame *m_audioFrame;
};

/** @brief Receives notifications from MediaCapture class.
 *
 * Any class that needs to receive MediaCapture notifications should
 * be derived of this class.
 *
 */
class MediaCaptureNotify
{
public:
	virtual void OnMediaCaptureNotify(MediaCaptureParam &param) = 0;
};

#endif
