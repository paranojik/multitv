/////////////////////////////////////////////////////////////////////////////
// Name:        PlaylistFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     11/11/2008 11:09:11
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include <fstream>
using namespace std;
////@end includes
#include "PlaylistFrame.h"
#include "SkinLite.h"
#include "MessageDialog.h"
//#include "PlaylistItem.h"

////@begin XPM images
////@end XPM images


/*!
 * PlaylistFrame type definition
 */

IMPLEMENT_CLASS( PlaylistFrame, wxFrame )


/*!
 * PlaylistFrame event table definition
 */

BEGIN_EVENT_TABLE( PlaylistFrame, wxFrame )

////@begin PlaylistFrame event table entries
    EVT_BUTTON( ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT, PlaylistFrame::OnBmpBtnPlaylistInsertClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_DELETE, PlaylistFrame::OnBmpBtnPlaylistDeleteClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP, PlaylistFrame::OnBmpBtnPlaylistMoveUpClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN, PlaylistFrame::OnBmpBtnPlaylistMoveDownClick )
	EVT_BUTTON( ID_PLAYLISTFRAME_BTN_PLAYLIST_OPEN, PlaylistFrame::OnBtnPlaylistOpenClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE, PlaylistFrame::OnBtnPlaylistSaveClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BTN_START, PlaylistFrame::OnBtnStartClick )
    EVT_BUTTON( ID_PLAYLISTFRAME_BTN_CANCEL, PlaylistFrame::OnBtnCancelClick )
	EVT_TIMER(wxID_ANY, PlaylistFrame::OnTimer)

////@end PlaylistFrame event table entries

END_EVENT_TABLE()


#define PLAYLISTFRAME_BMP_ADD wxT("Graphics/SkinLite/bmpAdd.png")
#define PLAYLISTFRAME_BMP_DELETE wxT("Graphics/SkinLite/bmpDelete.png")
#define PLAYLISTFRAME_BMP_MOVEUP wxT("Graphics/SkinLite/bmpMoveUp.png")
#define PLAYLISTFRAME_BMP_MOVEDOWN wxT("Graphics/SkinLite/bmpMoveDown.png")
#define PLAYLISTFRAME_FILEDLG_WILDCARD _("Media files|*.wmv;*.wma")


/** PlaylistFrame default constructor.
 * 
 */
PlaylistFrame::PlaylistFrame()
{
    Init();
}

/** PlaylistFrame constructor.
 * @param[in] parent. Parent Window.
 * @param[in] iface. Interface with application.
 * @param[in] id. Window id. Default SYMBOL_PLAYLISTFRAME_IDNAME.
 * @param[in] caption. Window caption. Default SYMBOL_PLAYLISTFRAME_TITLE.
 * @param[in] pos. Window position. Default SYMBOL_PLAYLISTFRAME_POSITION.
 * @param[in] size. Window size. Default SYMBOL_PLAYLISTFRAME_SIZE.
 * @param[in] style. Window style. Default SYMBOL_PLAYLISTFRAME_STYLE.
 */
PlaylistFrame::PlaylistFrame( wxWindow* parent, AppInterface *iface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
	m_appInterface = iface;
    Create( parent, id, caption, pos, size, style );

	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangePlaylistExistsStatus(true);

	sessionWnd->DisableSendFileTransfer();
}


/*!
 * PlaylistFrame creator
 */

bool PlaylistFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PlaylistFrame creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
	CreateStatusBar();
    Centre();
////@end PlaylistFrame creation
    return true;
}


/** PlaylistFrame destructor.
 * 
 */

PlaylistFrame::~PlaylistFrame()
{
////@begin PlaylistFrame destruction
	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangePlaylistExistsStatus(false);

	if (skinLite->CheckSessionWindowExists())
		sessionWnd->EnableSendFileTransfer();
////@end PlaylistFrame destruction
}


/*!
 * Member initialisation
 */

void PlaylistFrame::Init()
{
////@begin PlaylistFrame member initialisation
    m_mainVertSizer = NULL;
    m_mainPanel = NULL;
    m_mainPanelVertSizer = NULL;
    m_lblPlaylistCaption = NULL;
    m_lstBoxPlaylist = NULL;
    m_PlaylistControlHorSizer = NULL;
    m_bmpBtnPlaylistInsert = NULL;
    m_bmpBtnPlaylistDelete = NULL;
    m_bmpBtnPlaylistMoveUp = NULL;
    m_bmpBtnPlaylistMoveDown = NULL;
    m_playlistSaveHorSizer = NULL;
    m_btnPlaylistOpen = NULL;
    m_btnPlaylistSave = NULL;
    m_ctrlHorSizer = NULL;
    m_chkBoxRepeat = NULL;
    m_ctrlBtnHorSizer = NULL;
    m_btnStart = NULL;
    m_btnCancel = NULL;
	m_wasStarted = false;
	m_mediaId = 0;
	m_itemPlaying = 0;

	m_timer.SetOwner(this);
	m_timer.Stop();
	m_finishBtnPressed = false;
////@end PlaylistFrame member initialisation
}


/*!
 * Control creation for PlaylistFrame
 */

void PlaylistFrame::CreateControls()
{    
////@begin PlaylistFrame content construction
    PlaylistFrame* itemFrame1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_PLAYLISTFRAME_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertSizer->Add(m_mainPanel, 1, wxGROW, 5);

    m_mainPanelVertSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(m_mainPanelVertSizer);

    m_lblPlaylistCaption = new wxStaticText( m_mainPanel, wxID_STATIC, _("Select videos to be shown:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_mainPanelVertSizer->Add(m_lblPlaylistCaption, 0, wxALIGN_LEFT|wxALL, 5);

    wxArrayString m_lstBoxPlaylistStrings;
    m_lstBoxPlaylist = new wxListBox( m_mainPanel, ID_PLAYLISTFRAME_LSTBOX_PLAYLIST, wxDefaultPosition, wxDefaultSize, m_lstBoxPlaylistStrings, wxLB_ALWAYS_SB );
    m_mainPanelVertSizer->Add(m_lstBoxPlaylist, 1, wxGROW|wxALL, 5);

    m_PlaylistControlHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanelVertSizer->Add(m_PlaylistControlHorSizer, 0, wxGROW|wxALL, 5);

	wxBitmap bmpAdd(PLAYLISTFRAME_BMP_ADD, wxBITMAP_TYPE_PNG);
	wxBitmap bmpDelete(PLAYLISTFRAME_BMP_DELETE, wxBITMAP_TYPE_PNG);
	wxBitmap bmpMoveUp(PLAYLISTFRAME_BMP_MOVEUP, wxBITMAP_TYPE_PNG);
	wxBitmap bmpMoveDown(PLAYLISTFRAME_BMP_MOVEDOWN, wxBITMAP_TYPE_PNG);


    m_bmpBtnPlaylistInsert = new wxBitmapButton( m_mainPanel, ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT, bmpAdd, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_PlaylistControlHorSizer->Add(m_bmpBtnPlaylistInsert, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_bmpBtnPlaylistInsert->SetToolTip(_("Add video"));

    m_bmpBtnPlaylistDelete = new wxBitmapButton( m_mainPanel, ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_DELETE, bmpDelete, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_PlaylistControlHorSizer->Add(m_bmpBtnPlaylistDelete, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_bmpBtnPlaylistDelete->SetToolTip(_("Delete video"));

    m_bmpBtnPlaylistMoveUp = new wxBitmapButton( m_mainPanel, ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP, bmpMoveUp, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_PlaylistControlHorSizer->Add(m_bmpBtnPlaylistMoveUp, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_bmpBtnPlaylistMoveUp->SetToolTip(_("Move Up"));

    m_bmpBtnPlaylistMoveDown = new wxBitmapButton( m_mainPanel, ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN, bmpMoveDown, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
    m_PlaylistControlHorSizer->Add(m_bmpBtnPlaylistMoveDown, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_bmpBtnPlaylistMoveDown->SetToolTip(_("Move Down"));

    m_PlaylistControlHorSizer->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_playlistSaveHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_PlaylistControlHorSizer->Add(m_playlistSaveHorSizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnPlaylistOpen = new wxButton( m_mainPanel, ID_PLAYLISTFRAME_BTN_PLAYLIST_OPEN, _("Open"), wxDefaultPosition, wxDefaultSize, 0 );
    m_playlistSaveHorSizer->Add(m_btnPlaylistOpen, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnPlaylistSave = new wxButton( m_mainPanel, ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
    m_playlistSaveHorSizer->Add(m_btnPlaylistSave, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanelVertSizer->Add(m_ctrlHorSizer, 0, wxGROW|wxALL, 5);

    m_chkBoxRepeat = new wxCheckBox( m_mainPanel, ID_PLAYLISTFRAME_CHKBOX_REPEAT, _("Repeat video sequence"), wxDefaultPosition, wxDefaultSize, 0 );
    m_chkBoxRepeat->SetValue(false);
    m_ctrlHorSizer->Add(m_chkBoxRepeat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlHorSizer->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlBtnHorSizer = new wxBoxSizer(wxHORIZONTAL);
    m_ctrlHorSizer->Add(m_ctrlBtnHorSizer, 0, wxGROW|wxALL, 5);

    m_btnStart = new wxButton( m_mainPanel, ID_PLAYLISTFRAME_BTN_START, _("Start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlBtnHorSizer->Add(m_btnStart, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( m_mainPanel, ID_PLAYLISTFRAME_BTN_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlBtnHorSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end PlaylistFrame content construction
}

/*!
 * Should we show tooltips?
 */

bool PlaylistFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap PlaylistFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PlaylistFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PlaylistFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon PlaylistFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PlaylistFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PlaylistFrame icon retrieval
}


//Auxiliary functions

/** Updates the playlist listbox.
*
*/
void PlaylistFrame::UpdatePlaylist()
{
	//Clear playlist
	m_lstBoxPlaylist->Clear();

	m_lstBoxPlaylist->SetForegroundColour(wxColour(0,255,0));
	PlaylistItem item;
	std::list<PlaylistItem>::const_iterator it;
	for(it = m_playlist.Begin(); it != m_playlist.End(); it++)
	{
		item = *it;
		m_lstBoxPlaylist->Append(item.GetFilename());
	}
}
//END Auxiliary functions

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT
 */

void PlaylistFrame::OnBmpBtnPlaylistInsertClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT in PlaylistFrame.
	wxString wildcard = PLAYLISTFRAME_FILEDLG_WILDCARD;
    wxFileDialog dlg(this, _("Choose a file"), wxEmptyString, wxEmptyString, wildcard, wxFD_OPEN);
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString path = dlg.GetPath();
		wxString filename = dlg.GetFilename();
		
		m_playlist.Add(path, filename);

		UpdatePlaylist();
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_INSERT in PlaylistFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_LSTBOX_BMPBTN_PLAYLIST_DELETE
 */

void PlaylistFrame::OnBmpBtnPlaylistDeleteClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_LSTBOX_BMPBTN_PLAYLIST_DELETE in PlaylistFrame.
    int index = m_lstBoxPlaylist->GetSelection();
	wxString filename = m_lstBoxPlaylist->GetStringSelection();

	if ((!m_wasStarted) || (m_wasStarted && (index != m_itemPlaying)))
	{
		//Change the number of the current item playing if it�s before item playing
		if (index < m_itemPlaying)
			m_itemPlaying--;
		
		m_playlist.Delete(index, filename);

		UpdatePlaylist();
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_LSTBOX_BMPBTN_PLAYLIST_DELETE in PlaylistFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP
 */

void PlaylistFrame::OnBmpBtnPlaylistMoveUpClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP in PlaylistFrame.
    int index = m_lstBoxPlaylist->GetSelection();
	wxString filename = m_lstBoxPlaylist->GetStringSelection();

	if ((!m_wasStarted) || (m_wasStarted && (index > (m_itemPlaying + 1))))
	{
		m_playlist.MoveUp(index, filename);

		UpdatePlaylist();

		if (index > 0)
			m_lstBoxPlaylist->SetSelection(index - 1);
		else
			m_lstBoxPlaylist->SetSelection(index);
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEUP in PlaylistFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN
 */

void PlaylistFrame::OnBmpBtnPlaylistMoveDownClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN in PlaylistFrame.
	int index = m_lstBoxPlaylist->GetSelection();
	wxString filename = m_lstBoxPlaylist->GetStringSelection();

	if ((!m_wasStarted) || (m_wasStarted && (index > m_itemPlaying)))
	{
		m_playlist.MoveDown(index, filename);

		UpdatePlaylist();

		int size = m_playlist.GetSize();
		if (index < (size-1))
			m_lstBoxPlaylist->SetSelection(index + 1);
		else
			m_lstBoxPlaylist->SetSelection(index);
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BMPBTN_PLAYLIST_MOVEDOWN in PlaylistFrame. 
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_OPEN
 */

void PlaylistFrame::OnBtnPlaylistOpenClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE in PlaylistFrame.
    // Before editing this code, remove the block markers.
    OpenPlaylist();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE in PlaylistFrame. 
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE
 */

void PlaylistFrame::OnBtnPlaylistSaveClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE in PlaylistFrame.
    // Before editing this code, remove the block markers.
    SavePlaylist();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_PLAYLIST_SAVE in PlaylistFrame. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_START
 */

void PlaylistFrame::OnBtnStartClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_START in PlaylistFrame.
    // Before editing this code, remove the block markers.
	wxCHECK_RET(m_appInterface, wxT("PlaylistFrame::OnBtnStartClick(): I need AppInterface to work!!"));
	if (m_wasStarted)
	{
		PlaylistFinishTransmission();
	}
	else
	{
		m_itemPlaying = 0;
		PlaylistStartTransmission();
	}
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_START in PlaylistFrame. 
}

/** Start playlist transmission.
* Private method.
*/
void PlaylistFrame::PlaylistStartTransmission()
{
	BeginPlayFile();	
}

/** Finish current movie transmission and begin the next one..
* Private method.
*/
void PlaylistFrame::PlaylistFinishTransmission()
{
	/*bool ret = m_appInterface->MovieCancelTransmission(m_mediaId);
		
	if (!ret)
		BeginPlayFile();*/

	m_finishBtnPressed = true;
	if (!m_timer.IsRunning())
	{
		m_initTime= wxDateTime::UNow();
		m_timer.Start(PLAYLISTFRAME_TIMER_INTERVAL);
	}
}

/** Change Start/Finish button state according transmission.
* @param[in] fieldsEnable. If true, button state equal finish, else start.
* Private method.
*/
void PlaylistFrame::SetFieldsStates(const bool &fieldsEnable)
{
	if (fieldsEnable)
	{
		m_wasStarted = true;
		m_btnStart->SetLabel(_("Finish"));
		/*m_btnPlaylistSave->Enable(false);
		m_btnPlaylistOpen->Enable(false);*/
	}
	else
	{
		m_wasStarted = false;
		m_btnStart->SetLabel(_("Start"));
		/*m_btnPlaylistSave->Enable(true);
		m_btnPlaylistOpen->Enable(true);*/
	}
}

/** Begin play a movie in the playlist.
* Private method.
*/
void PlaylistFrame::BeginPlayFile()
{
	if (m_itemPlaying < m_playlist.GetSize())
	{
		const Channel *channel = m_appInterface->GetCurrentChannel(); 

		//Get channel bitrate
		Mode mode;
		ModeList modeList = channel->GetModeList();
		modeList.FindMode(IRM_MODE_CHANNEL_BITRATE, mode);
		long bitrate;
		mode.GetParameter().ToLong(&bitrate);
		
		PlaylistItem item;
		std::list<PlaylistItem>::const_iterator it;
		it = m_playlist.Begin();
		for(int i = 0;i<m_itemPlaying;i++)
			it++;

		item = *it;

		bool ret = m_appInterface->MovieBeginTransmission(channel->GetName(), item.GetPath(), bitrate);

		SetFieldsStates(ret);
		m_lstBoxPlaylist->SetSelection(m_itemPlaying);
		SetStatusText(wxString::Format(_("Playing file: %s"), item.GetFilename().c_str()));

		if (!ret)
		{
			wxString msg = wxString::Format(_("Error while attempt play %s"), item.GetFilename().c_str());
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK|wxICON_ERROR, this);
		}
	}
	else
	{
		if (m_chkBoxRepeat->IsChecked())
		{
			m_itemPlaying = 0;
			BeginPlayFile();
		}
		else
		{
			wxString msg = _("Finished send transmission of Playlist");
			wxMessageBox(msg, _("Information"), wxICON_INFORMATION|wxOK, this);
			m_appInterface->MovieCancelTransmission(m_mediaId);
			SetFieldsStates(false);
			SetStatusText(wxEmptyString);
		}
	}
}

//Received movie begin
void PlaylistFrame::OnMovieSendBegin(long mediaId, const wxString &channelName)
{
	m_mediaId = mediaId;
}

//Received movie eof
void PlaylistFrame::OnMovieSendEof(long mediaId, const wxString &channelName)
{
	if (m_finishBtnPressed)
	{
		m_finishBtnPressed = false;
		if (m_itemPlaying < m_playlist.GetSize())
		{
			m_itemPlaying++;
			BeginPlayFile();
		}
	}
	else
	{
		m_finishBtnPressed = false;
		if (!m_timer.IsRunning())
		{
			m_initTime= wxDateTime::UNow();
			m_timer.Start(PLAYLISTFRAME_TIMER_INTERVAL);

		}
	}
}

void PlaylistFrame::OnTimer(wxTimerEvent &event)
{
	wxDateTime curTime = wxDateTime::UNow();
	wxTimeSpan elapsedTime;
	elapsedTime = curTime - m_initTime;
	long sec = elapsedTime.GetSeconds().ToLong();
	
	//Show remaning time to cancel file transmission
	if (m_finishBtnPressed)
	{
		wxString statusText = wxString::Format(_("Time to end of transmission: %d sec"), PLAYLISTFRAME_MAXIMUMFINISHTIME_SEC - sec);
		SetStatusText(statusText);
	}

	if (sec >= PLAYLISTFRAME_MAXIMUMFINISHTIME_SEC)
	{
		m_timer.Stop();

		//Cancel tranmission of current file
		if (m_finishBtnPressed)//Finish button was pressed
		{
			bool ret = m_appInterface->MovieCancelTransmission(m_mediaId);
				
			////Begin the next file
			//if (ret)
			//{
			//	if (m_itemPlaying < m_playlist.GetSize())
			//	{
			//		m_itemPlaying++;
			//		BeginPlayFile();
			//	}
			//}
		}
		else//Movie send EOF
		{
			if (m_itemPlaying < m_playlist.GetSize())
			{
				m_itemPlaying++;
				BeginPlayFile();
			}
		}

		SetStatusText(wxEmptyString);
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_CANCEL
 */

void PlaylistFrame::OnBtnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_CANCEL in PlaylistFrame.
    // Before editing this code, remove the block markers.
    m_appInterface->MovieCancelTransmission(m_mediaId);
	Close();
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_PLAYLISTFRAME_BTN_CANCEL in PlaylistFrame. 
}

/** Open playlist from file. 
* Private method. Return true if file was opened or false if occurs a error.
*/
bool PlaylistFrame::OpenPlaylist()
{
	wxString wildcard = _("Playlist|*.ivl");
	wxFileDialog fileDlg(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wildcard, wxFD_OPEN);
	if (fileDlg.ShowModal() == wxID_OK)
	{
		wxString filePath = fileDlg.GetPath();
		if (!wxFile::Access(filePath, wxFile::read))
		{
			wxMessageBox(_("Inexistent file."), _("ERROR"), wxOK, this);
			return false;
		}

		ifstream playlistFile(WX_TO_FILE_CSTR(filePath));

		// get length of file:
		playlistFile.seekg (0, ios::end);
		int length = playlistFile.tellg();
		playlistFile.seekg (0, ios::beg);

		// allocate memory:
		char * buffer = new char [length];
		
		PlaylistItem item;
		
		m_playlist.Clear();
		while(!playlistFile.eof())
		{
			playlistFile.getline(buffer, length);
			filePath = WX_FROM_FILE_CSTR(buffer);
			item.SetPath(filePath);

#ifdef __WXGTK__
			item.SetFilename(filePath.AfterLast('/'));
#else
			item.SetFilename(filePath.AfterLast('\\'));
#endif


			
			m_playlist.Add(item);
		}

		m_playlist.Delete(m_playlist.GetSize() - 1, wxEmptyString);

		playlistFile.close();

		UpdatePlaylist();

		delete [] buffer;

		//When move receive EOF, before BeginPlayFile(), the item instructed to play is going to begin of list
		if (m_wasStarted)
			m_itemPlaying = -1;

		wxMessageBox(_("Playlist loaded successfully."), _("Information"), wxOK, this);

	}
	return true;
	
}

/** Save playlist to file.
* Private method. Return true if file was saved or false if occurs a error.
*/
bool PlaylistFrame::SavePlaylist()
{
	//If don�t have playlist, don�t create file
	if (m_playlist.GetSize() == 0)
		return false;

	wxString wildcard = _("Playlist|*.ivl");
	wxFileDialog fileDlg(this, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE);
	if (fileDlg.ShowModal() == wxID_OK)
	{
		wxString filePath = fileDlg.GetPath();
		if (wxFile::Access(filePath, wxFile::write))
		{
			MessageDialog msgDlg(this, _("Confirm"), _("Overwite file?"));
			if ( msgDlg.ShowModal() == ID_MESSAGEDIALOG_BTN_NO)	
				return false;
		}

#ifdef __WXGTK__
		filePath += wxT(".ivl");
		ofstream playlistFile(WX_TO_FILE_CSTR(filePath));
#else
		ofstream playlistFile(WX_TO_FILE_CSTR(filePath));
#endif
		
		PlaylistItem item;
		std::list<PlaylistItem>::iterator it;
		for(it = m_playlist.Begin(); it != m_playlist.End(); it++)
		{
			item = *it;
			playlistFile << WX_TO_FILE_CSTR(item.GetPath()) << "\n";
		}
		playlistFile.close();

		wxMessageBox(_("Playlist saved successfully."), _("Information"), wxOK, this);

	}
	return true;
	
}
