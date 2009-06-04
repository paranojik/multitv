#ifndef TRANSMISSION_DISPLAY_CONTROL_H
#define TRANSMISSION_DISPLAY_CONTROL_H

#include <wx/wx.h>

#include "ChannelUser.h"
#include "VideoDisplayControl.h"
#include "VideoSink.h"

DECLARE_LOCAL_EVENT_TYPE(iptvEVT_VDC_DBLCLICK, -1)

class Skin;

/** @brief Used to control display of Transmission video frames.
*
* This class is the VideoSink for Transmission video frames. It contains a VideoDisplayControl
* which does the actual display, and controls the (re-)creation of it.
*
* @remarks This class is purpose-built to deal with the problem caused by multiple display modes
* (which is a Skin-specific issue really, but is within the scope of the business rules) on the
* Linux/GTK+ version of the application (using Reparent() on a wxGLCanvas doesn't work).
*/
class TransmissionDisplayControl: public wxEvtHandler, public VideoSink
{
public:
	TransmissionDisplayControl();
	TransmissionDisplayControl(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxTAB_TRAVERSAL, const wxString& name = wxT("TransmissionDisplayControl"));
	
	void Create(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxTAB_TRAVERSAL, const wxString& name = wxT("TransmissionDisplayControl"));

	virtual ~TransmissionDisplayControl();

	void SetParentSkin(Skin *skin);
	
	void SetNewParentWindow(wxWindow *win);

	// VideoSink method overrides
	void NotifyActivate();
	void NotifyDeactivate();
	void NotifyStart();
	void NotifyStop();
	void NotifyUpdate(const unsigned char *frameData, int width, int height);
	
	VideoDisplayControl *GetVideoDisplayControl();
	void DestroyVideoDisplayControl();
	void Destroy();

	// wxEvtHandler that processes events from the VDC
	void SetVDCEventHandler(wxEvtHandler *handler);

	void SetChannelUser(ChannelUser *user);
	const ChannelUser *GetChannelUser() const;
	
	void SetCaption(const wxString &label);
	void DisableCaption();

	void SetAspectRatio(float aspect);

	void SetSpecialFlag(bool flag);
	bool GetSpecialFlag() const;

	// Handles VDC click events
	void OnVDCDblClick(wxMouseEvent &event);

	bool operator < (const TransmissionDisplayControl &that);

private:
	VideoDisplayControl *m_vdc;
	wxEvtHandler *m_vdcEventHandler;
	ChannelUser *m_channelUser;
	wxString m_strLabel;
	float m_aspectRatio;
	Skin *m_parentSkin;
	bool m_flgSpecial;
	
	void DoInit();
};

#endif	// TRANSMISSION_DISPLAY_CONTROL_H
