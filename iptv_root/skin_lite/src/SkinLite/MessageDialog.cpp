/////////////////////////////////////////////////////////////////////////////
// Name:        MessageDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     18/03/2008 10:59:54
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


#include "MessageDialog.h"
#include "SkinLiteConstants.h"


/*!
 * MessageDialog event table definition
 */
BEGIN_EVENT_TABLE( MessageDialog, wxDialog )
	EVT_CLOSE(MessageDialog::OnClose)
	EVT_BUTTON( ID_MESSAGEDIALOG_BTN_NO, MessageDialog::OnBtnNoClick )
	EVT_BUTTON( ID_MESSAGEDIALOG_BTN_YES, MessageDialog::OnBtnYesClick )
END_EVENT_TABLE()


/** MessageDialog default constructor.
 * 
 */
MessageDialog::MessageDialog()
{
    Init();
}

/** MessageDialog constructor.
 * 
 */
MessageDialog::MessageDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/** MessageDialog constructor.
 * @param[in] parent. Parent window.
 * @param[in] title. Title of message dialog.
 * @param[in] label. Label of message dialog.
 * On ShowModal() return ID_MESSAGEDIALOG_BTN_YES or ID_MESSAGEDIALOG_BTN_NO, if yes button clicked or no button clicked, respectively.
 */
MessageDialog::MessageDialog(wxWindow *parent, const wxString &title, const wxString &label)
{
	Init();
	m_label = label;
	Create(parent, SYMBOL_MESSAGEDIALOG_IDNAME, title, wxDefaultPosition, wxDefaultSize, SYMBOL_MESSAGEDIALOG_STYLE);
	
}


/*!
 * MessageDialog creator
 */
bool MessageDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}


/** MessageDialog destructor.
 * 
 */
MessageDialog::~MessageDialog()
{
}


/*!
 * Member initialisation
 */
void MessageDialog::Init()
{
    m_labelSizer = NULL;
    m_lblLabel = NULL;
    m_ctrlSizer = NULL;
    m_btnYes = NULL;
    m_btnNo = NULL;
}


/*!
 * Control creation for MessageDialog
 */
void MessageDialog::CreateControls()
{
    MessageDialog* itemDialog1 = this;

    m_labelSizer = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(m_labelSizer);

    m_lblLabel = new wxStaticText( itemDialog1, wxID_STATIC, m_label, wxDefaultPosition, wxDefaultSize, 0 );
    m_labelSizer->Add(m_lblLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxTOP, 30);

    m_ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
    m_labelSizer->Add(m_ctrlSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

    m_btnYes = new wxButton( itemDialog1, ID_MESSAGEDIALOG_BTN_YES, BUTTON_YES_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnYes, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnNo = new wxButton( itemDialog1, ID_MESSAGEDIALOG_BTN_NO, BUTTON_NO_LABEL, wxDefaultPosition, wxDefaultSize, 0 );
    m_ctrlSizer->Add(m_btnNo, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	SetAffirmativeId(ID_MESSAGEDIALOG_BTN_YES);
	SetEscapeId(ID_MESSAGEDIALOG_BTN_NO);
}

void MessageDialog::OnBtnNoClick(wxCommandEvent& event)
{
	EndModal(ID_MESSAGEDIALOG_BTN_NO);
}

void MessageDialog::OnBtnYesClick(wxCommandEvent& event)
{
	EndModal(ID_MESSAGEDIALOG_BTN_YES);
}

void MessageDialog::OnClose(wxCloseEvent &event)
{
	EndModal(ID_MESSAGEDIALOG_BTN_NO);
}
