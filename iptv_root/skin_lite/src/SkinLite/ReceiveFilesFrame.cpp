/////////////////////////////////////////////////////////////////////////////
// Name:        ReceiveFilesFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     20/05/2008 10:01:06
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

#include "SkinLiteConstants.h" 
#include "ReceiveFilesFrame.h"
#include "MessageDialog.h"
#include "SessionWindow.h"
#include "SkinLite.h"


/*!
 * ReceiveFilesFrame type definition
 */
IMPLEMENT_CLASS( ReceiveFilesFrame, wxFrame )


// ReceiveFilesFrame event table definition
BEGIN_EVENT_TABLE( ReceiveFilesFrame, wxFrame )
EVT_CLOSE(ReceiveFilesFrame::OnClose)
    EVT_BUTTON( ID_RECEIVEFILESFRAME_BTN_ACCEPT, ReceiveFilesFrame::OnBtnAcceptClick )
    EVT_BUTTON( ID_RECEIVEFILESFRAME_BTN_CANCEL, ReceiveFilesFrame::OnBtnCancelClick )
	EVT_TIMER( wxID_ANY, ReceiveFilesFrame::OnTimer )
END_EVENT_TABLE()


/** ReceiveFilesFrame default constructor.
 * 
 */
ReceiveFilesFrame::ReceiveFilesFrame()
{
    Init();
}

/** ReceiveFilesFrame constructor.
* @param[in] parent. Parent window.
* @param[in] iface. Interface with application.
* @param[in] id. Window id. Default SYMBOL_RECEIVEFILESFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_RECEIVEFILESFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_RECEIVEFILESFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_RECEIVEFILESFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_RECEIVEFILESFRAME_STYLE.
*/
ReceiveFilesFrame::ReceiveFilesFrame( wxWindow* parent, AppInterface* iface, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
	m_appInterface = iface;
    Create( parent, id, caption, pos, size, style );
	SetMinSize(SYMBOL_RECEIVEFILESFRAME_SIZE);
}

/** ReceiveFilesFrame creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_RECEIVEFILESFRAME_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_RECEIVEFILESFRAME_TITLE.
* @param[in] pos. Window position. Default SYMBOL_RECEIVEFILESFRAME_POSITION.
* @param[in] size. Window size. Default SYMBOL_RECEIVEFILESFRAME_SIZE.
* @param[in] style. Window style. Default SYMBOL_RECEIVEFILESFRAME_STYLE.
*/
bool ReceiveFilesFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();

	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangeReceiveFilesExistsStatus(true);

    return true;
}


/** ReceiveFilesFrame destructor.
 *
 */
ReceiveFilesFrame::~ReceiveFilesFrame()
{
	SessionWindow *sessionWnd = ((SessionWindow *)GetParent());
	SkinLite *skinLite = (SkinLite *)(sessionWnd->GetParent());
	skinLite->ChangeReceiveFilesExistsStatus(false);
}


/*!
 * Member initialisation
 */
void ReceiveFilesFrame::Init()
{
	m_renameFile = true;
    m_mainVertSizer = NULL;
    m_mainPanel = NULL;
    m_mainPanelVertSizer = NULL;
    m_groupBoxReceiveFileTransferSizer = NULL;
    m_lblReceivingFileInf = NULL;
    m_lblSizeCaption = NULL;
    m_lblSizeLength = NULL;
    m_lblReceivedCaption = NULL;
    m_lblReceivedPercentual = NULL;
    m_lblBytesReceivedCaption = NULL;
    m_lblBytesReceived = NULL;
    m_lblRemainingTimeCaption = NULL;
    m_lblRemainingTime = NULL;
    m_lblBitrateCaption = NULL;
    m_lblBitrateLength = NULL;
    m_gaugeReceivedFile = NULL;
    m_recoverInfSizer = NULL;
    m_lblPackagesToRecoverCaption = NULL;
    m_lblPackagesToRecoverCount = NULL;
    m_ctrlSizer = NULL;
    m_btnAccept = NULL;
    m_btnCancel = NULL;

	m_receptionFinished = false;

	m_lastPacket = 0;
	m_currentPacket = 0;
	m_timer.SetOwner(this);
	m_timer.Start(TIMER_INTERVAL);
}


/*!
 * Control creation for ReceiveFilesFrame
 */
void ReceiveFilesFrame::CreateControls()
{
    ReceiveFilesFrame* itemFrame1 = this;

    m_mainVertSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainVertSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_RECEIVEFILESFRAME_MAINPANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainVertSizer->Add(m_mainPanel, 1, wxGROW, 5);

    m_mainPanelVertSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(m_mainPanelVertSizer);

	m_animatedGif.LoadFile(RECEIVEFILESFRAME_ANIMATEDRECEIVEGIF);

	m_receiveAnimatedGif = new wxAnimationCtrl(m_mainPanel, wxID_ANY, m_animatedGif);
	m_mainPanelVertSizer->Add(m_receiveAnimatedGif, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    m_groupBoxReceiveFileTransferSizer = new wxStaticBox(m_mainPanel, wxID_ANY, wxEmptyString);
    wxStaticBoxSizer* itemStaticBoxSizer5 = new wxStaticBoxSizer(m_groupBoxReceiveFileTransferSizer, wxVERTICAL);
    m_mainPanelVertSizer->Add(itemStaticBoxSizer5, 0, wxGROW|wxALL, 5);

    m_lblReceivingFileInf = new wxStaticText( m_mainPanel, wxID_STATIC, wxT("Receiving file \"xxx.xxx\" ..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer5->Add(m_lblReceivingFileInf, 0, wxALIGN_LEFT|wxALL, 5);

    wxGridSizer* itemGridSizer7 = new wxGridSizer(3, 4, 0, 0);
    itemStaticBoxSizer5->Add(itemGridSizer7, 0, wxGROW|wxALL, 5);

    m_lblSizeCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_SIZE_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblSizeCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_lblSizeLength = new wxStaticText( m_mainPanel, wxID_STATIC, _("0.00 KB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblSizeLength, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemGridSizer7->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemGridSizer7->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblReceivedCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_RECEIVED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblReceivedCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_lblReceivedPercentual = new wxStaticText( m_mainPanel, wxID_STATIC, _("0 %"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblReceivedPercentual, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBytesReceivedCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_BYTESRECEIVED_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblBytesReceivedCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBytesReceived = new wxStaticText( m_mainPanel, wxID_STATIC, _("0.00 KB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblBytesReceived, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblRemainingTimeCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_REMAININGTIME_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblRemainingTimeCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_lblRemainingTime = new wxStaticText( m_mainPanel, wxID_STATIC, _("00:00:00"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblRemainingTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBitrateCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_BITRATE_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblBitrateCaption, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblBitrateLength = new wxStaticText( m_mainPanel, wxID_STATIC, _("0.0 kbps"), wxDefaultPosition, wxDefaultSize, 0 );
    itemGridSizer7->Add(m_lblBitrateLength, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_gaugeReceivedFile = new wxGauge( m_mainPanel, ID_RECEIVEFILESFRAME_GAUGE_RECEIVEDFILE, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
    m_gaugeReceivedFile->SetValue(0);
    itemStaticBoxSizer5->Add(m_gaugeReceivedFile, 0, wxGROW|wxALL, 5);

    m_recoverInfSizer = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer5->Add(m_recoverInfSizer, 0, wxALIGN_LEFT|wxALL, 5);

    m_lblPackagesToRecoverCaption = new wxStaticText( m_mainPanel, wxID_STATIC, RECEIVEFILESFRAME_LBL_PACKAGESTORECOVER_CAPTION, wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverInfSizer->Add(m_lblPackagesToRecoverCaption, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_lblPackagesToRecoverCount = new wxStaticText( m_mainPanel, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    m_recoverInfSizer->Add(m_lblPackagesToRecoverCount, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_mainPanelVertSizer->Add(m_ctrlSizer, 0, wxALIGN_RIGHT|wxALL, 5);

    m_btnAccept = new wxButton( m_mainPanel, ID_RECEIVEFILESFRAME_BTN_ACCEPT, RECEIVEFILESFRAME_BTN_SAVE_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
	m_btnAccept->Disable();
    m_ctrlSizer->Add(m_btnAccept, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( m_mainPanel, ID_RECEIVEFILESFRAME_BTN_CANCEL, RECEIVEFILESFRAME_BTN_CANCEL_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_mainVertSizer->Fit(this);
}


/*!
 * Should we show tooltips?
 */
bool ReceiveFilesFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */
wxBitmap ReceiveFilesFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
}

/*!
 * Get icon resources
 */
wxIcon ReceiveFilesFrame::GetIconResource( const wxString& name )
{
    // Icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
}

void ReceiveFilesFrame::OnBtnAcceptClick( wxCommandEvent& event )
{
	wxFileDialog fileDialog(this, RECEIVEFILESFRAME_DLG_SAVE_LABEL, wxEmptyString, m_fileName, wxEmptyString, wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (fileDialog.ShowModal() == wxID_OK)
	{
		// Check if we have permission to overwrite
		if(wxFile::Exists(fileDialog.GetPath()))
		{
			if(!wxFile::Access(fileDialog.GetPath(), wxFile::write))
			{
				wxMessageBox(_("You do not have permission to access this file."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
				return;
			}
		}

		// Check if we have permission to create
		if(!wxFile::Access(fileDialog.GetDirectory(), wxFile::write))
		{
			wxMessageBox(_("You do not have permission to access this file."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
			return;
		}

		// Copy the file
		if(!wxCopyFile(m_tempFilePath, fileDialog.GetPath()))
		{
			wxMessageBox(_("Fail to copy file."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
			return;
		}

		// Remove the temp file
		wxRemoveFile(m_tempFilePath);

		wxMessageBox(_("Save successful."), wxMessageBoxCaptionStr, 4|wxCENTRE, this);
		Close();
	}
}

void ReceiveFilesFrame::OnBtnCancelClick( wxCommandEvent& event )
{
	Close();
}

/** Is called for begin the receive file. Begin show the progress of download.
* @param[in] mediaId. Media id.
* @param[in] fileName. File name.
* @param[in] tempFilePath. Temporary file path.
* @param[in] senderNick. User name of who is sending file.
* @param[in] fileSize. File size.
* @param[in] packetSize. Packet size.
* @param[in] renameFile. Define if file name is going to be modified at end of transmission.
*/
void ReceiveFilesFrame::OnFileTransferReceptionBegin(	long mediaId,
														const wxString &fileName,
														const wxString &tempFilePath,
														const wxString &senderNick,
														unsigned long fileSize,
														unsigned packetSize,
														bool renameFile)
{
	m_renameFile = renameFile;
	m_mediaId = mediaId;
	m_fileName = fileName;
	m_tempFilePath = tempFilePath;
	m_fileSize = fileSize;
	m_packetSize = packetSize;

	wxString label;
	SizeToSizeLabel(fileSize, label);
	m_lblSizeLength->SetLabel(label);

	label = _("Receiving file");
	label += wxT(" \"") + fileName + wxT("\""); 
	m_lblReceivingFileInf->SetLabel(label);

	m_receiveAnimatedGif->Play();
}

/** Updates the progress of reception.
* @param[in] currentPacketIndex. Index of the current packet received.
* @param[in] lastPacketIndex. last packet to receive.
*/
void ReceiveFilesFrame::OnFileTransferReceptionProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex)
{
	// Save current packet to use in timer
	m_currentPacket = currentPacketIndex;

	wxString label;
	if(currentPacketIndex != lastPacketIndex)
		SizeToSizeLabel(currentPacketIndex * m_packetSize, label);
	else
		SizeToSizeLabel(m_fileSize, label);
	m_lblBytesReceived->SetLabel(label);

	float percentage = ((float)(currentPacketIndex)/(float)(lastPacketIndex))*100.0f;
	label.Printf(wxT("%.2f%%"), percentage);
	m_lblReceivedPercentual->SetLabel(label);

	m_gaugeReceivedFile->SetValue((int)percentage);
}

/** Begin show the progress of lost packets download.
* @param[in] lostPacket. Number of lost packets.
*/
void ReceiveFilesFrame::OnFileTransferReceptionLostPacket(unsigned long lostPackets)
{
	wxString label;
	label.Printf(wxT("%d"), lostPackets);
	m_lblPackagesToRecoverCount->SetLabel(label);
}

/** Show thats the reception ocurred successfully.
*
*/
void ReceiveFilesFrame::OnFileTransferReceptionSuccess()
{
	m_receptionFinished = true;
	m_receiveAnimatedGif->Stop();
	m_timer.Stop();

	if(m_renameFile)
		m_btnAccept->Enable();
	else
		Close();
}

/** Show thats an error ocurred with reception.
*
*/
void ReceiveFilesFrame::OnFileTransferReceptionError()
{
	m_receptionFinished = true;
	m_receiveAnimatedGif->Stop();
	wxMessageBox(_("File transfer reception error."), _("Error"), wxICON_ERROR, this);

	// Remove the temp file
	wxRemoveFile(m_tempFilePath);

	Close(true);
}

void ReceiveFilesFrame::OnClose(wxCloseEvent &event)
{
	if (event.CanVeto())
	{
		if(!m_receptionFinished)
		{
			MessageDialog cancelDlg(this,_("Cancel"), _("Do you want cancel receive file?"));
			if (cancelDlg.ShowModal() == ID_MESSAGEDIALOG_BTN_YES)
			{
				m_appInterface->FileTransferCancelReception(m_mediaId);

				// Remove the temp file
				wxRemoveFile(m_tempFilePath);

				Destroy();
			}
			else	// User answered No.
				event.Veto();
		}
		else
		{
			// Remove the temp file
			wxRemoveFile(m_tempFilePath);

			Destroy();
		}
	}
	else
	{
		if (!m_receptionFinished)
		{
			m_appInterface->FileTransferCancelReception(m_mediaId);

			// Remove the temp file
			wxRemoveFile(m_tempFilePath);

			Destroy();
		}
		else
		{
			// Remove the temp file
			wxRemoveFile(m_tempFilePath);

			Destroy();
		}
	}
}

void ReceiveFilesFrame::OnTimer( wxTimerEvent& event )
{
	unsigned long bytesPerSecond = m_packetSize * ((m_currentPacket - m_lastPacket)*1000/TIMER_INTERVAL);
	unsigned long timeLeft = 0;
	if(bytesPerSecond != 0)
		timeLeft = (m_fileSize - (m_currentPacket * m_packetSize))/bytesPerSecond;

	wxString label;
	SizeToSizeLabel(bytesPerSecond, label);
	label += wxT("/s");
	m_lblBitrateLength->SetLabel(label);

	wxTimeSpan timeLeftTS = wxTimeSpan::Seconds(timeLeft);
	m_lblRemainingTime->SetLabel(timeLeftTS.Format());

	m_lastPacket = m_currentPacket;
}

/** Receive the size of file and devolve your respective string in bytes, Kbytes, MBytes or GBytes.
* @param[in] size. File Size.
* @param[out] label. String containing the size in format described above.
* Private method.
*/
void ReceiveFilesFrame::SizeToSizeLabel(unsigned long size, wxString &label)
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
