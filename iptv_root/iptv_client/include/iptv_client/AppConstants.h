#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

// Helper macro. TODO: Move it elsewhere?
// NOTE: Forced ISO 8859-1 encoding until Unicode compatibility mode is implemented.
#define WX_TO_CSTR(s)	(const char *)(s.mb_str(wxConvISO8859_1))
#define WX_TO_FILE_CSTR(s) (const char *)(s.mb_str(wxConvFile))
#define WX_FROM_CSTR(s)	wxString(s, wxConvISO8859_1)

#define UNICAST_PORT	8000
// Global app timer interval
// TODO: Un-hardcode this
#define APP_TIMER_INTERVAL 500

// Dynamic library extension
#ifdef WIN32
#define DYNAMIC_LIBRARY_EXTENSION "dll"
#endif
#ifdef __unix__
	#define DYNAMIC_LIBRARY_EXTENSION "so"
#endif

// Skin directories
#ifdef _DEBUG
#define DYNAMIC_LIBRARY_FOLDER "Skin/Debug"
#else
#define DYNAMIC_LIBRARY_FOLDER "Skin"
#endif

// Media timeout interval
// TODO: Un-hardcode this
#define MEDIA_TIMEOUT_INTERVAL	wxTimeSpan::Milliseconds(5000)

#endif // APP_CONSTANTS_H
