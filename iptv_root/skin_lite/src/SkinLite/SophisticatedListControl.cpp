#include "SophisticatedListControl.h"

BEGIN_EVENT_TABLE(SophisticatedListControl, wxListCtrl)
	EVT_TIMER(ID_BLINK_TIMER, SophisticatedListControl::OnTimer)
END_EVENT_TABLE()

/**
* Sophisiticated Constructor
*/
SophisticatedListControl::SophisticatedListControl(wxWindow* parent, wxWindowID id, bool blink, int interval, const wxPoint& pos,
												   const wxSize& size, long style,
												   const wxValidator& validator,
												   const wxString& name) : wxListCtrl(parent, id, pos, size, style, validator, name)
{
	m_isBlinking = false;
	m_itemBlinkVisible = false;
	m_blinkEnabled = blink;

	if (blink)
	{
		m_blinkTimer = new wxTimer(this, ID_BLINK_TIMER);
		m_blinkTimer->Start(interval);
		m_blinkInterval = interval;
	}
}

/**
* Sophisiticated destructor
*/
SophisticatedListControl::~SophisticatedListControl()
{
	if (m_blinkTimer->IsRunning())
		m_blinkTimer->Stop();
	delete m_blinkTimer;
}

/**
* Sets the item to blink or not.
* blink:
*   false => disable item blink
*   true => enable item blink
*/
void SophisticatedListControl::BlinkItem(const long &item, const bool &blink)
{	
	if (m_blinkEnabled)
	{
		if (blink == true)
		{
			//m_isBlinking = true;
			m_item.Add(item);
		}
		else
		{
			m_item.Remove(item);
			this->SetItemBackgroundColour(item, m_erasedColor);
		}
		m_itemCount = m_item.GetCount();
	}
}

/**
* Sets the color to blink.
*    fillColor => background color when it's visible
*    erasedColor => background color when isn't visible
*/
void SophisticatedListControl::SetBlinkColor(const wxColour &fillColor, const wxColour &erasedColor)
{
	m_fillColor = fillColor;
	m_erasedColor = erasedColor;
}

/**
* Sets the new interval of blink in miliseconds.
*/
void SophisticatedListControl::SetNewInterval(const int &newInterval)
{
	if (m_blinkEnabled)
		m_blinkTimer->Start(newInterval);
}

/*Timer event*/
void SophisticatedListControl::OnTimer(wxTimerEvent &event)
{
	if (m_blinkEnabled)
	{
		if ((m_itemBlinkVisible == false))
		{
			for(long i = 0;i<m_itemCount;i++)
			{ 
				this->SetItemBackgroundColour(m_item.Item(i), m_fillColor);
			}
		}
		else
		{
			for(long i = 0;i<m_itemCount;i++)
			{
				this->SetItemBackgroundColour(m_item.Item(i), m_erasedColor);
			}
		}

		m_itemBlinkVisible = !m_itemBlinkVisible;
	}	
}

void SophisticatedListControl::ClearAll()
{
	m_item.Empty();
	m_itemCount = m_item.GetCount();
	wxListCtrl::ClearAll();
}
