#include "IPTVClientApp.h"
#include "CamVideoSource.h"

/** Constructor. Sets information needed to initialize.
 * @param[in] camID	ID of the camera device.
 */
CamVideoSource::CamVideoSource(int camID)
{
	m_camID = camID;
}

/** Destructor. Closes the camera device.
 */
CamVideoSource::~CamVideoSource()
{
	Close();
}

/** Sets the camera ID.
 * @param[in] camID	Camera ID.
 */
void CamVideoSource::SetCamID(int camID)
{
	m_camID = camID;
}

/** Initializes the CamVideoSource. Opens the camera device.
 * @remarks This is an example of a VideoSource that needs special initialization:
 * CamVideoSource needs to open the camera device before taking video frames and
 * relaying them to VideoSinks.
 */
bool CamVideoSource::Init()
{
	IPTVClientApp &app = wxGetApp();
	m_isOk = app.InitializeCaptureDevice(m_camID);
	Start();
	return m_isOk;
}

/** Starts putting out frames.
 */
void CamVideoSource::Start()
{
	if(m_isOk)
	{
		m_isRunning = true;
		NotifyStartAll();
	}
}

/** Stops the VideoSource.
 */
void CamVideoSource::Stop()
{
	if(m_isRunning)
	{
		m_isRunning = false;
		NotifyStopAll();
	}
}

/** Closes the VideoSource. Also closes the camera device.
 */
void CamVideoSource::Close()
{
	if(m_isOk)
	{
		Stop();
		IPTVClientApp &app = wxGetApp();
		app.ReleaseCaptureDevice();
		m_isOk = false;
	}
}

/** Sends a new frame to the VideoSinks.
 * @param[in] data	Frame data. Must be in the format required by VideoSinks.
 * @param[in] width	Image width (pixels).
 * @param[in] height	Image height (pixels).
 */
void CamVideoSource::NewFrame(const unsigned char *data, int width, int height)
{
	if(m_isRunning)
		UpdateAllVideoSinks(data, width, height);
}
