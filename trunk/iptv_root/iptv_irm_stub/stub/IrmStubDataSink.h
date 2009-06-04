// IrmStubDataSink.h: interface for the IrmStubDataSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_IRMSTUBDATASINK)
#define _IRMSTUBDATASINK

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the IRMSTUB30_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// IRMSTUB30_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef _WIN32
    #ifdef IRMSTUB30_EXPORTS
        #ifndef IRMSTUB_API
        #define IRMSTUB_API __declspec(dllexport)
        #endif
    #else
        #ifndef IRMSTUB_API
        #define IRMSTUB_API __declspec(dllimport)
        #endif
    #endif
#endif

#ifdef _LINUX
    #define IRMSTUB_API
#endif

#include "shared/compat.h"

struct _IrmParamU
{
    long        value;
    XStrDyn     pstr;
};


class IrmParam
{
private:
    _IrmParamU data;

 
 public:
    IrmParam(void)               { data.value = 0; data.pstr = 0; }
    IrmParam(long _value)        { data.value = _value; }
    IrmParam(const char *_pstr)  { data.pstr  = _pstr;  }

    long  GetValue (void)        { return data.value;   }
    const char *GetString(void)  { return data.pstr;    }
};


class IRMSTUB_API IrmStubDataSink
{
public:
	IrmStubDataSink();
	virtual ~IrmStubDataSink();
    virtual void Fire_OnMessage(long _cmd, IrmParam& _source, IrmParam& _target, 
                                IrmParam& _data, IrmParam& _compl1, IrmParam& _compl2, 
                                IrmParam& _compl3, IrmParam& _compl4, IrmParam& _compl5,
                                IrmParam& _compl6, IrmParam& _compl7) = 0;
};

#endif // _IRMSTUBDATASINK
