#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <list>
#include "PlaylistItem.h"

/** @brief Stores and manages the WMV movie playlist.
 *
 */
class Playlist
{
private:
	std::list<PlaylistItem> m_playlist;
public:
	Playlist();
	~Playlist();
	std::list<PlaylistItem> GetPlaylist();
	void SetPlaylist(const std::list<PlaylistItem> &playlist);
	void Add(const PlaylistItem &playlistItem);
	void Add(const wxString& path, const wxString& filename);
	void Delete(const PlaylistItem &playlistItem);
	void Delete(const int &index, const wxString &filename);
	PlaylistItem Find(const wxString &filename);
	void MoveUp(const int &index, const wxString &filename);
	void MoveDown(const int &index, const wxString &filename);
	void Clear();
	int GetSize();
	std::list<PlaylistItem>::iterator Begin();
	std::list<PlaylistItem>::iterator End();
};
#endif
