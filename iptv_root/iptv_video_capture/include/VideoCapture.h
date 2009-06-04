#pragma once
#ifndef IPTV_VIDEO_CAPTURE
#define IPTV_VIDEO_CAPTURE
///////////////////////////////////////////////////////////////////////////////
#include <string>
#include <deque>
#include "iptv_media_util/CVideoFrame.h"
#include "iptv_media_util/media_utilities.h"
#include "iptv_media_util/global_error.h"
///////////////////////////////////////////////////////////////////////////////
namespace tv {
namespace ip {
///////////////////////////////////////////////////////////////////////////////
#define MAX_DEVICE_NAME 256
#define VIDEODEVICEINFO_LIST std::deque<VideoDeviceInfo>
#define VIDEODEVICE_LIST     std::deque<VideoDevice*>
#define DEVICEINPUT_LIST     std::deque<DeviceInput>
#define ANALOGVIDEO_LIST     std::deque<AnagVideoStd>
///////////////////////////////////////////////////////////////////////////////
// Forward declarations
class DeviceEnumerator;
///////////////////////////////////////////////////////////////////////////////
enum AnagVideoStd
{
	PAL_B, PAL_B1, PAL_G, PAL_H, PAL_I, PAL_D, PAL_D1, PAL_K, 
    NTSC_M, NTSC_M_KR,NTSC_443, 
    SECAM_B, SECAM_D, SECAM_G, SECAM_H, SECAM_K,
    SECAM_K1, SECAM_L, SECAM_LC, 
    PAL_Nc, PAL_M, PAL_N, NTSC_M_JP, PAL_60, ATSC_8_VSB, ATSC_16_VSB
};
//enum inputType {INPUT_CAMERA, INPUT_COMPOSITE, INPUT_SVIDEO, INPUT_TUNER};
///////////////////////////////////////////////////////////////////////////////
struct DeviceInput
{
    unsigned    index;
    std::string name;
};
///////////////////////////////////////////////////////////////////////////////
struct VideoDeviceInfo
{
    int			index;
    std::string name;
};
///////////////////////////////////////////////////////////////////////////////
class VideoDevice
{
protected:
	PixFmt   m_pixFmt; 
    unsigned m_width,
             m_height;
    int      m_index;

public:
	VideoDevice()  
    {
        m_width  = 0;
        m_height = 0;
        m_pixFmt = NO_PIX_FMT;
        m_index  = -1;
    }
	int GetIndex() { return m_index; }

	virtual unsigned Init(unsigned _index) = 0;
	virtual unsigned SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt = RGB24) = 0;
    virtual unsigned StartCapture() = 0;
    virtual unsigned StopCapture() = 0;
    virtual unsigned Close() = 0;
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame) = 0;
    virtual bool     NewFrame() = 0;
	
	unsigned GetFormat(unsigned & _uWidth, unsigned & _uHeight, PixFmt   & _uPixFmt)
	{
		_uWidth  = m_width;
		_uHeight = m_height;
		_uPixFmt = m_pixFmt;
		return RET_OK;
	}
};
///////////////////////////////////////////////////////////////////////////////
class VideoDeviceEnumerator
{
protected:
	DeviceEnumerator* m_CameraDevEnum;
	DeviceEnumerator* m_ScreenDevEnum;

public:
	VideoDeviceEnumerator();

	VideoDevice* GetVideoDevice     ( int _deviceIndex );
	unsigned     GetVideoDeviceList ( VIDEODEVICEINFO_LIST& _deviceList,
									  bool _cameras = true,
									  bool _screen = false );	
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}}     // namespace tv::ip
#endif //IPTV_VIDEO_CAPTURE
