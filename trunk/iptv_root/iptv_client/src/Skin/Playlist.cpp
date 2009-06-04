#include "Playlist.h"

/** Default constructor.
 */
Playlist::Playlist()
{
}

/** Destructor.
 */
Playlist::~Playlist()
{
}

/** Returns the internal list.
 */
std::list<PlaylistItem> Playlist::GetPlaylist()
{
	return m_playlist;
}

/** Copies a given list into the internal list.
 * @param[in] playlist	List to set.
 */
void Playlist::SetPlaylist(const std::list<PlaylistItem> &playlist)
{
	m_playlist = playlist;
}

/** Adds an item to the internal list.
 * @param[in] playlistItem	Item to add.
 */
void Playlist::Add(const PlaylistItem &playlistItem)
{
	m_playlist.push_back(playlistItem);
}

/** Adds a new item to the internal list.
 * @param[in] path		Path to where the file is stored.
 * @param[in] filename	File name.
 */
void Playlist::Add(const wxString &path, const wxString &filename)
{
	PlaylistItem item(path, filename);
	m_playlist.push_back(item);
}

/** Removes an item from the list.
 * @param[in] playlistItem	Item to remove.
 * @remarks Currently the version which takes a PlaylistItem does nothing.
 */
void Playlist::Delete(const PlaylistItem &playlistItem)
{
	/*if (!m_playlist.empty())
		m_playlist.remove(playlistItem);*/
}

/** Removes an item from the list.
 * @param[in] index		Index of the item to remove.
 * @param[in] filename	File name.
 */
void Playlist::Delete(const int &index, const wxString &filename)
{
	int pos = 0;
	std::list<PlaylistItem> auxPlaylist;
	PlaylistItem item;

	std::list<PlaylistItem>::const_iterator it;
	for(it = m_playlist.begin(); it != m_playlist.end(); it++)
	{
		item = *it;
		if ((pos != index) || (item.GetFilename() != filename))
			auxPlaylist.push_back(*it);
		pos++;
	}

	m_playlist = auxPlaylist;
}

/** Find an item by file name.
 * @param[in] filename	File name.
 * @return The item found, or a blank item if not found.
 */
PlaylistItem Playlist::Find(const wxString &filename)
{
	PlaylistItem found;
	
	std::list<PlaylistItem>::const_iterator it;
	for(it = m_playlist.begin(); it != m_playlist.end(); it++)
	{
		PlaylistItem item = *it;
		wxString str = item.GetFilename();
		if (filename == str)
			return item;		
	}
	return found;
}

/** Moves an item up in the list.
 * @param[in] index		Index of the item to remove.
 * @param[in] filename	File name.
 */
void Playlist::MoveUp(const int &index, const wxString &filename)
{
	if (index > 0)
	{
		int pos = 0;
		std::list<PlaylistItem> auxPlaylist;
		PlaylistItem item, itemDown;

		std::list<PlaylistItem>::const_iterator it;
		for(it = m_playlist.begin(); it != m_playlist.end(); it++)
		{
			item = *it;

			if (pos < m_playlist.size() -1)
			{
				it++;
				itemDown = *(it);
				it--;
			
				if (pos == (index -1))
				{
					if (itemDown.GetFilename() == filename)
						auxPlaylist.push_back(itemDown);
					auxPlaylist.push_back(item);
					pos++;
					it++;
				}
				else
					auxPlaylist.push_back(item);

			}
			else
				auxPlaylist.push_back(item);
			
			pos++;
		}

		m_playlist = auxPlaylist;
	}
}

/** Moves an item down in the list.
 * @param[in] index		Index of the item to move.
 * @param[in] filename	File name.
 */
void Playlist::MoveDown(const int &index, const wxString &filename)
{
	if (index < (m_playlist.size()-1))
	{
		int pos = 0;
		std::list<PlaylistItem> auxPlaylist;
		PlaylistItem item, itemDown;

		std::list<PlaylistItem>::const_iterator it;
		for(it = m_playlist.begin(); it != m_playlist.end(); it++)
		{
			item = *it;

			if (pos < m_playlist.size() -1)
			{
				it++;
				itemDown = *(it);
				it--;
			
				if (pos == index)
				{
					if (item.GetFilename() == filename)
						auxPlaylist.push_back(itemDown);
					auxPlaylist.push_back(item);
					pos++;
					it++;
				}
				else
					auxPlaylist.push_back(item);

			}
			else
				auxPlaylist.push_back(item);
			
			pos++;
		}

		m_playlist = auxPlaylist;
	}
}

/** Returns size of the list.
 * @return Size of the list.
 */
int Playlist::GetSize()
{
	return m_playlist.size();
}

/** Returns an iterator to the beginning of the list.
 * @return The begin iterator.
 */
std::list<PlaylistItem>::iterator Playlist::Begin()
{
	return m_playlist.begin();
}

/** Returns an iterator to the end of the list.
 * @return The end iterator.
 */
std::list<PlaylistItem>::iterator Playlist::End()
{
	return m_playlist.end();
}

/** Clears the list.
 */
void Playlist::Clear()
{
	m_playlist.clear();
}
