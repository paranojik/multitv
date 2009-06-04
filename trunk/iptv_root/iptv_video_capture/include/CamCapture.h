#pragma once
#ifndef CAMCAPTURE
#define CAMCAPTURE
///////////////////////////////////////////////////////////////////////////////
#include "iptv_media_util/CVideoFrame.h"
#include "iptv_media_util/media_utilities.h"
#include "iptv_media_util/global_error.h"
#include "VideoCapture.h"
///////////////////////////////////////////////////////////////////////////////
namespace tv {
namespace ip {
///////////////////////////////////////////////////////////////////////////////
class DeviceEnumerator
{
protected:
	bool m_flInit;
    VIDEODEVICE_LIST m_deviceList;
    VIDEODEVICE_LIST& GetActiveCamIndexes() { return m_deviceList; }

public:
	DeviceEnumerator()
	{
		m_flInit = false;
	}
    ~DeviceEnumerator()
	{
		while (m_deviceList.size())
		{
			if (m_deviceList[0])
				delete m_deviceList[0];

			m_deviceList.pop_front();
		}
	}

    virtual unsigned     GetVideoDeviceList (VIDEODEVICEINFO_LIST& _videoDevList) = 0;
    virtual unsigned     ReleaseCamera      (VideoDevice* _pCamDevice)            = 0;
    virtual VideoDevice* GetDevice          (unsigned  _devIndex)                 = 0;
};
///////////////////////////////////////////////////////////////////////////////
class CameraDevice : public VideoDevice
{
protected:

    virtual unsigned SelectBestFmt(unsigned & _uSelectedWidth, 
                                   unsigned & _uSelectedHeight,
                                   PixFmt   & _uSelectedPixFmt) = 0;
public:

    virtual unsigned Init(unsigned _index) = 0;

    virtual unsigned SetControlValue(int _controlId, int _value) = 0;
    virtual unsigned GetControlValue(int _controlId, int & _value) = 0;
    
    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList) = 0;
    virtual unsigned SelectInput(unsigned _inputIndex) = 0;
    virtual unsigned GetAnagStdCount(unsigned & standardsCount) = 0;
    virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST& _videoStdList) = 0;
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd) = 0;

    virtual unsigned SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt) = 0;
    virtual unsigned StartCapture() = 0;
    virtual unsigned StopCapture() = 0;
    virtual unsigned Close() = 0;
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame) = 0;
    virtual bool NewFrame() = 0;
};
///////////////////////////////////////////////////////////////////////////////
class CamDevice
{

protected:

    bool     m_flInit;
    unsigned m_CamIndex;

public:

    CamDevice(unsigned _camIndex) {m_CamIndex = _camIndex; }
    ~CamDevice() {}

    bool DevOK() {return m_flInit;   }

    unsigned GetIndex() {return m_CamIndex; }

    virtual unsigned EnumPixFmt(std::deque<PixFmt> & _pixFmtList) = 0;    
    virtual unsigned EnumFmtDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensions) = 0;
    virtual unsigned Initialize() = 0;
    virtual unsigned StartCapture(PixFmt _pixFmt, unsigned _width, unsigned _height) = 0;
    virtual unsigned EndCapture() = 0;
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame) = 0;
};
/*
class CamDevice
{
protected:

    bool  m_bInit;
    int   m_camIndex;

public:

    CamDevice(int _camIndex)
    {
        m_bInit    = false;
        m_camIndex = _camIndex;
    }

    virtual ~CamDevice(){}

    virtual unsigned GetInputList(DILIST& _inputList) = 0;
    virtual unsigned SelectInput(unsigned _inputIndex) = 0;

    virtual unsigned GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList) = 0;    
    virtual unsigned GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensions) = 0; 
    virtual unsigned Initialize() = 0;
    virtual unsigned StartCapture() = 0;
    virtual unsigned EndCapture() = 0;
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame, unsigned _dstWidth, unsigned _dstHeight, PixFmt _dstPixFmt) = 0;
    virtual unsigned SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt) = 0;
    virtual unsigned GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt) = 0;
    virtual bool NewFrame() = 0;
};
*/
///////////////////////////////////////////////////////////////////////////////
}}     // namespace tv::ip
#endif // CAMCAPTURE
