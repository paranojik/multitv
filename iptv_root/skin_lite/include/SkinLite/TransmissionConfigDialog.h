/////////////////////////////////////////////////////////////////////////////
// Name:        TransmissionConfigDialog.h
// Purpose:     
// Author:      Vinicius Heineck
// Modified by: 
// Created:     22/07/2008 02:27:45
// RCS-ID:      
// Copyright:   Casa
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _TRANSMISSIONCONFIGDIALOG_H_
#define _TRANSMISSIONCONFIGDIALOG_H_


/*!
 * Includes
 */

////@begin includes
#include <list>
#include "MediaDeviceItem.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
struct DropDownItem;
struct TransmissionProfileItem;
class wxBoxSizer;
////@end forward declarations

typedef std::list<wxString> listOfWxString;

/*!
 * Control identifiers
 */

////@begin control identifiers
enum{
	ID_TRANSMISSIONCONFIGDIALOG = 1000,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEODVC,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEO_INPUTTYPE,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_VIDEO_CODEC,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_CONFERAUDIODVC,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_AUDIO_CONFERINPUTTYPE,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_AUDIO_CODEC,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_CONNECTIONTYPE,
	ID_TRANSMISSIONCONFIGDIALOG_CHOICE_TRANSMISSIONPROFILE,
	ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_HTTPTUNNELING,
	ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED,
	ID_TRANSMISSIONCONFIGDIALOG_BTN_OK,
	ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL,
	ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_ECHOCANCEL,
	ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_NOISECANCEL,
	ID_TRANSMISSIONCONFIGDIALOG_CHKBOX_AUTGAINCTRL,
	ID_TRANSMISSIONCONFIGDIALOG_SLIDER_AUTGAINCTRL,
	ID_TRANSMISSIONCONFIGDIALOG_TXT_AUTGAINCTRL
};

#define SYMBOL_TRANSMISSIONCONFIGDIALOG_STYLE wxCAPTION/*|wxRESIZE_BORDER*/|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_TRANSMISSIONCONFIGDIALOG_TITLE _("Transmission Configuration")
#define SYMBOL_TRANSMISSIONCONFIGDIALOG_IDNAME ID_TRANSMISSIONCONFIGDIALOG
#define SYMBOL_TRANSMISSIONCONFIGDIALOG_SIZE wxSize(500, 400)
#define SYMBOL_TRANSMISSIONCONFIGDIALOG_POSITION wxDefaultPosition
////@end control identifiers

struct DropDownItem
{
	wxString itemName;
	int index;
};

struct TransmissionProfileItem
{
	wxString bitrate;
	wxString bitrateLabel;
};

/** @brief Class that implements the transmission configuration dialog.
*
*/
class TransmissionConfigDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( TransmissionConfigDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    TransmissionConfigDialog();
    TransmissionConfigDialog( wxWindow* parent, wxWindowID id = SYMBOL_TRANSMISSIONCONFIGDIALOG_IDNAME, const wxString& caption = SYMBOL_TRANSMISSIONCONFIGDIALOG_TITLE, const wxPoint& pos = SYMBOL_TRANSMISSIONCONFIGDIALOG_POSITION, const wxSize& size = SYMBOL_TRANSMISSIONCONFIGDIALOG_SIZE, long style = SYMBOL_TRANSMISSIONCONFIGDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TRANSMISSIONCONFIGDIALOG_IDNAME, const wxString& caption = SYMBOL_TRANSMISSIONCONFIGDIALOG_TITLE, const wxPoint& pos = SYMBOL_TRANSMISSIONCONFIGDIALOG_POSITION, const wxSize& size = SYMBOL_TRANSMISSIONCONFIGDIALOG_SIZE, long style = SYMBOL_TRANSMISSIONCONFIGDIALOG_STYLE );

    /// Destructor
    ~TransmissionConfigDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	void AddVideoDevices(const std::list<VideoDeviceItem> &videoDevices, const wxString &defaultChoice = wxEmptyString);
	void ClearVideoDevices();

	void AddVideoInputTypes(const listOfWxString &videoInputTypes);
	void ClearVideoInputTypes();

	void AddVideoCodecs(const listOfWxString &videoCodecs, const wxString &defaultChoice = wxEmptyString);
	void ClearVideoCodecs();

	void AddConferAudioDevices(const std::list<AudioDeviceItem> &conferAudioDevices, const wxString &defaultChoice = wxEmptyString);
	void ClearConferAudioDevices();

	void AddConferAudioInputTypes(const listOfWxString &conferAudioInputTypes);
	void ClearConferAudioInputTypes();

	void AddAudioCodecs(const listOfWxString &audioCodecs, const wxString &defaultChoice = wxEmptyString);
	void ClearAudioCodecs();

	void AddConnectionTypes(const listOfWxString &connectionTypes, const wxString &defaultChoice = wxEmptyString);
	void ClearConnectionTypes();

	void AddTransmissionProfiles(const listOfWxString &transmissionProfiles, const wxString &defaultChoice = wxEmptyString);
	void ClearTransmissionProfiles();

	void DefaultConnectionTypes(const wxString &defaultChoice = wxEmptyString);
	void DefaultTransmissionProfile(const wxString &defaultChoice = wxEmptyString);

	DropDownItem GetVideoDevice(){return m_videoDvcSelected;}
	DropDownItem GetVideoInputType(){return m_videoInputTypeSelected;}
	DropDownItem GetVideoCodec(){return m_videoCodecSelected;}
	DropDownItem GetConferenceAudioDevice(){return m_conferAudioDvcSelected;}
	DropDownItem GetConferenceAudioInputType(){return m_conferAudioInputTypeSelected;}
	DropDownItem GetAudioCodec(){return m_audioCodecSelected;}
	DropDownItem GetConnectionType(){return m_connectionTypeSelected;}
	DropDownItem GetTransmissionProfile(){return m_transmissionProfileSelected;}

////@begin TransmissionConfigDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_ADVANCED
    void OnBtnAdvancedClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_OK
    void OnBtnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_TRANSMISSIONCONFIGDIALOG_BTN_CANCEL
    void OnBtnCancelClick( wxCommandEvent& event );

	void OnChkBoxAutGainCtrlClick(wxCommandEvent &event);
	void OnSliderAutGainCtrlUpdated(wxCommandEvent &event);

	void OnExit(wxCloseEvent &event);

////@end TransmissionConfigDialog event handler declarations

////@begin TransmissionConfigDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TransmissionConfigDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

private:
////@begin TransmissionConfigDialog member variables
    wxStaticBoxSizer* m_videoGroupBoxSizer;
    wxBoxSizer* m_videoDevicesVertSizer;
    wxStaticText* m_lblVideoDevice;
    wxChoice* m_choiceVideoDvc;
    wxBoxSizer* m_videoInputTypeVertSizer;
    wxStaticText* m_lblVideoInputType;
    wxChoice* m_choiceVideoInputType;

	wxBoxSizer* m_videoCodecVertSizer;
    wxStaticText* m_lblVideoCodec;
    wxChoice* m_choiceVideoCodec;

    wxStaticBoxSizer* m_audioGroupBoxSizer;
    wxBoxSizer* m_audioDvcVertSizer;
    wxStaticText* m_lblAudioDvc;
    /*wxStaticText* m_lblAudioConferenceDvc;*/
    wxChoice* m_choiceConferenceAudioDvc;

    wxBoxSizer* m_audioInputTypeVertSizer;
    wxStaticText* m_lblAudioDvcInputType;
    wxChoice* m_choiceAudioConferInputType;

	wxBoxSizer* m_audioCodecVertSizer;
    wxStaticText* m_lblAudioCodec;
    wxChoice* m_choiceAudioCodec;

	wxStaticBoxSizer* m_transmisisonGroupBoxSizer;

	wxStaticBoxSizer* m_connectionGroupBoxSizer;
    wxBoxSizer* m_connectionTypeVertSizer;
    wxStaticText* m_lblConnectionType;
    wxChoice* m_choiceConnectionType;
    
	wxBoxSizer* m_transmissionProfileVertSizer;
    wxStaticText* m_lblTransmissionProfile;
    wxChoice* m_choiceTransmissionProfile;

	wxCheckBox *m_chkBoxHttpTunneling;

	wxBoxSizer* m_dialogMainHorSizer;
    wxStaticText* m_lblCaptureState;
    wxButton* m_btnAdvanced;
    wxBoxSizer* m_cmdHorSizer;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;

	wxArrayString m_choiceVideoDvcStrings;
	wxArrayString m_choiceVideoInputTypeStrings;
	wxArrayString m_choiceVideoCodecStrings;
	wxArrayString m_choiceConferenceAudioDvcStrings;
	wxArrayString m_choiceAudioConferInputTypeStrings;
	wxArrayString m_choiceAudioCodecStrings;
	wxArrayString m_choiceConnectionTypeStrings;
	wxArrayString m_choiceTransmissionProfileStrings;

	DropDownItem m_videoDvcSelected;
	DropDownItem m_videoInputTypeSelected;
	DropDownItem m_videoCodecSelected;
	DropDownItem m_conferAudioDvcSelected;
	DropDownItem m_conferAudioInputTypeSelected;
	DropDownItem m_audioCodecSelected;
	DropDownItem m_connectionTypeSelected;
	DropDownItem m_transmissionProfileSelected;

	wxBoxSizer* m_ctrlAudioHorSizer;
    wxBoxSizer* m_autGainCtrlHorSizer;
	wxCheckBox *m_chkBoxEchoCancel;
	wxCheckBox *m_chkBoxNoiseCancel;
	wxCheckBox *m_chkBoxAutGainCtrl;
	wxSlider *m_sldAutGainCtrl;
	wxTextCtrl *m_txtAutGainCtrl;

	std::list<TransmissionProfileItem> m_transmissionProfileList;

	void ClearAllDropDownItems();
	void GetAllChoices();
	void BitrateToBitrateLabel(const wxString &bitrate, wxString &bitrateLabel);
////@end TransmissionConfigDialog member variables
};

#endif
    // _TransmissionConfigDialog_H_
