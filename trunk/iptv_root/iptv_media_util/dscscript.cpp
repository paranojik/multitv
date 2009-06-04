
/*
* DsCScript
*
* Script engine implementation using Lua (http://www.lua.org)
*
* $id:$
* Dario Andrade
*
* Defines the script engine interface
*/

#include "dscscript.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
}

#ifdef WIN32
#include <windows.h>
#endif 

#define _DSLIB_SCRIPT_STACKSIZE_DEFAULT 0

#define L   ( ( lua_State* ) m_hPrivateState )

/***************************************************************************
*
* DsCScriptEngine
*
****************************************************************************/

DsCScriptEngine::DsCScriptEngine() : 
    m_hPrivateState( DsNULL )
{

}

DsCScriptEngine::~DsCScriptEngine()
{
    if ( L )
    {
        lua_close( L );

        m_hPrivateState = DsNULL;
    }
}

DsRESULT DsCScriptEngine::Execute( DsPCSTR szFilename )
{
    DsRESULT hr = DsE_POINTER;

    if ( szFilename )
    {
        if ( !L )
        {
            m_hPrivateState = lua_open( _DSLIB_SCRIPT_STACKSIZE_DEFAULT );
        }

        // see defined macro for 'L'
        if ( L )
        {
            lua_baselibopen( L );

            int iRet = lua_dofile( L, szFilename );

            switch ( iRet )
            {
                case 0:
                    
                    hr = DsS_OK;
                    
                    break;

                    // error while running the chunk
                case LUA_ERRRUN:
                    // syntax error during pre-compilation. 
                case LUA_ERRSYNTAX:
                    // memory allocation error. For such errors, Lua does not call _ERRORMESSAGE (see Section 4.7). 
                case LUA_ERRMEM:
                    // error while running _ERRORMESSAGE. For such errors, Lua does not call _ERRORMESSAGE again, to avoid loops. 
                case LUA_ERRERR:

                    hr = DsE_SCRIPT_PARSE_FAILED;

                    break;

                    // error opening the file (only for lua_dofile). In this case, you may want to check errno, call strerror, or call perror to tell the user what went wrong. 
                case LUA_ERRFILE:

                    hr = DsE_SCRIPT_FILE_NOT_FOUND;

                    break;
            } // end switch lua error
        }
        else
        {
            hr = DsE_OUTOFMEMORY;
        } // end if lua opened ok
    } // end if filename pointer

    return hr;
}

DsCScriptValue DsCScriptEngine::GetGlobalValue( DsPCSTR szValue )
{
    DsCScriptValue value;

    // get the file we're working on
    // note that before this call the file entry table should be at -1
    lua_getglobal( L, szValue );

    value.m_hPrivateState   = m_hPrivateState;
    value.m_iRef            = lua_ref( L, 1 );

    return value;
}

DsRESULT DsCScriptEngine::CallGlobalFunction( DsPCSTR szFunctionName, DsINT32 nArgs, DsINT32 nRets, ... )
{
    DsRESULT hr = DsE_POINTER;

    if ( L && szFunctionName )
    {
        lua_getglobal( L, szFunctionName );

        if ( lua_type( L, -1 ) == LUA_TFUNCTION )
        {
            hr = DsS_OK;

            va_list vl;

            va_start( vl, nRets );

            DsINT32 nRealArgs = 0;

            for ( DsINT32 i = 0; i < nArgs; i++ )
            {
                DsCScriptValue* parg = va_arg( vl, DsCScriptValue* );

                if ( parg && parg->HasRef() && lua_getref( L, parg->m_iRef ) )
                {
                    nRealArgs++;
                }
            } // end if args

            lua_call( L, nRealArgs, nRets );

            // since the last result is on the top (-1),
            // we iterate deep in stack (-nRets) until top
            for ( DsINT32 iRet = nRets; iRet > 0; iRet-- )
            {
                // copy the result into top
                lua_pushvalue( L, -iRet );

                DsCScriptValue* pret = va_arg( vl, DsCScriptValue* );

                if ( pret )
                {
                    pret->m_hPrivateState = L;
                    pret->m_iRef = lua_ref( L, 1 );
                }
                else
                {
                    lua_pop( L, 1 );
                }
            } // end if rets

            lua_pop( L, nRets );

            va_end( vl );
        }
        else
        {
            hr = DsE_INVALIDARG;

            lua_pop( L, 1 );
        } // end if type is function
    } // end if valid args

    return hr;
}

/***************************************************************************
*
* DsCScriptNilValue
*
****************************************************************************/

DsCScriptNilValue::DsCScriptNilValue( const DsCScriptEngine& refEngine ) :
    DsCScriptValue( LUA_REFNIL, refEngine )
{ 
}

/***************************************************************************
*
* DsCScriptTableValue
*
****************************************************************************/

DsCScriptTableValue::DsCScriptTableValue( const DsCScriptEngine& refEngine ) :
    DsCScriptValue( LUA_NOREF, refEngine )
{
    lua_newtable( L );

    m_iRef = lua_ref( L, 1 );
}

void DsCScriptTableValue::AddValue( DsCScriptValue& refValValue )
{
    // we need a non nil valid reference to a value
    if ( IsValid() && refValValue.IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            if ( lua_getref( L, refValValue.m_iRef ) )
            {
                // set the table at -2, indexed by the incremented length
                // to the value we have just pushed
                lua_rawseti( L, -2, lua_getn( L, -2 ) + 1 );
            } // end if the value was pushed

            lua_pop( L, 1 );
        } // end if this table was pushed
    } // end if value and this table are valid
}

void DsCScriptTableValue::SetValue( DsCScriptValue& refKeyValue, DsCScriptValue& refValValue )
{
    // the value can be nil, but must be a valid reference hence the HasRef
    if ( IsValid() && refKeyValue.IsValid() && refValValue.HasRef() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            if ( lua_getref( L, refKeyValue.m_iRef ) )
            {
                if ( lua_getref( L, refValValue.m_iRef ) )
                {
                    // table is at -3 (from top), key -2, value -1
                    lua_settable ( L, -3 );
                }
                else
                {
                    // pop key and table from stack
                    lua_pop( L, 2 );
                }
            }
            else
            {
                // pop table from stack
                lua_pop( L, 1 );
            }
        }
    } // end if this table, key and values are valid
}

/***************************************************************************
*
* DsCScriptValue
*
****************************************************************************/

DsCScriptValue::DsCScriptValue( DsINT32 iRef, const DsCScriptEngine& refEngine ) :
    m_hPrivateState( refEngine.m_hPrivateState ),
    m_iRef( iRef )
{
}

DsCScriptValue::DsCScriptValue() :
    m_hPrivateState( DsNULL ),
    m_iRef( LUA_NOREF )
{
}

DsCScriptValue::DsCScriptValue( const DsCScriptValue& refValue ) :
    m_hPrivateState( refValue.m_hPrivateState ),
    m_iRef( LUA_NOREF )
{
    if ( refValue.IsValid() )
    {
        if ( lua_getref( L, refValue.m_iRef ) )
        {
            // 1 for locking
            m_iRef = lua_ref( L, 1 );
        }
    }
}

DsCScriptValue& DsCScriptValue::operator = ( const DsCScriptValue& refValue )
{
    m_hPrivateState = refValue.m_hPrivateState;
    m_iRef          = LUA_NOREF;

    if ( refValue.IsValid() )
    {
        if ( lua_getref( L, refValue.m_iRef ) )
        {
            // 1 for locking
            m_iRef = lua_ref( L, 1 );
        }
    }

    return *this;
}

DsCScriptValue::DsCScriptValue( const DsCScriptEngine& refEngine, DsDOUBLE dval ) :
    m_hPrivateState( refEngine.m_hPrivateState ),
    m_iRef( LUA_NOREF )
{
    lua_pushnumber( L, dval );

    m_iRef = lua_ref( L, 1 );
}

DsCScriptValue::DsCScriptValue( const DsCScriptEngine& refEngine, DsPCSTR szVal ) :
    m_hPrivateState( refEngine.m_hPrivateState ),
    m_iRef( LUA_NOREF )
{
    lua_pushstring( L, szVal );

    m_iRef = lua_ref( L, 1 );
}

DsCScriptValue::~DsCScriptValue()
{
    if ( IsValid() )
    {
        lua_unref( L, m_iRef );
    }

    m_iRef              = LUA_NOREF;
    m_hPrivateState     = DsNULL;
}

DsBOOL DsCScriptValue::IsValid() const
{
    return ( m_iRef != LUA_NOREF ) && !IsNil();
}

DsBOOL DsCScriptValue::IsNil() const
{
    return m_iRef == LUA_REFNIL;
}

DsBOOL DsCScriptValue::HasRef() const
{
    return m_iRef != LUA_NOREF;
}

DsCString DsCScriptValue::GetStr() const
{
    DsPCSTR szValue = DsNULL;

    if ( IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            if ( lua_isstring( L, -1 ) )
            {
                szValue = lua_tostring( L, -1 );
            }
        } // end if lua ref could be found and is on the stack
    } // end if ref is valid

    return szValue;
}

DsDOUBLE DsCScriptValue::GetInt() const
{
    DsDOUBLE dVal = 0.0;

    if ( IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            if ( lua_isnumber( L, -1 ) )
            {
                dVal = lua_tonumber( L, -1 );
            }
        } // end if lua ref could be found and is on the stack
    } // end if ref is valid

    return dVal;
}

DsINT32 DsCScriptValue::GetTableLength() const
{
    DsINT32 iLength = 0;

    if ( IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            iLength = lua_getn( L, -1 );

            lua_pop( L, 1 );
        }
    }

    return iLength;
}

DsCScriptValue DsCScriptValue::GetTableValue( DsPCSTR szKey )
{
    DsCScriptValue value;

    if ( IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            lua_pushstring( L, szKey );

            lua_gettable( L, -2 );

            value.m_iRef            = lua_ref( L, 1 );
            value.m_hPrivateState   = m_hPrivateState;
        }
    }

    return value;
}

DsCScriptValue DsCScriptValue::GetTableValue( DsINT32 uiIndex )
{
    DsCScriptValue value;

    if ( IsValid() )
    {
        if ( lua_getref( L, m_iRef ) )
        {
            lua_pushnumber( L, uiIndex );

            lua_gettable( L, -2 );

            value.m_iRef            = lua_ref( L, 1 );
            value.m_hPrivateState   = m_hPrivateState;
        }
    }

    return value;
}

DsRESULT DsCScriptValue::Call( DsINT32 nArgs, DsINT32 nRets, ... )
{
	DsRESULT hr = DsE_INVALIDARG;

    if ( IsValid() && lua_getref( L, m_iRef ) )
    {
		if ( lua_type( L, -1 ) == LUA_TFUNCTION )
		{
			hr = DsS_OK;

			va_list vl;

			va_start( vl, nRets );

			DsINT32 nRealArgs = 0;

			for ( DsINT32 i = 0; i < nArgs; i++ )
			{
				DsCScriptValue* parg = va_arg( vl, DsCScriptValue* );

				if ( parg && parg->HasRef() && lua_getref( L, parg->m_iRef ) )
				{
					nRealArgs++;
				}
			} // end if args

			lua_call( L, nRealArgs, nRets );

			// since the last result is on the top (-1),
			// we iterate deep in stack (-nRets) until top
			for ( DsINT32 iRet = nRets; iRet > 0; iRet-- )
			{
				// copy the result into top
				lua_pushvalue( L, -iRet );

				DsCScriptValue* pret = va_arg( vl, DsCScriptValue* );

				if ( pret )
				{
					pret->m_hPrivateState = L;
					pret->m_iRef = lua_ref( L, 1 );
				}
				else
				{
					lua_pop( L, 1 );
				}
			} // end if rets

			lua_pop( L, nRets );

			va_end( vl );
		}
		else
		{
			lua_pop( L, 1 );
		} // end if type is function
	}

	return hr;
}
