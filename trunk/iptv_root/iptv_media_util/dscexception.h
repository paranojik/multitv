
/*
* DsLibUtil
*
* DsCException
*
* $Id: dscexception.h 447 2006-04-19 14:47:00Z dario $
* Dario Andrade
*
* Describes the exception object and error handling code
*/

#ifndef _DS_CEXCEPTION_H_
#define _DS_CEXCEPTION_H_

#include "dscstring.h"

#define DSLIB_EXCEPTION_MAX_MSG                     1024

typedef DsCString                                   DsCStrException;

enum DsEXCEPTION_CATEGORY
{
    DsCAT_EXCEPTION         = 0x0,
	DsCAT_ERROR             = 0x1,
	DsCAT_WARNING           = 0x2,
	DsCAT_INFO              = 0x3,
    DsCAT_DEBUG             = 0x4
};

class DsCException  
{
public:
    DsCException( const DsCException& refException ) :
		m_strMsg( refException.m_strMsg ),
		m_resCode( refException.m_resCode )
    {
    }

    DsCException( DsRESULT resCode, DsPCSTR szMsg ) :
                m_strMsg( szMsg ),
                m_resCode( resCode )
    {
		#if defined(_DEBUG) && defined(WIN32)
        
			//OutputDebugString( m_strMsg + DsSTRNEWLINE );

		#endif // defined(_DEBUG) && defined(WIN32)
    }   
                                                     	
    ~DsCException()
    {
    }

	DsRESULT GetCode() const
	{
		return m_resCode;
	}

	const DsPCSTR GetMessage() const
	{
		return m_strMsg;
	}

private:
	DsCString                               m_strMsg;
	DsRESULT								m_resCode;
};

static void _DsTHROW( DsRESULT resCode, DsCStrBase& refstrMsg, DsPCSTR szFile, DsINT32 iLine )
{
	throw DsCException( resCode, DsCStrException().Format( "%s(%d): %s", szFile, iLine, refstrMsg.GetStr() ) );
}

#ifdef _DEBUG
#   define DsTHROWM( msg )            { DsCStrException __str; __str.Format msg; _DsTHROW( DsE_FAIL,   __str,  __FILE__, __LINE__ ); }
#   define DsTHROWC( res )            { DsCStrException __str;                   _DsTHROW( res,        __str,  __FILE__, __LINE__ ); }
#   define DsTHROW( res, msg )        { DsCStrException __str; __str.Format msg; _DsTHROW( res,        __str,  __FILE__, __LINE__ ); }
#else
#   define DsTHROWM( msg )            { DsCStrException __str;        _DsTHROW( DsE_FAIL,   __str,  __FILE__, __LINE__ ); }
#   define DsTHROWC( res )            { DsCStrException __str;        _DsTHROW( res,        __str,  __FILE__, __LINE__ ); }
#   define DsTHROW( res, msg )        { DsCStrException __str;        _DsTHROW( res,        __str,  __FILE__, __LINE__ ); }
#endif // _DEBUG

#define DsASSERT(expr)              { if ( !( expr ) ) { DsTHROWC( DsE_ASSERT_FAILED ); } }
#define DsASSERTM(expr, msg)        { if ( !( expr ) ) { DsTHROW( DsE_ASSERT_FAILED, msg ); } }

#endif // _DS_CEXCEPTION_H_
