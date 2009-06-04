///////////////////////////////////////////////////////////////////////////////
#include "iptv_shared/shared/compat.h"
#include "iptv_media_util/global_error.h"
#include "VideoCapture.h"
#include "CamCapture.h"
#ifdef WIN32
#include "iptv_video_capture/include/CamCaptureWin.h"
//#include "iptv_video_capture/include/ScreenCaptureWin.h"
#endif
#ifdef _LINUX
#include "iptv_video_capture/include/CamCaptureLinux.h"
#include "iptv_video_capture/include/ScreenCaptureLinux.h"
#endif
///////////////////////////////////////////////////////////////////////////////
namespace tv{
namespace ip{
///////////////////////////////////////////////////////////////////////////////
VideoDeviceEnumerator::VideoDeviceEnumerator()
{
	m_CameraDevEnum = NULL;
	m_ScreenDevEnum = NULL;
#ifdef WIN32
	m_CameraDevEnum = new WinCamDeviceCtrl();
//	m_ScreenDevEnum = new ScreenEnumWin();
#endif
#ifdef _LINUX
	m_CameraDevEnum = new LinuxCamDeviceCtrl();
	m_ScreenDevEnum = new ScreenEnumLinux();
#endif
}
///////////////////////////////////////////////////////////////////////////////
unsigned VideoDeviceEnumerator::GetVideoDeviceList (VIDEODEVICEINFO_LIST& _deviceList, bool _cameras, bool _screen)
{
	VIDEODEVICEINFO_LIST cameraDevList;
	VIDEODEVICEINFO_LIST screenDevList;
	VIDEODEVICEINFO_LIST::iterator it;
	unsigned ret;
	
	// Get all device lists
	if( m_CameraDevEnum && _cameras )
	{
		ret = m_CameraDevEnum->GetVideoDeviceList(cameraDevList);
		
		if ( ret != RET_OK )
			return ret;

		for ( it = cameraDevList.begin(); it != cameraDevList.end(); it++ )
			_deviceList.push_back(*it);
	}
	if( m_ScreenDevEnum && _screen )
	{
		ret = m_ScreenDevEnum->GetVideoDeviceList(screenDevList);

		if ( ret != RET_OK )
			return ret;

		for ( it = screenDevList.begin(); it != screenDevList.end(); it++ )
			_deviceList.push_back(*it);
	}

	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
VideoDevice* VideoDeviceEnumerator::GetVideoDevice (int _deviceIndex)
{
	VideoDevice* device = NULL;

	if ( _deviceIndex >= 0 && m_CameraDevEnum ) // Cameras have ID >=0
		device = m_CameraDevEnum->GetDevice(_deviceIndex);
	else if ( m_ScreenDevEnum )                  // Screens have ID < 0
		device = m_ScreenDevEnum->GetDevice(_deviceIndex);

	return device;
}
///////////////////////////////////////////////////////////////////////////////
}}     // namespace tv::ip
///////////////////////////////////////////////////////////////////////////////
