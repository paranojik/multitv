
/*
* DsLibUtil
*
* DsCScriptEngine
*
* $Id: dscscript.h 451 2006-04-19 15:39:26Z dario $
* Dario Andrade
*
* Describes the scripting engine
*/

#ifndef _DS_CSCRIPTENGINE_H_
#define _DS_CSCRIPTENGINE_H_

#include "dscstring.h"

DsREGISTER_FACILITY( DsFACILITY_SCRIPTING, 0x100 )
DsREGISTER_ERR_RESULT( DsE_SCRIPT_PARSE_FAILED,         DsFACILITY_SCRIPTING,       10 )
DsREGISTER_ERR_RESULT( DsE_SCRIPT_FILE_NOT_FOUND,       DsFACILITY_SCRIPTING,       20 )

    enum DS_SCRIPT_VALUE_TYPE
    {
        DS_VALUE_NONE       = -1,   // LUA_TNONE
        DS_VALUE_USERDATA   = 0,    // LUA_TUSERDATA
        DS_VALUE_NIL        = 1,    // LUA_TNIL
        DS_VALUE_NUMBER     = 2,    // LUA_TNUMBER
        DS_VALUE_STRING     = 3,    // LUA_TSTRING
        DS_VALUE_TABLE      = 4,    // LUA_TTABLE
        DS_VALUE_FUNCTION   = 5     // LUA_TFUNCTION
    };

// daniel:
    
    class DsCScriptEngine;
    class DsCScriptValue
    {
        friend class DsCScriptEngine;
        friend class DsCScriptNilValue;
        friend class DsCScriptTableValue;

    protected:
        DsCScriptValue( DsINT32 iRef, const DsCScriptEngine& refEngine );

    public:
        DsCScriptValue();
        DsCScriptValue( const DsCScriptValue& refValue );
        DsCScriptValue( const DsCScriptEngine& refEngine, DsDOUBLE dval );
        DsCScriptValue( const DsCScriptEngine& refEngine, DsPCSTR szVal );
        ~DsCScriptValue();

        DsCScriptValue&     operator = ( const DsCScriptValue& refValue );

        DsBOOL              IsValid() const;

		DsBOOL				IsNil() const;
        DsCString           GetStr() const;
        DsDOUBLE            GetInt() const;

		/**
		* CallLuaFunction
		*
		* Calls a given lua function stored in this value
		*
		* @param    [in,optional] nArgs the number of arguments that will be passed (as DsCScriptValue*)
		*           [out,optional] nRets the number of returns that will be retrieved (as DsCScriptValue*)
		*           [in/out,optional] ... arguments and returns (in that order), as DsCScriptValue* pointers
		* @return   DsS_OK the function was called
		*           DsE_INVALIDARG the function was not found or is not a function
		*/
		DsRESULT            Call( DsINT32 nArgs = 0, DsINT32 nRets = 0, ... );

        /**
        *
        * Gets the lua table associated with this key
        *
        * @param    [out] refValue the table value
        *           [in] szKey the table key (string keyed hash)
        *           [in] uiIndex the table key (integer indexed, array)
        * @return   DsCScriptValue the table (check for IsValid)
        */
        DsCScriptValue      GetTableValue( DsPCSTR szKey );
        DsCScriptValue      GetTableValue( DsINT32 uiIndex );

        /**
        * GetTableLength
        *
        * Gets the length of the array associated to this table
        * It may or may not contain hash keyed elements, as it counts
        * only array integer indexed items
        *
        * @return   DsINT32 the table length (or zero for no items)
        */
        DsINT32             GetTableLength() const;

    protected:
        DsPVOID             m_hPrivateState;    // the lua_State* that this value corresponds to
        DsINT32             m_iRef;             // the lua_ref locked value

        /**
        * HasRef
        *
        * Indicates if this value is valid (contains valid reference, including a ref to nil)
        *
        * @return   DsBOOL DsTRUE if ref is valid
        */
        DsBOOL              HasRef() const;
    };

    class DsCScriptNilValue : public DsCScriptValue
    {
    public:
        DsCScriptNilValue( const DsCScriptEngine& refEngine );
    };

    class DsCScriptTableValue : public DsCScriptValue
    {
    public:
        DsCScriptTableValue( const DsCScriptEngine& refEngine );

        /**
        * SetValue
        *
        * Sets the table with the specified value indexed by specified key
        *
        * @param    [in] refKeyValue a valid key value
        *           [in] refValValue a reference to a value (including nil)
        */
        void SetValue( DsCScriptValue& refKeyValue, DsCScriptValue& refValValue );

        /**
        * AddValue
        *
        * Add to the table the specified value indexed by the table length + 1 
        * (adding to a numeric table or array)
        *
        * @param    [in] refValValue a reference to a value (excluding nil)
        */
        void AddValue( DsCScriptValue& refValValue );
    };

class DsCScriptEngine
{
public:
    DsCScriptEngine();
    virtual ~DsCScriptEngine();

    /**
    * Execute
    *
    * Executes the given script
    *
    * @param    [in] szFilename the filename of the file to be parsed and executed
    * @return   DsS_OK file is ok
    *           DsE_SCRIPT_PARSE_FAILED when script in non parseable
    */
    DsRESULT            Execute( DsPCSTR szFilename );
    DsRESULT            Execute( const DsCBuffer& refBuffer );

    /**
    * GetGlobalValue
    *
    * Get the following value from global table
    *
    * @param    [in] szValue the global value
    * @return   DsCScriptValue the scripting engine value
    */
    DsCScriptValue      GetGlobalValue( DsPCSTR szValue );

    /**
    * CallLuaFunction
    *
    * Calls a given lua function stored in this global
    *
    * @param    [in] szFunctionName the function name stored in global table
    *           [in,optional] nArgs the number of arguments that will be passed (as DsCScriptValue*)
    *           [out,optional] nRets the number of returns that will be retrieved (as DsCScriptValue*)
    *           [in/out,optional] ... arguments and returns (in that order), as DsCScriptValue* pointers
    * @return   DsS_OK the function was called
    *           DsE_INVALIDARG the function was not found
    */
    DsRESULT            CallGlobalFunction( DsPCSTR szFunctionName, DsINT32 nArgs = 0, DsINT32 nRets = 0, ... );

private:
    friend class DsCScriptValue;

    DsPVOID             m_hPrivateState;
};

#endif // _DS_CSCRIPTENGINE_H_
