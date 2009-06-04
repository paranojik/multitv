#ifndef _SKIN_
#define _SKIN_

#ifdef _MSC_VER
#pragma warning (disable: 4005)	// _WINSOCKAPI_ macro redefinition
#pragma warning (disable: 4267) // size_t to int conversion
#endif

#include <wx/wx.h>

#include "Constants.h"
#include "AppInterface.h"
#include "VideoSink.h"
#include "TransmissionDisplayControl.h"
#include "PrivateChatWindowList.h"

/** @brief Base class for all Skins.
*
* This abstract class is the base class for all Skins. It inherits from wxFrame,
* thus it is also meant to represent the main window of a Skin. Skins may contain 
* other windows/classes as needed. It contains an AppInterface pointer, accessible
* via GetAppInterface() and initialized upon loading by App, which gives Skins
* access to needed App methods.
*/
class Skin : public wxFrame
{
private:

protected:
	AppInterface *m_appInterface;

	wxPoint m_delta;
	wxBitmap *m_bmp;
	bool m_hasShape;

	bool m_dragEnable;
	wxString m_skinSelected;
	wxString m_languageSelected;

	int m_indexSkin;
	int m_indexLanguage;
	// List of active private chat windows
	PrivateChatWindowList m_pcwList;
	// Video capture display control
	VideoDisplayControl *m_vdcCaptureView;
	bool m_captureEnabled;

	// Old code
	// FIXME: Should this *really* be here?
	wxButton *m_btnChangeSkin;
	wxButton *m_btnChangeLanguage;
	wxButton *m_btnExit;
	wxButton *m_btnUnicast;
	wxButton *m_btnMulticast;
	wxButton *m_btnFile;
	wxButton *m_btnAbout;
	wxButton *m_btnProfile;

	// Methods:
	void SetWindowShape(wxBitmap *);
	void InitializeBase();
	VideoDisplayControl *DoCreateVideoDisplayControl(unsigned long mediaID);

	virtual void CreateChangeSkinBtn();
	virtual void CreateChangeLanguageBtn();
	virtual void CreateExitBtn();
	virtual void CreateUnicastBtn();
	virtual void CreateMulticastBtn();
	virtual void CreateFileBtn();
	virtual void CreateAboutBtn();

	virtual void UpdateText();
	
	virtual void OnChangeSkin(wxCommandEvent &event);
	virtual void OnChangeLanguage(wxCommandEvent &event);
	virtual void OnExit(wxCommandEvent &event);

	void OnAboutBtnClick(wxCommandEvent &event);

	void OnPaint(wxPaintEvent &event);
	void OnMouseLeftDown(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
	
	void OnWindowCreate(wxWindowCreateEvent &WXUNUSED(event));
public:
	Skin();
	virtual ~Skin();

	virtual void Initialize() = 0;

	void SetAppInterface(AppInterface *iface);
	AppInterface *GetAppInterface() const;

	void PaintBaseSkin();
	
	void EnableDrag();
	void DisableDrag();

	// App->Skin command methods
	virtual void OnHostConnect() = 0;
	virtual void ShowChannelList() = 0;
	virtual void RefreshChannelList() = 0;
	virtual void RefreshUserList() = 0;
	virtual void OnConnectionFailure(wxString message = wxEmptyString) = 0;
	virtual void OnServerDisconnect(wxString message = wxEmptyString) = 0;
	virtual void OnChannelJoin() = 0;
	virtual void OnChannelBadPassword(const wxString &failureMessage = wxEmptyString) = 0;
	virtual void OnPrivateMessage(const wxString &sender, const wxString &msg) = 0;
	virtual void OnChannelMessage(const wxString &sender, const wxString &msg, const wxString &channel = wxEmptyString) = 0;
	virtual void OnActiveTransmissionCountChange(int newCount) = 0;
	virtual void UpdateFileProgressIndicator(float percent) = 0;
	virtual void OnFileTransferReceptionBegin(	long mediaId,
												const wxString &fileName,
												const wxString &tempFilePath,
												const wxString &senderNick,
												unsigned long fileSize,
												unsigned packetSize,
												bool renameFile = true) = 0;
	virtual void OnFileTransferReceptionProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex) = 0;
	virtual void OnFileTransferReceptionLostPacket(unsigned long lostPackets) = 0;
	virtual void OnFileTransferReceptionSuccess() = 0;
	virtual void OnFileTransferReceptionError() = 0;
	virtual void OnFileTranferSendBegin(long mediaId) = 0;
	virtual void OnFileTransferSendProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex) = 0;
	virtual void OnFileTransferSendLostPacket(unsigned long lostPackets, unsigned long lostPacketsTotal) = 0;
	virtual void OnFileTransferSendEof() = 0;
	virtual void OnFileTransferSendSuccess() = 0;
	virtual void OnFileTransferSendError() = 0;
	virtual void OnFileTransferSendReceiverNew(long mediaId) = 0;
	virtual void OnFileTransferSendReceiverCancel(long mediaId) = 0;
	virtual void OnFileTransferSendReceiverEnd(long mediaId) = 0;
	//Movie playlist events
	virtual void OnMovieSendBegin(long mediaId, const wxString &channelName) = 0;
	virtual void OnMovieSendEof(long mediaId, const wxString &channelName) = 0;
	// Poll events
	virtual void OnPoll() = 0;
	virtual void OnPollVote() = 0;
	virtual void OnPollStats(const PollInfo &info) = 0;
	// Application Sharing viewing
	virtual void OnAppSharingBegin() = 0;
	virtual void OnAppSharingEnd() = 0;
	// Whiteboard events
	virtual void OnWhiteboardMessage(const wxString &channel, const wxString &user, const wxString &data) = 0;
	//Collaboration Auto play
	virtual void OnCollaborationAutoPlay() = 0;
	virtual void OnCollaborationStop() = 0;

	// About box string
	virtual wxString GetAboutInfo();

	// VDC handling method, version 4.
	// Now uses a more general VideoSink approach, but constrained to the operation rules set by App.
	virtual VideoSink *CreateVideoSink(void *extraData = NULL) = 0;
	virtual void AddTransmissionDisplayControl(TransmissionDisplayControl *tdc) = 0;
	virtual void RemoveTransmissionDisplayControl(TransmissionDisplayControl *tdc) = 0;
	
	// This function will be called by App upon closing of Skin's main window.
	// App will always handle Skin's wxCloseEvent. This overrideable function
	// is provided to handle Skin-specific behavior IN ADDITION to App's normal
	// behavior on closing Skin's main window.
	// App will check the return value from this function, and will only effectively
	// proceed with the processing of the close event if it returns true.
	virtual bool OnMainWindowClose(bool force = false);


	virtual PrivateChatWindowBase *CreatePrivateChatWindow(const wxString &userName) = 0;
	PrivateChatWindowList &GetPrivateChatWindowList();

	// Called every app timer tick
	virtual void OnAppTimer();

	DECLARE_EVENT_TABLE()
};

enum
{
	ID_QUIT  = wxID_EXIT,
	ID_CHOOSE_SKIN = wxID_HIGHEST + 1,
	ID_CHOOSE_LANGUAGE,
	ID_BTN_UNICAST,
	ID_BTN_MULTICAST,
	ID_BTN_FILE,
	ID_BTN_KERNEL_TEST,
	ID_BTN_PROFILE,
	ID_BTN_CONFIGURE
};

#endif
