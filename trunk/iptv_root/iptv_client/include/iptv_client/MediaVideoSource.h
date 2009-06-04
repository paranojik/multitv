#ifndef MEDIAVIDEOSOURCE_H
#define MEDIAVIDEOSOURCE_H

#include <wx/wx.h>
#include "AppInterface.h"
#include "VideoSource.h"

#define MVS_UPDATE_PERIOD		30	// 33.33 frames/second

/** @brief Provides video frames from media (collaborations).
 *
 * This is a VideoSource-derived class that provides frames from media collaborations. It also
 * derived from wxEvtHandler and receives timer events, using them to poll the media library for
 * new frames and relaying them to registered VideoSinks when appropriate.
 *
 */
class MediaVideoSource: public wxEvtHandler, public VideoSource
{
private:
	AppInterface *m_appInterface;
	wxTimer m_timer;
	unsigned long m_mediaID;
public:
	MediaVideoSource(AppInterface *iface = NULL, unsigned long mediaID = 0);
	virtual ~MediaVideoSource();

	void SetAppInterface(AppInterface *iface);
	void SetMediaID(unsigned long mediaID);

	AppInterface *GetAppInterface();
	unsigned long GetMediaID();

	bool Init();
	void Start();
	void Stop();
	void Close();

	void OnTimer(wxTimerEvent &event);

	DECLARE_EVENT_TABLE()

	//unsigned char *GetFrame();
	//void GetSize(int &width, int &height);
};

#endif
