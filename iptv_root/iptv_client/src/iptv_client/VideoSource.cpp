#include "VideoSink.h"
#include "VideoSource.h"

/** Default constructor.
 *
 */
VideoSource::VideoSource()
{
	m_isOk = false;
	m_isRunning = false;
}

/** Destructor. Calls UnregisterAllVideoSinks().
 *
 */
VideoSource::~VideoSource()
{
	Close();
	UnregisterAllVideoSinks();
}

/** Queries the control flag.
 * @return true if the VideoSource is ready, false otherwise.
 */
bool VideoSource::IsOk() const
{
	return m_isOk;
}

/** Checks whether or not this VideoSource is running (putting out frames).
 * @return true if the VideoSource is running, false otherwise
 */
bool VideoSource::IsRunning() const
{
	return m_isRunning;
}

/** Checks whether or not this VideoSource has any VideoSinks registered to it.
 * @return true if there are any registered VideoSinks, false otherwise
 */
bool VideoSource::HasVideoSink() const
{
	return (!m_sinkList.empty());
}

/** Register a VideoSink, so that it will receive frames from this VideoSource.
 * @param[in] sink	A pointer to the VideoSink object to be registered.
 * @remarks As VideoSources run asynchronously and without any specific control
 * in this aspect, it's advised not to register the same VideoSink to more than one
 * VideoSource. Although this is possible, results are unpredictable.
 */
void VideoSource::RegisterVideoSink(VideoSink *sink)
{
	VideoSinkListType::iterator it;
	// Don't register if already registered
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
	{
		if(*it == sink)
			return;
	}
	m_sinkList.push_back(sink);
	sink->NotifyRegister();
}

/** Unregisters a VideoSink, so that the VideoSource object will no longer send frames to it.
 * @param[in] sink	A pointer to the VideoSink object to be unregistered.
 */
void VideoSource::UnregisterVideoSink(VideoSink *sink)
{
	VideoSinkListType::iterator it;
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
	{
		if(*it == sink)
		{
			m_sinkList.erase(it);
			sink->NotifyUnregister();
			return;
		}
	}
}

/** Clears the list of registered VideoSinks for a VideoSource object.
 *
 */
void VideoSource::UnregisterAllVideoSinks()
{
	VideoSinkListType::iterator it;
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
		(*it)->NotifyUnregister();
	
	m_sinkList.clear();
}

/** Sends start notification to all registered VideoSinks.
 *
 */
void VideoSource::NotifyStartAll()
{
	VideoSinkListType::iterator it;
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
	{
		if(*it)
			(*it)->NotifyStart();
	}
}

/** Sends stop notification to all registered VideoSinks.
 *
 */
void VideoSource::NotifyStopAll()
{
	VideoSinkListType::iterator it;
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
	{
		if(*it)
			(*it)->NotifyStop();
	}
}

/** Sends activate notification to all registered VideoSinks.
 *
 */
void VideoSource::NotifyActivateAll()
{
		VideoSinkListType::iterator it;
		for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
		{
			if(*it)
				(*it)->NotifyActivate();
		}
}

/** Sends deactivate notification to all registered VideoSinks.
 *
 */
void VideoSource::NotifyDeactivateAll()
{
		VideoSinkListType::iterator it;
		for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
		{
			if(*it)
				(*it)->NotifyDeactivate();
		}
}

/** Sends a video frame to all registered VideoSinks. Notifies them to update.
 * @param[in] data	Frame data, which should be in accordance to what VideoSinks expect.
 * @param[in] width	Frame width (pixels).
 * @param[in] height	Frame height (pixels).
 */
void VideoSource::UpdateAllVideoSinks(const unsigned char *data, int width, int height)
{
	VideoSinkListType::iterator it;
	for(it = m_sinkList.begin(); it != m_sinkList.end(); it++)
	{
		if((*it) && (*it)->IsOk())
			(*it)->NotifyUpdate(data, width, height);
	}
}
