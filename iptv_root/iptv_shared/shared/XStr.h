// XStr.h: interface for the XStr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__XSTR)
#define __XSTR


#include "compat.h"


#define TSTR_DYN         30
#define TSTR_STATIC     255
#define TSTR_TEMP       TAM_LBUFFER

XREGISTER_FACILITY( XFACILITY_XSTR, 0 );
XREGISTER_ERR_RESULT( XE_ARRAY_OUTOFBOUNDS, XFACILITY_XSTR, 10 );


class XStr
{
protected:
	LPSTR  m_pStr;
	UINT32 m_uiSize,
           m_uiMaxSize;

    XStr& _Format( LPCSTR _pstr, va_list _va);

public:
    XStr( LPSTR _pStr, UINT32 _uiMaxSize );
    virtual ~XStr(void);

    virtual XStr& Append( LPCSTR _pstr ) = 0;
    virtual XStr& Assign( LPCSTR _pstr ) = 0;
    XStr& Assign( const XStr& _refstr );
    XStr& Assign( INT32 _ival, int _base = 10 );
    XStr& Format( LPCSTR _pstr, ... );

    LPCSTR GetStr    (void) const { return m_pStr;      }
    UINT32 GetSize   (void) const { return m_uiSize;    }
    UINT32 GetMaxSize(void) const { return m_uiMaxSize; }

    BOOL   IsEmpty   (void) const { return (m_uiSize==0 || m_pStr==NULL ? TRUE : FALSE); }
    BOOL   IsBlank   (void) const;

	const  char& At  ( UINT32 _ind ) const;

    BOOL   StartsWith( LPCSTR _pstrMatch ) const;
    LPCSTR SubStr    ( LPSTR _pdata, UINT _pos, UINT _size ) const;
    UINT   StrChr    ( int _c ) const;
    UINT   StrStr    ( LPCSTR _pstrMatch ) const;

    operator LPCSTR() const { return GetStr(); }

	XStr& operator =  ( const XStr& _refstr );
	XStr& operator =  ( LPCSTR _pstr );
	XStr& operator += ( const XStr& _refstr );
	XStr& operator += ( LPCSTR _pstr );

    //static const DsCString NEWLINE;
};


class XStrDyn : public XStr
{
private:
	UINT32 m_uiBlockSize;

    void Alloc  ( UINT32 _uiSize );
    void Release( void );

public:
	XStrDyn( UINT32 _uiMaxSize=0 );
	XStrDyn( const XStr& _refstr );
	XStrDyn( const XStrDyn& _refstr );
	///XStrDyn( LPCSTR _pstr, ... );
	XStrDyn( LPCSTR _pstr );

	XStr& Assign( LPCSTR _pstr );
    XStr& Assign( INT32 _ival, int _base = 10 );
    XStr& Append( LPCSTR _pstr );

	~XStrDyn(void);

    XStrDyn& operator = ( const XStr& _refstr );
    XStrDyn& operator = ( LPCSTR _pstr );
};


//class XStrStatic : public XStr
template <UINT32 TSIZE=TSTR_STATIC> class XStrStatic : public XStr
{
private:
    char m_Str[TSIZE+1];

public:
    /*
	XStrStatic( UINT32 _uiMaxSize=0 );
	XStrStatic( const XStr& _refstr );
    //XStrStatic( LPCSTR _pstr, ... );
    XStrStatic( LPCSTR _pstr );

    XStrStatic<TSIZE>& operator = ( const XStr& _refstr );
    XStrStatic<TSIZE>& operator = ( LPCSTR _pstr );

	XStr& Assign( LPCSTR _pstr );
    XStr& Assign( INT32 _ival, int _base = 10 );
    XStr& Append( LPCSTR _pstr );

	~XStrStatic(void);
    */

    XStrStatic( UINT32 _uiMaxSize=0 ) : XStr( m_Str, _uiMaxSize )
    {
        *m_Str = '\0';

        if ( m_uiMaxSize == 0 || m_uiMaxSize > TSIZE )
            m_uiMaxSize = TSIZE;
    }

    XStrStatic( const XStr& _refstr ) : XStr( m_Str, TSIZE )
    {
        *m_Str = '\0';

        Assign( _refstr.GetStr() );
    }

    XStrStatic( const XStrStatic& _refstr ) : XStr( m_Str, TSIZE )
    {
        *m_Str = '\0';

        Assign( _refstr.GetStr() );
    }

    ///XStrStatic( LPCSTR _pstr, ... ) : XStr( m_Str, TSIZE )
    XStrStatic( LPCSTR _pstr ) : XStr( m_Str, TSIZE )
    {
        /*
    	char buf[ 32 * 1024 ]; // 32k should be enough to format a string
        va_list va;

        *m_Str = '\0';

        va_start( va, _pstr );
        vsprintf( buf, _pstr, va );
        va_end  ( va );

        Assign( buf );
        */

        *m_Str = '\0';

        if (_pstr && *_pstr)
            Assign( _pstr );
    }

    XStrStatic<TSIZE>& operator = ( const XStr& _refstr )
    {
	    Assign( _refstr.GetStr() );

	    return *this;
    }

    XStrStatic<TSIZE>& operator = ( LPCSTR _pstr )
    {
	    Assign( _pstr );

	    return *this;
    }

    XStr& Assign( LPCSTR _pStr )
    {
       *m_Str    = '\0';
        m_uiSize = 0;

        if ( _pStr )
        {
            m_uiSize = strlen( _pStr );

            if ( m_uiSize > m_uiMaxSize )
                m_uiSize = m_uiMaxSize;

            if ( m_uiSize )
                CopyString(m_Str, _pStr, m_uiSize);
        }

        return *this;
    }

    XStr& Assign( INT32 _ival, int _base=10 )
    {
        XStr::Assign( _ival, _base);

        return *this;
    }

    XStr& Append( LPCSTR _pstr )
    {
        UINT32 uiSizeLeft,
               uiSizeStr,
               uiAppendSize;

        if ( IsEmpty() )
            Assign( _pstr );
        else
        {
            if ( _pstr && *_pstr )
            {
                uiSizeStr  = strlen( _pstr );
                uiSizeLeft = m_uiMaxSize - GetSize();

                if ( uiSizeStr && uiSizeLeft )
                {
                    uiAppendSize = XMIN( uiSizeLeft, uiSizeStr );

                    if ( uiAppendSize )
                    {
                        strncpy(&m_Str[m_uiSize], _pstr, uiAppendSize);
                        m_uiSize += uiAppendSize;
                        m_Str[m_uiSize] = '\0';
                    }
                }
            }
        }

        return *this;
    }
};


bool operator == ( LPCSTR _pstr, const XStr& _refstr );
bool operator == ( const XStr& _refstr, LPCSTR _pstr );
bool operator == ( const XStr& _refstr1, const XStr& _refstr2 );
bool operator != ( LPCSTR _pstr, const XStr& _refstr );
bool operator != ( const XStr& _refstr, LPCSTR _pstr );
bool operator != ( const XStr& _refstr1, const XStr& _refstr2 );

XStrStatic<TSTR_TEMP> operator + ( LPCSTR _pstr, const XStr& _refstr );
XStrStatic<TSTR_TEMP> operator + ( const XStr& _refstr, LPCSTR _pstr );
XStrStatic<TSTR_TEMP> operator + ( const XStr& _refstr1, const XStr& _refstr2 );

//XStrStatic operator + ( LPCSTR _pstr, const XStr& _refstr );
//XStrStatic operator + ( const XStr& _refstr, LPCSTR _pstr );
//XStrStatic operator + ( const XStr& _refstr1, const XStr& _refstr2 );


#endif // !defined(__XSTR)


