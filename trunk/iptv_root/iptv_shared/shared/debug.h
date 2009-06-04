#ifndef _DEBUG_MACROS_H
#define _DEBUG_MACROS_H


#include "compat.h"


#ifndef SOURCE_STRING
#define SOURCE_STRING	"UNKNOWN LIBRARY"
#endif

// Tirar comentario para possibilitar TRACEs em release
//#define _DEBUG_RELEASE

#define LOGLEVEL0   0
#define LOGLEVEL1   1
#define LOGLEVEL2   2
#define LOGLEVEL3   3
#define LOGLEVEL4   4
#define LOGLEVEL5   5
#define LOGLEVEL6   6


//void _ShowDebugString(UINT level, LPCTSTR sourceid, LPCTSTR szError, ...);
void _DebugNotifyOnly(UINT level, LPCSTR sourceid, LPCSTR szError, ...);
//void _Debug(LPCTSTR sourceid, LPCTSTR szError, ...);
//void PrintError(LPCTSTR szBuff);
void SetErrorLevel(UINT level);
void _dummy(void);

void _GetStr(char *buf, unsigned int blen);
void _Pause (void);

#define _TRACEI(level, msg) _ShowDebugString(level, SOURCE_STRING, msg);


#if defined(_DEBUG) || defined(_DEBUG_RELEASE)

#define _TRACE(msg) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg);

#define _TRACE1(msg, arg1) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg,arg1);

#define _TRACE2(msg, arg1, arg2) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg, arg1, arg2);

#define _TRACE3(msg, arg1, arg2, arg3) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg, arg1, arg2, arg3);

#define _TRACE4(msg, arg1, arg2, arg3, arg4) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg, arg1, arg2, arg3, arg4);

#define _TRACE5(msg, arg1, arg2, arg3, arg4, arg5) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg, arg1, arg2, arg3, arg4, arg5);

#define _TRACE6(msg, arg1, arg2, arg3, arg4, arg5, arg6) \
		_DebugNotifyOnly(LOGLEVEL2, SOURCE_STRING, msg, arg1, arg2, arg3, arg4, arg5, arg6);

// level 5

#define _TRACEL(level, msg) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg);

#define _TRACE1L(level, msg, arg1) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg,arg1);

#define _TRACE2L(level, msg, arg1, arg2) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2);

#define _TRACE3L(level, msg, arg1, arg2, arg3) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2, arg3);

#define _TRACE4L(level, msg, arg1, arg2, arg3, arg4) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2, arg3, arg4);

#define _TRACE5L(level, msg, arg1, arg2, arg3, arg4, arg5) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2, arg3, arg4, arg5);

#define _TRACE6L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2, arg3, arg4, arg5, arg6);

#define _TRACE7L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		_DebugNotifyOnly(level, SOURCE_STRING, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7);

#define _ASSERTEF(exp, str) {							                            \
        if (!(exp))                                                                         \
        {                                                                                   \
            _Debug("Assert failed, expression", "%s, " #str,                                \
                #exp, __LINE__, __FILE__);                                            \
            _ASSERT(exp);                                                                   \
        }                                                                                   \
    }

#define _ASSERTEF1(exp, str, arg1) {							                            \
        if (!(exp))                                                                         \
        {                                                                                   \
            _Debug("Assert failed, expression", "%s, " #str,                                \
                #exp, arg1, __LINE__, __FILE__);                                            \
            _ASSERT(exp);                                                                   \
        }                                                                                   \
    }

#define _ASSERTEF2(exp, str, arg1, arg2) {							                            \
        if (!(exp))                                                                         \
        {                                                                                   \
            _Debug("Assert failed, expression", "%s, " #str,                                \
                #exp, arg1, arg2, __LINE__, __FILE__);                                            \
            _ASSERT(exp);                                                                   \
        }                                                                                   \
    }


#ifdef _DEBUG_SEMAPH
#define _WAIT(_psemaph, _debugstr) _psemaph->Wait(TRUE, _debugstr);
#define _SIGNAL(_psemaph, _debugstr) _psemaph->Signal(_debugstr);
#else
#define _WAIT(_psemaph, _debugstr) _psemaph->Wait(TRUE, NULL);
#define _SIGNAL(_psemaph, _debugstr) _psemaph->Signal(NULL);
#endif  // _DEBUG_SEMAPH


#else // _DEBUG

/*
#define _TRACE(msg) _dummy();
#define _TRACE1(msg, arg1) _dummy();
#define _TRACE2(msg, arg1, arg2) _dummy();
#define _TRACE3(msg, arg1, arg2, arg3) _dummy();
#define _TRACE4(msg, arg1, arg2, arg3, arg4) _dummy();
#define _TRACE5(msg, arg1, arg2, arg3, arg4, arg5) _dummy();
#define _TRACE6(msg, arg1, arg2, arg3, arg4, arg5, arg6) _dummy();
#define _TRACEL(level, msg) _dummy();
#define _TRACE1L(level, msg, arg1) _dummy();
#define _TRACE2L(level, msg, arg1, arg2) _dummy();
#define _TRACE3L(level, msg, arg1, arg2, arg3) _dummy();
#define _TRACE4L(level, msg, arg1, arg2, arg3, arg4) _dummy();
#define _TRACE5L(level, msg, arg1, arg2, arg3, arg4, arg5) _dummy();
#define _TRACE6L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6) _dummy();
#define _TRACE7L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) _dummy();

#define _ASSERTEF(exp, str) _dummy();
#define _ASSERTEF1(exp, str, arg1) _dummy();
#define _ASSERTEF2(exp, str, arg1, arg2) _dummy();
*/

#define _TRACE(msg)
#define _TRACE1(msg, arg1)
#define _TRACE2(msg, arg1, arg2)
#define _TRACE3(msg, arg1, arg2, arg3)
#define _TRACE4(msg, arg1, arg2, arg3, arg4)
#define _TRACE5(msg, arg1, arg2, arg3, arg4, arg5)
#define _TRACE6(msg, arg1, arg2, arg3, arg4, arg5, arg6)
#define _TRACEL(level, msg)
#define _TRACE1L(level, msg, arg1)
#define _TRACE2L(level, msg, arg1, arg2)
#define _TRACE3L(level, msg, arg1, arg2, arg3)
#define _TRACE4L(level, msg, arg1, arg2, arg3, arg4)
#define _TRACE5L(level, msg, arg1, arg2, arg3, arg4, arg5)
#define _TRACE6L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6)
#define _TRACE7L(level, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define _ASSERTEF(exp, str)
#define _ASSERTEF1(exp, str, arg1)
#define _ASSERTEF2(exp, str, arg1, arg2)


#define _WAIT(_psemaph, _debugstr) \
        _psemaph->Wait(TRUE, NULL);
#define _SIGNAL(_psemaph, _debugstr) \
        _psemaph->Signal(NULL);


#endif // _DEBUG

#endif // #ifndef _DEBUG_MACROS_H
