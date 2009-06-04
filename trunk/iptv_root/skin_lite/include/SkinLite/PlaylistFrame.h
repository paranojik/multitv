/////////////////////////////////////////////////////////////////////////////
// Name:        PlaylistFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     11/11/2008 11:09:11
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PLAYLISTFRAME_H_
#define _PLAYLISTFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "Playlist.h"
#include "AppInterface.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class Playlist;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
enum{ID_PLAYLISTFRAME = 11000,
	 ID_PLAYLISTFRAME_MAINPANEL,
	 ID_PLAYLISTFRAME_LSTBOX_PLAYLIST,
	 ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT,
	 ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_DELETE,
	 ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP,
	 ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN,
	 ID_PLAYLISTFRAME_BTN_PLAYLIST_OPEN,
	 ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE,
	 ID_PLAYLISTFRAME_CHKBOX_REPEAT,
	 ID_PLAYLISTFRAME_BTN_START,
	 ID_PLAYLISTFRAME_BTN_CANCEL
};

#define SYMBOL_PLAYLISTFRAME_STYLE wxCAPTION|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxFRAME_FLOAT_ON_PARENT
#define SYMBOL_PLAYLISTFRAME_TITLE _("Playlist")
#define SYMBOL_PLAYLISTFRAME_IDNAME ID_PLAYLISTFRAME
#define SYMBOL_PLAYLISTFRAME_SIZE wxSize(400, 300)
#define SYMBOL_PLAYLISTFRAME_POSITION wxDefaultPosition
#define PLAYLISTFRAME_TIMER_INTERVAL 500
#define PLAYLISTFRAME_MAXIMUMFINISHTIME_SEC 18
#define WX_TO_FILE_CSTR(s) (const char *)(s.mb_str(wxConvFile))
#define WX_FROM_FILE_CSTR(s)	wxString(s, wxConvFile)
////@end control identifiers


/** @brief Class thats implements playlist settings window.
*
*/
class PlaylistFrame: public wxFrame
{    
    DECLARE_CLASS( PlaylistFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PlaylistFrame();
    PlaylistFrame( wxWindow* parent, AppInterface *iface, wxWindowID id = SYMBOL_PLAYLISTFRAME_IDNAME, const wxString& caption = SYMBOL_PLAYLISTFRAME_TITLE, const wxPoint& pos = SYMBOL_PLAYLISTFRAME_POSITION, const wxSize& size = SYMBOL_PLAYLISTFRAME_SIZE, long style = SYMBOL_PLAYLISTFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PLAYLISTFRAME_IDNAME, const wxString& caption = SYMBOL_PLAYLISTFRAME_TITLE, const wxPoint& pos = SYMBOL_PLAYLISTFRAME_POSITION, const wxSize& size = SYMBOL_PLAYLISTFRAME_SIZE, long style = SYMBOL_PLAYLISTFRAME_STYLE );

    /// Destructor
    ~PlaylistFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	void UpdatePlaylist();

	void OnMovieSendBegin(long mediaId, const wxString &channelName);
	void OnMovieSendEof(long mediaId, const wxString &channelName);

////@begin PlaylistFrame event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT
    void OnBmpBtnPlaylistInsertClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_DELETE
    void OnBmpBtnPlaylistDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP
    void OnBmpBtnPlaylistMoveUpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN
    void OnBmpBtnPlaylistMoveDownClick( wxCommandEvent& event );

	/// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_OPEN
    void OnBtnPlaylistOpenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE
    void OnBtnPlaylistSaveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_START
    void OnBtnStartClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_CANCEL
    void OnBtnCancelClick( wxCommandEvent& event );

	void OnTimer(wxTimerEvent &event);
////@end PlaylistFrame event handler declarations

////@begin PlaylistFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PlaylistFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin PlaylistFrame member variables
    wxBoxSizer* m_mainVertSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_mainPanelVertSizer;
    wxStaticText* m_lblPlaylistCaption;
    wxListBox* m_lstBoxPlaylist;
    wxBoxSizer* m_PlaylistControlHorSizer;
    wxBitmapButton* m_bmpBtnPlaylistInsert;
    wxBitmapButton* m_bmpBtnPlaylistDelete;
    wxBitmapButton* m_bmpBtnPlaylistMoveUp;
    wxBitmapButton* m_bmpBtnPlaylistMoveDown;
    wxBoxSizer* m_playlistSaveHorSizer;
    wxButton* m_btnPlaylistOpen;
    wxButton* m_btnPlaylistSave;
    wxBoxSizer* m_ctrlHorSizer;
    wxCheckBox* m_chkBoxRepeat;
    wxBoxSizer* m_ctrlBtnHorSizer;
    wxButton* m_btnStart;
    wxButton* m_btnCancel;

	Playlist m_playlist;
	bool m_wasStarted;

	AppInterface *m_appInterface;
	long m_mediaId;
	long m_itemPlaying;

	wxTimer m_timer;
	wxDateTime m_initTime;
	bool m_finishBtnPressed;
////@end PlaylistFrame member variables

	void PlaylistStartTransmission();
	void PlaylistFinishTransmission();
	void SetFieldsStates(const bool &fieldsEnable = true);
	void BeginPlayFile();
	bool OpenPlaylist();
	bool SavePlaylist();
};

#endif
    // _PLAYLISTFRAME_H_
