#include <wx/wx.h>
#include <wx/glcanvas.h>

#ifdef IPTV_DEBUG
#include <iostream>
#endif

#ifdef SKIN_API
#undef SKIN_API
#endif
#define SKIN_API


#include "Texture.h"
#include "VideoDisplayControl.h"
#include "Constants.h"

#include "Skin.h"

BEGIN_EVENT_TABLE(VideoDisplayControl, wxPanel)
	EVT_LEFT_UP(VideoDisplayControl::OnMouseLeftUp)
	EVT_CLOSE(VideoDisplayControl::OnClose)
END_EVENT_TABLE()

/** Default constructor. Initializes the class.
 */
VideoDisplayControl::VideoDisplayControl()
{
	Init();
}

/** Constructor. This effectively creates the window.
 * @param[in] parent	Parent window. Must not be NULL.
 * @param[in] id		Window ID. See wxWidgets documentation.
 * @param[in] pos		Window position relative to parent.
 * @param[in] size		Window size.
 * @param[in] style		Window style. See wxWidgets documentation on wxPanel.
 * @param[in] name		Window name. See wxWidgets documentation.
 */
VideoDisplayControl::VideoDisplayControl(wxWindow* parent, wxWindowID id, 
		const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Init();
	Create(parent, id, pos, size, style, name);
}

void VideoDisplayControl::Init()
{
	// Initialize control member variables to its defaults
	m_hasLabel = false;
	m_createOk = false;
	m_isOk = false;		// inherited from VideoSink
	m_label = NULL;
	m_tex = NULL;
	m_aspect = VDC_ASPECT_AUTO;
	m_videoWidth = -1;
	m_videoHeight = -1;
	m_scaleX = 1.0f;
	m_scaleY = 1.0f;
}

/** Creates the window.
 * @param[in] parent	Parent window. Must not be NULL.
 * @param[in] id		Window ID. See wxWidgets documentation.
 * @param[in] pos		Window position relative to parent.
 * @param[in] size		Window size.
 * @param[in] style		Window style. See wxWidgets documentation on wxPanel.
 * @param[in] name		Window name. See wxWidgets documentation.
 * @return True if creation succeeds.
 * @remarks Do not call if object was constructed with the second constructor.
 */
bool VideoDisplayControl::Create(wxWindow* parent, wxWindowID id, 
		const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	bool ret = wxPanel::Create(parent, id, pos, size, style, name);
	if(ret)
	{
		Show();
		// Create the canvas and sizer
		
		// OpenGL initialization attribute list.
		int *attributeList = NULL;

#ifdef __WXGTK__
		// NVIDIA compatibility workaround
		int nvAttributeList[] = 
		{
			WX_GL_DOUBLEBUFFER,
			0
		};

		const unsigned char *szGLVersion = glGetString(GL_VERSION);
		if(szGLVersion)
		{
			wxString strGLVersion((const char *)szGLVersion, wxConvUTF8);
			int subStringPos = strGLVersion.Find(wxT("NVIDIA"));
			if(subStringPos != wxNOT_FOUND)
			{
				long nvVersionNumber;
				strGLVersion.AfterLast(' ').BeforeFirst('.').ToLong(&nvVersionNumber);
				if(nvVersionNumber > 96)	// 96.43.05 was the last NVIDIA driver version without the problem
				{
// #ifdef IPTV_DEBUG
					std::cout << "VideoDisplayControl::Create(): NVIDIA driver version " 
					          << strGLVersion.AfterLast(' ').mb_str() 
					          << " detected. Enabling workaround." << std::endl;
// #endif
					
					attributeList = nvAttributeList;
				}
			}
		}
#endif
		m_canvas = new wxGLCanvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("GLCanvas"), attributeList, wxNullPalette);
		m_sizer = new wxBoxSizer(wxVERTICAL);
		m_canvas->Show();
		SetSizer(m_sizer);
		m_sizer->Add(m_canvas, 1, wxALL | wxEXPAND);
		m_sizer->Layout();
		
		// Other controls may now be created
		m_createOk = true;

		// Connect wxGLCanvas' events to VideoDisplayControl member functions
		m_canvas->Connect(wxEVT_PAINT, wxPaintEventHandler(VideoDisplayControl::OnCanvasPaint), NULL, this);
		m_canvas->Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(VideoDisplayControl::OnCanvasEraseBackground), NULL, this);
		m_canvas->Connect(wxEVT_SIZE, wxSizeEventHandler(VideoDisplayControl::OnCanvasSize), NULL, this);
		m_canvas->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(VideoDisplayControl::OnCanvasMouseLeftUp), NULL, this);
		m_canvas->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(VideoDisplayControl::OnCanvasDblClick), NULL, this);

		// Create the texture
		if(!m_tex)
		{
			LoadStaticImage(VIDEO_BITMAP_FILE_NAME);
			ShowStaticImage();
		}

		// If label has been set, show it
		if(HasCaption())
		{
			DoEnableCaption();
			DoSetCaption();
		}
		m_isOk = m_createOk;
	}
	return ret;
}
/** Destructor.
 */
VideoDisplayControl::~VideoDisplayControl()
{
}

/** Set a pointer through which this control can access Skin methods.
 * @param[in] skin	Skin pointer.
 */
void VideoDisplayControl::SetParentSkin(Skin *skin)
{
	m_parentSkin = skin;
}

/** Set the video size, and recalculates aspect for the new size.
 * @param[in] width		New width.
 * @param[in] height	New height.
 */
void VideoDisplayControl::SetVideoSize(int width, int height)
{
	bool sizeChanged = false;
	// Check for size change
	if(m_videoWidth != width)
	{
		m_videoWidth = width;
		sizeChanged = true;
	}
	if(m_videoHeight != height)
	{
		m_videoHeight = height;
		sizeChanged = true;
	}

	if(sizeChanged)	// Determine whether or not the texture must be recreated
	{
		// All texture operations must be done for this canvas' GL context
		m_canvas->SetCurrent();	

		if(!m_tex)	// Create the texture if it hasn't been created yet
		{
			m_tex = new Texture(m_videoWidth, m_videoHeight);
		}
		else
		{
			if(m_tex->IsOk())
				m_tex->ChangeSize(m_videoWidth, m_videoHeight);
			else
				m_tex->New(m_videoWidth, m_videoHeight);
		}
		// Recalculate aspect after a size change
		RecalculateAspect();
	}
}

/** Does the OpenGL redrawing.
 */
void VideoDisplayControl::Draw()
{
	if(m_tex && m_tex->IsOk())
	{
		// Center the viewport on the window
		float w, h;
		int winWidth, winHeight;
		// OpenGL animation code goes here
		glBindTexture(GL_TEXTURE_2D, m_tex->GetID());
		glClear(GL_COLOR_BUFFER_BIT);
		// set viewport, centered on window
		m_canvas->GetSize(&winWidth, &winHeight);
		glViewport(0, 0, winWidth, winHeight);
		// translate normalized GL scale into a nicer scale
		// after this, each unit in the viewport 
		// will correspond to 1/2 pixel
		w = 1.0/winWidth;
		h = 1.0/winHeight;
		glPushMatrix();
		glScalef(w, h, 1.0f);
		glPushMatrix();
		glScalef(m_scaleX, m_scaleY, 1.0f);
		// -------------------------------------------------------
		// Coordinates:
		// -------------------------------------------------------
		// Texture coordinates:
		// x: "0" refers to the left edge of the texture image.
		//    "1" refers to the right edge of the texture image.
		// y: "0" refers to the top edge of the texture image.
		//    "1" refers to the bottom edge of the image texture.
		// So (0.0,0.0) refers to the top-left corner and
		// (1.0,1.0) refers to the bottom-right corner.
		// -------------------------------------------------------
		// 2D vertex coordinates (normalized):
		// A value of "0" refers to the center of each axis.
		// So (0, 0) refers to the center of the current viewport.
		// Negative values are to the left/below the center, and
		// positive values are to the right/above the center. So
		// (-1, -1) is the top-left corner and (1, 1) is the bottom
		// -right corner.
		// Cartesian axis scheme:
		//                        ^ y
		//(-1, 1)*                |                * (1, 1)
		//                        |
		//                        |(0, 0)
		//   ----|----------------+----------------|-->x
		//(-1, 0)                 |                (1, 0)
		//                        |
		//(-1,-1)*                | (0, -1)        *(1, -1)
		//
		// -------------------------------------------------------
		// Update: The scheme above is scaled by glScalef() above this comment
		// to correspond better to the image dimensions in pixels. This made
		// it better to code the aspect ratio correction.
		int px = m_tex->GetWidth();
		int py = m_tex->GetHeight();
		float tx = m_tex->GetActualToEffectiveRatioX();
		float ty = m_tex->GetActualToEffectiveRatioY();
		glBegin(GL_QUADS);
		glTexCoord2d(0.0,0.0); glVertex2i(-(px),+(py));
		glTexCoord2d(0.0,ty); glVertex2i(-(px),-(py));
		glTexCoord2d(tx,ty); glVertex2i(+(px),-(py));
		glTexCoord2d(tx,0.0); glVertex2i(+(px),+(py));
		glEnd();
		// There must be as many calls to glPopMatrix() here
		// as there are glPushMatrix() calls above
		glPopMatrix();
		glPopMatrix();
	}
}

/** Enable caption.
 */
void VideoDisplayControl::EnableCaption()
{
	// This creates the wxStaticText and adding it to the sizer.
	if(!m_hasLabel)
	{
		DoEnableCaption();
		m_hasLabel = true;
	}
}

void VideoDisplayControl::DoEnableCaption()
{
	if(m_createOk)
	{
		m_label = new wxStaticText(this, wxID_ANY, wxT("Caption"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
		m_sizer->Add(m_label, 0, wxEXPAND | wxALIGN_CENTER);
		m_sizer->Layout();
	}
}

/** Disable caption, leaving only the video.
 */
void VideoDisplayControl::DisableCaption()
{
	if(m_hasLabel)
	{
		DoDisableCaption();
		m_hasLabel = false;
	}
}

void VideoDisplayControl::DoDisableCaption()
{
	if(m_createOk && m_label)
	{
		m_sizer->Detach(m_label);
		m_sizer->Layout();
		m_label->Destroy();
	}
}

/** Checks whether or not the caption is enabled.
 * @return true if the caption is enabled.
 */
bool VideoDisplayControl::HasCaption() const
{
	return m_hasLabel;
}

/** Set the caption string.
 * @param[in] caption	String to set.
 */
void VideoDisplayControl::SetCaption(const wxString &caption)
{
	if(!HasCaption())
		EnableCaption();
	m_labelText = caption;
	DoSetCaption();
}

void VideoDisplayControl::DoSetCaption()
{
	if(m_createOk)
	{
		m_label->SetLabel(m_labelText);
		m_sizer->Layout();
	}
}

/** Loads a static image, to be displayed when there is no video.
 * @param[in] filename	Full path of the file to load.
 */
void VideoDisplayControl::LoadStaticImage(const wxString &filename)
{
	m_staticImage.LoadFile(filename);
}

/** Show the static image.
 */
void VideoDisplayControl::ShowStaticImage()
{
	if(IsShown() && m_staticImage.IsOk())
	{
		m_canvas->SetCurrent();
		FreeTexture();
		m_tex = new Texture(m_staticImage);
		m_videoWidth = m_tex->GetEffectiveWidth();
		m_videoHeight = m_tex->GetEffectiveHeight();
		RecalculateAspect();
		m_canvas->Refresh();
		m_canvas->Update();
	}
}

/** Does the aspect ratio correction.
 *
 * This method recalculates the aspect ratio when the image size or the control size changes.
 */
void VideoDisplayControl::RecalculateAspect()
{
	// Assert or exception are welcome here
	if(!m_tex) return;
	// zoom to window, keep image aspect ratio
	float aspect;
	int winWidth, winHeight;
	// get window dimensions
	m_canvas->GetSize(&winWidth, &winHeight);
	// if aspect is set to Auto, use aspect calculated from texture
	if(m_aspect <= 0)
		aspect = m_tex->GetAspectRatio();
	else // else, use manual aspect ratio
		aspect = m_aspect;
	float winAspect = (float)winWidth / (float)winHeight;
	if(aspect > winAspect)
	{
		m_scaleX = (float)winWidth / m_tex->GetWidth();
		m_scaleY = ((float)winWidth / aspect) / m_tex->GetHeight();
	}
	else
	{
		m_scaleY = (float)winHeight / m_tex->GetHeight();
		m_scaleX = ((float)winHeight * aspect) / m_tex->GetWidth();
	}
}

/** Handles the wxGLCanvas' paint event. Does the actual window redrawing.
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnCanvasPaint(wxPaintEvent &event)
{
	wxPaintDC dc(m_canvas);	// must be here or there'll be trouble
	if(m_canvas->IsShown())
	{
		m_canvas->SetCurrent();
		Draw();
		m_canvas->SwapBuffers();
	}
}

/** Handles the wxGLCanvas' size event. Calls RecalculateAspect().
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnCanvasSize(wxSizeEvent &event)
{
	m_canvas->OnSize(event);
	RecalculateAspect();
	m_canvas->Refresh();
	m_canvas->Update();
}

/** Handles the wxGLCanvas' erase background event. See wxWidgets documentation.
 */
void VideoDisplayControl::OnCanvasEraseBackground(wxEraseEvent &event)
{
}

/** Click event for the panel part.
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnMouseLeftUp(wxMouseEvent &event)
{
	event.SetEventObject(this);
	event.ResumePropagation(1);
	event.Skip();
}

/** Click event for the wxGLCanvas.
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnCanvasMouseLeftUp(wxMouseEvent &event)
{
	event.ResumePropagation(1);
	event.Skip();
}

/** Double-click event for the wxGLCanvas.
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnCanvasDblClick(wxMouseEvent &event)
{
	event.ResumePropagation(1);
	event.Skip();
}

/** Close event for the panel. Frees the OpenGL texture object.
 * @param[in] event	Event object.
 */
void VideoDisplayControl::OnClose(wxCloseEvent &event)
{
	FreeTexture();
}

// VideoSink notifications
/** Responds to the VideoSink update notification. Updates the display with new data.
 * @param[in] data		New frame data.
 * @param[in] width		Frame width.
 * @param[in] height	Frame height.
 */
void VideoDisplayControl::NotifyUpdate(const unsigned char *data, int width, int height)
{
	if(m_tex && m_isOk)
	{
		// Optional width/height handling
		int w = width;
		int h = height;
		if(width < 0)
			w = m_videoWidth;
		if(height < 0)
			h = m_videoHeight;
		// set current GL context to canvas prior to texture ops
		m_canvas->SetCurrent();
		// Update frame
		if(!m_tex->IsOk())
			m_tex->New(w, h);

		SetVideoSize(w, h);
		m_tex->ChangeSize(w, h);

		if(data)
			m_tex->Update(data);
		m_canvas->Refresh();
		m_canvas->Update();
	}
}

void VideoDisplayControl::NotifyStart()
{
	m_isOk = m_createOk;
}

void VideoDisplayControl::NotifyStop()
{
	ShowStaticImage();
}

/** Sets target aspect ratio for the aspect ratio correction algorithm.
 * @param[in] aspect	Aspect ratio to set.
 */
void VideoDisplayControl::SetAspectRatio(float aspect)
{
	m_aspect = aspect;
}

/** Frees the OpenGL texture object.
 */
void VideoDisplayControl::FreeTexture()
{
	if(!IsShown()) Show();
	// if(m_tex && IsShown())
	if(m_tex)
	{
		m_canvas->SetCurrent();
		m_tex->Free();
		delete m_tex;
		m_tex = NULL;
	}
}
