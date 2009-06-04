
/*
* DsLibUtil
*
* DsCStrBase
* DsCString
* DsCStrStatic
* DsCBuffer
*
* String and buffer management
*
* $Id: dscstring.cpp 472 2006-10-31 21:39:05Z dario $
* Dario Andrade
*
* Defines some string functions (resizable) and a buffer management (autodelete)
*/

#include "dscstring.h"
#include "dscexception.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>

#ifdef _LINUX
#define _stricmp strcasecmp
#endif // _LINUX

/* Radix Control Constants: Base64 -> 6 bits raw with 8 bit encoding */

#define RADIXBITS               6
#define RADIX                   ( 1 << RADIXBITS )
#define RADIXMASK               ( RADIX - 1 )
#define ALPHABITS               8
#define ALPHASIZE               ( 1 << ALPHABITS )
#define ALPHAMASK               ( ALPHASIZE - 1 )

/* Alpha-text encoding of raw bit values (6 bits in Radix 64)  */
/* Final Radix position (65th in Radix 64) has pad value ('=') */

static DsUINT8 radalpha[ RADIX + 2 ]   =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

/* Inverse of Alpha-text encoding (set in 1st call to decode64()) */

static DsINT32 initrev = 1;
static DsUINT8 revalpha[ ALPHASIZE ];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DsCStrBase::DsCStrBase() : m_pStr( DsNULL ), m_uiSize( 0 )
{
}

DsCStrBase::~DsCStrBase()
{
    m_pStr      = DsNULL;
    m_uiSize    = 0;
}

void DsCStrBase::Erase()
{
    Assign( "" );
}

DsUINT32 DsCStrBase::ToInteger() const
{
    return atol( GetStr() );
}

DsCStrBase& DsCStrBase::Format( DsPCSTR pstr, ... )
{
	DsCHAR str[ DSLIB_STRING_MAX_STRING ]; // 16k unichars should be enough to format a string

	va_list va;
	va_start( va, pstr );

    #if defined( WIN32 )
	
        _vsnprintf( str, DSLIB_STRING_MAX_STRING, pstr ? pstr : "", va );

    #else // WIN32
	    
        vsnprintf( str, DSLIB_STRING_MAX_STRING, pstr ? pstr : "", va );

    #endif // WIN32

	va_end( va );

	Assign( str );

    return *this;
}

DsCStrBase& DsCStrBase::Reserve( DsUINT32 cbLen )
{
	if ( GetSize() < cbLen )
	{
		DsPSTR pszReserved = Alloc( cbLen + sizeof( DsCHAR ) );
		if ( !pszReserved )
		{
			DsTHROWC( DsE_OUTOFMEMORY );
		}

		if ( GetStr() )
		{
			strcpy( pszReserved, GetStr() );
			Release( m_pStr );
		}

		m_pStr = pszReserved;
	}

    return *this;
}

/* Encode a string to Base64 representation   */ 
/* Return values:  0  everything cool
                  -1  output buffer too short */
 
DsCStrBase& DsCStrBase::Encode64From( const DsUINT8* pBuffer, DsUINT32 cbBuffer )
{
    DsINT32 pad         = 0;
    DsINT32 shiftcnt    = 0;
    DsUINT32 rawbits    = 0;
    DsUINT32 shiftreg   = 0;

    // zero out the object
    m_uiSize            = 0;

    DsPSTR p            = GetBuffer( cbBuffer * 2 );

    /* Keep going until EOS And all raw bits output */

    for ( DsUINT32 i = 0;
          ( i < cbBuffer ) || ( shiftcnt != 0 ); i++ ) 
    {
        if ( i < cbBuffer )
        {
            rawbits = ( DsUINT32 ) ( ( DsUINT8 ) pBuffer[ i ] );
        }
        else
        {
            rawbits = 0;  /* fake it - just shift bits out */
        }

        /* Add new raw bits to left end of shift register */

        shiftreg <<= ALPHABITS;   
        shiftcnt  += ALPHABITS;
        shiftreg  |= rawbits;

        /* Keep shifting out while enough bits remain */

        while ( shiftcnt >= RADIXBITS ) 
        {
            if ( m_uiSize >= GetMaxSize() )
            {
                p[ 0 ] = DsNULLCHAR;
                m_uiSize = 0;

                /* ran out of output buffer */
                return *this;
            }
            else 
            {
                shiftcnt -= RADIXBITS;
                p[ m_uiSize++ ] = !pad ? radalpha[ ( int ) ( ( shiftreg >> shiftcnt )
                               & ( DsUINT32 ) RADIXMASK ) ] : radalpha[ RADIX ]; /* pad out to even multiple */
            }

            /* Only one alpha output past end of original string */
            pad = ( i < cbBuffer ) ? 0 : 1;
        } // end while
    } // end for

    p[ m_uiSize ] = DsNULLCHAR;

    return *this;
}

/*
* Encode string per RFC1738 URL encoding rules
*/
DsCStrBase& DsCStrBase::URLEncodeFrom( const DsCStrBase& refstrFrom )
{
    //int rfc1738_encode(const char* src,int src_ln,char *dst,int dst_sz)
    /*static char safe[] = "$-_.+!*'(),";*/
    static DsCHAR hex[] = "0123456789ABCDEF";
    DsUINT8 c;

    m_uiSize = 0;

    DsPSTR p = GetBuffer( refstrFrom.GetSize() * 3 );
    DsPCSTR psrc = refstrFrom.GetStr();
        
    for ( DsUINT32 i = 0; i < refstrFrom.GetSize(); i++ )
    {
        *p = *psrc++;
        
        if ( !isalnum( *p ) ) 
        {
            c = * ( DsUINT8* ) p;
            *p++ = '%';
            *p++ = hex[ c/0x10 ];
            *p++ = hex[ c%0x10 ];
            m_uiSize += 2;
        }
        else
        {
            p++;
        }

        m_uiSize++;
    } // end for all chars in string

    GetBuffer()[ m_uiSize ] = DsNULLCHAR;

    ReleaseBuffer();

    return *this;
}

/*
* Decode string per RFC1738 URL encoding rules
*
* 11-10-01 Retval gives the number of characters decoded - note that this
* does not NULL terminate
*/
DsCStrBase& DsCStrBase::URLDecodeFrom( const DsCStrBase& refstrFrom )
{
    static DsCHAR buf[ 3 ];
    
    DsUINT32 uiSizeFrom = refstrFrom.GetSize();

    m_uiSize = 0;

    if ( uiSizeFrom > 0 )
    {
        DsPSTR p = GetBuffer( uiSizeFrom + sizeof( DsCHAR ) );
        DsPCSTR psrc = refstrFrom.GetStr();

        for ( DsUINT32 i = 0; i < uiSizeFrom; i++ )
        {
            *p = *psrc++;

            if ( ( *p == '%' ) 
                && ( i < ( uiSizeFrom - 2 ) ) 
                && isxdigit( psrc[ 0 ] ) 
                && isxdigit( psrc[ 1 ] ) )
            {
                buf[ 0 ] = *psrc++;
                buf[ 1 ] = *psrc++;
                * ( ( DsUINT8* ) p++ ) = ( DsUINT8 ) strtol( buf, NULL, 0x10 );
            }
            else
            {
                p++;
            }

            m_uiSize++;
        } // end for all chars in string

        *p = DsNULLCHAR;

        // no need to call ReleaseBuffer since m_uiSize is already set
    } // end if there are chars to decode from

    return *this;
}

DsCStrBase& DsCStrBase::FilterFrom( const DsCStrBase& refstrFrom, DsCHAR c )
{
    DsINT32 i, j;

    if ( refstrFrom.GetStr() && refstrFrom.GetSize() && GetBuffer( refstrFrom.GetSize() ) )
    {
        i = 0;
        j = 0;

        while ( refstrFrom[ i ] )
        {
            if ( refstrFrom[ i ] != c )
            {
                GetBuffer()[ j++ ] = refstrFrom[ i ];
            }

            i++;
        }

        GetBuffer()[ j ] = DsNULLCHAR;

        ReleaseBuffer();
    }

    return *this;
}

DsPCSTR DsCStrBase::ReplaceFirstAndGetNext( DsCHAR cFrom, DsCHAR cTo )
{
    DsCHAR *pc = GetBuffer();
    DsBOOL bReplaced = DsFALSE;
    DsUINT32 uiSize = ( DsUINT32 ) -1;

    while ( pc && *pc && !bReplaced )
    {
        if ( *pc == cFrom )
        {
            *pc = cTo;

            // we need to update the string length
            if ( cTo == DsNULLCHAR )
            {
                uiSize = ( ( pc - GetStr() ) / sizeof( DsCHAR ) );
            }

            bReplaced = DsTRUE;
        } // end if char is the one we want

        pc += sizeof( DsCHAR );
    } // end while pointer and not end of string

    if ( uiSize != ( DsUINT32 ) -1 )
    {
        m_uiSize = uiSize;
    }

    return pc;
}

DsCStrBase& DsCStrBase::Replace( DsCHAR cFrom, DsCHAR cTo )
{
    DsCHAR *pc = GetBuffer();
    DsUINT32 uiSize = ( DsUINT32 ) -1;

    while ( pc && *pc )
    {
        if ( *pc == cFrom )
        {
            *pc = cTo;

            // we need to update the string length
            if ( ( cTo == DsNULLCHAR ) && ( uiSize == ( DsUINT32 ) -1 ) )
            {
                uiSize = ( ( pc - GetStr() ) / sizeof( DsCHAR ) );
            }
        }

        pc++;
    } // end while pointer and not end of string

    if ( uiSize != ( DsUINT32 ) -1 )
    {
        m_uiSize = uiSize;
    }

    return *this;
}

DsCStrBase& DsCStrBase::ToLower()
{
    DsPSTR p = GetBuffer();

    while ( *p )
    {
        *p = tolower( *p );
        p++;
    }

    return *this;
}

DsCStrBase& DsCStrBase::RemoveFileSpec()
{
	DsPSTR p = ( DsPSTR ) strrchr( GetStr(), '\\' );

    if ( !p )
    {
        p = ( DsPSTR ) strrchr( GetStr(), '/' );
    }

    // last but not least, the last is the first char
    if ( !p )
    {
        p = ( DsPSTR ) GetStr();
    }

    DsUINT32 uiPathLen = ( ( p - GetStr() ) / sizeof( DsCHAR ) ) + 1;

    // what if the last is not the end of string?
    if ( uiPathLen < GetSize() )
    {
        p++;

        *p = DsNULLCHAR;

        m_uiSize = uiPathLen;
    } // else what if it is the beginning?
    else if ( p == GetStr() )
    {
        Assign( "" );
    } // end if last slash is not end of string

    return *this;
}

DsPCSTR DsCStrBase::RemovePath() const
{
	DsPCSTR pLast = strrchr( GetStr(), '\\' );

    if ( !pLast )
    {
        pLast = strrchr( GetStr(), '/' );
    }

	return pLast ? ( pLast + sizeof( DsCHAR ) ) : GetStr();
}

DsPCSTR DsCStrBase::ReplaceFromEnd( DsCHAR cFrom, DsCHAR cTo )
{
    DsCHAR *pc = GetBuffer();

    pc = ( DsCHAR* ) strrchr( ( DsPCSTR ) pc, cFrom );

    if ( pc )
    {
        *pc = cTo;

        // we need to update the string length
        if ( cTo == DsNULLCHAR )
        {
            m_uiSize = ( ( pc - GetStr() ) / sizeof( DsCHAR ) );
        }
    } // end if found

    return GetStr();
}

bool DsCStrBase::Equals( DsPCSTR pstr, DsBOOL bCaseInsensitive )
{
    return ( GetStr() && pstr ) 
        ? ( bCaseInsensitive ? ( _stricmp( GetStr(), pstr ) == 0 ) : ( strcmp( GetStr(), pstr ) == 0 ) ) : false;
}

DsPSTR DsCStrBase::GetBuffer( DsUINT32 cbLen )
{
	Reserve( cbLen );

	return ( DsPSTR ) GetStr();
}

void DsCStrBase::ReleaseBuffer()
{
    m_uiSize = strlen( GetStr() );
}

const DsCHAR& DsCStrBase::At ( DsUINT32 i ) const
{
	if ( i >= GetSize() ) 
	{
		DsTHROW( DsE_ARRAY_OUTOFBOUNDS, 
			( "array index our of bounds %d >= %d", i, GetSize() ) );
	}
	
	return GetStr() [ i ];
}

bool DsCStrBase::operator < ( DsPCSTR pstr ) const
{
    return ( GetStr() && pstr ) ? ( strcmp( GetStr(), pstr ) < 0 ) : true;
}

bool operator < ( DsPCSTR pstr, const DsCStrBase& refstr )
{
    return ( refstr.GetStr() && pstr ) ? ( strcmp( pstr, refstr.GetStr() ) < 0 ) : true;
}

bool operator < ( const DsCStrBase& refstr, DsPCSTR pstr )
{
	return ( refstr.GetStr() && pstr ) ? ( strcmp( refstr.GetStr(), pstr ) < 0 ) : true;
}

bool operator < ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 )
{
	return refstr1.GetStr() < refstr2;
}

bool operator == ( DsPCSTR pstr, const DsCStrBase& refstr )
{
	return ( refstr.GetStr() && pstr ) ? ( strcmp( refstr.GetStr(), pstr ) == 0 ) : false;
}

bool operator == ( const DsCStrBase& refstr, DsPCSTR pstr )
{
	return pstr == refstr;
}

bool operator == ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 )
{
	return refstr1.GetStr() == refstr2;
}

bool operator != ( DsPCSTR pstr, const DsCStrBase& refstr )
{
	return !( pstr == refstr );
}

bool operator != ( const DsCStrBase& refstr, DsPCSTR pstr )
{
	return !( pstr == refstr );
}

bool operator != ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 )
{
	return !( refstr1.GetStr() == refstr2 );
}

DsCStrBase& DsCStrBase::operator = ( const DsCBuffer& refbuf )
{
	Assign( ( DsPCSTR ) refbuf.GetPtr(), refbuf.GetActualSize() );

	return *this;
}

DsCStrBase& DsCStrBase::operator = ( const DsCStrBase& refstr )
{
	Assign( refstr.GetStr() );

	return *this;
}

DsCStrBase& DsCStrBase::operator = ( DsPCSTR pstr )
{
	Assign( pstr );

	return *this;
}

DsCStrBase& DsCStrBase::operator += ( DsPCSTR pstr )
{
	return *this += pstr;
}

DsCStrBase& DsCStrBase::operator += ( DsUINT32 val )
{
	return *this += val;
}

bool DsCStrBase::StartsWith( DsPCSTR pstrMatch ) const
{
	size_t nToCompare = DSLIB_MIN( ( DsUINT32 ) strlen( pstrMatch ), GetSize() );

	return ( GetStr() && nToCompare ) ? ( strncmp( GetStr(), pstrMatch, nToCompare ) == 0 ) : false;
}

bool DsCStrBase::StartsWith( DsPCSTR pstrMatch, DsCStrBase& refStrRest ) const
{
    bool bRet = StartsWith( pstrMatch );

    if ( bRet )
    {
        refStrRest.Assign( GetStr() + DSLIB_MIN( GetSize(), strlen( pstrMatch ) ) );
    }

    return bRet;
}


//////////////////////////////////////////////////////////////////////
// DsCString

DsCString::DsCString()
{
	Assign( "" );
}

DsCString::DsCString( DsUINT32 nChars, DsPCSTR pstr )
{
    Assign( pstr, nChars );
}

DsCString::DsCString( const DsCBuffer& refbuf )
{
    Assign( ( DsPCSTR ) refbuf.GetPtr(), refbuf.GetActualSize() );
}

DsCString::DsCString( DsPUNICSTR punistr )//, ... )
{/*
	DsUNICHAR unistr[ DSLIB_STRING_MAX_STRING ]; // 16k unichars should be enough to format a string

	va_list va;
	va_start( va, punistr );
	_vswprintf( unistr, DSLIB_STRING_MAX_STRING, punistr ? punistr : ( ( DsPUNICSTR ) L"" ), va );
	va_end( va );

	Assign( unistr );*/
    Assign( punistr );
}

DsCString::DsCString( DsPCSTR pstr )//, ... )
{/*
	DsCHAR str[ DSLIB_STRING_MAX_STRING ]; // 16k chars should be enough to format a string

	va_list va;
	va_start( va, pstr );
	_vsnprintf( str, DSLIB_STRING_MAX_STRING, pstr ? pstr : "", va );
	va_end( va );

	Assign( str );*/
    Assign( pstr );
}

DsCString::DsCString( const DsCStrBase& refstr )
{
	Assign( refstr.GetStr() );
}

DsCString::DsCString( const DsCString& refstr )
{
	Assign( refstr.GetStr() );
}

DsCString::DsCString( DsINT32 val, int base )
{
	DsCHAR str[16];

	Assign( _itoa( val, str, base ) );
}

DsCString::~DsCString()
{
	if ( m_pStr ) 
    {
        Release( m_pStr );
    }
}

DsPSTR DsCString::Alloc( DsINT32 size )
{
    return new DsCHAR[ size ];
}

void DsCString::Release( DsPSTR& refp )
{
	delete [] ( DsPSTR ) refp;
    refp = DsNULL;
}

DsCStrBase& DsCString::Assign( DsPUNICSTR punistr, DsUINT32 nMax )
{
	if ( m_pStr ) 
    {
        Release( m_pStr );
    }

	if ( punistr )
	{
		m_uiSize = wcslen( ( const wchar_t* ) punistr );

        DsUINT32 nToAssign = DSLIB_MIN( m_uiSize, nMax ) + sizeof( DsCHAR );

	    m_pStr = Alloc( nToAssign );
	    if ( !m_pStr )
	    {
		    DsTHROWC( DsE_OUTOFMEMORY );
	    }

	    m_uiSize = wcstombs( m_pStr, ( const wchar_t* ) punistr, nToAssign );    
    }
    else
	{
        Assign( ( DsPUNICSTR ) L"" );
	}

    return *this;
}

DsCStrBase& DsCString::Assign( DsPCSTR pstr, DsUINT32 nMax )
{
	if ( m_pStr ) 
    {
        Release( m_pStr );
    }

	if ( pstr )
	{
		m_uiSize = strlen( pstr );

        DsUINT32 nToAssign = DSLIB_MIN( m_uiSize, nMax ) + sizeof( DsCHAR );

	    m_pStr = Alloc( nToAssign );
	    if ( !m_pStr )
	    {
		    DsTHROWC( DsE_OUTOFMEMORY );
	    }

        if ( m_uiSize < nToAssign )
        {
            // just copy
            strcpy( m_pStr, pstr );
        }
        else
        {
            m_uiSize = DSLIB_MIN( m_uiSize, nMax );

            strncpy( m_pStr, pstr, m_uiSize );

            // let's correct the size and include null in the end
            m_pStr[ m_uiSize ] = DsNULLCHAR;
        }
    } 
    else
	{
        Assign( "" );
	}

    return *this;
}

DsCStrBase& DsCString::operator += ( DsUINT32 val )
{
    return *this += DsCString( val );
}

DsCStrBase& DsCString::operator += ( DsPCSTR pstr )
{
	if ( pstr )
	{			
		DsINT32 i = strlen( pstr );

		if (i > 0)
		{
			DsPSTR p = Alloc( m_uiSize + i + sizeof( DsCHAR ) );
			if ( !p )
			{
				DsTHROWC( DsE_OUTOFMEMORY );
			}

			strcpy( p, GetStr() );
			strcat( p, pstr );
			
			if ( m_pStr ) Release( m_pStr );
			
			m_pStr = p;

			m_uiSize += i;
		}
	}

	return *this;
}

DsCString& DsCString::operator = ( const DsCStrBase& refstr )
{
	Assign( refstr.GetStr() );

	return *this;
}

DsCString& DsCString::operator = ( DsPCSTR pstr )
{
	Assign( pstr );

	return *this;
}

DsCString& DsCString::operator = ( const DsCBuffer& refbuf )
{
	Assign( ( DsPCSTR ) refbuf.GetPtr(), refbuf.GetActualSize() );

	return *this;
}

DsCString DsCString::operator + ( DsPCSTR pstr ) const
{
	return DsCString( GetStr() ) += pstr;
}

DsCString DsCString::operator + ( DsUINT32 val ) const
{
    return DsCString( GetStr() ) += val;
}

DsCString DsCString::operator + ( const DsCStrBase& refstr ) const
{
	return DsCString( GetStr() ) += refstr.GetStr();
}

/////////////////////////////////////////////////////////////////////////////
// DsCStrStaticT

DsCStrStaticT<> operator + ( DsPCSTR pstr, const DsCStrBase& refstr )
{
	return DsCStrStatic( pstr ) += refstr.GetStr();
}

DsCStrStaticT<> operator + ( const DsCStrBase& refstr, DsUINT32 val )
{
    return DsCStrStatic( refstr.GetStr() ) += val;
}

DsCStrStaticT<> operator + ( const DsCStrBase& refstr, DsPCSTR pstr )
{
	return DsCStrStatic( refstr.GetStr() ) += pstr;
}

DsCStrStaticT<> operator + ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 )
{
	return DsCStrStatic( refstr1.GetStr() ) += refstr2.GetStr();
}

/////////////////////////////////////////////////////////////////////////////
// DsCBuffer
void DsCBuffer::Alloc( DsINT32 lSize )
{
	//m_p = ( T* ) new T[ lSize ];
#   ifdef HAVE_MEMALIGN
	    m_p = ( T* ) memalign( 16, lSize );
#   else
        DsCHAR * ptr = ( DsCHAR * ) malloc( lSize + 16 + 1 );
        DsINT32 diff = ( ( -( DsINT32 ) ptr - 1 ) & 15 ) + 1;
        ptr += diff;
        ( ( DsCHAR * ) ptr )[ -1 ] = ( DsCHAR ) diff;
        m_p = ( T* ) ptr;
#   endif // HAVE_MEMALIGN

	if ( !m_p )
	{
		DsTHROWC( DsE_OUTOFMEMORY );
	}
}

void DsCBuffer::Free()
{
#   ifdef HAVE_MEMALIGN
        free( m_p );
#   else
        free( m_p - ( ( DsCHAR* ) m_p )[ -1 ] );
#   endif // HAVE_MEMALIGN
//	delete [] m_p;
	m_p = DsNULL;
}

/* Translate alpha text back to raw bit values */
/* Return values: DsS_OK  everything cool
                  DsE_POINTER  output buffer too short 
                  DsE_INVALIDARG  illegal input character
                  DsE_UNEXPECTED  incomplete input string  */
DsRESULT DsCBuffer::Decode64From( DsPCSTR szBase64 )
{
    DsRESULT hr = DsS_OK;

    DsCString strBase64( szBase64 );

    if ( strBase64.GetSize() > 0 )
    {
        try
        {
            // align 256bits (for encryption/decryption)
            SetSizeAligned( strBase64.GetSize(), 32 );

            DsUINT32 shiftreg   = 0;
            DsUINT32 shiftcnt   = 0;
            DsUINT32 rawbits    = 0;

            m_lActualSize       = 0;

            /* Initialize inversion string on 1st call */

            if ( initrev ) 
            {
                memset( revalpha, ALPHAMASK, sizeof( revalpha ) );

                for ( DsUINT32 i = 0; i <= RADIX; i++ )
                {
                   revalpha[ (int) radalpha[ i ] ] = ( DsUINT8 ) i;
                }

                initrev = 0;
            }

            /* Process all alpha up to EOS Or 1st pad character ('=') */
            // this is to avoid ISO warnings on gcc
            // we will use the i variable later
            DsUINT32 i = 0;

            for ( ; i < strBase64.GetSize(); i++ ) 
            {
                rawbits = ( DsUINT32 ) revalpha[ (int) ( ( DsUINT8 ) szBase64[ i ] ) ];

                if ( rawbits < RADIX ) 
                {

                    /* Add new raw bits to left end of shift register */

                    shiftreg <<= RADIXBITS;
                    shiftcnt  += RADIXBITS; 
                    shiftreg  |= rawbits;

                    /* Output next raw alpha bits if enough ready */

                    if ( shiftcnt >= ALPHABITS ) 
                    {
                        shiftcnt -= ALPHABITS;

                        if ( m_lActualSize >= ( DsINT32 ) GetSize() )
                        {
                            hr = DsE_POINTER;

                            /* error: No room left for output   */
                            SetActualSize( 0 );
                            break;
                        }
                        else
                        {
                            m_p[ m_lActualSize++ ] = ( char ) ( ( shiftreg >> shiftcnt ) & ( DsUINT32 ) ALPHAMASK );
                        }
                    } // end if shiftcnt
                }
                else if ( rawbits == RADIX )
                {
                    /* ok: Pad character */
                    break;
                }
                else
                {
                    hr = DsE_INVALIDARG;

                    /* error: Illegal character -2 */
                    SetActualSize( 0 );

                    break;
                }
            } // end for

            if ( i == strBase64.GetSize() )
            {
                if ( shiftcnt == 0 )
                {
                    /* OK: No pad & no danglng bits */
                }
                else
                {
                    hr = DsE_UNEXPECTED;
            
                    /* error: Left bits on the table -3 */
                    SetActualSize( 0 );
                }
            } // end if we ran all characters

            m_p[ GetActualSize() ] = DsNULLCHAR;
        }
        catch ( DsCException e )
        {
            hr = e.GetCode();
        }
    } // end if source string is more than zero

    return hr;
}

bool DsCBuffer::Equals( DsPVOID p1, DsPVOID p2, DsUINT32 cb )
{
    return memcmp( p1, p2, cb ) == 0;
}

void DsCBuffer::OverlappedCopy( DsPVOID ptarget, DsPVOID psrc, DsUINT32 cb )
{
    memmove( ptarget, psrc, cb );
}

void DsCBuffer::Copy( DsPVOID ptarget, DsPVOID psrc, DsUINT32 cb )
{
    memcpy( ptarget, psrc, cb );
}

void DsCBuffer::Reset()
{
    if ( GetSize() > 0 )
    {
        memset( m_p, 0, GetSize() );
    }
}

#if defined( WIN32 )

DsINT32 _vswprintf( DsPUNISTR wcs, DsINT32 maxlen,
                      DsPUNICSTR format, va_list args)
{
    return _vsnwprintf( wcs, maxlen, format, args );
}

#elif defined( _LINUX )

DsPSTR _itoa( DsINT32 value, DsPSTR str, DsINT32 radix )
{
    if ( str )
    {
        switch ( radix )
    	{
            case 10:
                sprintf( str, "%ld", value );
                break;
        }
   }

    return str;
}

DsINT32 _vswprintf( DsPUNISTR wcs, DsINT32 maxlen,
                      DsPUNICSTR format, va_list args)
{
    return vswprintf( ( wchar_t* ) wcs, maxlen, ( const wchar_t* ) format, args );
}

#endif /* _LINUX */
