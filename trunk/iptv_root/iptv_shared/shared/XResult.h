#ifndef _X_RESULT_H
#define _X_RESULT_H

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+---------------------------+-------------------------------+
//  |Sev|        Facility           |               Code            |
//  +---+---------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

enum XSEVERITY
{
    XSEVERITY_SUCCESS           = 0x0,
    XSEVERITY_WARNING           = 0x1,
    XSEVERITY_ERROR             = 0x2
};

enum XFACILITY
{
    XFACILITY_BASE              = 0x0001,
    XFACILITY_XSTRING           = 0x000B,
    XFACILITY_USER              = 0x1000
};

#define XMAKE_RESULT(sev, fac, code) ( ((sev & 0x3) << 30) | ((fac & 0x7fff) << 16) |  (code & 0xffff) )

#define XS_OK                   XMAKE_RESULT( XSEVERITY_SUCCESS, XFACILITY_BASE, 0 )

#define XE_FAIL                 XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 10 )
#define XE_OUTOFMEMORY          XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 20 )
#define XE_NULLPOINTER          XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 30 )
#define XE_NOTINIT              XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 40 )
#define XE_ALREADYINIT          XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 50 )
#define XE_INVALIDARG           XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 60 )
#define XE_ASSERT_FAILED        XMAKE_RESULT( XSEVERITY_ERROR, XFACILITY_BASE, 70 )

typedef UINT32  XRESULT;

#define XREGISTER_FACILITY( fac, base ) static const UINT16 fac = XFACILITY_USER + ( base );
#define XREGISTER_ERR_RESULT( err, fac, res ) static const XRESULT err = (XRESULT) XMAKE_RESULT( XSEVERITY_ERROR, fac, res );
#define XREGISTER_SUC_RESULT( err, fac, res ) static const XRESULT err = (XRESULT) XMAKE_RESULT( XSEVERITY_SUCCESS, fac, res );

#endif /* _X_RESULT_H */

