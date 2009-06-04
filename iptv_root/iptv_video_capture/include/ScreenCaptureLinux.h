#pragma once
#ifndef SCREEN_CAPTURE_H
#define SCREEN_CAPTURE_H
///////////////////////////////////////////////////////////////////////////////
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>
#include <deque>
#include "iptv_media_util/pixel_formats.h"
#include "iptv_media_util/global_error.h"
#include "iptv_media_util/media_utilities.h"
#include "iptv_media_util/CVideoFrame.h"
#include "VideoCapture.h"
#include "CamCapture.h"
///////////////////////////////////////////////////////////////////////////////
namespace tv {
namespace ip {
///////////////////////////////////////////////////////////////////////////////
class ScreenDeviceLinux : public VideoDevice
{
private:
    Display*          m_display;
    Window            m_window;
    XWindowAttributes m_windowInfo;
    unsigned          m_requestedWidth,
                      m_requestedHeight;
	PixFmt            m_requestedPixFmt;

public:
    ScreenDeviceLinux()
	{
		m_display         = NULL;
		m_requestedWidth  = 0;
		m_requestedHeight = 0;
		m_requestedPixFmt = RGB24;
	}

	virtual unsigned Init(unsigned _index);
	virtual unsigned Close();
	virtual bool     NewFrame()     { return true;      }
    virtual unsigned StartCapture() { return RET_OK;    }
	virtual unsigned StopCapture()  { return RET_OK;    }
	virtual unsigned GrabFrame(CVideoFrame & _videoFrame);
	virtual unsigned SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt = RGB24)
	{
		m_requestedWidth  = _width;
        m_requestedHeight = _height;
		m_requestedPixFmt = _pixFmt;
		return RET_OK;
	}
};
///////////////////////////////////////////////////////////////////////////////
class ScreenEnumLinux: public DeviceEnumerator
{
public:
	virtual unsigned GetVideoDeviceList(VIDEODEVICEINFO_LIST& _videoDevList);
    virtual unsigned ReleaseCamera(VideoDevice*_pCamDevice);
    virtual VideoDevice* GetDevice(unsigned _index);
};
///////////////////////////////////////////////////////////////////////////////
}}     // namespace tv::ip
#endif // SCREEN_CAPTURE_H
