#ifndef VIDEODISPLAYCONTROL_H
#define VIDEODISPLAYCONTROL_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "Texture.h"
#include "VideoSink.h"

class Skin;

#define VDC_ASPECT_AUTO			-1

/** @brief Displays video frames on an OpenGL component.
*
* This is a common VideoSink which is used for displaying video frames. Uses OpenGL,
* Consists of a wxPanel containing a wxGLCanvas and a label (to display information such
* as the corresponding user name) placed below it. It does automatic scaling and video
* aspect ratio correction.
*/
class VideoDisplayControl: public wxPanel, public VideoSink
{
protected:
	wxGLCanvas *m_canvas;				// For OpenGL drawing
	wxStaticText *m_label;				// Label control
	Skin *m_parentSkin;				// Skin to which this object relates
	wxImage m_staticImage;				// Optional static image
	bool m_createOk;				// Control creation went OK
	bool m_hasLabel;				// Controls label display
	wxString m_labelText;				// Label text
	wxBoxSizer *m_sizer;				// Sizer to organize things inside
	Texture *m_tex;					// OpenGL texture object
	float m_aspect;					// Aspect ratio
	float m_scaleX, m_scaleY;			// X/Y scale factors
	int m_videoWidth, m_videoHeight;		// Effective texture width/height

	void DoEnableCaption();
	void DoDisableCaption();
	void DoSetCaption();
	// Redrawing
	void Draw();

public:
	VideoDisplayControl();
	VideoDisplayControl(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel"));

	virtual ~VideoDisplayControl();

	bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
		long style = wxTAB_TRAVERSAL, const wxString& name = wxT("panel"));

	void Init();

	// Parent Skin setting
	void SetParentSkin(Skin *skin);

	// Video aspect control functions
	void SetVideoSize(int width, int height);
	void SetAspectRatio(float aspect);
	void RecalculateAspect();
	
	// VideoSink notifications
	void NotifyUpdate(const unsigned char *data, int width = -1, int height = -1); 
	void NotifyStart();
	void NotifyStop();

	// Caption control
	void SetCaption(const wxString &caption);
	void EnableCaption();
	void DisableCaption();
	bool HasCaption() const;

	// Static image to be displayed when there is no video
	void LoadStaticImage(const wxString &filename);
	void ShowStaticImage();

	// Events
	void OnCanvasSize(wxSizeEvent &event);
	void OnCanvasPaint(wxPaintEvent &event);
	void OnCanvasEraseBackground(wxEraseEvent &event);
	void OnCanvasMouseLeftUp(wxMouseEvent &event);
	void OnMouseLeftUp(wxMouseEvent &event);
	void OnCanvasDblClick(wxMouseEvent &event);
	void OnClose(wxCloseEvent &event);
	
	// Deinitialization
	void FreeTexture();

	DECLARE_EVENT_TABLE()
};

#endif
