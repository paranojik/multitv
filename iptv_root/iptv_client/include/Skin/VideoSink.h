#ifndef VIDEOSINK_H
#define VIDEOSINK_H

/** @brief Represents an object that receives video frames.
 *
 * This abstract class represent a video sink, i.e. an object that receives video frames.
 * Actual video frame displaying objects inherit from this class.
 *
 */
class VideoSink
{
protected:
	bool m_isOk;
public:
	VideoSink() { m_isOk = false; }
	virtual ~VideoSink(){}
	
	virtual bool IsOk() const { return m_isOk; }
	virtual void NotifyRegister(){}		// Called when the object is registered to a VideoSource
	virtual void NotifyUnregister(){}	// Called when the object is unregistered from a VideoSource
	virtual void NotifyActivate(){}		// Special notification for media events. Implementation not mandatory.
	virtual void NotifyDeactivate(){}	// Special notification for media events. Implementation not mandatory.
	virtual void NotifyStart() = 0;		// Called when VideoSource starts.
	virtual void NotifyStop() = 0;		// Called when VideoSource stops.
	virtual void NotifyUpdate(const unsigned char *frameData, int width, int height) = 0;	// Used by VideoSource to send frames
};

#endif
