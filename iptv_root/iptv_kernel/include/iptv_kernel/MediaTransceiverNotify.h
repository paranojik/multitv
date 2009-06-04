#ifndef MEDIA_TRANSCEIVER_NOTIFY_H
#define MEDIA_TRANSCEIVER_NOTIFY_H

enum MediaTransceiverNotifyCode	{	
									MTNC_NEW_TRANSMISSION,		// Parameter class: MediaTransceiverIDParam
									MTNC_DATA_READ_MEDIA,		// Parameter class: MediaTransceiverDataParam
									MTNC_DATA_READ_APP,			// Parameter class: MediaTransceiverDataParam
									MTNC_NOTIFY_PROGRESS		// Parameter class: MediaTransceiverProgressParam
								};			
														

/** @brief Base structure to send MediaTransceiver notifications.
 *
 */
struct MediaTransceiverParam
{
    MediaTransceiverNotifyCode m_code;
};

/** @brief MediaTransceiver notification structure.
 *
 */
struct MediaTransceiverIDParam : public MediaTransceiverParam
{
	unsigned long m_mediaID;
};

/** @brief MediaTransceiver notification structure.
 *
 */
struct MediaTransceiverDataParam : public MediaTransceiverParam
{
	unsigned char  *m_data;
	unsigned long m_dataLength;
};

/** @brief MediaTransceiver notification structure.
 *
 */
struct MediaTransceiverProgressParam : public MediaTransceiverParam
{
	unsigned long m_mediaID;
	double m_percent;
	unsigned char m_flags;
};

/** @brief Receives notifications from MediaTransceiver class.
 *
 * Any class that needs to receive MediaTransceiver notifications should
 * be derived of this class.
 *
 */
class MediaTransceiverNotify
{
public:
	virtual bool OnMediaTransceiverNotify(MediaTransceiverParam &param) = 0;
};

#endif
