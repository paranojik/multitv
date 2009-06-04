///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>
#include <deque>
#include "iptv_media_util/global_error.h"
#include "iptv_media_util/pixel_formats.h"
#include "iptv_media_util/pixel_conversion.h"
#include "iptv_media_util/CVideoFrame.h"
#include "ScreenCaptureLinux.h"
///////////////////////////////////////////////////////////////////////////////
#define _DEBUG
#ifdef _DEBUG
#include <iostream>
using namespace std;
#endif
///////////////////////////////////////////////////////////////////////////////
using namespace tv::ip;
///////////////////////////////////////////////////////////////////////////////
unsigned ScreenDeviceLinux::Init(unsigned _index)
{
	if (!m_display)
	{
		// Open a new connection to the X server
		m_display = XOpenDisplay( NULL );

		if(!m_display)
			return RET_XSERVER_CONNECTION_ERROR;
	}

	// Get the root window
	m_window = XDefaultRootWindow( m_display );

	// Get window info
	if( !XGetWindowAttributes(m_display, m_window, &m_windowInfo) )
	{
		XCloseDisplay( m_display );
		return RET_WINDOW_ATTR_ERROR;
	}

	// Fill internal attributes
	m_index = _index;
    m_width  = m_windowInfo.width;
    m_height = m_windowInfo.height;
	
	switch( m_windowInfo.depth )
	{
	case 16:
	case 24:
	case 32:
		m_pixFmt = RGBA32;
		break;
	default:
		m_pixFmt = UNKNOWN_PIX_FMT;
	}

	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned ScreenDeviceLinux::Close()
{
	if (!m_display)
		return RET_XSERVER_NOT_CONNECTED;

		//XDestroyWindow(m_display, m_window);	// Need to check
		XCloseDisplay( m_display );

	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned ScreenDeviceLinux::GrabFrame(CVideoFrame & _videoFrame)
{
	if (m_requestedPixFmt != RGB24)
		return RET_COULD_NOT_CONVERT_PIX_FMT;

	XImage* image = NULL;
	image = XGetImage( m_display, m_window,
                       m_windowInfo.x    , m_windowInfo.y     ,
                       m_windowInfo.width, m_windowInfo.height,
                       AllPlanes, ZPixmap );

	if (!image)
		return RET_XSERVER_CANT_GET_IMAGE;
//-------------------
	unsigned bufferStreched_lenght = m_requestedWidth * m_requestedHeight * 3;
	BYTE*    bufferStreched        = new BYTE[bufferStreched_lenght];
	unsigned bufferFinal_lenght    = m_requestedWidth * m_requestedHeight * 3;
	BYTE*    bufferFinal           = new BYTE[bufferFinal_lenght];

	// Strech to YUV420
	if (StretchPicture((BYTE*)image->data,
					   image->bytes_per_line * image->height,
					   image->width,
					   image->height,
					   RGBA32,
                       bufferStreched,
					   bufferStreched_lenght,
					   m_requestedWidth,
					   m_requestedHeight) != RET_OK)
	{
		delete[] bufferStreched;
		XDestroyImage( image );
		return RET_STRECH_ERROR;
	}

	// Convert to RGB24 or any other requested format
	if (ConvertPixFmt(bufferStreched,
                       bufferStreched_lenght,
                       YUV420P,
                       bufferFinal,
                       bufferFinal_lenght,
                       m_requestedPixFmt,
                       m_requestedWidth,
                       m_requestedHeight) != RET_OK)
	{
		XDestroyImage( image );
		delete[] bufferFinal;
		delete[] bufferStreched;
		return RET_CONVERSION_ERROR;
	}

	// Fill CVideoFrame
	_videoFrame.AttachData(bufferFinal, bufferFinal_lenght);
	_videoFrame.SetVideoFormat( m_requestedWidth, m_requestedHeight, m_requestedPixFmt);
	
	XDestroyImage( image );
	delete[] bufferStreched;
	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned ScreenEnumLinux::GetVideoDeviceList(VIDEODEVICEINFO_LIST& _videoDevList)
{
	VideoDeviceInfo deviceInfo;
	deviceInfo.index = -1; // Screen devices have ID < 0
	deviceInfo.name  = "Linux Screen Capture [X11]";
	_videoDevList.clear();
	_videoDevList.push_back(deviceInfo);
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned ScreenEnumLinux::ReleaseCamera(VideoDevice *_pCamDevice)
{
	if (_pCamDevice)
	{
		// Copy into temporary pointer to prevent multithreading issues
		VideoDevice* temp = _pCamDevice;
		_pCamDevice = NULL;
		temp->Close();
		delete temp;
	}
	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
VideoDevice* ScreenEnumLinux::GetDevice(unsigned _index)
{
	VideoDevice* device = new ScreenDeviceLinux();
	device->Init(_index);
	return device;
}
///////////////////////////////////////////////////////////////////////////////
