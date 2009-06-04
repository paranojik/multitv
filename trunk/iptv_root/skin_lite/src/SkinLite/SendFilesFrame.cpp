/////////////////////////////////////////////////////////////////////////////
// Name:        SendFilesFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     19/05/2008 09:50:49
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/filename.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "SendFilesFrame.h"
#include "MessageDialog.h"
#include "SkinLite.h"


/*!
 * SendFilesFrame type definition
 */
IMPLEMENT_CLASS( SendFilesFrame, wxFrame )


/*!
 * SendFilesFrame event table definition
 */
BEGIN_EVENT_TABLE( SendFilesFrame, wxFrame )
    EVT_BUTTON( ID_SENDFILESFRAME_BTN_SEARCHFILE, SendFilesFrame::OnBtnSearchFileClick )
    EVT_BUTTON( ID_SENDFILESFRAME_BTN_SEND, SendFilesFrame::OnBtnSendClick )
    EVT_BUTTON( ID_SENDFILESFRAME_BTN_CANCEL, SendFilesFrame::OnBtnCancelClick )
	EVT_TEXT(ID_SENDFILESFRAME_TXT_FILENAME, SendFilesFrame::OnTxtFileNameChange)
	EVT_CLOSE(SendFilesFrame::OnClose)
	EVT_TIMER( wxID_ANY, SendFilesFrame::OnTimer )
END_EVENT_TABLE()


/** SendFilesFrame default constructor.
 *
 */
SendFilesFrame::SendFilesFrame()
{
    Init();
}

/** SendFilesFrame constructor.
* @param[in] parent. Parent window.
* @param[in] iface. Interface with application.
* @param[in] id. Window id. Default SYMBOL_SENDFILESFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SENDFILESFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SENDFILESFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SENDFILESFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SENDFILESFRAME_STYLE.
*/
SendFilesFrame::SendFilesFrame( wxWindow* parent, AppInterface* iface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
	m_appInterface = iface;
    Create( parent, id, caption, pos, size, style );
}


/** SendFilesFrame creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_SENDFILESFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_SENDFILESFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_SENDFILESFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_SENDFILESFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_SENDFILESFRAME_STYLE.
*/
bool SendFilesFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxFrame::Create( parent, id, caption, pos, size, style );
	SetMinSize(SYMBOL_SENDFILESFRAME_SIZE);
    CreateControls();

	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangeSendFilesExistsStatus(true);
	sessionWnd->DisableSendPlaylist();
    return true;
}


/** SendFilesFrame destructor.
 * 
 */
SendFilesFrame::~SendFilesFrame()
{
	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangeSendFilesExistsStatus(false);

	if (skinLite->CheckSessionWindowExists())
		sessionWnd->EnableSendPlaylist();
}


/*!
 * Member initialisation
 */
void SendFilesFrame::Init()
{
	m_whiteboardTransmissionEnabled = false;

    m_mainVertSizer = NULL;
    m_mainPanel = NULL;
    m_mainPanelVertSizer = NULL;
    m_fileGroupBoxSizer = NULL;
    m_lblFileName = NULL;
    m_fileNameSizer = NULL;
    m_txtFileName = NULL;
    m_btnSearchFile = NULL;
    m_statusTransferGroupBoxSizer = NULL;
    m_transferLabelsGridSizer = NULL;
    m_lblFileSizeCaption = NULL;
    m_lblFileSizeLength = NULL;
    m_lblSentCaption = NULL;
    m_lblSentPercentual = NULL;
    m_lblSentBytesCaption = NULL;
    m_lblSentBytesLength = NULL;
    m_lblRemainingTimeTransferCaption = NULL;
    m_lblRemainingTimeTransfer = NULL;
    m_lblBitrateCaption = NULL;
    m_lblBitrate = NULL;
    m_gaugeTransferFile = NULL;
    m_recoverTransferGridSizer = NULL;
    m_lblRecoveredCaption = NULL;
    m_lblRecoveredPercentual = NULL;
    m_lblReamainingTimeRecoveredCaption = NULL;
    m_lblRemainingTimeRecovered = NULL;
    m_gaugeRecovered = NULL;
    m_transferCtrlSizer = NULL;
    m_btnSend = NULL;
    m_btnCancel = NULL;
    m_StatisticGroupBoxSizer = NULL;
    m_statisticGridSizer = NULL;
    m_lblStatisticSentCaption = NULL;
    m_lblStatisticSent = NULL;
    m_lblStatisticCancelledCaption = NULL;
    m_lblStatisticCancelled = NULL;
    m_lblStatisticReceivedCaption = NULL;
    m_lblStatisticReceived = NULL;
    m_lblStatisticToReceiveCaption = NULL;
    m_lblStatisticToReceive = NULL;

	m_mediaId = 0;

	m_sendFinished = true;
	m_totalLostPackets = 0;
	m_isTotalLostPackets = false;
	m_isRecoverPhase = false;
	m_beginningTransmission = false;

	m_toReceive = 0;
	m_sent = 0;
	m_cancelled = 0;
	m_received = 0;

	m_filePath = wxEmptyString;
	m_lastPacket = 0;
	m_currentPacket = 0;
	m_timer.SetOwner(this);
	m_timer.Start(TIMER_INTERVAL);
	m_timer.Stop();
}


/*!
 * Control creation for SendFilesFrame
 */
void SendFilesFrame::CreateControls()
{
    SendFilesFrame* itemFrame1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_SENDFILESFRAME_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertSizer->Add(m_mainPanel, 1, wxGROW, 5);

    m_mainPanelVertSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(m_mainPanelVertSizer);

	m_animatedGif.LoadFile(SENDFILESFRAME_ANIMATEDSENDGIF);

	m_sendAnimatedGif = new wxAnimationCtrl(m_mainPanel, wxID_ANY, m_animatedGif);
	m_mainPanelVertSizer->Add(m_sendAnimatedGif, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    m_fileGroupBoxSizer = new wxStaticBox(m_mainPanel, wxID_ANY, wxEmptyString);
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(m_fileGroupBoxSizer, wxVERTICAL);
    m_mainPanelVertSizer->Add(itemStaticBoxSizer5, 0, wxGROW|wxALL, 5);

    m_lblFileName = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_FILENAME_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer5->Add(m_lblFileName, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_fileNameSizer = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer5->Add(m_fileNameSizer, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_txtFileName = new wxTextCtrl( m_mainPanel, ID_SENDFILESFRAME_TXT_FILENAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    m_fileNameSizer->Add(m_txtFileName, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_btnSearchFile = new wxButton( m_mainPanel, ID_SENDFILESFRAME_BTN_SEARCHFILE, SENDFILESFRAME_BTN_SEARCH_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_fileNameSizer->Add(m_btnSearchFile, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_statusTransferGroupBoxSizer = new wxStaticBox(m_mainPanel, wxID_ANY, wxEmptyString);
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(m_statusTransferGroupBoxSizer, wxVERTICAL);
    m_mainPanelVertSizer->Add(itemStaticBoxSizer10, 0, wxGROW|wxALL, 5);

    m_transferLabelsGridSizer = new wxGridSizer(5, 5, 0, 0);
    itemStaticBoxSizer10->Add(m_transferLabelsGridSizer, 0, wxGROW|wxALL, 5);

    m_lblFileSizeCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_FILESIZE_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblFileSizeCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblFileSizeLength = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0.00 KB"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblFileSizeLength, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_transferLabelsGridSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_transferLabelsGridSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_transferLabelsGridSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_FILESENT_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblSentCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentPercentual = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0 %"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblSentPercentual, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_transferLabelsGridSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentBytesCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_FILESENTBYTES_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblSentBytesCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblSentBytesLength = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0.00 KB"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblSentBytesLength, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblRemainingTimeTransferCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_REMAININGTIME_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblRemainingTimeTransferCaption, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblRemainingTimeTransfer = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblRemainingTimeTransfer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_transferLabelsGridSizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBitrateCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_BITRATE_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblBitrateCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBitrate = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0 kB/s"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transferLabelsGridSizer->Add(m_lblBitrate, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_gaugeTransferFile = new wxGauge( m_mainPanel, ID_SENDFILESFRAME_GAUGE_TRANSFERFILE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gaugeTransferFile->SetValue(0);
    itemStaticBoxSizer10->Add(m_gaugeTransferFile, 0, wxGROW|wxALL, 5);

    m_recoverTransferGridSizer = new wxGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer10->Add(m_recoverTransferGridSizer, 0, wxALIGN_LEFT|wxALL, 5);

    m_lblRecoveredCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_RECOVERED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverTransferGridSizer->Add(m_lblRecoveredCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblRecoveredPercentual = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0 %"), wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverTransferGridSizer->Add(m_lblRecoveredPercentual, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblReamainingTimeRecoveredCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_REMAININGTIMERECOVERED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverTransferGridSizer->Add(m_lblReamainingTimeRecoveredCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblRemainingTimeRecovered = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("00:00:00"), wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverTransferGridSizer->Add(m_lblRemainingTimeRecovered, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_gaugeRecovered = new wxGauge( m_mainPanel, ID_SENDFILESFRAME_GAUGE_RECOVERED, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gaugeRecovered->SetValue(0);
    itemStaticBoxSizer10->Add(m_gaugeRecovered, 0, wxGROW|wxALL, 5);

    m_transferCtrlSizer = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer10->Add(m_transferCtrlSizer, 0, wxALIGN_LEFT|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_btnSend = new wxButton( m_mainPanel, ID_SENDFILESFRAME_BTN_SEND, SENDFILESFRAME_BTN_SEND_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferCtrlSizer->Add(m_btnSend, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_btnSend->Disable();

    m_btnCancel = new wxButton( m_mainPanel, ID_SENDFILESFRAME_BTN_CANCEL, SENDFILESFRAME_BTN_CANCEL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_transferCtrlSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_btnCancel->Disable();

	////Cancel button was hidden for send release  
	//m_btnCancel->Hide();

    m_StatisticGroupBoxSizer = new wxStaticBox(m_mainPanel, wxID_ANY, wxEmptyString);
    wxStaticBoxSizer* itemStaticBoxSizer37 = new wxStaticBoxSizer(m_StatisticGroupBoxSizer, wxVERTICAL);
    m_mainPanelVertSizer->Add(itemStaticBoxSizer37, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_statisticGridSizer = new wxGridSizer(4, 4, 0, 0);
    itemStaticBoxSizer37->Add(m_statisticGridSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_lblStatisticSentCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_STATISTICSENT_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticSentCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticSent = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticSent, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticCancelledCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_STATISTICCANCELLED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticCancelledCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticCancelled = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticCancelled, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticReceivedCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_STATISTICRECEIVED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticReceivedCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticReceived = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticReceived, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticToReceiveCaption = new wxStaticText( m_mainPanel, wxID_STATIC, SENDFILESFRAME_LBL_STATISTICTORECEIVE_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticToReceiveCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblStatisticToReceive = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_statisticGridSizer->Add(m_lblStatisticToReceive, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_mainVertSizer->Fit(this);
}


/*!
 * Should we show tooltips?
 */
bool SendFilesFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */
wxBitmap SendFilesFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

/*!
 * Get icon resources
 */
wxIcon SendFilesFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDFILESFRAME_BTN_SEARCHFILE
 */
void SendFilesFrame::OnBtnSearchFileClick( wxCommandEvent& event )
{
	wxString wildcard;

#ifdef WIN32	// "All files" means "*.*" in Windows, "*" in other systems
	wildcard = _("All files|*.*");
#else
	wildcard = _("All files|*");
#endif // WIN32

	wxFileDialog fileDialog(this, SENDFILESFRAME_DLG_CHOOSEFILE_LABEL, wxEmptyString, wxEmptyString, wildcard, wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (fileDialog.ShowModal() == wxID_OK)
	{
		m_txtFileName->SetValue(fileDialog.GetPath());
	}
}

void SendFilesFrame::OnClose(wxCloseEvent &event)
{
	wxCHECK_RET(m_appInterface, wxT("SendFilesFrame::OnClose(): I need AppInterface to work!!"));
	if (event.CanVeto())
	{
		if (!m_sendFinished)
		{
			int ret = OnFileTransferCancelSendFile();

			if (ret == SENDFILESFRAME_RET_ONCANCEL_OK)
			{
				this->Destroy();
			}
			else if (ret == SENDFILESFRAME_RET_ONCANCEL_CANCELERROR)
			{
				wxMessageBox(_("any files being sent."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
			}
		}
		else
		{
			this->Destroy();
		}
	}
	else
	{
		if(!m_sendFinished)
			m_appInterface->FileTransferCancelTransmission(m_mediaId);
		
		this->Destroy();
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDFILESFRAME_BTN_SEND
 */
void SendFilesFrame::OnBtnSendClick( wxCommandEvent& event )
{
	wxCHECK_RET(m_appInterface, wxT("SendFilesFrame::OnBtnSendClick( wxCommandEvent& event ): No AppInterface!!"));
	m_filePath = m_txtFileName->GetValue();
	
	// Check if file exists and we have permission to read
	if(wxFile::Exists(m_filePath))
	{
		if(!wxFile::Access(m_filePath, wxFile::read))
		{
			wxMessageBox(_("You do not have permission to access this file."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
			return;
		}
	}
	else
	{
		wxMessageBox(_("This file don't exist."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
		return;
	}
	//wxMessageBox(m_filePath);

	wxFileName file = wxFileName(m_filePath);
	unsigned long fileSize = file.GetSize().ToULong();

	const Channel *channel = m_appInterface->GetCurrentChannel(); 
	
	//Get channel bitrate
	Mode mode;
	ModeList modeList = channel->GetModeList();
	modeList.FindMode(IRM_MODE_CHANNEL_BITRATE, mode);
	long bitrate;
	mode.GetParameter().ToLong(&bitrate);
	
	OnFileTransferSendFile(channel->GetName(), m_filePath, fileSize, bitrate);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SENDFILESFRAME_BTN_CANCEL
 */
void SendFilesFrame::OnBtnCancelClick( wxCommandEvent& event )
{
	int ret = OnFileTransferCancelSendFile();

	if (ret == SENDFILESFRAME_RET_ONCANCEL_OK)
	{
		m_btnSend->Enable();
		m_btnCancel->Disable();
		m_btnSearchFile->Enable();
		m_txtFileName->Enable();
		m_sendAnimatedGif->Stop();
		InitializeAllFields();
	}	
	else if (ret == SENDFILESFRAME_RET_ONCANCEL_CANCELERROR)
	{
		m_btnSend->Enable();
		m_btnCancel->Disable();
		m_btnSearchFile->Enable();
		m_txtFileName->Enable();
		m_sendAnimatedGif->Stop();
		InitializeAllFields();

		wxMessageBox(_("any files being sent."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);	
	}
}

/** Cancel send file.
* @return SENDFILESFRAME_RET_ONCANCEL_OK. Send file was cancelled successfully.
* @return SENDFILESFRAME_RET_ONCANCEL_CANCELERROR. Send file couldn't cancelled.
* @return SENDFILESFRAME_RET_ONCANCEL_CANCELABORT. Send file cancel was aborted.
*/
int SendFilesFrame::OnFileTransferCancelSendFile()
{
	//wxCHECK_RET(m_appInterface, wxT("SendFilesFrame::OnFileTransferCancelSendFile(): I need AppInterface to work!!"));
	MessageDialog cancelDlg(this,_("Cancel"), _("Do you want cancel send file?"));
	if (cancelDlg.ShowModal() == ID_MESSAGEDIALOG_BTN_YES)
	{
		
		bool ret = m_appInterface->FileTransferCancelTransmission(m_mediaId);
		
		if (ret)
			return SENDFILESFRAME_RET_ONCANCEL_OK;			//SendFiles cancel successfull
		else
			return SENDFILESFRAME_RET_ONCANCEL_CANCELERROR;	//Can't cancel SendFiles
	}
	return SENDFILESFRAME_RET_ONCANCEL_CANCELABORT;			//Abort cancel SendFiles
}

void SendFilesFrame::OnTxtFileNameChange(wxCommandEvent &event)
{
	wxString str = event.GetString();
	if (str != wxEmptyString)
		m_btnSend->Enable();
	else
		m_btnSend->Disable();
}

void SendFilesFrame::OnTimer( wxTimerEvent& event )
{
	if (!m_isRecoverPhase)
	{
		unsigned long bytesPerSecond = m_packetSize * ((m_currentPacket - m_lastPacket)*1000/TIMER_INTERVAL);
		unsigned long timeLeft = 0;
		if(bytesPerSecond != 0)
			timeLeft = (m_fileSize - (m_currentPacket * m_packetSize))/bytesPerSecond;

		wxString label;
		SizeToSizeLabel(bytesPerSecond, label);
		label += wxT("/s");
		m_lblBitrate->SetLabel(label);

		wxTimeSpan timeLeftTS = wxTimeSpan::Seconds(timeLeft);
		m_lblRemainingTimeTransfer->SetLabel(timeLeftTS.Format());

		m_lastPacket = m_currentPacket;
	}
	else
	{
		unsigned long bytesPerSecond = ((m_packetSize/* * m_totalLostPackets*/)*1000/TIMER_INTERVAL);
		unsigned long timeLeft = 0;
		if(bytesPerSecond != 0)
			timeLeft = ((m_totalLostPackets - m_lostPackets) * m_packetSize) / bytesPerSecond;

		wxTimeSpan timeLeftLP = wxTimeSpan::Seconds(timeLeft);
		m_lblRemainingTimeRecovered->SetLabel(timeLeftLP.Format());
	}
}

/** Start whiteBoard file transmission.
* @param[in] filePath. Whiteboard path.
*/
void SendFilesFrame::StartWhiteboardFileTransmission(const wxString &filePath)
{
	if(filePath == wxEmptyString)
		return;

	m_filePath = filePath;
	m_txtFileName->SetValue(m_filePath);

	//if(m_btnSend->IsEnabled())
	{
		m_whiteboardTransmissionEnabled = true;
	
		wxCommandEvent e;

		this->OnBtnSendClick(e);
	}
}

/** Begin send file trasmission.
* @param[in] channel. Channel to send file.
* @param[in] filePath. File path.
* @param[in] filPath. File path.
* @param[in] fileSize. File size.
* @param[in] bitrate. Transmission Bitrate.
*/
void SendFilesFrame::OnFileTransferSendFile(const wxString &channel, const wxString &filePath, unsigned long fileSize, unsigned bitRate)
{
	InitializeAllFields();

	wxCHECK_RET(m_appInterface, wxT("SendFilesFrame::OnFileTransferSendBegin(): No AppInterface!!"));

	m_fileSize = fileSize;
	wxString label;
	SizeToSizeLabel(fileSize, label);
	m_lblFileSizeLength->SetLabel(label);

	//Checks if the user has media voice
	if (m_appInterface->GetCurrentChannelUser()->HasMediaVoice())
	{
		if(m_whiteboardTransmissionEnabled)
		{
			if (m_appInterface->FileTransferBeginWhiteboardTransmission(channel, filePath, fileSize, bitRate))
			{
				m_btnSend->Disable();
				m_btnCancel->Enable();
				m_btnSearchFile->Disable();
				m_txtFileName->Disable();
				m_timer.Start();
			}
			else
			{
				m_btnSend->Enable();
				m_btnCancel->Disable();
				m_btnSearchFile->Enable();
				m_txtFileName->Enable();
				m_timer.Stop();
			}	
		}

		else
		{
			if (m_appInterface->FileTransferBeginTransmission(channel, filePath, fileSize, bitRate))
			{
				m_btnSend->Disable();
				m_btnCancel->Enable();
				m_btnSearchFile->Disable();
				m_txtFileName->Disable();
				m_timer.Start();
			}
			else
			{
				m_btnSend->Enable();
				m_btnCancel->Disable();
				m_btnSearchFile->Enable();
				m_txtFileName->Enable();
				m_timer.Stop();
			}
		}
	}
	else
		wxMessageBox(_("User don't have voice"), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
}

/** Updates the progress of send.
* @param[in] currentPacketIndex. Index of the current packet sent.
* @param[in] lastPacketIndex. last packet to send.
*/
void SendFilesFrame::OnFileTransferSendProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	if (m_isTotalLostPackets)
	{
		m_packetSize = (float)m_fileSize / (float)lastPacketIndex;
		m_isTotalLostPackets = false;
	}

	if (m_beginningTransmission)
	{
		m_sendAnimatedGif->Play();
		m_beginningTransmission = false;
	}

	// Save current packet to use in timer
	m_currentPacket = currentPacketIndex;

	wxString label;
	if(currentPacketIndex != lastPacketIndex)
		SizeToSizeLabel(currentPacketIndex * m_packetSize, label);
	else
		SizeToSizeLabel(m_fileSize, label);
	m_lblSentBytesLength->SetLabel(label);

	float percentage = ((float)(currentPacketIndex)/(float)(lastPacketIndex))*100.0f;
	label.Printf(wxT("%.2f%%"), percentage);
	m_lblSentPercentual->SetLabel(label);

	m_gaugeTransferFile->SetValue((int)percentage);
}

/** Begin show the progress of lost packets upload.
* @param[in] lostPacket. Number of lost packets.
*/
void SendFilesFrame::OnFileTransferSendLostPacket(unsigned long lostPackets, unsigned long lostPacketsTotal)
{
	wxString label;

	m_totalLostPackets = lostPacketsTotal;
	m_lostPackets = lostPackets;

	float percentage = (((float)(lostPackets)/(float)(lostPacketsTotal)))*100.0f;
	label.Printf(wxT("%.2f%%"), percentage);
	m_lblRecoveredPercentual->SetLabel(label);

	m_gaugeRecovered->SetValue((int)percentage);
}

/** Informs thats file came to at end.
*
*/
void SendFilesFrame::OnFileTransferSendEof()
{
	m_isTotalLostPackets = true;
	m_isRecoverPhase = true;
}

/** Show thats the send ocurred successfully.
*
*/
void SendFilesFrame::OnFileTransferSendSuccess()
{
	m_sendFinished = true;
	m_sendAnimatedGif->Stop();
	m_timer.Stop();

	m_btnSend->Enable();
	m_btnCancel->Disable();
	m_btnSearchFile->Enable();
	m_txtFileName->Enable();
	m_sendAnimatedGif->Stop();

	InitializeAllFields();

	if(!m_whiteboardTransmissionEnabled)
		wxMessageBox(_("File transfer send successfull."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
	else
	{
		m_whiteboardTransmissionEnabled = false;
		Close();
	}

	
}

/** Initaialize all dialog fields.
* Private method.
*/
void SendFilesFrame::InitializeAllFields()
{
	m_sendFinished = true;
	m_beginningTransmission = true;
	m_totalLostPackets = 0;
	m_isTotalLostPackets = true;
	m_isRecoverPhase = false;
	m_gaugeTransferFile->SetValue(0);
	m_gaugeRecovered->SetValue(0);

	m_cancelled = 0;
	m_toReceive = 0;
	m_received = 0;
	m_sent = 0;

	m_lblSentPercentual->SetLabel(wxT("0 %"));
	m_lblSentBytesLength->SetLabel(wxT("0.00 KB"));
	m_lblRemainingTimeTransfer->SetLabel(wxT("00:00:00"));
	m_lblBitrate->SetLabel(wxT("0 bytes/s"));
	m_lblRecoveredPercentual->SetLabel(wxT("0 %"));
	m_lblRemainingTimeRecovered->SetLabel(wxT("00:00:00"));

	m_lblStatisticCancelled->SetLabel(wxString::Format(wxT("%d"), m_cancelled));
	m_lblStatisticReceived->SetLabel(wxString::Format(wxT("%d"), m_received));
	m_lblStatisticSent->SetLabel(wxString::Format(wxT("%d"), m_sent));
	m_lblStatisticToReceive->SetLabel(wxString::Format(wxT("%d"), m_toReceive));


}

/** Show thats an error ocurred with send.
*
*/
void SendFilesFrame::OnFileTransferSendError()
{
	m_sendFinished = true;
	m_sendAnimatedGif->Stop();

	//Checks if the user has media voice
	if (!m_appInterface->GetCurrentChannelUser()->HasMediaVoice())
		wxMessageBox(wxString::Format(_("File transfer Send error.\n%s lost voice."),m_appInterface->GetCurrentUser()->GetNickName().c_str()), wxMessageBoxCaptionStr, wxICON_ERROR|wxCENTRE, this);
	else
		wxMessageBox(_("File transfer Send error."), wxMessageBoxCaptionStr, wxICON_ERROR|wxCENTRE, this);

	m_whiteboardTransmissionEnabled = false;

	Close();
}

/** Show thats file send began.
*
*/
void SendFilesFrame::OnFileTranferSendBegin(long mediaId)
{
	m_mediaId = mediaId;
	m_sendFinished = false;
}

/** Informs that a new receiver entered.
* @param[in] mediaId. Media id of the new receiver.
*/
void SendFilesFrame::OnFileTransferSendReceiverNew(long mediaId)
{
	m_sent++;
	UpdateToReceiveStatistic();
}

/** Informs that a receiver cancelled the transmission.
* @param[in] mediaId. Media id of the receiver.
*/
void SendFilesFrame::OnFileTransferSendReceiverCancel(long mediaId)
{
	m_cancelled++;
	UpdateToReceiveStatistic();
}

/** Informs that a receiver terminate of download.
* @param[in] mediaId. Media id of the receiver.
*/
void SendFilesFrame::OnFileTransferSendReceiverEnd(long mediaId)
{
	m_received++;
	UpdateToReceiveStatistic();
}

/** Updates the number of users receivers statistics.
* Private method.
*/
void SendFilesFrame::UpdateToReceiveStatistic()
{
	m_toReceive = m_sent - m_cancelled - m_received;
	
	m_lblStatisticCancelled->SetLabel(wxString::Format(wxT("%d"), m_cancelled));
	m_lblStatisticReceived->SetLabel(wxString::Format(wxT("%d"), m_received));
	m_lblStatisticSent->SetLabel(wxString::Format(wxT("%d"), m_sent));
	m_lblStatisticToReceive->SetLabel(wxString::Format(wxT("%d"), m_toReceive));
}

/** Receive the size of file and devolve your respective string in bytes, Kbytes, MBytes or GBytes.
* @param[in] size. File Size.
* @param[out] label. String containing the size in format described above.
* Private method.
*/
void SendFilesFrame::SizeToSizeLabel(unsigned long size, wxString &label)
{
	// show in GBytes
	if((size/1073741824) > 0)
	{
		label.Printf(wxT("%d.%d GB"), size/1073741824, ((size*100)/1073741824)%100);
		return;
	}

	// show in MBytes
	if((size/1048576) > 0)
	{
		label.Printf(wxT("%d.%d MB"), size/1048576, ((size*100)/1048576)%100);
		return;
	}

	// show in KBytes
	if((size/1024) > 0)
	{
		label.Printf(wxT("%d.%d kB"), size/1024, ((size*10)/1024)%10);
		return;
	}

	// show in bytes
	label.Printf(wxT("%d Bytes"), size);
	return;
}
