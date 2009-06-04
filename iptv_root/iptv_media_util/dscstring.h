/*
* DsLibUtil
*
* DsCStrBase
* DsCString
* DsCStrStaticT
* DsCBuffer
* DsCStrStaticCleanT
*
* $Id: dscstring.h 424 2006-03-13 00:29:42Z dario $
* Dario Andrade
*
* A simple dynamic sized string class
*/


#ifndef _DS_CSTRING_H
#define _DS_CSTRING_H

#include "dstypes.h"
#include "dsresult.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

//daniel dantas: 
#ifdef _LINUX
#define lstrlen strlen
#endif
 

DsINT32 _vswprintf( DsPUNISTR wcs, DsINT32 maxlen,
                      DsPUNICSTR format, va_list args );
// Andre Cotrim 2008-07-08
// Uncommented the ifdef block below because its absence made GCC throw an error.
#if defined( _LINUX ) && !defined( _COMPAT_STR)
DsPSTR  _itoa( DsINT32 value, DsPSTR str, DsINT32 radix );
#endif /* _LINUX */

DsREGISTER_ERR_RESULT( DsE_ARRAY_OUTOFBOUNDS,           DsFACILITY_LIBUTIL_STRING,      10 );

#define DsSTRNEWLINE    "\r\n"
#define DsSTRLINEFEED   "\r"
#define DsNEWLINE       '\n'
#define DsLINEFEED      '\r'
#define DsWHITESPACE    ' '
#define DsNULLCHAR      '\0'

class DsCBuffer;

class DsCStrBase
{
protected:
	DsPSTR m_pStr;
	DsUINT32 m_uiSize;

	virtual DsPSTR Alloc( DsINT32 size ) = 0;
	virtual void Release( DsPSTR & refp ) = 0;

public:
    DsCStrBase();
    virtual ~DsCStrBase();

	/* return count of string items (length) */
    DsUINT32                GetSize() const { return m_uiSize; }
    DsUINT32                GetMaxSize() const { return DSLIB_STRING_MAX_STATIC_STRING; }
    /* returns the total number of bytes including terminating zero */
	DsUINT32                GetRawSize() const { return m_uiSize + sizeof( DsCHAR ); }
    
    bool                    IsEmpty() const { return m_uiSize == 0; }

    DsCStrBase&             Format( DsPCSTR pstr, ... );

	virtual DsCStrBase&     Reserve( DsUINT32 cbLen );

	DsPSTR                  GetBuffer( DsUINT32 cbLen = 0 );
    void                    ReleaseBuffer();

	/* returns a non-frozen const pointer to buffer */
    operator                DsPCSTR() const { return GetStr(); }

    DsPCSTR                 GetStr() const { return (DsPCSTR) m_pStr; }

    void                    Erase();

    DsCStrBase&             ToLower();
    DsUINT32                ToInteger() const;

	bool                    StartsWith( DsPCSTR pstrMatch ) const;
	bool                    StartsWith( DsPCSTR pstrMatch, DsCStrBase& refStrRest ) const;
    
    DsPCSTR                 RemovePath() const;
    DsCStrBase&             RemoveFileSpec();
    DsPCSTR                 ReplaceFromEnd( DsCHAR cFrom, DsCHAR cTo );
    DsCStrBase&             Replace( DsCHAR cFrom, DsCHAR cTo );
    DsPCSTR                 ReplaceFirstAndGetNext( DsCHAR cFrom, DsCHAR cTo );
    DsCStrBase&             FilterFrom( const DsCStrBase& refstrFrom, DsCHAR c );
    DsCStrBase&             URLEncodeFrom( const DsCStrBase& refstrFrom );
    DsCStrBase&             URLDecodeFrom( const DsCStrBase& refstrFrom );
	
    DsCStrBase&             Encode64From( const DsUINT8* pBuffer, DsUINT32 cbBuffer );

    // concat
	virtual DsCStrBase&     Assign( DsPCSTR pstr, DsUINT32 nMax = DSLIB_STRING_MAX_STRING ) = 0;
    virtual DsCStrBase&     Assign( DsPUNICSTR punistr, DsUINT32 nMax = DSLIB_STRING_MAX_STRING ) = 0;
	DsCStrBase&             operator = ( DsPCSTR pstr );
	DsCStrBase&             operator = ( const DsCBuffer& refbuf );
	DsCStrBase&             operator = ( const DsCStrBase& refstr );
	
    virtual DsCStrBase&     operator += ( DsPCSTR pstr );
	virtual DsCStrBase&     operator += ( DsUINT32 val );
	
    bool                    Equals( DsPCSTR pstr, DsBOOL bCaseInsensitive = DsFALSE );

	// logic operator
	bool					operator < ( DsPCSTR pstr ) const;
	friend bool             operator < ( DsPCSTR pstr, const DsCStrBase& refstr );
	friend bool             operator < ( const DsCStrBase& refstr, DsPCSTR pstr );
	friend bool             operator < ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 );
	friend bool             operator == ( DsPCSTR pstr, const DsCStrBase& refstr );
	friend bool             operator == ( const DsCStrBase& refstr, DsPCSTR pstr );
	friend bool             operator == ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 );
	friend bool             operator != ( DsPCSTR pstr, const DsCStrBase& refstr );
	friend bool             operator != ( const DsCStrBase& refstr, DsPCSTR pstr );
	friend bool             operator != ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 );

	// random access
	const DsCHAR&           At ( DsUINT32 i ) const;
}; // end DsCString

/////////////////////////////////////////////////////////////////////////////////////////////
// DsCBuffer

class DsCBuffer
{
public:
	typedef DsUINT8 T;

private:
	// neg if user buffer
	DsINT32 m_lSize;
	DsINT32 m_lActualSize;

	T* operator = ( T* lp );

	void Release()
	{
		if ( m_lSize > 0 )
		{
			Free();
			m_lSize = 0;
            m_lActualSize = 0;
		}

		m_p = DsNULL;
	}

protected:
	T* m_p;

	virtual void Alloc( DsINT32 lSize );
	virtual void Free();

public:
	DsCBuffer() : m_lSize( 0 ), m_lActualSize( 0 ), m_p( DsNULL ) {}

	DsCBuffer( DsINT32 lSize ) : 
            m_lSize( 0 ),
            m_lActualSize( 0 ),
            m_p( DsNULL )
	{
		SetSize( lSize );
	}

	DsCBuffer( DsPCSTR p, DsUINT32 ulSize ) :
		    m_lSize( - ( DsINT32 ) ulSize ), 
            m_lActualSize( ulSize ),
            m_p( ( T* ) p )
	{
	}

	DsCBuffer( DsPVOID pVoid, DsUINT32 ulSize ) :
		    m_lSize( - ( DsINT32 ) ulSize ), 
            m_lActualSize( ulSize ),
            m_p( ( T* ) pVoid )
	{
	}

	DsCBuffer( const DsCStrBase& refstr ) :
		    m_lSize( - ( DsINT32 ) refstr.GetRawSize() ), 
            m_lActualSize( refstr.GetSize() ),
            m_p( ( T* ) refstr.GetStr() )
	{
	}

	virtual ~DsCBuffer()
	{
		Release();
	}

    const T* GetPtr() const
    {
        return m_p;
    }

	operator T* () const
	{
		return ( T* ) m_p;
	}

	T& operator * () const
	{
		return * m_p;
	}

	T& operator [] ( DsINT32 i ) const
	{
		return m_p[ i ];
	}

	T* Attach( T* lp )
	{
		Release();

		return ( T* ) ( m_p = lp );
	}

	T* Detach()
	{
		T* lp = m_p;
		m_p = DsNULL;
		m_lSize = 0;
        m_lActualSize = 0;
		return lp;
	}

	DsUINT32 GetSize() const
	{
		return m_lSize < 0 ? -m_lSize : m_lSize;
	}

    DsUINT32 GetActualSize() const
    {
        return m_lActualSize;
    }

	void SetSize( DsUINT32 ulSize )
	{
		if ( ulSize > GetSize() )
		{
			Release();

			Alloc( ulSize );
		}

		if ( ulSize == 0 )
		{
			Release();
		}

		m_lSize = m_p ? ulSize : 0;
	}

    void Reset();

	void SetSizeAligned( DsUINT32 ulSize, DsUINT32 nBytesAligned )
	{
        if ( ulSize % nBytesAligned )
        {
            ulSize += nBytesAligned - ( ulSize % nBytesAligned );
        }

        SetSize( ulSize );

        Reset();
    }

    void SetActualSize( DsUINT32 cb )
    {
        m_lActualSize = cb;
    }

    void Copy( const DsPVOID psrc, DsUINT32 cb )
    {
        SetSize( cb );

        DsCBuffer::Copy( m_p, psrc, cb );

        m_lActualSize = cb;
    }

    void Copy( const DsCBuffer& refInput )
    {
        Copy( ( DsPVOID ) refInput.GetPtr(), refInput.GetActualSize() );
    }

    void Append( const DsCBuffer& refInput )
    {
        Append( ( DsPVOID ) refInput.GetPtr(), refInput.GetActualSize() );
    }

    void Append( const DsPVOID psrc, DsUINT32 cb )
    {
        DsCBuffer::Copy( m_p + GetActualSize(), psrc, cb );

        m_lActualSize += cb;
    }

    void TrimBytes( DsUINT32 cb )
    {
        if ( cb > GetActualSize() )
        {
            cb = GetActualSize();
        }
        else if (cb < GetActualSize() )
        {
            OverlappedCopy( m_p, m_p + cb, GetActualSize() - cb );
        }

        m_lActualSize -= cb;
    }

    /** Decode64From
    * Translate alpha text back to raw bit values
    * @param    szBase64 the base64 zero terminated input string
    * @return   DsS_OK  everything cool
    *           DsE_POINTER  output buffer too short 
    *           DsE_INVALIDARG  illegal input character
    *           DsE_UNEXPECTED  incomplete input string  
    */
    DsRESULT    Decode64From( DsPCSTR szBase64 );

    static bool Equals( const DsPVOID p1, const DsPVOID p2, DsUINT32 cb );
    static void Copy( DsPVOID ptarget, const DsPVOID psrc, DsUINT32 cb );
    static void OverlappedCopy( DsPVOID ptarget, const DsPVOID psrc, DsUINT32 cb );
};

/////////////////////////////////////////////////////////////////////////////////////////////
// DsCString, DsCStrStatic[*]

class DsCString : public DsCStrBase
{
protected:
	DsPSTR Alloc( DsINT32 size );
	void Release( DsPSTR & refp );

public:
	DsCString();
    // copy only n chars
	DsCString( DsUINT32 nChars, DsPCSTR pstr );
	DsCString( const DsCBuffer& refbuf );
	DsCString( DsPCSTR pstr );//, ... );
    DsCString( DsPUNICSTR punistr );//, ... );
	DsCString( const DsCStrBase& refstr );
    DsCString( const DsCString& refstr );
	DsCString( DsINT32 val, int base = 10 );

    // base class does not need a destructor
	virtual ~DsCString();

	DsCStrBase& Assign( DsPCSTR pstr, DsUINT32 nMax = DSLIB_STRING_MAX_STRING );
    DsCStrBase& Assign( DsPUNICSTR punistr, DsUINT32 nMax = DSLIB_STRING_MAX_STRING );
	DsCString& operator = ( DsPCSTR pstr );
	DsCString& operator = ( const DsCBuffer& refbuf );
	DsCString& operator = ( const DsCStrBase& refstr );
	
    DsCStrBase& operator += ( DsPCSTR pstr );
	DsCStrBase& operator += ( DsUINT32 val );


    DsCString operator + ( DsPCSTR pstr ) const;
    DsCString operator + ( DsUINT32 val ) const;
    DsCString operator + ( const DsCStrBase& refstr ) const;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// template DsCStrStaticT
template < DsUINT32 _TnChars = DSLIB_STRING_MAX_STATIC_STRING >
class DsCStrStaticT : public DsCStrBase
{
    DsCHAR m_Str[ _TnChars + sizeof( DsCHAR ) ];

protected:

    DsPSTR Alloc( DsINT32 size )
    {
        return m_Str;
    }

    void Release( DsPSTR & refp )
    {
        refp = DsNULL;
    }

public:
    DsCStrStaticT()
    {
        *m_Str = DsNULLCHAR;
    }

    DsCStrStaticT( const DsCStrBase& refstr )
    {
        *m_Str = DsNULLCHAR;

        Assign( refstr.GetStr() );
    }

    DsCStrStaticT( const DsCStrStaticT& refstr )
    {
        *m_Str = DsNULLCHAR;

        Assign( refstr.GetStr() );
    }

    DsCStrStaticT( const DsCBuffer& refbuf )//, ... )
    {/*
    	DsCHAR str[ _TnChars + sizeof( DsCHAR ) ]; // to make sure formatting will fit we double the template argument (just a guess)
        va_list va;

        va_start( va, pstr );
        _vsnprintf( str, _TnChars, pstr, va );
        va_end( va );

        Assign( str );*/
        Assign( ( DsPCSTR ) refbuf.GetPtr(), refbuf.GetActualSize() );
    }

    DsCStrStaticT( DsPCSTR pstr )//, ... )
    {/*
    	DsCHAR str[ _TnChars + sizeof( DsCHAR ) ]; // to make sure formatting will fit we double the template argument (just a guess)
        va_list va;

        va_start( va, pstr );
        _vsnprintf( str, _TnChars, pstr, va );
        va_end( va );

        Assign( str );*/
        Assign( pstr );
    }

    DsCStrStaticT( DsPUNICSTR punistr )//, ... )
    {/*
    	DsUNICHAR unistr[ _TnChars + sizeof( DsUNICHAR ) ]; // to make sure formatting will fit we double the template argument (just a guess)
        va_list va;

        va_start( va, punistr );
        _vswprintf( unistr, _TnChars, punistr, va );
        va_end( va );

        Assign( unistr );*/
        Assign( punistr );
    }

	DsCStrStaticT( DsINT32 val, int base = 10 )
    {
	    DsCHAR str[ 16 ];

	    Assign( _itoa( val, str, base ) );
    }

    virtual ~DsCStrStaticT() {}

    DsUINT32    GetMaxSize() const { return _TnChars; }

    DsCStrBase& Reserve( DsUINT32 cbLen )
    {
        Release( m_pStr );

        m_pStr = Alloc( cbLen );

        return *this;
    }

    DsCStrBase& Assign( DsPCSTR pstr, DsUINT32 nMax = _TnChars )
    {
        *m_Str = DsNULLCHAR;

        m_pStr = Alloc( nMax );
        m_uiSize = 0;

        if ( pstr )
        {
            m_uiSize = strlen( pstr );

            if ( m_uiSize > _TnChars )
            {
                m_uiSize = _TnChars;
            }

            if ( m_uiSize )
            {
                strncpy( m_pStr, pstr, m_uiSize + sizeof( DsCHAR ) );
            }
        }

        return *this;
    }

    DsCStrBase& Assign( DsPUNICSTR punistr, DsUINT32 nMax = _TnChars )
    {
        *m_Str = DsNULLCHAR;

        m_pStr = Alloc( nMax );
        m_uiSize = 0;

        if ( punistr )
        {
            m_uiSize = wcslen( ( const wchar_t* ) punistr );

            if ( m_uiSize > ( _TnChars / sizeof( DsUNICHAR ) ) )
                m_uiSize = _TnChars / sizeof( DsUNICHAR );

            if ( m_uiSize )
            {
        	    m_uiSize = wcstombs( m_pStr, ( const wchar_t* ) punistr, m_uiSize + sizeof( DsCHAR ) );    
            }
        }

        return *this;
    }

    DsCStrStaticT& operator = ( const DsCBuffer& refbuf )
    {
        Assign( ( DsPCSTR ) refbuf.GetPtr(), refbuf.GetActualSize() );

        return *this;
    }

    DsCStrStaticT& operator = ( const DsCStrBase& refstr )
    {
	    Assign( refstr.GetStr() );

	    return *this;
    }

    DsCStrStaticT& operator = ( DsPCSTR pstr )
    {
	    Assign( pstr );

	    return *this;
    }

    DsCStrBase& operator += ( DsPCSTR pstr )
    {
        DsUINT32 uiSizeLeft;
        DsUINT32 uiSizeStr;
        DsUINT32 uiAppendSize;

        if ( IsEmpty() )
        {
            Assign( pstr );
        }
        else
        {
            if ( pstr && *pstr )
            {
                uiSizeStr  = strlen( pstr );
                uiSizeLeft = _TnChars - GetSize();

                if ( uiSizeStr && uiSizeLeft )
                {
                    uiAppendSize = DSLIB_MIN( uiSizeLeft, uiSizeStr );

                    if ( uiAppendSize )
                    {
                        strncpy( &m_Str[ m_uiSize * sizeof( DsCHAR ) ], pstr, uiAppendSize + sizeof( DsCHAR ) );
                        m_uiSize += uiAppendSize;
                        m_Str[ m_uiSize * sizeof( DsCHAR ) ] = DsNULLCHAR;
                    } // end if we need to append
                } // end if size to copy and room left
            } // end if ptrs are valid
        } // end if this string is empty

        return *this;
    }

	DsCStrBase& operator += ( DsUINT32 val )
    {
        return *this += DsCStrStaticT< _TnChars >( val );
    }

    DsCStrBase& AppendTerminator()
    {
        m_Str[ m_uiSize * sizeof( DsCHAR ) ] = DsNULLCHAR;

        return *this;
    }

	// construction
	friend DsCStrStaticT<> operator + ( DsPCSTR pstr, const DsCStrBase& refstr );
	friend DsCStrStaticT<> operator + ( const DsCStrBase& refstr, DsUINT32 val );
	friend DsCStrStaticT<> operator + ( const DsCStrBase& refstr, DsPCSTR pstr );
	friend DsCStrStaticT<> operator + ( const DsCStrBase& refstr1, const DsCStrBase& refstr2 );
};


typedef DsCStrStaticT<> DsCStrStatic;

//////////////////////////////////////////////////////////////////////////////////////////////
// template DsCStrStaticCleanT
template < DsUINT32 _TnChars = DSLIB_STRING_MAX_STATIC_STRING >
class DsCStrStaticCleanT
{
    DsCHAR m_Str[ _TnChars + sizeof( DsCHAR ) ];

public:
    DsCStrStaticCleanT()
    {
        *m_Str = DsNULLCHAR;
    }

    DsCStrStaticCleanT( const DsCStrStaticCleanT& refstr )
    {
        *m_Str = DsNULLCHAR;

        Assign( refstr.GetStr() );
    }

    DsCStrStaticCleanT( DsPCSTR pstr )//, ... )
    {/*
    	DsCHAR str[ _TnChars + sizeof( DsCHAR ) ]; // to make sure formatting will fit we double the template argument (just a guess)
        va_list va;

        va_start( va, pstr );
        _vsnprintf( str, _TnChars, pstr, va );
        va_end( va );

        Assign( str );*/
        Assign( pstr );
    }

    DsCStrStaticCleanT( DsPUNICSTR punistr )//, ... )
    {/*
    	DsUNICHAR unistr[ _TnChars + sizeof( DsUNICHAR ) ]; // to make sure formatting will fit we double the template argument (just a guess)
        va_list va;

        va_start( va, punistr );
        _vswprintf( unistr, _TnChars, punistr, va );
        va_end( va );

        Assign( unistr );*/
        Assign( punistr );
    }

	DsCStrStaticCleanT( DsINT32 val, int base = 10 )
    {
	    DsCHAR str[ 16 ];

	    Assign( _itoa( val, str, base ) );
    }

	DsPCSTR GetStr() const
	{
		return ( DsPCSTR ) m_Str;
	}

    DsCStrStaticCleanT& Assign( DsPCSTR pstr, DsUINT32 nMax = _TnChars )
    {
        *m_Str = DsNULLCHAR;

        if ( pstr )
        {
            DsUINT32 uiSize = DSLIB_MIN( lstrlen( pstr ), _TnChars );

            if ( uiSize )
            {
                lstrcpyn( m_Str, pstr, uiSize + sizeof( DsCHAR ) );
            }
        }

        return *this;
    }

    DsCStrStaticCleanT& Assign( DsPUNICSTR punistr, DsUINT32 nMax = _TnChars )
    {
        *m_Str = DsNULLCHAR;

        if ( punistr )
        {
            DsUINT32 uiSize = DSLIB_MIN( wcslen( punistr ), ( _TnChars / sizeof( DsUNICHAR ) ) );

            if ( uiSize )
            {
        	    uiSize = wcstombs( m_Str, ( const wchar_t* ) punistr, uiSize + sizeof( DsCHAR ) );    
            }
        }

        return *this;
    }

    DsCStrStaticCleanT& operator = ( const DsCStrStaticCleanT& refstr )
    {
	    Assign( refstr.GetStr() );

	    return *this;
    }

    DsCStrStaticCleanT& operator = ( DsPCSTR pstr )
    {
	    Assign( pstr );

	    return *this;
    }

    DsCStrStaticCleanT& operator += ( DsPCSTR pstr )
    {
        DsUINT32 uiSizeLeft;
        DsUINT32 uiSizeStr;
        DsUINT32 uiAppendSize;

        if ( IsEmpty() )
        {
            Assign( pstr );
        }
        else
        {
            if ( pstr && *pstr )
            {
                uiSizeStr  = lstrlen( pstr );
                uiSizeLeft = _TnChars - GetSize();

                if ( uiSizeStr && uiSizeLeft )
                {
                    uiAppendSize = DSLIB_MIN( uiSizeLeft, uiSizeStr );

                    if ( uiAppendSize )
                    {
                        lstrcpyn( &m_Str[ uiSizeStr * sizeof( DsCHAR ) ], pstr, uiAppendSize + sizeof( DsCHAR ) );
                        uiAppendSize += uiSizeStr;
                        m_Str[ uiAppendSize * sizeof( DsCHAR ) ] = DsNULLCHAR;
                    } // end if we need to append
                } // end if size to copy and room left
            } // end if ptrs are valid
        } // end if this string is empty

        return *this;
    }

	DsCStrStaticCleanT& operator += ( DsUINT32 val )
    {
        return *this += DsCStrStaticCleanT< _TnChars >( val );
    }

	/* return count of string items (length) */
	DsUINT32 GetSize() const 
	{ 
		return lstrlen( GetStr() ); 
	}

    bool IsEmpty() const 
    { 
       return *m_Str == DsNULLCHAR;
    }

    void Erase()
    {
        Assign( "" );
    }

	/* returns a non-frozen const pointer to buffer */
	operator DsPCSTR() const
	{
		return GetStr();
	}

	bool operator == ( DsPCSTR pstr ) const
    {
    	return ( GetStr() && pstr ) ? ( strcmp( GetStr(), pstr ) == 0 ) : false;
    }

	bool operator != ( DsPCSTR pstr ) const
    {
        return !( *this == pstr );
    }
};

typedef DsCStrStaticCleanT<> DsCStrStaticClean;

#endif // _DS_CSTRING_H
