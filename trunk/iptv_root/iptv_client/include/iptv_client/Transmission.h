#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "MediaVideoSource.h"
#include "VideoSink.h"

/** @brief Represents a media transmission.
 *
 * This class represents a media transmission. In short, it manages information passed along by
 * the lower layers (kernel, media) and provides a way for the application to keep track of them
 * (see TransmissionManager). Each object contains a MediaVideoSource, which is used to relay
 * video frames corresponding to the Transmission to the VideoSinks registered with it.
 */
class Transmission
{
private:
	MediaVideoSource *m_mediaVideoSource;
	wxString m_userName;		// Has to be saved this way because of the different handling on MULTICAST mode.
	unsigned long m_mediaID;	// Saved because of the way MediaVideoSource is handled
	bool m_audioAlive;
	bool m_videoAlive;
	bool m_active;

public:
	Transmission(unsigned long mediaID = 0, const wxString &userName = wxEmptyString);
	virtual ~Transmission();

	MediaVideoSource *GetMediaVideoSource();
	unsigned long GetMediaID() const;
	const wxString &GetUserName() const;
	bool IsAudioAlive() const;
	bool IsVideoAlive() const;
	bool IsActive() const;

	void SetMediaID(unsigned long mediaID);
	void SetUserName(const wxString &userName);
	void SetAudioAlive(bool alive);
	void SetVideoAlive(bool alive);
	void RegisterVideoSink(VideoSink *sink);
	bool HasVideoSink() const;
	
	void Activate();
	void Deactivate();

	void Terminate();
};

#endif	// TRANSMISSION_H
