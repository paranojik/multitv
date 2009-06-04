/////////////////////////////////////////////////////////////////////////////
// Name:        TransmissionConfigDialog.cpp
// Purpose:     
// Author:      Vinicius Heineck
// Modified by: 
// Created:     22/07/2008 02:27:45
// RCS-ID:      
// Copyright:   Casa
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
////@end includes

#include "TransmissionConfigDialog.h"

////@begin XPM images
////@end XPM images


/*!
 * TransmissionConfigDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TransmissionConfigDialog, wxDialog )


/*!
 * TransmissionConfigDialog event table definition
 */

BEGIN_EVENT_TABLE( TransmissionConfigDialog, wxDialog )

////@begin TransmissionConfigDialog event table entries
    EVT_BUTTON( ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED, TransmissionConfigDialog::OnBtnAdvancedClick )

    EVT_BUTTON( ID_TRANSMISSIONCONFIGDIALOG_BTN_OK, TransmissionConfigDialog::OnBtnOkClick )

    EVT_BUTTON( ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL, TransmissionConfigDialog::OnBtnCancelClick )

	EVT_CHECKBOX( ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_AUTGAINCTRL, TransmissionConfigDialog::OnChkBoxAutGainCtrlClick )
	EVT_SLIDER( ID_TRANSMISSIONCONFIGDIALOG_SLIDER_AUTGAINCTRL, TransmissionConfigDialog::OnSliderAutGainCtrlUpdated )
	
	EVT_CLOSE(TransmissionConfigDialog::OnExit)
////@end TransmissionConfigDialog event table entries

END_EVENT_TABLE()


/** TransmissionConfigDialog default constructor.
 *
 */
TransmissionConfigDialog::TransmissionConfigDialog()
{
    Init();
}

/** TransmissionConfigDialog constructor.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_STYLE.
*/
TransmissionConfigDialog::TransmissionConfigDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}


/** TransmissionConfigDialog creator.
* @param[in] parent. Parent window.
* @param[in] id. Window id. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_IDNAME.
* @param[in] caption. Window caption. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_TITLE.
* @param[in] pos. Window position. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_POSITION.
* @param[in] size. Window size. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_SIZE.
* @param[in] style. Window style. Default SYMBOL_TRANSMISSIONCONFIGDIALOG_STYLE.
*/
bool TransmissionConfigDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TransmissionConfigDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    /*if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }*/
    
	SetAffirmativeId(ID_TRANSMISSIONCONFIGDIALOG_BTN_OK);
	SetEscapeId(ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL);

////@end TransmissionConfigDialog creation
    return true;
}


/** TransmissionConfigDialog destructor.
 * 
 */
TransmissionConfigDialog::~TransmissionConfigDialog()
{
////@begin TransmissionConfigDialog destruction
////@end TransmissionConfigDialog destruction
}


/*!
 * Member initialisation
 */

void TransmissionConfigDialog::Init()
{
////@begin TransmissionConfigDialog member initialisation
    m_videoGroupBoxSizer = NULL;
    m_videoDevicesVertSizer = NULL;
    m_lblVideoDevice = NULL;
    m_choiceVideoDvc = NULL;
    m_videoInputTypeVertSizer = NULL;
    m_lblVideoInputType = NULL;
    m_choiceVideoInputType = NULL;
    m_audioGroupBoxSizer = NULL;
    m_audioDvcVertSizer = NULL;
    m_lblAudioDvc = NULL;
    m_choiceConferenceAudioDvc = NULL;
    m_lblAudioCodec = NULL;
	m_choiceConnectionType = NULL;
    m_audioInputTypeVertSizer = NULL;
    m_lblAudioDvcInputType = NULL;
    m_choiceAudioConferInputType = NULL;
    m_choiceAudioCodec = NULL;
    m_dialogMainHorSizer = NULL;
    m_lblCaptureState = NULL;
    m_btnAdvanced = NULL;
    m_cmdHorSizer = NULL;
    m_btnOk = NULL;
    m_btnCancel = NULL;

	ClearAllDropDownItems();
////@end TransmissionConfigDialog member initialisation
}

/** Clear all controls.
* Private method.
*/
void TransmissionConfigDialog::ClearAllDropDownItems()
{
	m_videoDvcSelected.itemName = wxEmptyString;
	m_videoDvcSelected.index = 0;

	m_videoInputTypeSelected.itemName = wxEmptyString;
	m_videoInputTypeSelected.index = 0;

	m_videoCodecSelected.itemName = wxEmptyString;
	m_videoCodecSelected.index = 0;

	m_conferAudioDvcSelected.itemName = wxEmptyString;
	m_conferAudioDvcSelected.index = 0;

	m_conferAudioInputTypeSelected.itemName = wxEmptyString;
	m_conferAudioInputTypeSelected.index = 0;

	m_audioCodecSelected.itemName = wxEmptyString;
	m_audioCodecSelected.index = 0;

	m_connectionTypeSelected.itemName = wxEmptyString;
	m_connectionTypeSelected.index = 0;

	m_transmissionProfileSelected.itemName = wxEmptyString;
	m_transmissionProfileSelected.index = 0;
}

/*!
 * Control creation for TransmissionConfigDialog
 */
void TransmissionConfigDialog::CreateControls()
{    
////@begin TransmissionConfigDialog content construction
    TransmissionConfigDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

/*----------------------------------TRANSMISSION----------------------------*/
	wxStaticBox* stBoxTransmission = new wxStaticBox(itemDialog1, wxID_ANY, _("Transmission"));
    m_transmisisonGroupBoxSizer = new wxStaticBoxSizer(stBoxTransmission, wxHORIZONTAL);
    itemBoxSizer2->Add(m_transmisisonGroupBoxSizer, 0, wxGROW|wxALL, 5);

	m_connectionTypeVertSizer = new wxBoxSizer(wxVERTICAL);
    m_transmisisonGroupBoxSizer->Add(m_connectionTypeVertSizer, 1, wxGROW|wxALL, 5);

    m_lblConnectionType = new wxStaticText( itemDialog1, wxID_STATIC, _("Connection Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_connectionTypeVertSizer->Add(m_lblConnectionType, 0, wxALIGN_LEFT|wxALL, 5);

	m_choiceConnectionType = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_CONNECTIONTYPE, wxDefaultPosition, wxDefaultSize, m_choiceConnectionTypeStrings, 0 );
    m_connectionTypeVertSizer->Add(m_choiceConnectionType, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	m_transmissionProfileVertSizer = new wxBoxSizer(wxVERTICAL);
	m_transmisisonGroupBoxSizer->Add(m_transmissionProfileVertSizer, 1, wxGROW|wxALL, 5);

    m_lblTransmissionProfile = new wxStaticText( itemDialog1, wxID_STATIC, _("Transmission Profile:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_transmissionProfileVertSizer->Add(m_lblTransmissionProfile, 0, wxALIGN_LEFT|wxALL, 5);

	m_choiceTransmissionProfile = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_TRANSMISSIONPROFILE, wxDefaultPosition, wxDefaultSize, m_choiceTransmissionProfileStrings, 0 );
    m_transmissionProfileVertSizer->Add(m_choiceTransmissionProfile, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	m_chkBoxHttpTunneling = new wxCheckBox(itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_HTTPTUNNELING, _("Activate HTTP Tunnel"));
	m_transmisisonGroupBoxSizer->Add(m_chkBoxHttpTunneling, 1, wxGROW|wxALL, 5);

//-------------------------------------------
//-------------------------------------------
	m_connectionTypeVertSizer->Show(false);
	m_chkBoxHttpTunneling->Hide();
//-------------------------------------------
//-------------------------------------------

/*--------------------------------END TRANSMISSION--------------------------*/

/*----------------------------------Video Device----------------------------*/
    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Video"));
    m_videoGroupBoxSizer = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxHORIZONTAL);
    itemBoxSizer2->Add(m_videoGroupBoxSizer, 0, wxGROW|wxALL, 5);

    m_videoDevicesVertSizer = new wxBoxSizer(wxVERTICAL);

    m_lblVideoDevice = new wxStaticText( itemDialog1, wxID_STATIC, _("Video Device:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_videoDevicesVertSizer->Add(m_lblVideoDevice, 0, wxALIGN_LEFT|wxALL, 5);

    m_choiceVideoDvc = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEODVC, wxDefaultPosition, wxDefaultSize, m_choiceVideoDvcStrings, 0 );
    m_videoDevicesVertSizer->Add(m_choiceVideoDvc, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_videoInputTypeVertSizer = new wxBoxSizer(wxVERTICAL);
    m_videoGroupBoxSizer->Add(m_videoInputTypeVertSizer, 1, wxGROW|wxALL, 5);

    m_lblVideoInputType = new wxStaticText( itemDialog1, wxID_STATIC, _("Input Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_videoInputTypeVertSizer->Add(m_lblVideoInputType, 0, wxALIGN_LEFT|wxALL, 5);

    m_choiceVideoInputType = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEO_INPUTTYPE, wxDefaultPosition, wxDefaultSize, m_choiceVideoInputTypeStrings, 0 );
    m_videoInputTypeVertSizer->Add(m_choiceVideoInputType, 0, wxALIGN_LEFT|wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 5);

//-------------------------------------------
//-------------------------------------------
	m_videoInputTypeVertSizer->Show(false);
//-------------------------------------------
//-------------------------------------------

	m_videoCodecVertSizer = new wxBoxSizer(wxVERTICAL);
    
	m_lblVideoCodec = new wxStaticText( itemDialog1, wxID_STATIC, _("Codec:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_videoCodecVertSizer->Add(m_lblVideoCodec, 0, wxALIGN_LEFT|wxALL, 5);

    m_choiceVideoCodec = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEO_CODEC, wxDefaultPosition, wxDefaultSize, m_choiceVideoCodecStrings, 0 );
	m_choiceVideoCodec->SetMinSize(wxSize(96, -1));
    m_videoCodecVertSizer->Add(m_choiceVideoCodec, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

	m_videoGroupBoxSizer->Add(m_videoDevicesVertSizer, 1, wxGROW|wxALL, 5);
	m_videoGroupBoxSizer->Add(m_videoCodecVertSizer, 0, wxGROW|wxALL, 5);
	m_videoGroupBoxSizer->Layout();
/*--------------------------------END Video Device--------------------------*/

/*----------------------------------Audio Device----------------------------*/
    wxStaticBox* itemStaticBoxSizer10Static = new wxStaticBox(itemDialog1, wxID_ANY, _("Audio"));
    m_audioGroupBoxSizer = new wxStaticBoxSizer(itemStaticBoxSizer10Static, wxHORIZONTAL);
    itemBoxSizer2->Add(m_audioGroupBoxSizer, 0, wxGROW|wxALL, 5);

    m_audioDvcVertSizer = new wxBoxSizer(wxVERTICAL);
    m_audioGroupBoxSizer->Add(m_audioDvcVertSizer, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_lblAudioDvc = new wxStaticText( itemDialog1, wxID_STATIC, _("Audio Devices:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_audioDvcVertSizer->Add(m_lblAudioDvc, 0, wxALIGN_LEFT|wxALL, 5);

    m_choiceConferenceAudioDvc = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_CONFERAUDIODVC, wxDefaultPosition, wxDefaultSize, m_choiceConferenceAudioDvcStrings, 0 );
    m_audioDvcVertSizer->Add(m_choiceConferenceAudioDvc, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_audioInputTypeVertSizer = new wxBoxSizer(wxVERTICAL);
    m_audioGroupBoxSizer->Add(m_audioInputTypeVertSizer, 0, wxGROW|wxALL, 5);

    m_lblAudioDvcInputType = new wxStaticText( itemDialog1, wxID_STATIC, _("Input Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_audioInputTypeVertSizer->Add(m_lblAudioDvcInputType, 0, wxALIGN_LEFT|wxALL, 5);

    m_choiceAudioConferInputType = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_AUDIO_CONFERINPUTTYPE, wxDefaultPosition, wxDefaultSize, m_choiceAudioConferInputTypeStrings, 0 );
    m_audioInputTypeVertSizer->Add(m_choiceAudioConferInputType, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

//-------------------------------------------
//-------------------------------------------
	m_audioInputTypeVertSizer->Show(false);
//-------------------------------------------
//-------------------------------------------

	m_audioCodecVertSizer = new wxBoxSizer(wxVERTICAL);
    m_audioGroupBoxSizer->Add(m_audioCodecVertSizer, 0, wxGROW|wxALL, 5);

    m_lblAudioCodec = new wxStaticText( itemDialog1, wxID_STATIC, _("Codec:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_audioCodecVertSizer->Add(m_lblAudioCodec, 0, wxALIGN_LEFT|wxALL, 5);

	m_choiceAudioCodec = new wxChoice( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHOICE_AUDIO_CODEC, wxDefaultPosition, wxDefaultSize, m_choiceAudioCodecStrings, 0 );
	m_choiceAudioCodec->SetMinSize(wxSize(96, -1));
    m_audioCodecVertSizer->Add(m_choiceAudioCodec, 0, wxALIGN_CENTER_HORIZONTAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

//-------------------ECHO and NOISE CANCELLATION-----------------------------------
	m_ctrlAudioHorSizer = new wxBoxSizer(wxHORIZONTAL);
	m_audioDvcVertSizer->Add(m_ctrlAudioHorSizer, 0, wxALIGN_LEFT|wxTOP|wxRIGHT|wxBOTTOM, 5);

	m_chkBoxEchoCancel = new wxCheckBox(itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_ECHOCANCEL, _("Echo Cancellation"));
	m_chkBoxEchoCancel->SetValue(false);
    m_ctrlAudioHorSizer->Add(m_chkBoxEchoCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_chkBoxNoiseCancel = new wxCheckBox(itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_NOISECANCEL, _("Noise Cancellation"));
	m_chkBoxNoiseCancel->SetValue(false);
    m_ctrlAudioHorSizer->Add(m_chkBoxNoiseCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
//---------------END ECHO and NOISE CANCELLATION-----------------------------------

//------------------------Automatic gain control-----------------------------------
    m_autGainCtrlHorSizer = new wxBoxSizer(wxHORIZONTAL);
	m_audioDvcVertSizer->Add(m_autGainCtrlHorSizer, 0, wxALIGN_LEFT|wxTOP|wxRIGHT|wxBOTTOM, 5);

	m_chkBoxAutGainCtrl = new wxCheckBox(itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_AUTGAINCTRL, _("Automatic gain control:"));
	m_chkBoxAutGainCtrl->SetValue(false);
    m_autGainCtrlHorSizer->Add(m_chkBoxAutGainCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	
	m_sldAutGainCtrl = new wxSlider( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_SLIDER_AUTGAINCTRL, 0, 0, 100, wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL );
	m_autGainCtrlHorSizer->Add(m_sldAutGainCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_sldAutGainCtrl->Enable(m_chkBoxAutGainCtrl->IsChecked());

	m_txtAutGainCtrl = new wxTextCtrl( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_TXT_AUTGAINCTRL, wxT("0"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY );
	m_autGainCtrlHorSizer->Add(m_txtAutGainCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	m_txtAutGainCtrl->Enable(m_chkBoxAutGainCtrl->IsChecked());
//--------------------END Automatic gain control-----------------------------------

/*--------------------------------END Audio Device--------------------------*/

/*------------Capture Status and Advanced Button Area-----------------------*/
    m_dialogMainHorSizer = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(m_dialogMainHorSizer, 0, wxGROW|wxALL, 5);

    m_lblCaptureState = new wxStaticText( itemDialog1, wxID_STATIC, _("Capture State:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_dialogMainHorSizer->Add(m_lblCaptureState, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnAdvanced = new wxButton( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED, _("Advanced"), wxDefaultPosition, wxDefaultSize, 0 );
    m_dialogMainHorSizer->Add(m_btnAdvanced, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	m_dialogMainHorSizer->Show(false);
/*------------END Capture Status and Advanced Button Area---------------------*/

/*-----------------------------------Cmd buttons----------------------------*/
    m_cmdHorSizer = new wxBoxSizer(wxHORIZONTAL);
	itemBoxSizer2->Add(m_cmdHorSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 20);

    m_btnOk = new wxButton( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_BTN_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cmdHorSizer->Add(m_btnOk, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_btnCancel = new wxButton( itemDialog1, ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_cmdHorSizer->Add(m_btnCancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
/*---------------------------------END Cmd buttons--------------------------*/

	Fit();

////@end TransmissionConfigDialog content construction
}


/*!
 * Should we show tooltips?
 */

bool TransmissionConfigDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TransmissionConfigDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TransmissionConfigDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TransmissionConfigDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TransmissionConfigDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TransmissionConfigDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TransmissionConfigDialog icon retrieval
}

/** Shows in connection types dropdown, a default items.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::DefaultConnectionTypes(const wxString &defaultChoice)
{
	ClearConnectionTypes();

	m_choiceConnectionTypeStrings.Add(_("Dial Up"));
	m_choiceConnectionTypeStrings.Add(_("Satellite"));
	m_choiceConnectionTypeStrings.Add(_("Internet Broadband"));

	m_choiceConnectionType->Append(m_choiceConnectionTypeStrings);

	if (defaultChoice == wxEmptyString)
		m_choiceConnectionType->SetSelection(2);
	else
		m_choiceConnectionType->SetStringSelection(defaultChoice);
}

/** Shows in transmission profile dropdown, a default items.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::DefaultTransmissionProfile(const wxString &defaultChoice)
{
	ClearTransmissionProfiles();

	m_choiceTransmissionProfileStrings.Add(_("33.6 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("64 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("128 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("256 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("384 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("512 kbps"));
	m_choiceTransmissionProfileStrings.Add(_("1.0 Mbps"));
	
	m_choiceTransmissionProfile->Append(m_choiceTransmissionProfileStrings);
	
	//m_choiceTransmissionProfile->SetSelection(2);
	if (defaultChoice == wxEmptyString)
		m_choiceTransmissionProfile->SetSelection(2);
	else
		m_choiceTransmissionProfile->SetStringSelection(defaultChoice);
}

/** Adds video devices on the respective control.
* @param[in] videoDevices. List containing the video devices names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddVideoDevices(const std::list<VideoDeviceItem> &videoDevices, const wxString &defaultChoice)
{
	ClearVideoDevices();

	std::list<VideoDeviceItem>::const_iterator it;

	for(it = videoDevices.begin();it != videoDevices.end();it++)
	{
		// m_choiceVideoDvcStrings.Add(*it);
		int n = m_choiceVideoDvc->Append(it->name);
		m_choiceVideoDvc->SetClientData(n, (void *)&(*it));
	}

	// m_choiceVideoDvc->Append(m_choiceVideoDvcStrings);
	/*if (!m_choiceVideoDvc->IsEmpty())
		m_choiceVideoDvc->SetSelection(0);*/

	if (defaultChoice == wxEmptyString)
		m_choiceVideoDvc->SetSelection(0);
	else
		m_choiceVideoDvc->SetStringSelection(defaultChoice);

}

/** Clear the video devices dropdown.
*
*/
void TransmissionConfigDialog::ClearVideoDevices()
{
	m_choiceVideoDvcStrings.Clear();
	m_choiceVideoDvc->Clear();
	
	m_videoDvcSelected.itemName = wxEmptyString;
	m_videoDvcSelected.index = 0;
}

/** Adds video input types on the respective control.
* @param[in] videoInputTypes. List containing the video input types names.
*/
void TransmissionConfigDialog::AddVideoInputTypes(const listOfWxString &videoInputTypes)
{
	ClearVideoInputTypes();

	listOfWxString::const_iterator it;

	for(it = videoInputTypes.begin(); it != videoInputTypes.end(); it++)
		m_choiceVideoInputTypeStrings.Add(*it);

	m_choiceVideoInputType->Append(m_choiceVideoInputTypeStrings);
	if (!m_choiceVideoInputType->IsEmpty())
		m_choiceVideoInputType->SetSelection(0);
}

/** Clear the video input types dropdown.
*
*/
void TransmissionConfigDialog::ClearVideoInputTypes()
{
	m_choiceVideoInputTypeStrings.Clear();
	m_choiceVideoInputType->Clear();
	
	m_videoInputTypeSelected.itemName = wxEmptyString;
	m_videoInputTypeSelected.index = 0;
}

/** Adds video codecs on the respective control.
* @param[in] videoCodecs. List containing the video codecs names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddVideoCodecs(const listOfWxString &videoCodecs, const wxString &defaultChoice)
{
	ClearVideoCodecs();

	listOfWxString::const_iterator it;

	for(it = videoCodecs.begin(); it != videoCodecs.end(); it++)
	 	m_choiceVideoCodecStrings.Add(*it);

	m_choiceVideoCodec->Append(m_choiceVideoCodecStrings);
	/*if (!m_choiceVideoCodec->IsEmpty())
		m_choiceVideoCodec->SetSelection(0);*/

	if (defaultChoice == wxEmptyString)
		m_choiceVideoCodec->SetSelection(0);
	else
		m_choiceVideoCodec->SetStringSelection(defaultChoice);
}

/** Clear the video codecs dropdown.
*
*/
void TransmissionConfigDialog::ClearVideoCodecs()
{
	m_choiceVideoCodecStrings.Clear();
	m_choiceVideoCodec->Clear();

	m_videoCodecSelected.itemName = wxEmptyString;
	m_videoCodecSelected.index = 0;
}

/** Adds audio devices on the respective control.
* @param[in] conferAudioDevices. List containing the audio devices names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddConferAudioDevices(const std::list<AudioDeviceItem> &conferAudioDevices, const wxString &defaultChoice)
{
	ClearConferAudioDevices();

	std::list<AudioDeviceItem>::const_iterator it;

	for(it = conferAudioDevices.begin(); it != conferAudioDevices.end(); it++)
	{
		// m_choiceConferenceAudioDvcStrings.Add(*it);
		int n = m_choiceConferenceAudioDvc->Append(it->name);
		m_choiceConferenceAudioDvc->SetClientData(n, (void *)&(*it));
	}
	//m_choiceConferenceAudioDvc->Append(m_choiceConferenceAudioDvcStrings);
	/*if (!m_choiceConferenceAudioDvc->IsEmpty())
		m_choiceConferenceAudioDvc->SetSelection(0);*/
	if (defaultChoice == wxEmptyString)
		m_choiceConferenceAudioDvc->SetSelection(0);
	else
		m_choiceConferenceAudioDvc->SetStringSelection(defaultChoice);
}

/** Clear the audio devices dropdown.
*
*/
void TransmissionConfigDialog::ClearConferAudioDevices()
{
	m_choiceConferenceAudioDvcStrings.Clear();
	m_choiceConferenceAudioDvc->Clear();
	
	m_conferAudioDvcSelected.itemName = wxEmptyString;
	m_conferAudioDvcSelected.index = 0;
}

/** Adds audio input types on the respective control.
* @param[in] conferAudioInputTypes. List containing the audio input types names.
*/
void TransmissionConfigDialog::AddConferAudioInputTypes(const listOfWxString &conferAudioInputTypes)
{
	ClearConferAudioInputTypes();

	listOfWxString::const_iterator it;

	for(it = conferAudioInputTypes.begin(); it != conferAudioInputTypes.end(); it++)
		m_choiceAudioConferInputTypeStrings.Add(*it);

	m_choiceAudioConferInputType->Append(m_choiceAudioConferInputTypeStrings);
	if (!m_choiceAudioConferInputType->IsEmpty())
		m_choiceAudioConferInputType->SetSelection(0);
}

/** Clear the audio input types dropdown.
*
*/
void TransmissionConfigDialog::ClearConferAudioInputTypes()
{
	m_choiceAudioConferInputTypeStrings.Clear();
	m_choiceAudioConferInputType->Clear();

	m_conferAudioInputTypeSelected.itemName = wxEmptyString;
	m_conferAudioInputTypeSelected.index = 0;
}

/** Adds audio codecs on the respective control.
* @param[in] audioCodecs. List containing the audio codecs names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddAudioCodecs(const listOfWxString &audioCodecs, const wxString &defaultChoice)
{
	ClearAudioCodecs();

	listOfWxString::const_iterator it;

	for(it = audioCodecs.begin(); it != audioCodecs.end(); it++)
		m_choiceAudioCodecStrings.Add(*it);

	m_choiceAudioCodec->Append(m_choiceAudioCodecStrings);
	/*if (!m_choiceAudioCodec->IsEmpty())
		m_choiceAudioCodec->SetSelection(0);*/
	if (defaultChoice == wxEmptyString)
		m_choiceAudioCodec->SetSelection(0);
	else
	{
		if (!m_choiceAudioCodec->SetStringSelection(defaultChoice))
			m_choiceAudioCodec->SetSelection(0);
	}
}

/** Clear the audio codecs dropdown.
*
*/
void TransmissionConfigDialog::ClearAudioCodecs()
{
	m_choiceAudioCodecStrings.Clear();
	m_choiceAudioCodec->Clear();
	
	m_audioCodecSelected.itemName = wxEmptyString;
	m_audioCodecSelected.index = 0;
}

/** Adds connection types on the respective control.
* @param[in] connectionTypes. List containing the connection types names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddConnectionTypes(const listOfWxString &connectionTypes, const wxString &defaultChoice)
{
	ClearConnectionTypes();

	listOfWxString::const_iterator it;

	for(it = connectionTypes.begin(); it != connectionTypes.end(); it++)
		m_choiceConnectionTypeStrings.Add(*it);

	m_choiceConnectionType->Append(m_choiceConnectionTypeStrings);
	/*if (!m_choiceConnectionType->IsEmpty())
		m_choiceConnectionType->SetSelection(0);*/
	if (defaultChoice == wxEmptyString)
		m_choiceConnectionType->SetSelection(0);
	else
		m_choiceConnectionType->SetStringSelection(defaultChoice);
}

/** Clear the connection types dropdown.
*
*/
void TransmissionConfigDialog::ClearConnectionTypes()
{
	m_choiceConnectionTypeStrings.Clear();
	m_choiceConnectionType->Clear();
	
	m_connectionTypeSelected.itemName = wxEmptyString;
	m_connectionTypeSelected.index = 0;
}

/** Adds transmission profiles on the respective control.
* @param[in] trsnmissionProfiles. List containing the trsnmission profiles names.
* @param[in] defaultChoice. Select the item to be marked as default.
*/
void TransmissionConfigDialog::AddTransmissionProfiles(const listOfWxString &transmissionProfiles, const wxString &defaultChoice)
{
	ClearTransmissionProfiles();

	listOfWxString::const_iterator it;
	std::list<TransmissionProfileItem>::iterator transmissionProfileItemIt;

	for(it = transmissionProfiles.begin();it != transmissionProfiles.end();it++)
	{
		TransmissionProfileItem transmissionProfileItem;

		transmissionProfileItem.bitrate = *it;
		BitrateToBitrateLabel(transmissionProfileItem.bitrate, transmissionProfileItem.bitrateLabel);

		m_transmissionProfileList.push_back(transmissionProfileItem);

		transmissionProfileItemIt = m_transmissionProfileList.end();
		transmissionProfileItemIt--;
		int n = m_choiceTransmissionProfile->Append(transmissionProfileItemIt->bitrateLabel);
		m_choiceTransmissionProfile->SetClientData(n, (void *)&(*transmissionProfileItemIt));
	}

	wxString defaultChoiceLabel;
	BitrateToBitrateLabel(defaultChoice, defaultChoiceLabel);

	if (defaultChoice == wxEmptyString)
		m_choiceTransmissionProfile->SetSelection(0);
	else
		m_choiceTransmissionProfile->SetStringSelection(defaultChoiceLabel);
}

/** Clear the transmission profiles dropdown.
*
*/
void TransmissionConfigDialog::ClearTransmissionProfiles()
{
	m_choiceTransmissionProfileStrings.Clear();
	m_choiceTransmissionProfile->Clear();
	m_transmissionProfileList.clear();

	m_transmissionProfileSelected.itemName = wxEmptyString;
	m_transmissionProfileSelected.index = 0;

}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED
 */

void TransmissionConfigDialog::OnBtnAdvancedClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED in TransmissionConfigDialog.
    // Before editing this code, remove the block markers.
    wxMessageBox(_("To be implemented Advanced button"));
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED in TransmissionConfigDialog. 
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_OK
 */

void TransmissionConfigDialog::OnBtnOkClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_OK in TransmissionConfigDialog.
    // Before editing this code, remove the block markers.
    
	//wxMessageBox(_("To be implemented OK Button"));

	GetAllChoices();

	EndModal(ID_TRANSMISSIONCONFIGDIALOG_BTN_OK);
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_OK in TransmissionConfigDialog. 
}

/** Gets the user choices.
* When the OK button is clicked, use this method to get the choices. 
* Private method.
*/
void TransmissionConfigDialog::GetAllChoices()
{
	if (m_choiceVideoDvc->GetSelection() != wxNOT_FOUND)
	{
		VideoDeviceItem *videoItem = (VideoDeviceItem *)m_choiceVideoDvc->GetClientData(m_choiceVideoDvc->GetSelection());
		if (videoItem)
		{
			m_videoDvcSelected.itemName = videoItem->name;
			m_videoDvcSelected.index = videoItem->index;
		}
	}

	m_videoInputTypeSelected.itemName = m_choiceVideoInputType->GetStringSelection();
	m_videoInputTypeSelected.index = m_choiceVideoInputType->GetSelection();

	m_videoCodecSelected.itemName = m_choiceVideoCodec->GetStringSelection();
	m_videoCodecSelected.index = m_choiceVideoCodec->GetSelection();

	if (m_choiceConferenceAudioDvc->GetSelection() != wxNOT_FOUND)
	{
		AudioDeviceItem *audioItem = (AudioDeviceItem *)m_choiceConferenceAudioDvc->GetClientData(m_choiceConferenceAudioDvc->GetSelection());
		if (audioItem)
		{
			m_conferAudioDvcSelected.itemName = audioItem->name;
			m_conferAudioDvcSelected.index = audioItem->index;
		}
	}

	m_conferAudioInputTypeSelected.itemName = m_choiceAudioConferInputType->GetStringSelection();
	m_conferAudioInputTypeSelected.index = m_choiceAudioConferInputType->GetSelection();

	m_audioCodecSelected.itemName = m_choiceAudioCodec->GetStringSelection();
	m_audioCodecSelected.index = m_choiceAudioCodec->GetSelection();

	m_connectionTypeSelected.itemName = m_choiceConnectionType->GetStringSelection();
	m_connectionTypeSelected.index = m_choiceConnectionType->GetSelection();


	if (m_choiceTransmissionProfile->GetSelection() != wxNOT_FOUND)
	{
		TransmissionProfileItem *transmissionProfileItem = (TransmissionProfileItem *)(m_choiceTransmissionProfile->GetClientData(m_choiceTransmissionProfile->GetSelection()));
		if (transmissionProfileItem)
		{
			m_transmissionProfileSelected.itemName = transmissionProfileItem->bitrate;
			m_transmissionProfileSelected.index = m_choiceTransmissionProfile->GetSelection();
		}
	}
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL
 */

void TransmissionConfigDialog::OnBtnCancelClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL in TransmissionConfigDialog.
    // Before editing this code, remove the block markers.

    //wxMessageBox(_("To be Implemented Cancel Buton"));

	EndModal(ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL);

////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL in TransmissionConfigDialog. 
}

void TransmissionConfigDialog::OnChkBoxAutGainCtrlClick(wxCommandEvent &event)
{
	m_sldAutGainCtrl->Enable(event.IsChecked());
	m_txtAutGainCtrl->Enable(event.IsChecked());
}

void TransmissionConfigDialog::OnSliderAutGainCtrlUpdated(wxCommandEvent &event)
{
	m_txtAutGainCtrl->SetValue(wxString::Format(wxT("%d"), event.GetInt()));
}

void TransmissionConfigDialog::OnExit(wxCloseEvent &event)
{
	EndModal(ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL);
}

/** Receive the bitrate on server format ("128k") and devolve your respective string in bps, kbps or Mbps.
* @param[in] bitrate. Bitrate on format described above.
* @param[out] bitrateLabel. String containing the bitrate on format like this example, 128 kbps.
* Private method.
*/
void TransmissionConfigDialog::BitrateToBitrateLabel(const wxString &bitrate, wxString &bitrateLabel)
{
	long size;

	bitrate.ToLong(&size);
	// show in Mbps
	if((size/1024) > 0)
	{
		bitrateLabel.Printf(wxT("%d Mbps"), size/1024);
		return;
	}

	// show in kbps
	if (size > 0)
	{
		bitrateLabel.Printf(wxT("%d kbps"), size);
		return;
	}

	// show in bps
	bitrateLabel.Printf(wxT("%d bps"), size);
	return;
}
