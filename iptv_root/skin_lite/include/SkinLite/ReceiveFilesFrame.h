/////////////////////////////////////////////////////////////////////////////
// Name:        ReceiveFilesFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     20/05/2008 10:01:06
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RECEIVEFILESFRAME_H_
#define _RECEIVEFILESFRAME_H_


/*!
 * Includes
 */
#include "wx/frame.h"
#include "wx/animate.h"
#include "AppInterface.h"

/*!
 * Forward declarations
 */
class wxBoxSizer;

/*!
 * Control identifiers
 */
enum{
ID_RECEIVEFILESFRAME = 800,
ID_RECEIVEFILESFRAME_MAINPANEL,
ID_RECEIVEFILESFRAME_GAUGE_RECEIVEDFILE,
ID_RECEIVEFILESFRAME_TXT_FILENAME,
ID_RECEIVEFILESFRAME_BTN_SAVE,
ID_RECEIVEFILESFRAME_BTN_ACCEPT,
ID_RECEIVEFILESFRAME_BTN_CANCEL
};

#define SYMBOL_RECEIVEFILESFRAME_STYLE wxCAPTION|wxSYSTEM_MENU//|wxCLOSE_BOX|wxFRAME_NO_TASKBAR|wxRESIZE_BORDER
#define SYMBOL_RECEIVEFILESFRAME_TITLE _("Receive Files")
#define SYMBOL_RECEIVEFILESFRAME_IDNAME ID_RECEIVEFILESFRAME
#define SYMBOL_RECEIVEFILESFRAME_SIZE wxDefaultSize
#define SYMBOL_RECEIVEFILESFRAME_POSITION wxDefaultPosition

#define	TIMER_INTERVAL 1000	// in miliseconds


/** @brief Class thats implements receive files window.
*
* When a user thats have voice send a file, the others users receiving this window informing the status of file download.
*/
class ReceiveFilesFrame: public wxFrame
{
private:
    DECLARE_CLASS( ReceiveFilesFrame )
    DECLARE_EVENT_TABLE()

	bool m_renameFile;

	AppInterface* m_appInterface;
	long m_mediaId;
	wxString m_fileName;
	wxString m_tempFilePath;
	wxString m_senderNick;
	unsigned long m_fileSize;
	unsigned m_packetSize;
	bool m_receptionFinished;
	unsigned long m_lastPacket;
	unsigned long m_currentPacket;

	wxTimer m_timer;

    wxBoxSizer* m_mainVertSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_mainPanelVertSizer;
    wxStaticBox* m_groupBoxReceiveFileTransferSizer;
    wxStaticText* m_lblReceivingFileInf;
    wxStaticText* m_lblSizeCaption;
    wxStaticText* m_lblSizeLength;
    wxStaticText* m_lblReceivedCaption;
    wxStaticText* m_lblReceivedPercentual;
    wxStaticText* m_lblBytesReceivedCaption;
    wxStaticText* m_lblBytesReceived;
    wxStaticText* m_lblRemainingTimeCaption;
    wxStaticText* m_lblRemainingTime;
    wxStaticText* m_lblBitrateCaption;
    wxStaticText* m_lblBitrateLength;
    wxGauge* m_gaugeReceivedFile;
    wxBoxSizer* m_recoverInfSizer;
    wxStaticText* m_lblPackagesToRecoverCaption;
    wxStaticText* m_lblPackagesToRecoverCount;
    wxBoxSizer* m_ctrlSizer;
    wxButton* m_btnAccept;
    wxButton* m_btnCancel;

	wxAnimation m_animatedGif;
	wxAnimationCtrl *m_receiveAnimatedGif;

	void SizeToSizeLabel(unsigned long size, wxString &label);

public:
    /// Constructors
    ReceiveFilesFrame();
    ReceiveFilesFrame( wxWindow* parent, AppInterface* iface, wxWindowID id = SYMBOL_RECEIVEFILESFRAME_IDNAME, const wxString& caption = SYMBOL_RECEIVEFILESFRAME_TITLE, const wxPoint& pos = SYMBOL_RECEIVEFILESFRAME_POSITION, const wxSize& size = SYMBOL_RECEIVEFILESFRAME_SIZE, long style = SYMBOL_RECEIVEFILESFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_RECEIVEFILESFRAME_IDNAME, const wxString& caption = SYMBOL_RECEIVEFILESFRAME_TITLE, const wxPoint& pos = SYMBOL_RECEIVEFILESFRAME_POSITION, const wxSize& size = SYMBOL_RECEIVEFILESFRAME_SIZE, long style = SYMBOL_RECEIVEFILESFRAME_STYLE );

    // Destructor
    ~ReceiveFilesFrame();

    // Initialises member variables
    void Init();

    // Creates the controls and sizers
    void CreateControls();

	
	void OnFileTransferReceptionBegin(	long mediaId,
										const wxString &fileName,
										const wxString &tempFilePath,
										const wxString &senderNick,
										unsigned long fileSize,
										unsigned packetSize,
										bool renameFile);
	void OnFileTransferReceptionProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex);
	void OnFileTransferReceptionLostPacket(unsigned long lostPackets);
	void OnFileTransferReceptionSuccess();
	void OnFileTransferReceptionError();


    void OnBtnSaveClick( wxCommandEvent& event );
    void OnBtnAcceptClick( wxCommandEvent& event );
    void OnBtnCancelClick( wxCommandEvent& event );
	void OnTimer( wxTimerEvent& event );
	void OnClose(wxCloseEvent &event);

    // Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    // Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    // Should we show tooltips?
    static bool ShowToolTips();
};

#endif
    // _RECEIVEFILESFRAME_H_
