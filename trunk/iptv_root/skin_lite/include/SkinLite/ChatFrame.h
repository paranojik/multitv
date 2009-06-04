/////////////////////////////////////////////////////////////////////////////
// Name:        chatframe.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     25/02/2008 15:34:50
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CHATFRAME_H_
#define _CHATFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "Constants.h"
#include "PrivateChatWindowBase.h"
#include "AppInterface.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_CHATFRAME 500
#define SYMBOL_CHATFRAME_STYLE wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT
#define SYMBOL_CHATFRAME_TITLE _("Chat Frame")
#define SYMBOL_CHATFRAME_IDNAME ID_CHATFRAME
#define SYMBOL_CHATFRAME_SIZE wxSize(400, 300)
#define SYMBOL_CHATFRAME_POSITION wxDefaultPosition
////@end control identifiers


/** @brief Class thats implements chat window.
*
*/
class ChatFrame: public wxFrame, public PrivateChatWindowBase
{    
	////@begin ChatFrame member variables
    AppInterface *m_appInterface;
	
	wxBoxSizer* m_mainSizer;
    wxPanel* m_mainPanel;
    wxBoxSizer* m_conversationSizer;
    wxTextCtrl* m_txtConversation;
    wxBoxSizer* m_sendMessageSizer;
    wxTextCtrl* m_txtSendMessage;
    wxButton* m_btnSend;
	
	// Set only for the public chat window, as it is in fact a special case of the private chat.
	bool m_isPublic;
	// Marks initialization.
	bool m_created;

	wxString m_userName;//UserName
	wxString m_userNameStatus;//UserName with Status logged
	wxString m_nameUserChat;//Username in chat
////@end ChatFrame member variables

public:
    /// Constructors
    ChatFrame();
    ChatFrame( wxWindow* parent, wxWindowID id = SYMBOL_CHATFRAME_IDNAME, const wxString& caption = SYMBOL_CHATFRAME_TITLE, const wxString &name = SYMBOL_CHATFRAME_TITLE, const wxPoint& pos = SYMBOL_CHATFRAME_POSITION, const wxSize& size = SYMBOL_CHATFRAME_SIZE, long style = SYMBOL_CHATFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CHATFRAME_IDNAME, const wxString& caption = SYMBOL_CHATFRAME_TITLE, const wxString &name = SYMBOL_CHATFRAME_TITLE, const wxPoint& pos = SYMBOL_CHATFRAME_POSITION, const wxSize& size = SYMBOL_CHATFRAME_SIZE, long style = SYMBOL_CHATFRAME_STYLE );

    /// Destructor
    ~ChatFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	void SetAppInterface(AppInterface *iface);

	void SetUserNameStatus(const wxString &userName);
	// Sets the 'public chat' status flag for this window
	void SetPublicChatFlag(bool isPublic = true);

	// PrivateChatWindowBase implementations
	void OpenWindow();
	void CloseWindow();

	void SetMessage(const wxString &user, const wxString &message);
////@begin ChatFrame event handler declarations
	void OnSendBtnClick(wxCommandEvent &event);
	void OnSendMessageTxtChange(wxCommandEvent &event);
////@end ChatFrame event handler declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    DECLARE_EVENT_TABLE()
};

enum{
	ID_CHAT_FRM_MAIN_PANEL = 501,
	ID_CHAT_FRM_TXT_SENDMESSAGE,
	ID_CHAT_FRM_BTN_SEND,
	ID_CHAT_FRM_TXT_CONVERSATION
};

#endif
    // _CHATFRAME_H_
