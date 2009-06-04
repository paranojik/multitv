#include <wx/wx.h>

#include "ChannelUser.h"
#include "TransmissionDisplayControl.h"
#include "VideoDisplayControl.h"
#include "Skin.h"

DEFINE_EVENT_TYPE(iptvEVT_VDC_DBLCLICK)

// C'tors
/** Default constructor. Initializes the class.
 */
TransmissionDisplayControl::TransmissionDisplayControl()
{
	DoInit();
	m_vdc = new VideoDisplayControl;
	m_vdc->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(TransmissionDisplayControl::OnVDCDblClick), NULL, this);
}

/** Constructor. Passes given parameters to internally-managed VideoDisplayControl, creating it.
 * @param[in] parent	Parent window. Must not be NULL.
 * @param[in] id		VDC's window ID.
 * @param[in] pos		VDC's position on parent.
 * @param[in] size		VDC's size.
 * @param[in] style		Window style. See wxWidgets documentation.
 * @param[in] name		Window name. See wxWidgets documentation.
 */
TransmissionDisplayControl::TransmissionDisplayControl(wxWindow *parent, wxWindowID id, 
													   const wxPoint &pos, const wxSize &size, 
													   long style, const wxString &name)
{
	DoInit();
	m_vdc = new VideoDisplayControl(parent, id, pos, size, style, name);
	m_vdc->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(TransmissionDisplayControl::OnVDCDblClick), NULL, this);
}


/** Destructor.
 */
TransmissionDisplayControl::~TransmissionDisplayControl()
{
	DestroyVideoDisplayControl();
}

// Private member initialization
void TransmissionDisplayControl::DoInit()
{
	m_vdc = NULL;
	m_channelUser = NULL;
	m_parentSkin = NULL;
	m_vdcEventHandler = NULL;
	m_flgSpecial = false;
	m_isOk = false;			// inherited from VideoSink
}

/** Creates the internally-managed VDC. Use for two-step creation.
 * @param[in] parent	Parent window. Must not be NULL.
 * @param[in] id		VDC's window ID.
 * @param[in] pos		VDC's position on parent.
 * @param[in] size		VDC's size.
 * @param[in] style		Window style. See wxWidgets documentation.
 * @param[in] name		Window name. See wxWidgets documentation.
 * @remarks Do not call if the object was constructed using the second constructor.
 */
void TransmissionDisplayControl::Create(wxWindow *parent, wxWindowID id, 
										const wxPoint &pos, const wxSize &size, 
										long style, const wxString &name)
{
	m_vdc->Create(parent, id, pos, size, style, name);
}

/** Relocates the internally-managed VDC to a new parent window.
 *
 * This method relocates the internally-managed VideoDisplayControl to a new parent window.
 * This is a wrapper around wxWindow::Reparent() for the VideoDisplayControl to work around
 * a quirk in wxWidgets involving wxWindow::Reparent() for wxGLCanvas or windows containing one
 * which causes crashes in Unix/GTK+.
 *
 * @param[in] win	New parent window.
 * @remarks Under Win32 this function just calls Reparent() on the VDC.
 */
void TransmissionDisplayControl::SetNewParentWindow(wxWindow *win)
{
	if(m_vdc)
	{

#ifdef __WXGTK__
		m_vdc->Destroy();
		m_vdc = new VideoDisplayControl(win, wxID_ANY);
		m_vdc->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(TransmissionDisplayControl::OnVDCDblClick), NULL, this);
		SetCaption(m_strLabel);
		SetAspectRatio(m_aspectRatio);
#else
		m_vdc->Reparent(win);
#endif
	}
}

/** Set the wxEvtHandler object to receive events coming from the internally-managed VDC.
 * @param[in] handler	Pointer to the event handler object.
 * @remarks This may be useful to capture clicks on the VDCs, for instance.
 */
void TransmissionDisplayControl::SetVDCEventHandler(wxEvtHandler *handler)
{
	m_vdcEventHandler = handler;	
}

/** Set Skin pointer inside the VDC.
 * @param[in] skin	Skin pointer
 * @remarks This is an item to be revised. May be eliminated in a future revision.
 */
void TransmissionDisplayControl::SetParentSkin(Skin *skin)
{
	m_parentSkin = skin;
}

/** Set "special" flag on the TransmissionDisplayControl object.
 *
 * This method sets the "special" flag on the TransmissionDisplayControl object.
 * This flag influences ordering of the TransmissionDisplayControls. Objects with
 * the flag set will take precedence over those whose flag is not set.
 *
 * @param[in] flag	Value to set.
 */
void TransmissionDisplayControl::SetSpecialFlag(bool flag)
{
	m_flgSpecial = flag;
}

/** Queries the value of "special" flag.
 * @return Flag value.
 */
bool TransmissionDisplayControl::GetSpecialFlag() const
{
	return m_flgSpecial;
}

/** Sets ChannelUser associated to this object.
 * @param[in] user	User to associate to this control.
 */
void TransmissionDisplayControl::SetChannelUser(ChannelUser *user)
{
	m_channelUser = user;
	if(m_channelUser)
	{
		if(m_vdc)
		{
			m_vdc->EnableCaption();
			m_vdc->SetCaption(user->GetNickName());
		}
	}
	else
		if(m_vdc)
			m_vdc->DisableCaption();
}

/** Gets ChannelUser associated to this object.
 * @return Const pointer to User to associated this control.
 */
const ChannelUser *TransmissionDisplayControl::GetChannelUser() const
{
	return m_channelUser;
}

// Label control
/** Enable the VDC label. Sets text.
 * @param[in] label	Text to set to the label.
 */
void TransmissionDisplayControl::SetCaption(const wxString &label)
{
	if(label == wxEmptyString)
	{
		DisableCaption();
	}
	else
	{
		m_strLabel = label;
		if(m_vdc)
			m_vdc->SetCaption(m_strLabel);
	}
}

/** Disable label.
 */
void TransmissionDisplayControl::DisableCaption()
{
	m_strLabel = wxEmptyString;
	if(m_vdc)
		m_vdc->DisableCaption();
}

/** Set VDC's target aspect ratio.
 *
 * VideoDisplayControl has automatic aspect ratio correction capability. Use this function to
 * set the target aspect ratio for the internally-managed VDC (usually 4/3)
 *
 * @param[in] aspect	Aspect ratio to set. Use VDC_ASPECT_AUTO (-1) for automatic (based on texture size).
 */
void TransmissionDisplayControl::SetAspectRatio(float aspect)
{
	m_aspectRatio = aspect;
	if(m_vdc)
	{
		m_vdc->SetAspectRatio(m_aspectRatio);
	}
}

/* Accesses the internally-managed VDC.
 * @return A pointer to the internally-managed VDC.
 */
VideoDisplayControl *TransmissionDisplayControl::GetVideoDisplayControl()
{
	return m_vdc;
}

/** Destroys the internally-managed VDC.
 */
void TransmissionDisplayControl::DestroyVideoDisplayControl()
{
	if(m_vdc)
	{
		m_vdc->Destroy();
		m_vdc = NULL;
	}
}

/** Destroys the object. Takes the internally-managed VDC with it.
 */
void TransmissionDisplayControl::Destroy()
{
	DestroyVideoDisplayControl();
	delete this;
}

/** Passes the double-click event of the VDC over to the registered wxEvtHandler, as
 * an iptvEVT_VDC_DBLCLICK type event.
 */
void TransmissionDisplayControl::OnVDCDblClick(wxMouseEvent &event)
{
	if(m_vdcEventHandler)
	{
		// Send out an iptvEVT_VDC_DBLCLICK to the specified handler.
		wxCommandEvent cmdEvent(iptvEVT_VDC_DBLCLICK);
		cmdEvent.SetClientData((void *)this);
		wxPostEvent(m_vdcEventHandler, cmdEvent);
	}
}

/** Responds to VideoSource's activation notification.
 */
void TransmissionDisplayControl::NotifyActivate()
{

	if(m_vdc && !m_vdc->IsShown())
	{
		m_vdc->Show();
		m_vdc->ShowStaticImage();
/*
		m_canvas->SetCurrent();
		if(!m_tex)
			m_tex = new Texture(m_videoWidth, m_videoHeight);
		else if(!m_tex->IsOk())
			m_tex->New(m_videoWidth, m_videoHeight);
*/
	}

	if(m_parentSkin)
		m_parentSkin->AddTransmissionDisplayControl(this);
}

/** Responds to VideoSource's deactivation notification.
 */
void TransmissionDisplayControl::NotifyDeactivate()
{
	if(m_vdc && m_vdc->IsShown())
	{
		m_vdc->FreeTexture();
		m_vdc->Hide();
	}
	// TODO: give out some sort of notification to its parent

	if(m_parentSkin)
		m_parentSkin->RemoveTransmissionDisplayControl(this);
}

/** Responds to VideoSource's start notification.
 */
void TransmissionDisplayControl::NotifyStart()
{
	if(m_vdc)
	{
		m_vdc->NotifyStart();
		m_isOk = true;
	}
}

/** Responds to VideoSource's stop notification.
 */
void TransmissionDisplayControl::NotifyStop()
{
	if(m_vdc)
	{
		m_vdc->NotifyStop();
		m_isOk = false;
	}
}

/** Responds to VideoSource's display update notification.
 */
void TransmissionDisplayControl::NotifyUpdate(const unsigned char *frameData, int width, int height)
{
	if(m_vdc)
	{
		m_vdc->NotifyUpdate(frameData, width, height);
	}
}

/** Handles ordering of the Transmissions in the window.
 * Criteria are the same for ordering in the User List (see ChannelUser::operator <),
 * with the addition that transmissions marked with the "special" flag take further
 * precedence.
 *
 * @param[in] that	Right-hand side of the operator.
 * @return true if left-hand side precedes right-hand side.
 */
bool TransmissionDisplayControl::operator <(const TransmissionDisplayControl &that)
{
	const ChannelUser *otherUser = that.GetChannelUser();
	
	// No special treatment if both controls' special flag status is equal
	if(m_flgSpecial == that.m_flgSpecial)
	{
		if(!otherUser)
		{
			if(!m_channelUser) 
				// Return false if both are NULL - if you use the standard operator < in a standard type,
				// say 3 < 3, it returns false no matter what the order of the '3's is.
				return false;
			else
				// Non-NULL has precedence
				return true;
		}
		else
		{
			if(!m_channelUser)
				return false;
			return (*m_channelUser < *otherUser);
		}
	}
	else // If only one of them is flagged, it has precedence
	{
		// Control reaching here means that if m_flgSpecial is true then
		// that.m_flgSpecial is false and vice versa
		return m_flgSpecial;
	}
}
