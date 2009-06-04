#ifndef _X_EXCEPTION_H_
#define _X_EXCEPTION_H_


#include "XResult.h"
#include "XStr.h"


class XException  
{
public:
    XException( XRESULT _resCode, LPCSTR _szMsg ) : m_strMsg( _szMsg ), m_resCode( _resCode )
    {
    }

    XRESULT GetCode(void) const
    {
        return m_resCode;
    }

    const XStr& GetMessage(void) const
    {
        return m_strMsg;
    }

private:
    XStrStatic<> m_strMsg;
    XRESULT      m_resCode;
};

inline void _XTHROW( XRESULT _resCode, const XStr& _refstrMsg, LPCSTR _szFile, INT32 _iLine )
{
	throw XException( _resCode, XStrStatic<>( _szFile ) + "(" + XStrStatic<>().Assign(_iLine) + "): " + _refstrMsg );
}

#define XTHROWM(msg)            { _TRACE3L(LOGLEVEL1, "EXCEPTION (%s), at line %d in %s", res, msg, __LINE__, __FILE__ ); _XTHROW( XE_FAIL, XStrStatic<>(msg), __FILE__, __LINE__ ); }
#define XTHROWC(res)            { _TRACE3L(LOGLEVEL1, "EXCEPTION (%d), at line %d in %s", res, __LINE__, __FILE__ ); _XTHROW( res, XStrStatic<>(), __FILE__, __LINE__ ); }
#define XTHROW(res, msg)        { _TRACE4L(LOGLEVEL1, "EXCEPTION (%d): %s, at line %d in %s", res, msg, __LINE__, __FILE__ ); _XTHROW( res, XStrStatic<>(msg), __FILE__, __LINE__ ); }

//#define XASSERT(expr)           { if ( !( expr ) ) { XTHROWC( XE_ASSERT_FAILED ); } }
//#define XASSERTM(expr, msg)     { if ( !( expr ) ) { XTHROW( XE_ASSERT_FAILED, msg ); } }

#endif // _X_EXCEPTION_H_
