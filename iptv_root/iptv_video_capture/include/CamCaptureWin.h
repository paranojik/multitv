#pragma once
#ifndef CAM_CAPTURE_WIN
#define CAM_CAPTURE_WIN
///////////////////////////////////////////////////////////////////////////////
#include <streams.h>
#include <qedit.h>
#include <atlbase.h>
#include "iptv_shared/shared/semaphore.h"
#include "VideoCapture.h"
#include "CamCapture.h"
///////////////////////////////////////////////////////////////////////////////
namespace tv {
namespace ip {
///////////////////////////////////////////////////////////////////////////////
class CSampleGrabberCB : public ISampleGrabberCB 
{
private:

    bool        m_NewFrame;

    unsigned    m_uWidth,
                m_uHeight;

    CVideoFrame m_curFrame;
    
    _SEMAPHORE  m_Semaph;

public:

    CSampleGrabberCB()
    {
        m_NewFrame = false;

        m_uWidth  = 0;
        m_uHeight = 0;
    }

    void SetDimensions(ULONG _uWidth, ULONG _uHeight)
    {
        m_uWidth  = _uWidth;
        m_uHeight = _uHeight;
    }

    STDMETHODIMP_(ULONG) AddRef()  { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
    STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample );
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize);

    bool GetLastFrame(CVideoFrame & _lastFrame);
};
///////////////////////////////////////////////////////////////////////////////
class WinCamDevice : public CamDevice
{
private:

    bool m_flInit;

    IBaseFilter    * m_pSource;
    ISampleGrabber * m_pGrabber;
    IGraphBuilder  * m_pGraph;
    IMediaControl  * m_pControl;
    IMediaEvent    * m_pEvent;
    IBaseFilter    * m_pGrabberBase;
    IVideoWindow   * m_pWindow;
	ICaptureGraphBuilder2 *m_pCaptureBuilder;

    CSampleGrabberCB m_frameGrabber;

    HRESULT GetPin  (IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
    
    IPin * GetInPin (IBaseFilter * pFilter, int nPin);
    IPin * GetOutPin(IBaseFilter * pFilter, int nPin);

    unsigned GetCapDevice(IBaseFilter **ppCap);

public:

    WinCamDevice(unsigned _camIndex);
    ~WinCamDevice();
      
    virtual unsigned EnumPixFmt(std::deque<PixFmt> & _pixFmtList);    
    virtual unsigned EnumFmtDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensions); 
    virtual unsigned Initialize();
    virtual unsigned StartCapture(PixFmt _pixFmt, unsigned _width, unsigned _height);
    virtual unsigned EndCapture();
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame);
};
///////////////////////////////////////////////////////////////////////////////
class WinCamDeviceCtrl : public DeviceEnumerator
{
private:   
    
    CComPtr<ICreateDevEnum> m_pCreateDevEnum;
    CComPtr<IEnumMoniker>   m_pEm;

    unsigned InitializeEnum();

public:

    WinCamDeviceCtrl();
    ~WinCamDeviceCtrl();

    virtual unsigned     GetVideoDeviceList (VIDEODEVICEINFO_LIST& _videoDevList);
    virtual unsigned     ReleaseCamera      (VideoDevice *_pCamDevice);
	virtual VideoDevice* GetDevice          (unsigned _index);
};
///////////////////////////////////////////////////////////////////////////////
class CamCaptureDeviceWin : public CameraDevice
{
    WinCamDevice *m_pCamDevice;

	// Compatibility method. Only valid for Linux build
	virtual unsigned SelectBestFmt(unsigned & _uSelectedWidth, 
                                   unsigned & _uSelectedHeight,
                                   PixFmt   & _uSelectedPixFmt)         { return RET_ERROR; }

public:

    CamCaptureDeviceWin();
    ~CamCaptureDeviceWin();

    unsigned Init(unsigned _index);
    unsigned SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt);
    unsigned StartCapture();
    unsigned StopCapture();
    unsigned Close();
    unsigned GrabFrame(CVideoFrame & _videoFrame);

	// Compatibility methods. Only valid for Linux build
	virtual bool NewFrame()                                        { return true;      }
	virtual unsigned SetControlValue(int _controlId, int _value)   { return RET_ERROR; }
    virtual unsigned GetControlValue(int _controlId, int & _value) { return RET_ERROR; }
    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList)              { return RET_ERROR; }
    virtual unsigned SelectInput(unsigned _inputIndex)             { return RET_ERROR; }
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd)         { return RET_ERROR; }
	virtual unsigned GetAnagStdCount(unsigned & standardsCount)    { return RET_ERROR; }
	virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST& _videoStdList) { return RET_ERROR; }
};
///////////////////////////////////////////////////////////////////////////////
}} // Close ip.tv namespace
#endif //CAM_CAPTURE_WIN
