#include <wx/wx.h>
#include "TimeoutRegister.h"

// Event type definition.
DEFINE_EVENT_TYPE(iptvEVT_TIMEOUT)

// Event table.
BEGIN_EVENT_TABLE(TimeoutRegister, wxEvtHandler)
	EVT_TIMER(wxID_ANY, TimeoutRegister::OnTimer)
END_EVENT_TABLE()

/** Constructor. Initializes the class.
 * @param[in] handler	Event handler that will receive timeout events. Default: NULL (no events are sent).
 * @param[in] interval	Timer interval. Default: TR_DEFAULT_CHECK_INTERVAL (500 ms).
 */
TimeoutRegister::TimeoutRegister(wxEvtHandler *handler, long interval)
{
	m_list.clear();
	m_timer.SetOwner(this);
	SetEventHandler(handler);
	SetCheckInterval(interval);
}

/** Destructor. Deletes all allocated TimeoutObject structures.
 */
TimeoutRegister::~TimeoutRegister()
{
	m_timer.Stop();
	TimeoutIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
		delete *it;
	m_list.clear();
}

/** Register a new timeout, or replaces its information (and resets count) if it already exists.
 * @param[in] type		Timeout notification type. (See TimeoutRegister.h)
 * @param[in] id		User-defined identifier (media ID, usually)
 * @param[in] interval	Time until expiration.
 * @param[in] userData	User-defined data.
 */
void TimeoutRegister::Register(TimeoutNotificationType type, unsigned long id, wxTimeSpan interval, void *userData)
{
	
	TimeoutObject *tobj = GetObject(type, id);
	if(!tobj)
	{
		tobj = new TimeoutObject;
		m_list.push_back(tobj);
	}
	
	tobj->m_type = type;
	tobj->m_id = id;
	tobj->m_interval = interval;
	tobj->m_userData = userData;
	tobj->m_regTime = wxDateTime::UNow();
}

/** Reset an existing timeout.
 *
 * "Resetting" a timeout here means resetting the time count associated to it,
 * e.g. a 10-second timeout that has not yet expired (fired) will be given another 10 seconds
 * from the time this function is called.
 *
 * @param[in] type		Notification type.
 * @param[in] id		Identifier of the timeout to reset.
 */
void TimeoutRegister::Reset(TimeoutNotificationType type, unsigned long id)
{
	TimeoutObject *tobj = GetObject(type, id);
	if(tobj)
		tobj->m_regTime = wxDateTime::UNow();
}

/** Remove an existing timeout from the list.
 * @param[in] type		Notification type.
 * @param[in] id		Identifier of the timeout to reset.
 */
void TimeoutRegister::Clear(TimeoutNotificationType type, unsigned long id)
{
	TimeoutObject *tobj = GetObject(type, id);
	if(tobj)
	{
		delete tobj;
		m_list.remove(tobj);
	}
}

/** Searches list for a TimeoutObject with given type and id. 
 * @param[in] type		Notification type.
 * @param[in] id		Identifier of the timeout to reset.
 * @return A pointer to a TimeoutObject with given type and id, or NULL if not found.
 */
TimeoutObject *TimeoutRegister::GetObject(TimeoutNotificationType type, unsigned long id)
{
	TimeoutIterator it;
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(((*it)->m_type == type) && ((*it)->m_id == id))
			return *it;
	}
	return NULL;
}

/** Checks for expired timeouts. Triggers a wxCommandEvent with type iptvEVT_TIMEOUT for each expired timeout.
 *
 * This method checks for expired timeouts, and will send to the registered event handler
 * a wxCommandEvent with type iptvEVT_TIMEOUT.
 * All timeout objects that are verified to have expired are deallocated and removed from the
 * internal list after their events are triggered.
 */
void TimeoutRegister::Check()
{
	TimeoutIterator it;
	// Create a list of "expired" iterators. This is needed because MS's implementation of STL
	// doesn't like it when an element is removed from a container in the middle of an iteration loop.
	std::list<TimeoutIterator> expiredList;	
	for(it = m_list.begin(); it != m_list.end(); it++)
	{
		if(wxDateTime::UNow().IsLaterThan((*it)->m_regTime + (*it)->m_interval))	// Timeout for current object has expired
		{
			SendCommandEvent((*it)->m_type, (*it)->m_id, (*it)->m_userData);
			delete *it;	// Deallocate object now
			expiredList.push_back(it);
		}
	}
	std::list<TimeoutIterator>::iterator it2;
	for(it2 = expiredList.begin(); it2 != expiredList.end(); it2++)
		m_list.erase(*it2);	// Remove all "expired" objects (from the previous pass) from the main list
	
	expiredList.clear();
}

/** Sends a wxCommandEvent to the specified handler.
 * @param[in] type		Notification type.
 * @param[in] id		Identifier.
 * @param[in] userData	Pointer to user data.
 */
bool TimeoutRegister::SendCommandEvent(TimeoutNotificationType type, unsigned long id, void *userData)
{
	if(m_handler)
	{
		wxCommandEvent event;
		event.SetEventType(iptvEVT_TIMEOUT);
		event.SetId(type);	// event ID - not to be confused with timeout ID.
		event.SetExtraLong(id);	// timeout ID
		event.SetClientData(userData);
		wxPostEvent(m_handler, event);
		return true;
	}
	return false;
}

/** Set the wxEvtHandler which will receive the timeout events.
 * @param[in] handler	Pointer to wxEvtHandler object which will receive the timeout events.
 */
void TimeoutRegister::SetEventHandler(wxEvtHandler *handler)
{
	m_handler = handler;
}

/** Set the timer interval. Starts the timer.
 */
void TimeoutRegister::SetCheckInterval(long interval)
{
	m_timer.Start(interval);
}

/** Timer event. All this does is check for expired timeouts.
 */
void TimeoutRegister::OnTimer(wxTimerEvent &event)
{
	Check();
}
