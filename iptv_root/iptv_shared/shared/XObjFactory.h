#ifndef _X_OBJFACTORY_H_
#define _X_OBJFACTORY_H_


#if defined(_WIN32)
#define X_OBJPROXY_SHARED_LIB_IMPORT __declspec(dllimport)
#define X_OBJPROXY_SHARED_LIB_EXPORT __declspec(dllexport)
#elif defined(_LINUX)
#define X_OBJPROXY_SHARED_LIB_IMPORT
#define X_OBJPROXY_SHARED_LIB_EXPORT
#endif

#ifdef X_OBJPROXY_EXPORT
#define X_OBJPROXY_SHARED_LIB X_OBJPROXY_SHARED_LIB_EXPORT
#else
#define X_OBJPROXY_SHARED_LIB X_OBJPROXY_SHARED_LIB_IMPORT
#endif


#include "debug.h"

#ifdef _WIN32
class __declspec(dllexport) XIObject
#else
class XIObject
#endif
{
public:
};

/* XIObjProxy
*  Implement this interface if you want to create the Proxy pattern
*  to provide public users with object access
*/
class X_OBJPROXY_SHARED_LIB XIObjProxy
{
public:

	/*  Build()
	*   Build the encapsulated object
	*
	*   @throws XException 
	*/
	virtual void Build() = 0;

	/*  Destroy()
	*   Destroy the encapsulated object
	*   Obs: Will be called by base proxy destructor
	*
	*   @throws XException 
	*/
	virtual void Destroy() = 0;
};


//template <class T = XIObject>
class X_OBJPROXY_SHARED_LIB XProxy : public XIObjProxy
{
protected:
	XProxy(void) : m_pEncapsulatedObject( NULL ) {}

	virtual ~XProxy(void)
	{
		m_pEncapsulatedObject = NULL;
	}

    bool TestObj(void)
    {
        bool ret;

        ret = m_pEncapsulatedObject ? true : false;

        return ret;
    }

	void* GetObj(void)
	{
		if ( !m_pEncapsulatedObject ) 
		{
			XTHROW( XE_NOTINIT, ( "Proxy is not built, use XIObjProxy::Build()" ) );
		}
		
		return m_pEncapsulatedObject;
	}

	void SetObj( void* pEncObj )
	{
		if ( !pEncObj )
		{
			XTHROW( XE_OUTOFMEMORY, ( "Out of memory building object encapsulated" ))
		}

		if ( m_pEncapsulatedObject )
			Destroy();

		m_pEncapsulatedObject = pEncObj;
	}

	void ResetObj(void)
	{
		if ( m_pEncapsulatedObject )
        {
			//Destroy(); para evitar reentranca
		    m_pEncapsulatedObject = NULL;
        }
	}

	void* m_pEncapsulatedObject;
};

#endif // _X_OBJFACTORY_H_
