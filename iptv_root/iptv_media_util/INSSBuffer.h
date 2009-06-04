#ifndef INSS_BUFFER_H
#define INSS_BUFFER_H

#include "defs.h"
#include "BufManager.h"

#ifdef _LINUX
    #define _HEAP_MAXREQ 0xFFFFFFE0
#endif

class INSSBuffer : public BufManager
{
public:
    //CMCINSSBuffer(IMediaSample* pSample);
    //CMCINSSBuffer(LPUNKNOWN pUnk = NULL);
    INSSBuffer() {m_bKeyframe = 0; }
    INSSBuffer(LPBYTE p, DWORD cb);
    virtual ~INSSBuffer();

    //STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void **ppv);
    //STDMETHOD_(ULONG, NonDelegatingAddRef)();
    //STDMETHOD_(ULONG, NonDelegatingRelease)();

    unsigned GetBuffer(LPBYTE* ppBuffer);
    unsigned GetBufferAndLength(LPBYTE* ppBuffer, LPDWORD pdwLen);
    unsigned GetLength(LPDWORD pdwLen);
    unsigned GetMaxLength(LPDWORD pdwLen);
    unsigned SetLength(DWORD dwLen);

    unsigned(GetSampleProperties)( 
        /* [in] */ DWORD cbProperties,
        /* [out] */ BYTE *pbProperties) { return 0; /*E_NOTIMPL;*/ }
    
    unsigned(SetSampleProperties)( 
        /* [in] */ DWORD cbProperties,
        /* [in] */ BYTE *pbProperties) { return 0; /*E_NOTIMPL;*/ }


    unsigned GetProperty(
		GUID_W   guidBufferProperty,
		void*  pvBufferProperty,
		DWORD* pdwBufferPropertySize);

    unsigned SetProperty(
		GUID_W   guidBufferProperty,
		void*  pvBufferProperty,
		DWORD pdwBufferPropertySize);

    //IMediaSample* DetachMediaSample() { return m_pSample.Detach(); }

//protected:
	//CComPtr<IMediaSample> m_pSample;
	BOOL m_bKeyframe;
};


#endif
