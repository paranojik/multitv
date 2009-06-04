#ifndef _UNICASTCONFIGDIALOG_H_
#define _UNICASTCONFIGDIALOG_H_


/*!
 * Includes
 */
#include "AppConfig.h"
#include "Constants.h"
#include "AppInterface.h"
#include "SkinLiteConstants.h"

/*!
 * Forward declarations
 */
class wxBoxSizer;

/*!
 * Control identifiers
 */
#define ID_UNICASTDIALOG 10000
#define ID_TXT_SERVER_NAME 10001
#define ID_TXT_SERVER_PASSWORD 10002
#define ID_TXT_USER_NAME 10003
#define ID_TXT_USER_PASSWORD 10004
#define ID_BTN_OK 10005
#define ID_BTN_CANCEL 10006
#define SYMBOL_UNICASTDIALOG_STYLE wxDEFAULT_DIALOG_STYLE
#define SYMBOL_UNICASTDIALOG_TITLE _("Unicast Settings")
#define SYMBOL_UNICASTDIALOG_IDNAME ID_UNICASTDIALOG
#define SYMBOL_UNICASTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_UNICASTDIALOG_POSITION wxDefaultPosition


/** @brief Dialog to begin the conference in unicast mode. 
*
*/
class UnicastConfigDialog: public wxDialog
{
	AppInterface* m_appInterface;
	AppConfig m_config;

	wxBoxSizer* m_dialogSizer;
    wxStaticBox* m_serverBox;
    wxStaticBoxSizer* m_serverSizer;
    wxStaticText* m_lblServerName;
    wxTextCtrl* m_txtServerName;
    wxStaticText* m_lblServerPassword;
    wxTextCtrl* m_txtServerPassword;
    wxStaticBox* m_userBox;
    wxStaticBoxSizer* m_userSizer;
    wxStaticText* m_lblUserName;
    wxTextCtrl* m_txtUserName;
    wxStaticText* m_lblUserPassword;
    wxTextCtrl* m_txtUserPassword;
    wxBoxSizer* m_ctrlSizer;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;

public:
    /// Constructors
    UnicastConfigDialog();
	UnicastConfigDialog( wxWindow* parent, AppInterface* iface, wxWindowID id = SYMBOL_UNICASTDIALOG_IDNAME, const wxString& caption = SYMBOL_UNICASTDIALOG_TITLE, const wxPoint& pos = SYMBOL_UNICASTDIALOG_POSITION, const wxSize& size = SYMBOL_UNICASTDIALOG_SIZE, long style = SYMBOL_UNICASTDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_UNICASTDIALOG_IDNAME, const wxString& caption = SYMBOL_UNICASTDIALOG_TITLE, const wxPoint& pos = SYMBOL_UNICASTDIALOG_POSITION, const wxSize& size = SYMBOL_UNICASTDIALOG_SIZE, long style = SYMBOL_UNICASTDIALOG_STYLE );

    /// Destructor
    virtual ~UnicastConfigDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

	//void SetAppInterface(AppInterface *iface);

	const AppConfig &GetConfig() const;

	// Button events
	void OnBtnOkClick( wxCommandEvent& event );
	void OnBtnCancelClick( wxCommandEvent& event );

    /// Should we show tooltips?
    static bool ShowToolTips();

	// wxWidgets virtual "transfer data" methods
	bool TransferDataToWindow();
	bool TransferDataFromWindow();

	DECLARE_EVENT_TABLE()
};

#endif
    // _UNICASTCONFIGDIALOG_H_
