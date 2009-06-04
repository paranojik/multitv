#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <list>
#include "VideoSink.h"

typedef std::list<VideoSink *> VideoSinkListType;

/** @brief Base class for anything that provides video frames.
 *
 * This is an abstract class that represents a video source, i.e. anything that is able to
 * provide video frames. It keeps within itself a list of pointers to VideoSinks, which are
 * the objects which will receive those frames and process them. VideoSinks are registered
 * to receive frames using VideoSource::RegisterVideoSink() and removed from the list using
 * VideoSource::UnregisterVideoSink().
 *
 * @remarks VideoSinks should be unregistered prior to their destruction, so that VideoSources
 * don't send frames their way.
 */
class VideoSource
{
protected:
	bool m_isOk;
	bool m_isRunning;
	VideoSinkListType m_sinkList;
public:
	VideoSource();
	virtual ~VideoSource();

	bool IsOk() const;
	bool IsRunning() const;

	bool HasVideoSink() const;
	void RegisterVideoSink(VideoSink *sink);
	void UnregisterVideoSink(VideoSink *sink);
	void UnregisterAllVideoSinks();
	void NotifyStartAll();
	void NotifyStopAll();
	void NotifyActivateAll();
	void NotifyDeactivateAll();

	void UpdateAllVideoSinks(const unsigned char *data, int width, int height);

	virtual bool Init(){ return true; }
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Close(){}
};

#endif
