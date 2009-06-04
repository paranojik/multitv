#include "PlaylistItem.h"

/** Constructor.
 * @param[in] path		File path.
 * @param[in] filename	File name.
 */
PlaylistItem::PlaylistItem(const wxString &path, const wxString &filename)
{
	m_path = path;
	m_filename = filename;
}

/** Destructor.
 */
PlaylistItem::~PlaylistItem()
{
}

/** Copy constructor.
 */
PlaylistItem::PlaylistItem(const PlaylistItem &playlistItem)
{
	m_path = playlistItem.m_path;
	m_filename = playlistItem.m_filename;
}

/** Returns the path.
 * @return The path.
 */
wxString PlaylistItem::GetPath()
{
	return m_path;
}

/** Returns the file name.
 * @return The file name.
 */
wxString PlaylistItem::GetFilename()
{
	return m_filename;
}

/** Set the path.
 * @param[in] path	The path to set.
 */
void PlaylistItem::SetPath(const wxString &path)
{
	m_path = path;
}

/** Set the path.
 * @param[in] filename	The file name to set.
 */
void PlaylistItem::SetFilename(const wxString &filename)
{
	m_filename = filename;
}
