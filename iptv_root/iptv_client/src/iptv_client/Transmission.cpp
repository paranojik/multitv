#include <wx/wx.h>

#include "ChannelUser.h"
#include "TransmissionDisplayControl.h"
#include "Transmission.h"
#include "MediaVideoSource.h"
#include "VideoSink.h"
#include "IPTVClientApp.h"

/** Constructor. Initializes the object.
 * @param[in] mediaID	Media ID. Default: 0.
 * @param[in] userName	User nickname. Default: wxEmptyString.
 */
Transmission::Transmission(unsigned long mediaID, const wxString &userName)
{
	IPTVClientApp &app = wxGetApp();
	m_videoAlive = false;
	m_audioAlive = false;
	m_active = false;
	m_userName = userName;
	m_mediaID = mediaID;
	m_mediaVideoSource = new MediaVideoSource(&app, m_mediaID);
}

/** Destructor. Terminates the Transmission.
 */
Transmission::~Transmission()
{
	Terminate();
}

/** Get the MediaVideoSource object that handles video frames from the Transmission.
 * @return Pointer to the MediaVideoSource object.
 * @remarks Do not delete the MediaVideoSource object using the pointer returned. 
 * It is managed internally. Results will be unpredictable if it is deleted.
 */
MediaVideoSource *Transmission::GetMediaVideoSource()
{
	return m_mediaVideoSource;
}

/** Get the user nickname associated to this Transmission.
 * @return The user nickname.
 */
const wxString &Transmission::GetUserName() const
{
	return m_userName;
}

/** Checks whether the VideoSource has any VideoSinks registered to it.
 * @return true if there are any VideoSinks, false otherwise.
 */
bool Transmission::HasVideoSink() const
{
	if(!m_mediaVideoSource)
		return false;
	return m_mediaVideoSource->HasVideoSink();
}

/** Checks if the audio transmission is alive.
 * @return true if the audio transmission is alive.
 */
bool Transmission::IsAudioAlive() const
{
	return m_audioAlive;
}

/** Checks if the video transmission is alive.
 * @return true if the video transmission is alive.
 */
bool Transmission::IsVideoAlive() const
{
	return m_videoAlive;
}

/** Checks if the Transmission is active.
 *
 * This function checks to see if the Transmission is active. Being "active" means it is
 * receiving either audio or video alive notifications.
 *
 * @return true if the Transmission is active.
 */
bool Transmission::IsActive() const
{
	return m_active;
}

/** Returns the Transmission's Media ID.
 * @return The Media ID.
 */
unsigned long Transmission::GetMediaID() const
{
	return m_mediaID;
}

/** Sets the Transmission's Media ID.
 * @param[in] mediaID	The ID value to set.
 */
void Transmission::SetMediaID(unsigned long mediaID)
{
	m_mediaID = mediaID;
	if(m_mediaVideoSource)
		m_mediaVideoSource->SetMediaID(mediaID);
}

/** Sets the "audio alive" flag.
 * @param[in] alive	Value to set.
 */
void Transmission::SetAudioAlive(bool alive)
{
	m_audioAlive = alive;
}

/** Sets the "video alive" flag
 * @param[in] alive	Value to set.
 */
void Transmission::SetVideoAlive(bool alive)
{
	m_videoAlive = alive;
	if(m_active)
	{
		if(!m_mediaVideoSource)
		{
			IPTVClientApp &app = wxGetApp();
			m_mediaVideoSource = new MediaVideoSource(&app, m_mediaID);
		}
		if(m_videoAlive)
			m_mediaVideoSource->Start();
		else
			m_mediaVideoSource->Stop();
	}
}

/** Registers a VideoSink to the MediaVideoSource object inside.
 * @param[in] sink	Pointer to the VideoSink object to register.
 */
void Transmission::RegisterVideoSink(VideoSink *sink)
{
	if((!m_mediaVideoSource) || (!sink))
		return;
	m_mediaVideoSource->RegisterVideoSink(sink);
}

/** Set active flag.
 */
void Transmission::Activate()
{
	if(!m_active)
	{
		m_active = true;
		if(!m_mediaVideoSource)
		{
			IPTVClientApp &app = wxGetApp();
			m_mediaVideoSource = new MediaVideoSource(&app, m_mediaID);
		}
		//if(m_videoAlive)
		//{
		m_mediaVideoSource->Init();
		if(m_mediaVideoSource->IsOk())
			m_mediaVideoSource->Start();
		//}
	}
}

/** Reset active flag.
 */
void Transmission::Deactivate()
{
	if(m_active)
	{
		if(m_mediaVideoSource)
			m_mediaVideoSource->Close();
		m_active = false;
	}
}

/** Terminates the Transmission, and cleans up the object.
 */
void Transmission::Terminate()
{
	if(m_mediaVideoSource)
		delete m_mediaVideoSource;
	m_mediaVideoSource = NULL;
	m_videoAlive = false;
	m_audioAlive = false;
	m_active = false;
	m_userName = wxEmptyString;
}
