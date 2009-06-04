#ifndef TEXTURE_H
#define TEXTURE_H

#include <wx/wx.h>
#include <GL/gl.h>

/** @brief Represents an OpenGL texture object.
 *
 * This class represents an OpenGL 2D texture object.
 *
 * @remarks Make sure to have an active OpenGL context when operating with this class.
 * As this class does not use any advanced OpenGL features, it honors the power-of-two
 * size limitation of OpenGL 1.0, so this must be taken into consideration when allocating
 * textures. Some video cards have severe limitations on texture size (e.g. Voodoo cards
 * can't do anything larger than 256x256). Most modern cards (128 MB or more memory)
 * will handle textures up to 2048x2048, but some may not be able to handle them.
 */
class Texture
{
private:
	GLuint m_tex;
	int m_width, m_height;			// Actual size of texture image
	int m_effWidth, m_effHeight;	// Effective size of texture (power of 2)
	float m_aspect;
	bool m_ok;
public:
	// c'tors
	Texture();
	Texture(int width, int height);
	Texture(const wxBitmap &bmp, bool wrap = false);
	Texture(const wxImage &img, bool wrap = false);
	Texture(const wxString &filename, bool wrap = false);

	virtual ~Texture();

	void New(int width, int height);
	void FromBitmap(const wxBitmap &bmp, bool wrap = false);
	void FromImage(const wxImage &img, bool wrap = false);
	void Load(const wxString &filename, bool wrap = false);
	void ChangeSize(int width, int height, bool wrap = false);
	void Update(const unsigned char *data);
	void Free();

	bool IsOk();

	GLuint GetID();
	int GetWidth();
	int GetHeight();
	int GetEffectiveWidth();
	int GetEffectiveHeight();
	float GetActualToEffectiveRatioX();
	float GetActualToEffectiveRatioY();
	float GetAspectRatio();
};

#endif
