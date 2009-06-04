#ifdef WIN32	// Windows only

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SKINVIEWER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SKINLITE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SKINLITE_EXPORTS
#define SKINLITE_API __declspec(dllexport)
#define SKIN_API __declspec(dllexport)
#else
#define SKINLITE_API __declspec(dllimport)
#define SKIN_API __declspec(dllimport)
#endif

#else

#define SKINLITE_API
#define SKIN_API

#endif		// WIN32

#include <wx/wx.h>
#include "Skin.h"
#include "Constants.h"
#include "SkinLitePanel.h"
#include "ChannelWindow.h"
#include "SessionWindow.h"
#include "ChatFrame.h"
#include "ReceiveFilesFrame.h"
#include "VideoSink.h"
#include "SendFilesFrame.h"
#include "PlaylistFrame.h"
// #include "ChannelsListFrame.h"

/** @brief Class derived of the Skin.
*
* Implements the Skin Lite dll or so, in windows or linux, respectively
*/
class SKINLITE_API SkinLite : public Skin
{
private:
	SkinLitePanel *m_panel;
	wxBoxSizer* m_BoxSizerPanel;
	wxBoxSizer* m_cmdSizer;
	wxBoxSizer* m_ctrlSizer;
	wxStaticBox* m_staticBox;
	ChannelWindow *m_channelWindow;
	SessionWindow *m_sessionWindow;
	ReceiveFilesFrame *m_activeReceiveFilesFrame;
	SendFilesFrame *m_sendFilesFrame;
	PlaylistFrame *m_playlistFrame;

	wxButton *m_btnConfigure;

	wxMenu *m_menuSettings,
           *m_menuHelp;

	bool m_sessionWindowExists;
	bool m_sendFilesExists;
	bool m_receiveFilesExists;
	bool m_playlistExists;

	void DisableAllButtons();
	void EnableAllButtons();

	void CreateMenuBar();

	void CreateSessionWindow();

	long BoolToLong(const bool &boolVariable);
	bool LongToBool(const long &longVariable);

	wxString GetConferenceRecordingFilename(const wxString& channel);


// Methods
public:
	SkinLite();
	~SkinLite();
	void Welcome();
	void Initialize();
	void UpdateText();
	void PaintScreen();

	void ConfigureBtnClick();

	void CreateChangeSkinBtn();
	void CreateChangeLanguageBtn();
	void CreateUnicastBtn();
	void CreateMulticastBtn();
	void CreateFileBtn();

	void CreateProfileBtn();
	void CreateConfigureBtn();
	void CreateAboutBtn();

	void CreateSendFiles();
	void CreatePlaylist();

	void StartWhiteboardFileTransmission(const wxString &filePath);

	void ChangeSendFilesExistsStatus(const bool &sendFilesExists);
	void ChangeReceiveFilesExistsStatus(const bool &receiveFilesExists);
	void ChangePlaylistExistsStatus(const bool &playlistExists);

	void OnSendFilesClose();
	void OnReceiveFilesClose();

	void CheckConferenceRecording(const wxString& channel);
	
	bool CheckSessionWindowExists();	

	// Events
	void OnUnicastBtnClick(wxCommandEvent &event);
	void OnMulticastBtnClick(wxCommandEvent &event);
	void OnFileBtnClick(wxCommandEvent &event);
	void OnProfileBtnClick(wxCommandEvent &event);
	void OnConfigureBtnClick(wxCommandEvent &event);
	void OnRecordingSettingsClick(wxCommandEvent &event);

	// App->Skin communication methods
	void OnHostConnect();
	void ShowChannelList();
	void RefreshChannelList();
	void RefreshUserList();
	void OnConnectionFailure(wxString message);
	void OnServerDisconnect(wxString message);
	void OnChannelJoin();
	void OnChannelBadPassword(const wxString &failureMessage = wxEmptyString);
	void OnPrivateMessage(const wxString &sender, const wxString &msg);
	void OnChannelMessage(const wxString &sender, const wxString &msg, const wxString &channel = wxEmptyString);
	void OnActiveTransmissionCountChange(int newCount);
	void UpdateFileProgressIndicator(float percent);
	void OnFileTransferReceptionBegin(	long m_mediaId,
												const wxString &m_fileName,
												const wxString &m_tempFilePath,
												const wxString &m_senderNick,
												unsigned long m_fileSize,
												unsigned m_packetSize,
												bool renameFile = false);
	void OnFileTransferReceptionProgress(unsigned long m_currentPacketIndex, unsigned long m_lastPacketIndex);
	void OnFileTransferReceptionLostPacket(unsigned long m_lostPackets);
	void OnFileTransferReceptionSuccess();
	void OnFileTransferReceptionError();
	void OnFileTranferSendBegin(long mediaId);
	void OnFileTransferSendProgress(unsigned long currentPacketIndex, unsigned long lastPacketIndex);
	void OnFileTransferSendLostPacket(unsigned long lostPackets, unsigned long lostPacketsTotal);
	void OnFileTransferSendEof();
	void OnFileTransferSendSuccess();
	void OnFileTransferSendError();
	void OnFileTransferSendReceiverNew(long mediaId);
	void OnFileTransferSendReceiverCancel(long mediaId);
	void OnFileTransferSendReceiverEnd(long mediaId);
	void OnMovieSendBegin(long mediaId, const wxString &channelName);
	void OnMovieSendEof(long mediaId, const wxString &channelName);
	void OnPoll();
	void OnPollVote();
	void OnPollStats(const PollInfo &info);
	void OnAppSharingBegin();
	void OnAppSharingEnd();
	void OnWhiteboardMessage(const wxString &channel, const wxString &user, const wxString &data);
	void OnCollaborationAutoPlay();
	void OnCollaborationStop();


	// VideoDisplayControl handling method, version 4
	VideoSink *CreateVideoSink(void *extraData = NULL);
	void AddTransmissionDisplayControl(TransmissionDisplayControl *tdc);
	void RemoveTransmissionDisplayControl(TransmissionDisplayControl *tdc);

	// About text
	wxString GetAboutInfo();

	PrivateChatWindowBase *CreatePrivateChatWindow(const wxString &userName);
	
	// This event is triggered when a private chat window is closed
	void OnPrivateChatWindowClose(wxCloseEvent &event);
	// This event is triggered when the session window is closed
	void OnSessionWindowClose(wxCloseEvent &event);

	// GTK+ 2 compatibility
	void OnWindowCreate(wxWindowCreateEvent &WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};

enum{
	ID_SESSION_TIME_TIMER = 500
};

extern "C" 
{
	SKINLITE_API Skin* AllocSkin();
	SKINLITE_API const char* GetSkinName();
}
