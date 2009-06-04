#include <wx/wx.h>
#include "AppInterface.h"
#include "MediaVideoSource.h"

BEGIN_EVENT_TABLE(MediaVideoSource, wxEvtHandler)
	EVT_TIMER(wxID_ANY, MediaVideoSource::OnTimer)
END_EVENT_TABLE()

/** Constructor. Takes an AppInterface pointer and a media ID relating to a media transmission.
 * @param[in] iface		AppInterface pointer to use for interfacing with the application.
 * @param[in] mediaID	Transmission media ID.
 */
MediaVideoSource::MediaVideoSource(AppInterface *iface, unsigned long mediaID)
{
	m_isOk = false;
	m_timer.SetOwner(this);
	SetAppInterface(iface);
	SetMediaID(mediaID);
}

/** Destructor. Closes the transmission.
 */
MediaVideoSource::~MediaVideoSource()
{
	Close();
}

/** Sets the AppInterface pointer and initializes the MediaVideoSource if not done already.
 */
void MediaVideoSource::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
	if(!m_isOk)
		Init();
}

/** Sets transmission media ID.
 */
void MediaVideoSource::SetMediaID(unsigned long mediaID)
{
	m_mediaID = mediaID;
}

/** Returns the current AppInterface pointer.
 * @return The current AppInterface pointer.
 */
AppInterface *MediaVideoSource::GetAppInterface()
{
	return m_appInterface;
}

/** Returns the current transmission media ID.
 * @return The current transmission media ID.
 */
unsigned long MediaVideoSource::GetMediaID()
{
	return m_mediaID;
}

/** Initializes the MediaVideoSource. Activates all registered VideoSinks.
 * @return true on success, false on failure.
 * @remarks AppInterface pointer must be set before calling this function. SetAppInterface() calls it.
 */
bool MediaVideoSource::Init()
{
	if(!m_appInterface)
		m_isOk = false;
	else
		m_isOk = true;

	if(m_isOk)
		NotifyActivateAll();

	return m_isOk;
}

/** Starts the media poll timer, and begins delivering media frames.
 */
void MediaVideoSource::Start()
{
	if(m_isOk)
	{
		if(!m_isRunning)
		{
			m_timer.Start(MVS_UPDATE_PERIOD);
			m_isRunning = true;
			NotifyStartAll();
		}
	}
}

/** Stops the media poll timer. Notifies stop to all registered VideoSinks.
 */
void MediaVideoSource::Stop()
{
	if(m_isOk)
	{
		if(m_isRunning)
		{
			m_timer.Stop();
			m_isRunning = false;
			NotifyStopAll();
		}
	}
}

/** Closes the MediaVideoSource.
 * @remarks Currently, all this does is call Stop(), and notify deactivation to all VideoSinks.
 */
void MediaVideoSource::Close()
{
	Stop();
	NotifyDeactivateAll();
	m_isOk = false;
}

/** Timer event.
 * @param[in] event	Event object.
 */
void MediaVideoSource::OnTimer(wxTimerEvent &event)
{
	unsigned char *frameData;
	unsigned long width, height;
	// Bail out if not OK
	if(m_isOk)
	{
		// if GetVideoFrame succeeds, update the VDC.
		if(m_appInterface->GetVideoFrame(m_mediaID, &frameData, &width, &height))
			UpdateAllVideoSinks(frameData, width, height);			
	}
}
