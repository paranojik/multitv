
/*
* DsLibUtil
*
* DsResult
*
*
* $Id: dsresult.h 375 2005-11-24 11:34:34Z dario $
* Dario Andrade
*
* Describes basic mechanisms to register facilities and error codes
*/

#ifndef _DS_RESULT_H
#define _DS_RESULT_H

#include "dstypes.h"

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
enum DsSEVERITY
{
	DsSEVERITY_SUCCESS			    = 0x0,
    DsSEVERITY_INFO                 = 0x1,
	DsSEVERITY_WARNING			    = 0x2,
	DsSEVERITY_ERROR			    = 0x3
};

enum DsFACILITY
{
	DsFACILITY_BASE				    = 0x0000,
	DsFACILITY_LIBUTIL			    = 0x000A,
	DsFACILITY_LIBUTIL_STRING	    = 0x000B,
	DsFACILITY_LIBUTIL_HASH		    = 0x000C,
    DsFACILITY_LIBUTIL_PERSISTENT   = 0x000D,
    DsFACILITY_LIBUTIL_OBJFACTORY   = 0x000E,
    DsFACILITY_LIBUTIL_IO           = 0x000F,
	DsFACILITY_USER				    = 0x1000
};

#define DsMAKE_RESULT(sev, fac, code) ( ( ( sev & 0x3 ) << 30 ) | ( ( fac & 0x3fff ) << 16 ) | (code & 0xffff) )

#define DsS_OK						DsMAKE_RESULT( DsSEVERITY_SUCCESS, DsFACILITY_BASE, 0 )
#define DsS_FALSE   				DsMAKE_RESULT( DsSEVERITY_SUCCESS, DsFACILITY_BASE, 1 )

#define DsI_UNDEFINED               DsMAKE_RESULT( DsSEVERITY_INFO, DsFACILITY_BASE, 0 )

#define DsE_FAIL					DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 10 )
#define DsE_OUTOFMEMORY				DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 20 )
#define DsE_POINTER					DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 30 )
#define DsE_NOTINIT					DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 40 )
#define DsE_ALREADYINIT				DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 50 )
#define DsE_INVALIDARG				DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 60 )
#define DsE_ASSERT_FAILED			DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 70 )
#define DsE_UNEXPECTED              DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 80 )
#define DsE_BUFFER_OVERFLOW         DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 90 )
#define DsE_NOTIMPLEMENTED          DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 100 )
#define DsE_NOINTERFACE             DsMAKE_RESULT( DsSEVERITY_ERROR, DsFACILITY_BASE, 110 )

typedef DsUINT32	                DsRESULT;

#define DsIS_OK(hr)                 ( ( ( DsINT32 ) hr ) >= 0 )    

#define DsREGISTER_FACILITY( fac, base ) static const DsUINT16 fac = DsFACILITY_USER + ( base );
#define DsREGISTER_ERR_RESULT( err, fac, res ) static const DsRESULT err = DsMAKE_RESULT( DsSEVERITY_ERROR, fac, res );
#define DsREGISTER_SUC_RESULT( err, fac, res ) static const DsRESULT err = DsMAKE_RESULT( DsSEVERITY_SUCCESS, fac, res );

#endif /* _DS_RESULT_H */

