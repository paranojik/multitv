#include <wx/wx.h>

#include <GL/gl.h>
#include <cmath>

#include "Texture.h"

#define LOG2(x) (logf(x) / logf(2))

// Constructors
/** Default constructor.
 */
Texture::Texture()
{
	m_ok = false;
}

/** Constructor. Creates an OpenGL texture image with given dimensions.
 * @param[in] width		Image width.
 * @param[in] height	Image height.
 */
Texture::Texture(int width, int height)
{
	m_ok = false;
	New(width, height);
}

/** Constructor. Creates an OpenGL texture image from the given bitmap.
 * @param[in] bmp	Bitmap image.
 * @param[in] wrap	True if texture should wrap around the edges.
 */
Texture::Texture(const wxBitmap &bmp, bool wrap)
{
	m_ok = false;
	FromBitmap(bmp, wrap);
}

/** Constructor. Creates an OpenGL texture image from the given image.
 * @param[in] img	Image.
 * @param[in] wrap	True if texture should wrap around the edges.
 */
Texture::Texture(const wxImage &img, bool wrap)
{
	m_ok = false;
	FromImage(img, wrap);
}

/** Constructor. Creates an OpenGL texture image from an image loaded from a file.
 * @param[in] filename	Image file name.
 * @param[in] wrap		True if texture should wrap around the edges.
 */
Texture::Texture(const wxString &filename, bool wrap)
{
	m_ok = false;
	Load(filename, wrap);
}

/** Destructor. Frees texture.
 */
Texture::~Texture()
{
	Free();
}

/** Creates a new, black texture.
 * @param[in] width		Image width.
 * @param[in] height	Image height.
 */
void Texture::New(int width, int height)
{
	FromImage(wxImage(width, height));
}


/** Creates an OpenGL texture image from the given bitmap.
 * @param[in] bmp	Bitmap image.
 * @param[in] wrap	True if texture should wrap around the edges.
 */
void Texture::FromBitmap(const wxBitmap &bmp, bool wrap)
{
	FromImage(bmp.ConvertToImage(), wrap);
}

/** Creates an OpenGL texture image from the given image.
 * @param[in] img	Image.
 * @param[in] wrap	True if texture should wrap around the edges.
 */
void Texture::FromImage(const wxImage &img, bool wrap)
{
	if(img.IsOk())
	{
		unsigned char *data;
		wxImage newImg;
		bool resize = false;
		m_width = img.GetWidth();
		m_height = img.GetHeight();
		// Texture dimensions must be powers of 2. NOTE: Only the latest
		// OpenGL drivers/video cards support non-power-of-2 textures.
		float intPart, fracPart;
		// test width
		fracPart = modff(LOG2(m_width), &intPart);
		if(fracPart)	// texture is not power-of-2 in width
		{
			m_effWidth = 1 << ((int)intPart + 1);	// make it the next power of 2
			resize = true;
		}
		else	// texture is power-of-2 in width
			m_effWidth = m_width;	// leave width as is

		// test height
		fracPart = modff(LOG2(m_height), &intPart);
		if(fracPart)	// texture is not power-of-2 in height
		{
			m_effHeight = 1 << ((int)intPart + 1);	// make it the next power of 2
			resize = true;
		}
		else	// texture is power-of-2 in height
			m_effHeight = m_height;	// leave height as is

		if(resize)
		{
#if 0
			wxString msg = wxEmptyString;
			msg.Printf(wxT("Texture size %d x %d"), m_effWidth, m_effHeight);
			wxMessageBox(msg, wxT("Debug info"), wxICON_EXCLAMATION);
#endif
			// Center the image on its new dimensions
//			int x = (m_effWidth - m_width) / 2;
//			int y = (m_effHeight - m_height) / 2;
			newImg = img.Copy();
			newImg.Resize(wxSize(m_effWidth, m_effHeight), wxPoint(0,0));
//			m_width = newImg.GetWidth();
//			m_height = newImg.GetHeight();
			data = newImg.GetData();
		}
		else
			data = img.GetData();
#ifdef _DEBUG
		if(img.IsOk()) img.SaveFile(wxT("normal.bmp"));
		if(newImg.IsOk()) newImg.SaveFile(wxT("resized.bmp"));
#endif
		m_aspect = (float)m_width / (float)m_height;
		// create the texture from the wxImage data
		// allocate a texture name
		glGenTextures(1, &m_tex);
		// enable 2D texturing
		glEnable(GL_TEXTURE_2D);
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, m_tex);

		// select modulate to mix texture with color for shading
		// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		///// Using MIP maps /////
		// when texture area is small, bilinear filter the closest MIP map
		// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		//			   GL_LINEAR_MIPMAP_NEAREST);
		
		///// Not using MIP maps /////
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
					   wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
					   wrap ? GL_REPEAT : GL_CLAMP);
		
		///// Using MIP maps /////

		// build our texture MIP maps
		//gluBuild2DMipmaps( GL_TEXTURE_2D, 3, m_width, 
		//	m_height, GL_RGB, GL_UNSIGNED_BYTE, data );

		///// Not using MIP maps /////

		// (old code)
		glTexImage2D(GL_TEXTURE_2D, 0, 3, m_effWidth, m_effHeight, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data); 



		m_ok = true;
	}
}

/** Changes the texture image size.
 * @param[in] width		New width.
 * @param[in] height	New height.
 * @param[in] wrap		True if texture should wrap around the edges.
 */
void Texture::ChangeSize(int width, int height, bool wrap)
{
	if((width != m_width) || (height != m_height))
	{
		// Test whether the new sizes are in the same
		// power of 2 as the old sizes
		float ip1, ip2, fp1, fp2;
		// Test on width
		fp1 = modff(LOG2(width), &ip1);
		fp2 = modff(LOG2(m_width), &ip2);
		if(ip1 != ip2)	// Widths are in different powers of 2
			New(width, height);	// Recreate texture with new dimensions
		else
		{
			// Test on height
			fp1 = modff(LOG2(height), &ip1);
			fp2 = modff(LOG2(m_height), &ip2);
			if(ip1 != ip2) // Heights are in different powers of 2
				New(width, height);	// Recreate texture with new dimensions
		}

		m_width = width;
		m_height = height;
		m_aspect = (float)m_width / (float)m_height;
	}
}

/** Replaces texture image.
 * @param[in] data		New image data. Pixel format is RGB 8-8-8.
 * @remarks The new image data must correspond to the same image dimensions as the current
 * texture data. In case it doesn't, the results will be unpredictable.
 */
void Texture::Update(const unsigned char *data)
{
	// select our current texture
	glBindTexture(GL_TEXTURE_2D, m_tex);
	// update only the actual image area of the texture
	// void glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, data)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, data);
};

/** Creates an OpenGL texture from an image loaded from a file.
 * @param[in] filename	Image file name.
 * @param[in] wrap		True if texture should wrap around the edges.
 */
void Texture::Load(const wxString &filename, bool wrap)
{
	wxInitAllImageHandlers();
	FromImage(wxImage(filename));
}

/** Frees the OpenGL texture.
 */
void Texture::Free()
{
	if(m_ok)
	{
		glDeleteTextures(1, &m_tex);
		m_ok = false;
	}
}

/** Control flag.
 * @return The "OK" control flag value.
 */
bool Texture::IsOk()
{
	return m_ok;
}

/** Returns the OpenGL texture ID.
 * @return the OpenGL texture ID.
 */
GLuint Texture::GetID()
{
	return m_tex;
}

/** Returns the total texture image width.
 * @return The total texture image width.
 */
int Texture::GetWidth()
{
	return m_width;
}

/** Returns the total texture image height.
 * @return The total texture image height.
 */
int Texture::GetHeight()
{
	return m_height;
}

/** Returns the effective texture image width.
 * @return The effective texture image width.
 * @remarks The "effective" texture dimensions differs from the "total" texture dimensions
 * in that the "effective" dimensions mean the area where there is actual data, while the
 * "total" dimensions mean the dimensions as allocated by OpenGL, and for this class, which
 * does not use advanced OpenGL features, are always a power of 2.
 */
int Texture::GetEffectiveWidth()
{
	return m_effWidth;
}

/** Returns the effective texture image height.
 * @return The effective texture image height.
 * @remarks The "effective" texture dimensions differs from the "total" texture dimensions
 * in that the "effective" dimensions mean the area where there is actual data, while the
 * "total" dimensions mean the dimensions as allocated by OpenGL, and for this class, which
 * does not use advanced OpenGL features, are always a power of 2.
 */
int Texture::GetEffectiveHeight()
{
	return m_effHeight;
}

/** Returns the ratio of the total width to the effective width.
*/
float Texture::GetActualToEffectiveRatioX()
{
	return (float)m_width / (float)m_effWidth;
}

/** Returns the ratio of the total height to the effective height.
*/
float Texture::GetActualToEffectiveRatioY()
{
	return (float)m_height / (float)m_effHeight;
}

/** Returns the aspect ratio.
 */
float Texture::GetAspectRatio()
{
	return m_aspect;
}
