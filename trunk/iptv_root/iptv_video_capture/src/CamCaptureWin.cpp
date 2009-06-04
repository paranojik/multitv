///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_media_util/global_error.h"
#include "CamCaptureWin.h"
///////////////////////////////////////////////////////////////////////////////
using namespace tv::ip;
///////////////////////////////////////////////////////////////////////////////
WinCamDeviceCtrl::WinCamDeviceCtrl()
{
    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        if (InitializeEnum() == RET_OK)
            m_flInit = true;
    }
}
///////////////////////////////////////////////////////////////////////////////
WinCamDeviceCtrl::~WinCamDeviceCtrl()
{

    m_pCreateDevEnum.Release();
    m_pEm.Release();

	CoUninitialize();
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDeviceCtrl::InitializeEnum()
{
	if (SUCCEEDED(m_pCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum)))
    {
	    ASSERT(m_pCreateDevEnum);

	    if(!m_pCreateDevEnum)
		    return RET_ERROR;

	    if (SUCCEEDED(m_pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &m_pEm, 0)))
        {
	        ASSERT(m_pEm);

	        if(!m_pEm)
		        return RET_ERROR;
        }
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDeviceCtrl::GetVideoDeviceList(VIDEODEVICEINFO_LIST& _videoDevList)
{
    if (!m_flInit)
        return RET_INIT_ERROR;

	m_pEm->Reset();
    _videoDevList.clear();

    HRESULT hr;
    bool flEndEnum = false;

    char szDevName[MAX_DEVICE_NAME];

	for(int index=0; !flEndEnum; index++)
	{
		ULONG ulFetched = 0;
		CComPtr<IMoniker> pM;
     
        hr = m_pEm->Next(1, &pM, &ulFetched);

        if (ulFetched)
        {
            IPropertyBag *pBag;
            hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		
            if (SUCCEEDED(hr))
            {
                VARIANT var;
                var.vt = VT_BSTR;

                pBag->Read(L"FriendlyName", &var, NULL);
                int size = (int)wcslen(var.bstrVal);
            
                VideoDeviceInfo devInfo;
                devInfo.index = index;
                    
                wcsrtombs(szDevName, (const wchar_t **)&var.bstrVal, size, NULL);
                szDevName[size] = '\0';

                devInfo.name = szDevName;
           
                _videoDevList.push_back(devInfo);
            }
        }
        else
            flEndEnum = true;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
VideoDevice* WinCamDeviceCtrl::GetDevice(unsigned _index)
{
    std::deque<VideoDevice*> activeIndexes = GetActiveCamIndexes();

    VideoDevice *pCamDevice;

    for (unsigned index=0; index < activeIndexes.size(); index++)
    {
        pCamDevice = activeIndexes[index];
         
        if (pCamDevice)
        {
            if (pCamDevice->GetIndex() == _index)
                return pCamDevice;
        }
        else
            return NULL;
    }
    
    pCamDevice = new CamCaptureDeviceWin();

    if (pCamDevice)
    {
        if ( pCamDevice->Init(_index) == RET_OK )
        {   
            activeIndexes.push_back(pCamDevice);
            return pCamDevice;
        }
        else
            return NULL;
    }
    else
        return NULL;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDeviceCtrl::ReleaseCamera(VideoDevice *_pCamDevice)
{
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
WinCamDevice::WinCamDevice(unsigned _camIndex) : CamDevice(_camIndex)
{
    m_flInit          = false;

	m_pCaptureBuilder = NULL;
    m_pSource         = NULL;
    m_pGrabber        = NULL;
    m_pGraph          = NULL;
    m_pControl        = NULL;
    m_pEvent          = NULL;
    m_pGrabberBase    = NULL;
    m_pWindow         = NULL;
}
///////////////////////////////////////////////////////////////////////////////
WinCamDevice::~WinCamDevice()
{
    if (m_pSource)
        m_pSource->Release();

    if (m_pGrabber)
        m_pGrabber->Release();

    if (m_pGraph)
        m_pGraph->Release();

    if (m_pControl)
        m_pControl->Release();

    if (m_pEvent)
        m_pEvent->Release();

    if (m_pGrabberBase)
        m_pGrabberBase->Release();

    if (m_pWindow)
        m_pWindow->Release();
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::Initialize()
{
    if (m_flInit)
        return RET_OK;

    if (FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter,(void **)&m_pSource)))
        return RET_ERROR;

    if (FAILED(m_pSource->QueryInterface(IID_ISampleGrabber,(void **)&m_pGrabber)))
        return RET_ERROR;

    if (FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraph)))
        return RET_ERROR;
 
    if (FAILED(m_pGrabber->QueryInterface(IID_IBaseFilter, (void **)&m_pGrabberBase)))
        return RET_ERROR;

	GetCapDevice(&m_pSource);

    if (FAILED(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,IID_ICaptureGraphBuilder2,(void **)&m_pCaptureBuilder)))
        return 0;

	if (FAILED(m_pCaptureBuilder->SetFiltergraph(m_pGraph)))
        return 0;
    
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::GetCapDevice(IBaseFilter **ppCap)
{
	// Create an enumerator
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	pCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
	
	ASSERT(pCreateDevEnum);
	if(!pCreateDevEnum)
		return RET_ERROR;

	// Enumerate video capture devices
	CComPtr<IEnumMoniker> pEm;
	pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);

	ASSERT(pEm);

	if(!pEm)
		return RET_ERROR;
	
    pEm->Reset();
	
    ULONG ulFetched = 0;
	CComPtr<IMoniker> pM;
		
    if (SUCCEEDED(pEm->Next(m_CamIndex+1, &pM, &ulFetched)))	
    {
        pM->BindToObject(0, 0, IID_IBaseFilter, (void **)ppCap);
    }

    pM.Release();

    pCreateDevEnum.Release();
    pEm.Release();

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::EnumPixFmt(std::deque<PixFmt> & _pixFmtList)
{
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::EnumFmtDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensions)
{
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
HRESULT WinCamDevice::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    IEnumPins *pEnum;
    *ppPin = NULL;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;
                hr = S_OK;
                break;
            }

            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}
///////////////////////////////////////////////////////////////////////////////
IPin * WinCamDevice::GetInPin( IBaseFilter * pFilter, int nPin )
{
    IPin *pComPin = NULL;
    GetPin(pFilter, PINDIR_INPUT, nPin, &pComPin);
    return pComPin;
}
///////////////////////////////////////////////////////////////////////////////
IPin * WinCamDevice::GetOutPin( IBaseFilter * pFilter, int nPin )
{
    IPin *pComPin = NULL;
    GetPin(pFilter, PINDIR_OUTPUT, nPin, &pComPin);
    return pComPin;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::StartCapture(PixFmt _pixFmt, unsigned _width, unsigned _height)
{
    IAMStreamConfig *pConfig = NULL;

	HRESULT hRet;
	if (FAILED(hRet = m_pCaptureBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, 0, m_pSource, IID_IAMStreamConfig, (void**)&pConfig)))
	{
        return RET_ERROR;
	}

    int iCount = 0, 
		iSize  = 0;

    if (FAILED(pConfig->GetNumberOfCapabilities(&iCount, &iSize)))
        return RET_ERROR;

    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
    {
        // Use the video capabilities structure.

        for (int iFormat = 0; iFormat < iCount; iFormat++)
        {
            VIDEO_STREAM_CONFIG_CAPS scc;
            AM_MEDIA_TYPE *pmtConfig;
            HRESULT hr = pConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);
            
			if (SUCCEEDED(hr))
            {

                if ((pmtConfig->majortype == MEDIATYPE_Video) &&
                    /*(pmtConfig->subtype == MEDIASUBTYPE_RGB24) &&*/
                    (pmtConfig->formattype == FORMAT_VideoInfo) &&
                    (pmtConfig->cbFormat >= sizeof (VIDEOINFOHEADER)) &&
                    (pmtConfig->pbFormat != NULL))
                {
                    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
                    // pVih contains the detailed format information.
                    
					LONG lWidth  = pVih->bmiHeader.biWidth;
                    LONG lHeight = pVih->bmiHeader.biHeight;

				    if ((lWidth == _width) && (lHeight == _height))
					{    
						hr = pConfig->SetFormat(pmtConfig);
					    break;
					}
			    }

                // Delete the media type when you are done.
                DeleteMediaType(pmtConfig);
            }
		}
	}

    if (FAILED(m_pGraph->AddFilter(m_pSource, L"Source" )))
        return RET_ERROR;

    if (FAILED(m_pGraph->AddFilter(m_pGrabberBase, L"Grabber")))
        return RET_ERROR;

    CMediaType GrabType;
    GrabType.SetType( &MEDIATYPE_Video );
    GrabType.SetSubtype( &MEDIASUBTYPE_RGB24 );

    if (FAILED(m_pGrabber->SetMediaType( &GrabType )))
        return RET_ERROR;

    IPin *pSourcePin, 
         *pGrabPin;

    pSourcePin = GetOutPin(m_pSource, 0);
    pGrabPin   = GetInPin(m_pGrabberBase, 0);

    if (FAILED(m_pGraph->Connect( pSourcePin, pGrabPin )))
        return RET_ERROR;

    AM_MEDIA_TYPE mt;

    if (FAILED(m_pGrabber->GetConnectedMediaType(&mt)))
        return RET_ERROR;

    VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
    
    m_frameGrabber.SetDimensions(vih->bmiHeader.biWidth, vih->bmiHeader.biHeight);
    
    FreeMediaType( mt );
    
    IPin *pGrabOutPin = GetOutPin( m_pGrabberBase, 0);

    if (FAILED(m_pGraph->Render( pGrabOutPin )))
        return RET_ERROR;

    if (FAILED(m_pGrabber->SetBufferSamples( FALSE )))
        return RET_ERROR;

    if (FAILED(m_pGrabber->SetCallback(&m_frameGrabber,1)))
        return RET_ERROR;

    if (FAILED(m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&m_pWindow)))
        return RET_ERROR;

    if (m_pWindow)
    {    
        if (FAILED(m_pWindow->put_AutoShow(OAFALSE)))
            return RET_ERROR;
    }

    if (FAILED(m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pControl)))
        return 0;

    if (FAILED(m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent)))
        return 0;

    m_pControl->Run();

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::EndCapture()
{
	if (m_pControl)
        m_pControl->Stop();

    if (m_pEvent)
	{
		long evCode = 0;

        if (FAILED(m_pEvent->WaitForCompletion( INFINITE, &evCode )))
            return RET_ERROR;
    }

	m_pGraph->Abort();
    
	return RET_OK;

}
///////////////////////////////////////////////////////////////////////////////
unsigned WinCamDevice::GrabFrame(CVideoFrame & _videoFrame)
{
    m_frameGrabber.GetLastFrame(_videoFrame);

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSampleGrabberCB::QueryInterface(REFIID riid, void ** ppv)
{
   CheckPointer(ppv,E_POINTER);
        
    if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
        return NOERROR;
    }    

    return E_NOINTERFACE;
}
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSampleGrabberCB::SampleCB( double SampleTime, IMediaSample * pSample )
{
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSampleGrabberCB::BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
{
    if (!pBuffer)
        return E_POINTER;

    m_Semaph.Wait();

    m_curFrame.SetData(pBuffer, lBufferSize, GetTickCount(), m_uWidth, m_uHeight, RGB24);
    m_NewFrame = true; 
    
    m_Semaph.Signal();
    
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
bool CSampleGrabberCB::GetLastFrame(CVideoFrame & _lastFrame)
{
    bool ret = true;

    m_Semaph.Wait();

    if (m_NewFrame)
    {
        _lastFrame = m_curFrame;
        m_NewFrame = false;
    }
    else
        ret = false;

    m_Semaph.Signal();
    
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
CamCaptureDeviceWin::CamCaptureDeviceWin()
{
    m_pCamDevice = NULL;
}
///////////////////////////////////////////////////////////////////////////////
CamCaptureDeviceWin::~CamCaptureDeviceWin()
{
    if (m_pCamDevice)
        delete m_pCamDevice;

	Close();
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::Init(unsigned _index)
{
    unsigned ret = RET_ERROR;

    if (!m_pCamDevice)
    {
        m_pCamDevice = new WinCamDevice(_index);

        if (m_pCamDevice && (m_pCamDevice->Initialize() == RET_OK) )
        {    
            m_index = _index;          
            
            ret = RET_OK;
        }
        else
            ret = RET_LOW_MEMORY;
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt)
{
    if (_pixFmt != RGB24)
        return RET_INVALID_ARG;

    m_width  = _width;
    m_height = _height;
    m_pixFmt = _pixFmt;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::StartCapture()
{
    unsigned ret;

    if  (m_width && m_height && (m_pixFmt != NO_PIX_FMT))
    {
        if (m_pCamDevice)
            ret = m_pCamDevice->StartCapture(m_pixFmt, m_width, m_height);
        else
            ret = RET_INIT_ERROR;
    }
    else
        ret = RET_INIT_ERROR;

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::StopCapture()
{
    unsigned ret = RET_OK;

    if (m_pCamDevice)
        ret = m_pCamDevice->EndCapture();
    else
        ret = RET_ERROR;
   
    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::Close()
{
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CamCaptureDeviceWin::GrabFrame(CVideoFrame & _videoFrame)
{
    unsigned ret = RET_OK;

    if (m_pCamDevice)
        ret = m_pCamDevice->GrabFrame(_videoFrame);
    else
        ret = RET_ERROR;

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
