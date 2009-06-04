#ifndef TIMEOUTREGISTER_H
#define TIMEOUTREGISTER_H

#include <wx/wx.h>
#include <list>

/** @brief Define timeout notification types.
 *
 * Timeout notification types:
 * TR_MEDIA_AUDIO_TIMEOUT       Audio timeout.
 * TR_MEDIA_VIDEO_TIMEOUT       Video timeout.
 */
enum TimeoutNotificationType
{
	TR_MEDIA_AUDIO_TIMEOUT,
	TR_MEDIA_VIDEO_TIMEOUT
};

// Stores information about timeouts.
struct TimeoutObject
{
	TimeoutNotificationType m_type;
	unsigned long m_id;
	wxDateTime m_regTime;
	wxTimeSpan m_interval;
	void *m_userData;
};

// wxEventType declaration for iptvEVT_TIMEOUT.
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_LOCAL_EVENT_TYPE(iptvEVT_TIMEOUT, -1)
END_DECLARE_EVENT_TYPES()

// A few handy typedefs.
typedef std::list<TimeoutObject *> TimeoutList;
typedef TimeoutList::iterator TimeoutIterator;

// Default time resolution, in milliseconds.
#define TR_DEFAULT_CHECK_INTERVAL	500

/** @brief Used for managing timeouts.
 *
 * The TimeoutRegister class handles timeouts. It also derives from wxEvtHandler and contains
 * a timer, which it uses to keep track of timeouts. The default time resolution is TR_DEFAULT_CHECK_INTERVAL,
 * currently defined to 500 milliseconds, and may be changed using TimeoutRegister::SetCheckInterval().
 * On each timeout event a wxCommandEvent with type iptvEVT_TIMEOUT is sent to the registered event handler 
 * (set either in constructor or using TimeoutRegister::SetEventHandler()). Timeouts are registered using
 * TimeoutRegister::Register(). Each timeout registered has an ID, and this ID is passed along with the timeout
 * event when it fires. Timeouts may also carry user data, which TimeoutRegister has no control over (so the user
 * is responsible for managing it).
 *
 * The wxCommandEvent object sent by TimeoutRegister will have data that can be retrieved as follows:@n
 * @b event.GetID()				Timeout notification type (interpreted as "window ID" in the wxWidgets docs)@n
 * @b event.GetExtraLong()		Timeout object ID.@n
 * @b event.GetClientData()		Pointer to user data.@n
 *
 */
class TimeoutRegister: public wxEvtHandler
{
private:
	TimeoutList m_list;
	wxEvtHandler *m_handler;
	wxTimer m_timer;

	bool SendCommandEvent(TimeoutNotificationType type, unsigned long id, void *userData = NULL);
public:
	TimeoutRegister(wxEvtHandler *handler = NULL, long interval = TR_DEFAULT_CHECK_INTERVAL);
	~TimeoutRegister();

	void SetEventHandler(wxEvtHandler *handler);
	void SetCheckInterval(long interval);

	void Register(TimeoutNotificationType type, unsigned long id, wxTimeSpan interval, void *userData = NULL);
	void Reset(TimeoutNotificationType type, unsigned long id);
	void Clear(TimeoutNotificationType type, unsigned long id);
	void Check();
	TimeoutObject *GetObject(TimeoutNotificationType type, unsigned long id);

	void OnTimer(wxTimerEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
