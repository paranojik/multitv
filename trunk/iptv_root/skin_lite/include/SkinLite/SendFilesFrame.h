/////////////////////////////////////////////////////////////////////////////
// Name:        SendFilesFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     19/05/2008 09:50:49
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _SENDFILESFRAME_H_
#define _SENDFILESFRAME_H_


/*!
 * Includes
 */
#include "wx/frame.h"
#include "wx/animate.h"
#include "SkinLiteConstants.h"
#include "AppInterface.h"

/*!
 * Forward declarations
 */
class wxBoxSizer;
class wxGridSizer;

/*!
 * Control identifiers
 */
enum{
	ID_SENDFILESFRAME = 700,
	ID_SENDFILESFRAME_MAINPANEL,
	ID_SENDFILESFRAME_TXT_FILENAME,
	ID_SENDFILESFRAME_BTN_SEARCHFILE,
	ID_SENDFILESFRAME_GAUGE_TRANSFERFILE,
	ID_SENDFILESFRAME_GAUGE_RECOVERED,
	ID_SENDFILESFRAME_BTN_SEND,
	ID_SENDFILESFRAME_BTN_CANCEL
};


#define SYMBOL_SENDFILESFRAME_STYLE wxCAPTION/*|wxRESIZE_BORDER*/|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SENDFILESFRAME_TITLE _("Send Files")
#define SYMBOL_SENDFILESFRAME_IDNAME ID_SENDFILESFRAME
#define SYMBOL_SENDFILESFRAME_SIZE wxDefaultSize
#define SYMBOL_SENDFILESFRAME_POSITION wxDefaultPosition

#define	TIMER_INTERVAL 1000	// in miliseconds

//Returns of OnFileTransferCancelSendFile
#define SENDFILESFRAME_RET_ONCANCEL_OK 0			//SendFiles cancel successfull
#define SENDFILESFRAME_RET_ONCANCEL_CANCELERROR 1	//Can't cancel SendFiles
#define SENDFILESFRAME_RET_ONCANCEL_CANCELABORT 2	//Abort cancel SendFiles


/** @brief Class thats implements send files window.
*
* This window choose the file that must be send and show the status of this upload.
*/
class SendFilesFrame: public wxFrame
{
private:
    DECLARE_CLASS( SendFilesFrame )
    DECLARE_EVENT_TABLE()

    wxBoxSizer* m_mainVertSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_mainPanelVertSizer;
    wxStaticBox* m_fileGroupBoxSizer;
    wxStaticText* m_lblFileName;
    wxBoxSizer* m_fileNameSizer;
    wxTextCtrl* m_txtFileName;
    wxButton* m_btnSearchFile;
    wxStaticBox* m_statusTransferGroupBoxSizer;
    wxGridSizer* m_transferLabelsGridSizer;
    wxStaticText* m_lblFileSizeCaption;
    wxStaticText* m_lblFileSizeLength;
    wxStaticText* m_lblSentCaption;
    wxStaticText* m_lblSentPercentual;
    wxStaticText* m_lblSentBytesCaption;
    wxStaticText* m_lblSentBytesLength;
    wxStaticText* m_lblRemainingTimeTransferCaption;
    wxStaticText* m_lblRemainingTimeTransfer;
    wxStaticText* m_lblBitrateCaption;
    wxStaticText* m_lblBitrate;
    wxGauge* m_gaugeTransferFile;
    wxGridSizer* m_recoverTransferGridSizer;
    wxStaticText* m_lblRecoveredCaption;
    wxStaticText* m_lblRecoveredPercentual;
    wxStaticText* m_lblReamainingTimeRecoveredCaption;
    wxStaticText* m_lblRemainingTimeRecovered;
    wxGauge* m_gaugeRecovered;
    wxBoxSizer* m_transferCtrlSizer;
    wxButton* m_btnSend;
    wxButton* m_btnCancel;
    wxStaticBox* m_StatisticGroupBoxSizer;
    wxGridSizer* m_statisticGridSizer;
    wxStaticText* m_lblStatisticSentCaption;
    wxStaticText* m_lblStatisticSent;
    wxStaticText* m_lblStatisticCancelledCaption;
    wxStaticText* m_lblStatisticCancelled;
    wxStaticText* m_lblStatisticReceivedCaption;
    wxStaticText* m_lblStatisticReceived;
    wxStaticText* m_lblStatisticToReceiveCaption;
    wxStaticText* m_lblStatisticToReceive;
	wxAnimation m_animatedGif;
	wxAnimationCtrl *m_sendAnimatedGif;

	wxTimer m_timer;

	AppInterface* m_appInterface;
	wxString m_filePath;

	long m_mediaId;

	unsigned m_packetSize;
	unsigned long m_currentPacket;
	unsigned long m_lastPacket;
	unsigned long m_fileSize;
	bool m_sendFinished;
	bool m_isRecoverPhase;
	bool m_beginningTransmission;

	unsigned long m_lostPackets;
	unsigned long m_totalLostPackets;
	bool m_isTotalLostPackets;

	long m_toReceive;
	long m_sent;
	long m_cancelled;
	long m_received;

	bool m_whiteboardTransmissionEnabled;

	void SizeToSizeLabel(unsigned long size, wxString &label);
	void UpdateToReceiveStatistic();
	void InitializeAllFields();
public:
    // Constructors
    SendFilesFrame();
    SendFilesFrame( wxWindow* parent, AppInterface* iface, wxWindowID id = SYMBOL_SENDFILESFRAME_IDNAME, const wxString& caption = SYMBOL_SENDFILESFRAME_TITLE, const wxPoint& pos = SYMBOL_SENDFILESFRAME_POSITION, const wxSize& size = SYMBOL_SENDFILESFRAME_SIZE, long style = SYMBOL_SENDFILESFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SENDFILESFRAME_IDNAME, const wxString& caption = SYMBOL_SENDFILESFRAME_TITLE, const wxPoint& pos = SYMBOL_SENDFILESFRAME_POSITION, const wxSize& size = SYMBOL_SENDFILESFRAME_SIZE, long style = SYMBOL_SENDFILESFRAME_STYLE );

    // Destructor
    ~SendFilesFrame();

    // Initialises member variables
    void Init();

    // Creates the controls and sizers
    void CreateControls();

	void StartWhiteboardFileTransmission(const wxString &filePath);

	void OnFileTransferSendFile(const wxString &channel, const wxString &filePath, unsigned long fileSize, unsigned bitRate);
	void OnFileTransferSendProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex);
	void OnFileTransferSendLostPacket(unsigned long lostPackets, unsigned long lostPacketsTotal);
	void OnFileTransferSendEof();
	void OnFileTransferSendSuccess();
	void OnFileTransferSendError();
	int OnFileTransferCancelSendFile();
	void OnFileTranferSendBegin(long mediaId);
	void OnFileTransferSendReceiverNew(long mediaId);
	void OnFileTransferSendReceiverCancel(long mediaId);
	void OnFileTransferSendReceiverEnd(long mediaId);

    // Events
    void OnBtnSearchFileClick( wxCommandEvent& event );
    void OnBtnSendClick( wxCommandEvent& event );
    void OnBtnCancelClick( wxCommandEvent& event );
	void OnClose(wxCloseEvent &event);
	void OnTxtFileNameChange(wxCommandEvent &event);
	void OnTimer( wxTimerEvent& event );

    // Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    // Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    // Should we show tooltips?
    static bool ShowToolTips();
};

#endif
    // _SENDFILESFRAME_H_
