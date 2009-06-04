#define SKIN_API

#include <wx/wx.h>
#include <wx/debug.h> // for wxASSERT_MSG()

#include "Skin.h"
#include "AppInterface.h"
#include "SkinnableSingleChoiceDlg.h"
#include "VideoDisplayControl.h"

BEGIN_EVENT_TABLE(Skin, wxFrame)
	EVT_LEFT_DOWN(Skin::OnMouseLeftDown)
	EVT_MOTION(Skin::OnMouseMove)
	EVT_PAINT(Skin::OnPaint)
#ifdef __WXGTK__
	EVT_WINDOW_CREATE(Skin::OnWindowCreate)
#endif
END_EVENT_TABLE()

/** Default constructor.
*
*/
Skin::Skin() : wxFrame()
{
	// initialize bmp
	m_bmp = NULL;
	m_vdcCaptureView = NULL;
	m_appInterface = NULL;
	m_hasShape = false;
	m_captureEnabled = false;
}

/** Destructor.
*
*/
Skin::~Skin()
{
}

/** Initialize skin.
*
*/
void Skin::InitializeBase()
{
	EnableDrag();
}

/** Set app interface pointer.
*
*/
void Skin::SetAppInterface(AppInterface *iface)
{
	m_appInterface = iface;
}

/** Returns current app interface pointer.
*
*/
AppInterface *Skin::GetAppInterface() const
{
	return m_appInterface;
}

/** Set window shape.
*
*/
void Skin::SetWindowShape(wxBitmap *bmp)
{
	m_bmp = bmp;
	// Set window size to bitmap size
	SetSize(wxSize(m_bmp->GetWidth(), m_bmp->GetHeight()));
	// Create region
	wxRegion rgn(*bmp);
	// Set window shape to region
	m_hasShape = SetShape(rgn);
}

/** Creates the change skin button.
*
*/
void Skin::CreateChangeSkinBtn()
{
	/*Constants a;

	m_btnChangeSkin = new wxButton(this, ID_CHOOSE_SKIN, a.LABEL_CHANGE_SKIN, wxPoint(100, 180));
	Connect(ID_CHOOSE_SKIN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Skin::OnChangeSkin));*/
}

/** Creates the change language button.
*
*/
void Skin::CreateChangeLanguageBtn()
{
	/*Constants a;

	m_btnChangeLanguage = new wxButton(this, ID_CHOOSE_LANGUAGE, a.LABEL_CHANGE_LANGUAGE, wxPoint(100, 210));
	Connect(ID_CHOOSE_LANGUAGE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Skin::OnChangeLanguage));*/
}

/** Creates the exit button.
*
*/
void Skin::CreateExitBtn()
{
}

/** Creates the Unicast button.
*
*/
void Skin::CreateUnicastBtn()
{
}

/** Creates the Multicast button.
*
*/
void Skin::CreateMulticastBtn()
{
}

/** Creates the File button.
*
*/
void Skin::CreateFileBtn()
{
}

void Skin::CreateAboutBtn()
{
}

/** Triggered when the Change Skin command is selected.
*
*/
void Skin::OnChangeSkin(wxCommandEvent &event)
{
// 	int choiceIndex;

	// We rely on m_appInterface not being NULL here!
	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));
	// 
	StringHash skinHash = m_appInterface->GetAvailableSkins();

	// explicit type cast to avoid warning on conversion from size_t to int;
	// we reckon we'll never have over 4 billion (!) skins.
	int n_skinLibrary = (int)skinHash.size();

	if(n_skinLibrary == 0)
	{
		wxMessageBox(NO_SKIN_FOUND);
	}
	else
	{
		// 
		wxString *choices = new wxString [n_skinLibrary];

		m_skinSelected = m_appInterface->GetSkinSelected();

		// 
		StringHash::iterator j = skinHash.begin();
		for(int i = 0; i < n_skinLibrary; i++, j++)
		{
			choices[i] = j->first;

			if (m_skinSelected == choices[i])
				m_indexSkin = i;
		}

		// 
		SkinnableSingleChoiceDlg dialog(this,
											CHOOSE_SKIN_CAPTION,
											CHOOSE_SKIN,
											n_skinLibrary, choices);
		
		dialog.SetSelection(m_indexSkin);
		
		if (dialog.ShowModal() == wxID_OK)
		{
			wxString choice = choices[dialog.GetSelection()];
			Hide();	// done so that there is no more than one instance of this window visible at any time
			m_appInterface->SetSkinSelected(choice);

			// save skin in settings manager
			m_appInterface->GetSettingsManager()->SetSkin(choice);

			m_appInterface->LoadSkin(skinHash[choice]);
		}
		// Delete the choices (after the dialog modal, no more need of them)
		delete [] choices;
	}
}

/** Changes the current language.
*
*/
void Skin::OnChangeLanguage(wxCommandEvent &event)
{
	// We rely on m_appInterface not being NULL here!
	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));

	wxArrayInt languageCodes = m_appInterface->GetAvailableLanguageCodes();
	wxArrayString languageNames;

	wxString languageSelected = m_appInterface->GetLanguageSelected();

	for(unsigned int i = 0; i < languageCodes.GetCount(); i++)
	{

		languageNames.Add(m_appInterface->GetLanguageName(languageCodes[i]));

		/*if (languageSelected != m_appInterface->GetLanguageName(wxLANGUAGE_ENGLISH_US))
			languageNames.Add(m_appInterface->GetLanguageName(languageCodes[i]) + _(" ") + _("TEST"));
		else
			languageNames.Add(m_appInterface->GetLanguageName(languageCodes[i]));*/
	}

	wxString strLanguage;

	int indexLanguage = languageNames.Index(languageSelected);

	SkinnableSingleChoiceDlg languageDlg(this, CHOOSE_LANGUAGE_CAPTION,
											LABEL_LANGUAGE,
											languageNames );

	languageDlg.SetSelection(indexLanguage);

	if (languageDlg.ShowModal() == wxID_OK)
	{
       strLanguage = languageDlg.GetStringSelection();
	   m_indexLanguage = languageDlg.GetSelection();

	   m_appInterface->SetLanguageSelected(strLanguage.BeforeFirst(' '));
	}

	int language = languageNames.Index(strLanguage);

	if(language != wxNOT_FOUND)
	{
		m_appInterface->SelectLanguage( languageCodes[ language ] );
		UpdateText();

		// save language in settings manager
		wxString languageCode;
		languageCode.Printf(wxT("%d"), languageCodes[language]);
		m_appInterface->GetSettingsManager()->SetLanguage(languageCode);
	}
}

//void Skin::OnUnicastBtnClick(wxCommandEvent &event)
//{
//	// We rely on m_appInterface not being NULL here!
//	wxCHECK_RET(m_appInterface, wxT("m_appInterface is NULL!!"));
//}

/** Default About info
 */
wxString Skin::GetAboutInfo()
{
	return _("Not supplied");
}

/** Shows a default "About" dialog (may be overridden by skins, so making it possible to e.g. embed the about text in a pane in the window)
 */
void Skin::OnAboutBtnClick(wxCommandEvent &event)
{
	m_appInterface->ShowAboutDialog();
}

void Skin::UpdateText()
{
}

/** Exits application.
*
*/
void Skin::OnExit(wxCommandEvent &event)
{
	Close();
}


/** Called when Skin's main window is closed.
 *
 * This function will be called by App upon closing of Skin's main window.
 * App will always handle Skin's wxCloseEvent. This overrideable function
 * is provided to handle Skin-specific behavior IN ADDITION to App's normal
 * behavior on closing Skin's main window.
 * App will check the return value from this function, and will only effectively
 * proceed with the processing of the close event if it returns true.
 *
 * @param[in] force	True if closing is forced.
 * @return True if App should proceed with closing, false otherwise.
 */
bool Skin::OnMainWindowClose(bool force)
{
	return true;
}

/** Paints the bitmap on window.
*
*/
void Skin::OnPaint(wxPaintEvent &event)
{
	PaintBaseSkin();
}

void Skin::PaintBaseSkin()
{
	wxPaintDC paintDC(this);
	if(m_hasShape)
	{
		paintDC.DrawBitmap(*m_bmp, 0, 0, true);
	}
}

/** Returns the current list of active private chat windows.
 */
PrivateChatWindowList &Skin::GetPrivateChatWindowList()
{
	return m_pcwList;
}

/** Override this function to take periodic action based on global application timer.
 */
void Skin::OnAppTimer()
{

}

/** Update delta from top-left corner.
*
*/
void Skin::OnMouseLeftDown(wxMouseEvent &event)
{	
	if (m_dragEnable)
	{
		wxPoint pt = ClientToScreen(event.GetPosition());
		wxPoint pos = GetPosition();
		m_delta = pt - pos;	
	}
}

/** Move window together with mouse.
*	
*/
void Skin::OnMouseMove(wxMouseEvent &event)
{
	wxPoint pt = event.GetPosition();
	if(event.Dragging() && event.LeftIsDown() && m_dragEnable)
	{
		wxPoint pos = ClientToScreen(pt);
		Move(pos - m_delta);
	}
}

/** Enable shaped window drag.
 */
void Skin::EnableDrag()
{
	m_dragEnable = true;
}

/** Disable shaped window drag.
 */
void Skin::DisableDrag()
{
	m_dragEnable = false;
}

#ifdef __WXGTK__
/** For compatibility with Unix/GTK+
*
*/
void Skin::OnWindowCreate(wxWindowCreateEvent &WXUNUSED(event))
{
	if(m_bmp)
		SetWindowShape(m_bmp);
}
#endif
