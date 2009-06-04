#ifndef _SOPHISTICATED_LISTCONTROL_H_
#define _SOPHISTICATED_LISTCONTROL_H_

#include <wx/wx.h>
#include <wx/listctrl.h>

/** @brief Control overrided the wxListControl, with function blink yours items.
*
* This control is used in SessionWindow for blink the user that send a private chat.
*/
class SophisticatedListControl: public wxListCtrl
{
private:
	bool m_isBlinking;
	bool m_itemBlinkVisible;
	bool m_blinkEnabled;
	
	wxTimer *m_blinkTimer;
	int m_blinkInterval;

	wxColour m_fillColor;
	wxColour m_erasedColor;

	wxArrayLong m_item;
	long m_itemCount;

public:
	//Constructor
	SophisticatedListControl(wxWindow* parent, wxWindowID id, bool blink = false, int interval = 1000, const wxPoint& pos = wxDefaultPosition,
							 const wxSize& size = wxDefaultSize, long style = wxLC_ICON,
							 const wxValidator& validator = wxDefaultValidator,
							 const wxString& name = wxListCtrlNameStr);
	
	//Destructor
	virtual ~SophisticatedListControl();

	/**
	* Sets the item to blink or not.
	* blink:
	*   false => disable item blink
	*   true => enable item blink
	*/
	void BlinkItem(const long &item, const bool &blink = true);
	void ClearAll();

	/**
	* Sets the color to blink.
	*    fillColor => background color when it's visible
	*    erasedColor => background color when isn't visible
	*/
	void SetBlinkColor(const wxColour &fillColor, const wxColour &erasedColor);

	/**
	* Sets the new interval of blink in miliseconds.
	*/
	void SetNewInterval(const int &newInterval);

	void OnTimer(wxTimerEvent &event);

	DECLARE_EVENT_TABLE()

};

enum{
	ID_BLINK_TIMER = 11000
};


#endif
