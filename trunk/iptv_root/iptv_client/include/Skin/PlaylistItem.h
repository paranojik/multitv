#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <wx/wx.h>

/** @brief Represents an item in the WMV movie playlist.
 *
 */
class PlaylistItem
{
private:
	wxString m_path;
	wxString m_filename;
public:
	PlaylistItem(const wxString &path = wxEmptyString, const wxString &filename = wxEmptyString);
	~PlaylistItem();

	//Copy Constructor
	PlaylistItem(const PlaylistItem &playlistItem);

	wxString GetPath();
	wxString GetFilename();
	void SetPath(const wxString &path);
	void SetFilename(const wxString &filename);
};

#endif
