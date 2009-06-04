/////////////////////////////////////////////////////////////////////////////
// Name:        chatframe.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     25/02/2008 15:34:50
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
#include "ChatFrame.h"
#include "SessionWindow.h"
#include "AppInterface.h"
#include "SkinLiteConstants.h"
////@end includes

/*!
 * ChatFrame event table definition
 */

BEGIN_EVENT_TABLE( ChatFrame, wxFrame )

////@begin ChatFrame event table entries
EVT_BUTTON(ID_CHAT_FRM_BTN_SEND, ChatFrame::OnSendBtnClick)
EVT_TEXT(ID_CHAT_FRM_TXT_SENDMESSAGE, ChatFrame::OnSendMessageTxtChange)
////@end ChatFrame event table entries

END_EVENT_TABLE()


/** ChatFrame default constructor.
 * 
 */
ChatFrame::ChatFrame()
{
    Init();
}

/** ChatFrame constructor.
 * 
 */
ChatFrame::ChatFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxString &name, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, name, pos, size, style );
}


/** ChatFrame creator.
 * 
 */
bool ChatFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxString &name, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ChatFrame creation
	SetMinSize(SYMBOL_CHATFRAME_SIZE);
    wxFrame::Create( parent, id, caption, pos, size, style, name );
	
	wxIcon icon = wxIcon(S_LITE_ICON_FILENAME, wxBITMAP_TYPE_ICO);
	SetIcon(icon);

	m_nameUserChat = name;
    CreateControls();
    //Centre();

	m_btnSend->Enable(false);
	m_created = true;
////@end ChatFrame creation
    return true;
}


/** ChatFrame destructor.
 * 
 */
ChatFrame::~ChatFrame()
{
////@begin ChatFrame destruction

	//This code change StatusChat.OpenedChatFrame to false
	SessionWindow *parent;
	int index;
	bool found;
	StatusChat statusChat;
	wxString userNameChat;
	
	parent = (SessionWindow *)GetParent();
	userNameChat = m_nameUserChat;
	statusChat.SetOpenedChatFrame(false);
	statusChat.SetNameChatFrame(userNameChat);

	parent->FindStatusChat(userNameChat, index, found);
	if (found == true)
	{
		parent->SetStatusChat(statusChat, index);
	}

	//END This code change StatusChat.OpenedChatFrame to false

////@end ChatFrame destruction
}


/*!
 * Member initialisation
 */

void ChatFrame::Init()
{
////@begin ChatFrame member initialisation
	m_isPublic = false;
	m_created = false;

	m_appInterface = NULL;
    m_mainSizer = NULL;
    m_mainPanel = NULL;
    m_conversationSizer = NULL;
    m_txtConversation = NULL;
    m_sendMessageSizer = NULL;
    m_txtSendMessage = NULL;
    m_btnSend = NULL;
////@end ChatFrame member initialisation
}


/*!
 * Control creation for ChatFrame
 */
void ChatFrame::CreateControls()
{    
////@begin ChatFrame content construction
    ChatFrame* itemFrame1 = this;

    m_mainSizer = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(m_mainSizer);

    m_mainPanel = new wxPanel( itemFrame1, ID_CHAT_FRM_MAIN_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    m_mainSizer->Add(m_mainPanel, 1, wxGROW, 5);

    m_conversationSizer = new wxBoxSizer(wxVERTICAL);
    m_mainPanel->SetSizer(m_conversationSizer);

    m_txtConversation = new wxTextCtrl( m_mainPanel, ID_CHAT_FRM_TXT_CONVERSATION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
    m_conversationSizer->Add(m_txtConversation, 1, wxGROW|wxALL, 5);

    m_sendMessageSizer = new wxBoxSizer(wxHORIZONTAL);
    m_conversationSizer->Add(m_sendMessageSizer, 0, wxGROW|wxALL|wxFIXED_MINSIZE, 5);

    m_txtSendMessage = new wxTextCtrl( m_mainPanel, ID_CHAT_FRM_TXT_SENDMESSAGE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
    m_sendMessageSizer->Add(m_txtSendMessage, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_txtSendMessage->SetFocus();
	m_txtSendMessage->Connect(wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(ChatFrame::OnSendBtnClick),NULL,this);

    m_btnSend = new wxButton( m_mainPanel, ID_CHAT_FRM_BTN_SEND, LABEL_FRM_CHAT_BTN_SEND, wxDefaultPosition, wxDefaultSize, 0 );
    m_sendMessageSizer->Add(m_btnSend, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end ChatFrame content construction
}

/** Sets linkage with application.
* @param[in] appInterface Application interface
*/
void ChatFrame::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
}

//Send a message for the other user on chat
void ChatFrame::OnSendBtnClick(wxCommandEvent &event)
{
	wxString strMessage = m_txtSendMessage->GetValue();
	if (strMessage != wxEmptyString)
	{
		// Can't proceed without m_appInterface
		wxCHECK_RET(m_appInterface, wxT("Aaaaah!! m_appInterface is NULL!! Now what do I do?!"));
		
		const Channel *channel = m_appInterface->GetCurrentChannel();
		wxString user = m_appInterface->GetCurrentUser()->GetNickName();
		
		if(m_isPublic)	// Send public message if this window is the public chat window,
			m_appInterface->SendChannelMessage(channel->GetName(), strMessage);
		else	// Send private message to current "remote user" otherwise.
			m_appInterface->SendPrivateMessage(GetUserName(), strMessage);
		// Echo the message on this window
		SetMessage(user, strMessage);
		
		//Clear txtSendMessage and disable send button
		m_txtSendMessage->Clear();
		m_txtSendMessage->SetFocus();
		m_btnSend->Enable(false);
	}
}

/** Sets the chat text color format.
* @param[in] user. User name thats send message.
* @param[in] message. Message was sent.
*/
void ChatFrame::SetMessage(const wxString &user, const wxString &message)
{
	//UserName Identification
	m_txtConversation->SetDefaultStyle(wxTextAttr(*wxRED));
	m_txtConversation->AppendText(wxT("<") + user + wxT("> "));

	//UserName Message
	m_txtConversation->SetDefaultStyle(wxTextAttr(*wxBLACK));
	m_txtConversation->AppendText(message + wxT("\n"));
}

/** Set the userName with your status.
* @param[in] userName. Puts the user name in window title.
*/
void ChatFrame::SetUserNameStatus(const wxString &userName)
{
	m_userName = userName;
	m_userNameStatus = wxString::Format(LABEL_FRM_CHAT_USER_MESSAGE_SAY, m_userName.c_str());
	SetTitle(wxString::Format(FRM_CHAT_PUBLIC_CHAT_CAPTION, m_userName.c_str()));
}

//Disables sendBtn if txtSendMessage is empty
void ChatFrame::OnSendMessageTxtChange(wxCommandEvent &event)
{
	wxString str = m_txtSendMessage->GetValue();  
	if (str.Len()> 0)
		m_btnSend->Enable(true);
	else
		m_btnSend->Enable(false);
}

/**Sets (or resets) the public chat window flag.
* @param[in] isPublic. Set the chat state for public or private.
*/
void ChatFrame::SetPublicChatFlag(bool isPublic)
{
	m_isPublic = isPublic;
}

/** PrivateChatWindowBase implementations.
* Open the chat window.
*/
void ChatFrame::OpenWindow()
{
	if(!m_created) return;
	if(!IsShown()) Show();
	if(IsIconized())
	{
		Iconize(false);
		RequestUserAttention();
	}
}

/** PrivateChatWindowBase implementations.
* Close the chat window.
*/
void ChatFrame::CloseWindow()
{
	Close();
}

/*!
 * Should we show tooltips?
 */

bool ChatFrame::ShowToolTips()
{
    return true;
}
