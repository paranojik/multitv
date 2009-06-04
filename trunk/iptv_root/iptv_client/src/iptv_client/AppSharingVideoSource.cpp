#include <wx/wx.h>
#include "AppSharingVideoSource.h"

/** Default constructor.
 */
AppSharingVideoSource::~AppSharingVideoSource()
{
}

/** Initializes the VideoSource.
 * @remarks Currently, all this method does is set the control flag, but is provided
 * from the base class as in the general case VideoSources may require special initialization.
 */
bool AppSharingVideoSource::Init()
{
	m_isOk = true;
	return true;
}

/** Starts putting out video frames.
 */
void AppSharingVideoSource::Start()
{
	if(m_isOk)
	{
		NotifyStartAll();
		m_isRunning = true;
	}
}

/** Stops the VideoSource.
 */
void AppSharingVideoSource::Stop()
{
	if(m_isRunning)
	{
		m_isRunning = false;
		NotifyStopAll();
	}
}

/** Stops the VideoSource, and notifies all VideoSinks of deactivation.
 */
void AppSharingVideoSource::Close()
{
	if(m_isOk)
	{
		if(m_isRunning)
		{
			Stop();
			NotifyDeactivateAll();
		}
		m_isOk = false;
	}
}

/** Sends out a new video frame to all registered VideoSinks.
 * @param[in] data	Video data. Must be in the format required by the VideoSinks.
 * @param[in] width	Image width (pixels).
 * @param[in] height	Image height (pixels).
 */
void AppSharingVideoSource::NewFrame(const unsigned char *data, int width, int height)
{
	if(m_isOk && m_isRunning && HasVideoSink())
	{
		UpdateAllVideoSinks(data, width, height);
	}
}

