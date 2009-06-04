// CONTROL IDENTIFIERS
#pragma once
#ifndef DEVCAM_H
#define DEVCAM_H
///////////////////////////////////////////////////////////////////////////////
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>	// for snprintf()
#include <string>
#include <deque>
#include "iptv_shared/shared/ithread.h"
#include "iptv_shared/shared/semaphore.h"
#include "iptv_media_util/pixel_formats.h"
#include "iptv_media_util/global_error.h"
#include "iptv_media_util/media_utilities.h"
#include "VideoCapture.h"
#include "CamCapture.h"
///////////////////////////////////////////////////////////////////////////////
namespace tv {
namespace ip {
///////////////////////////////////////////////////////////////////////////////
// Control id
#define CONTRAST                        0
#define HUE                             1
#define SATURATION                      2
#define BRIGHTNESS                      3
// Constants
#define MAX_FRAME_SIZE                  1024*1024*2
#define INFINITE_TIME                   1000000
#define BUFFERS_TO_MAP                  4
#define V4L_PIXFMT_COUNT                8
#define COMMON_FRAME_DIMENSIONS_COUNT   9
// Device Access
#define MAX_PATH_LENGTH					256
#define DEVICE_PATH						"/dev"
#define DEVICE_NAME_PREFIX				"video"
#define DEVICE_NAME_STR					"/dev/video%d"
#define DEVICE_NAME_PREFIX_LENGTH		5
///////////////////////////////////////////////////////////////////////////////
enum DeviceType{V4L_DEVICE_CAPTURE, V4L2_DEVICE_CAPTURE};
///////////////////////////////////////////////////////////////////////////////
struct VideoBufferInfo
{
    unsigned char * pStartAddress;
};
///////////////////////////////////////////////////////////////////////////////
struct V4L_VideoBufferInfo : VideoBufferInfo
{
    unsigned frameSize,
             index,
             pixFmt;

    video_mbuf     mbuf;
    FrameDimension dimension;
};
///////////////////////////////////////////////////////////////////////////////
struct V4L2_VideoBufferInfo : VideoBufferInfo
{
    unsigned index, 
             bufLength;
};
///////////////////////////////////////////////////////////////////////////////
class CDevice
{
private:

    int         m_Fd;
    bool        m_DevOpened;

    std::string m_DevId;

    _SEMAPHORE  m_Semaph;

    virtual unsigned InitMMap()   = 0;
    virtual unsigned FinishMMap() = 0;
    virtual unsigned SetFramePtr(unsigned & _index)   = 0;
    virtual unsigned ReleaseFramePtr(unsigned _index) = 0;
    virtual unsigned TranslateFromPixFmt(PixFmt _srcPixFmt, unsigned & _dstPixFmt) = 0;
    virtual unsigned TranslateToPixFmt(unsigned _srcPixFmt, PixFmt & _dstPixFmt) = 0;
    virtual unsigned TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd) =0;
    virtual unsigned TranslateFromAnagStd(AnagVideoStd _videoStd, v4l2_std_id & std_id)=0;

protected:

    IThread *  m_pGrabFrames;

    DeviceType m_DevType;

    bool       m_ReadyToCapture,
               m_ThreadRunning,
               m_bNewFrame;

    unsigned char *m_pCurFrame;
    unsigned       m_CurFrameLen,
                   m_CurFrameTimestamp;

    static int GrabFrames(IThread *_pThread, void *_pParam);

public:

    CDevice(std::string & _devId); 
    virtual ~CDevice();

    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList) = 0;
    virtual unsigned SelectInput(unsigned _inputIndex) = 0;
    virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList) = 0;
    virtual unsigned GetAnagStdCount(unsigned & standardsCount) = 0;
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd) = 0;

    virtual unsigned StartCapture() = 0;
    virtual unsigned StopCapture()  = 0;
    virtual unsigned GetControlValue(int _controlId, int & _value) = 0;
    virtual unsigned SetControlValue(int _controlId, int _value)  = 0;
    virtual unsigned GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList) = 0;
    virtual unsigned GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensionsList) = 0;
    virtual unsigned SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt) = 0;
    virtual unsigned GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt) = 0;

    bool NewFrame() {return m_bNewFrame; }

    unsigned Open();
    unsigned Close();
    unsigned GetFd(int & _fd);
    unsigned GetLastFrame(CVideoFrame & videoFrame, unsigned _dstWidth, unsigned _dstHeight, PixFmt _dstPixFmt);

    std::string & GetDevId()   { return m_DevId;     }
    DeviceType GetDeviceType() { return m_DevType;   }

    bool DeviceOpened()        { return m_DevOpened; }
    bool V4L2Device()          { return ( m_DevType == V4L2_DEVICE_CAPTURE); }
    bool V4LDevice()           { return ( m_DevType == V4L_DEVICE_CAPTURE);  }

};
///////////////////////////////////////////////////////////////////////////////
class CDeviceV4L2 : public CDevice
{

private:

    std::deque<V4L2_VideoBufferInfo> m_BufferList;

    CDeviceV4L2(std::string & _devId);

    virtual unsigned InitMMap();
    virtual unsigned FinishMMap();
    virtual unsigned SetFramePtr(unsigned & _index);
    virtual unsigned ReleaseFramePtr(unsigned _index);

    virtual unsigned TranslateFromPixFmt(PixFmt _srcPixFmt, unsigned & _v4l2DstPixFmt);
    virtual unsigned TranslateToPixFmt(unsigned _v4l2SrcPixFmt, PixFmt & _dstPixFmt);

    virtual unsigned TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd);
    virtual unsigned TranslateFromAnagStd(AnagVideoStd _videoStd, v4l2_std_id & std_id);

    unsigned TranslateCtrlId(int _controlId, int & _v4l2ControlId);

    unsigned StartStreaming();
    unsigned FinishStreaming();

    unsigned GetV4L2Fmt(v4l2_format & fmt);
    unsigned SetV4L2Fmt(v4l2_format & fmt);

public:

    ~CDeviceV4L2();

    friend class CDeviceGrabber;

    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList);
    virtual unsigned SelectInput(unsigned _inputIndex);
    virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList);
    virtual unsigned GetAnagStdCount(unsigned & standardsCount);
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd);

    virtual unsigned StartCapture();
    virtual unsigned StopCapture();
    virtual unsigned GetControlValue(int _controlId, int & _value);
    virtual unsigned SetControlValue(int _controlId, int _value);
    virtual unsigned GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList);
    virtual unsigned GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensionsList);
    virtual unsigned SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt);
    virtual unsigned GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt);
};
///////////////////////////////////////////////////////////////////////////////
class CDeviceV4L : public CDevice
{

private:

    static struct V4LPixFmt 
    {
        unsigned    v4lPixFmt;
        std::string pixFmtDesc;

    } v4lPixFmtList[V4L_PIXFMT_COUNT];

    static FrameDimension CommonFrameDimensions[COMMON_FRAME_DIMENSIONS_COUNT];
    
    V4L_VideoBufferInfo m_VideoInfo;

    CDeviceV4L(std::string & _devId);

    virtual unsigned InitMMap();
    virtual unsigned FinishMMap();
    virtual unsigned SetFramePtr(unsigned & _index);
    virtual unsigned ReleaseFramePtr(unsigned _index);

    virtual unsigned TranslateFromPixFmt(PixFmt _srcPixFmt, unsigned & _v4lDstPixFmt);
    virtual unsigned TranslateToPixFmt(unsigned v4lSrcPixFmt, PixFmt & _dstPixFmt);

    virtual unsigned TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd) {return 0;}
    virtual unsigned TranslateFromAnagStd(AnagVideoStd _videoStd, v4l2_std_id & std_id){return 0;}

    unsigned GetV4LPixFmtDepth(unsigned _pixFmt, unsigned & _depth);
    unsigned GetV4LPixFmt(video_picture & _picture);
    unsigned SetV4LPixFmt(video_picture & _picture);

    unsigned GetV4LDimensions(video_window & _clipArea);
    unsigned SetV4LDimensions(video_window & _clipArea);

public:

    ~CDeviceV4L();

    friend class CDeviceGrabber;

    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList)    { return RET_OK; }
    virtual unsigned SelectInput(unsigned _inputIndex)                     { return RET_OK; }
    virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList) { return RET_OK; }
    virtual unsigned GetAnagStdCount(unsigned & standardsCount)  { standardsCount = 0; return RET_OK; }
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd) { return 0;}

    virtual unsigned StartCapture();
    virtual unsigned StopCapture();
    virtual unsigned GetControlValue(int _controlId, int & _value);
    virtual unsigned SetControlValue(int _controlId, int _value);
    virtual unsigned GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList);
    virtual unsigned GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensionsList);
    virtual unsigned SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt);
    virtual unsigned GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt);
};
///////////////////////////////////////////////////////////////////////////////
class CDeviceGrabber
{

private:

    CDevice * m_pDevice;
    
public:
  
    CDeviceGrabber();
    CDeviceGrabber(std::string & _devId);  
    ~CDeviceGrabber();

    unsigned Init(std::string & _devId);

    bool deviceGrabbed()    
    {
        bool ret;
        m_pDevice ? ret = true : ret = false; 
        
        return ret;
    }
    
    CDevice *GetDevicePtr() {return m_pDevice; }        
};
///////////////////////////////////////////////////////////////////////////////
class CameraCaptureDeviceLinux : public CameraDevice
{
private:

    CDeviceGrabber m_devGrabber;
    
    unsigned SelectBestFmt(unsigned & _uSelectedWidth, 
                           unsigned & _uSelectedHeight,
                           PixFmt   & _uSelectedPixFmt);


public:

    CameraCaptureDeviceLinux();
    ~CameraCaptureDeviceLinux();

    virtual unsigned SetControlValue(int _controlId, int _value);
    virtual unsigned GetControlValue(int _controlId, int & _value);
    
    CDevice * GetDevice();

    virtual unsigned SelectInput(unsigned _inputIndex);
    virtual unsigned GetInputList(DEVICEINPUT_LIST& _inputList);
    virtual unsigned EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList);
    virtual unsigned GetAnagStdCount(unsigned & standardsCount);
    virtual unsigned SelectAnagStd(AnagVideoStd _videoStd);

    virtual unsigned Init(unsigned _index);
    virtual unsigned SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt);
    virtual unsigned StartCapture();
    virtual unsigned GrabFrame(CVideoFrame & _videoFrame);
    virtual unsigned StopCapture();
    virtual unsigned Close();
    virtual bool NewFrame();
};
///////////////////////////////////////////////////////////////////////////////
class LinuxCamDeviceCtrl : public DeviceEnumerator
{
public:

    LinuxCamDeviceCtrl() {}
    ~LinuxCamDeviceCtrl() {}

	std::string          GetDeviceName(unsigned index);
    virtual unsigned     GetVideoDeviceList(VIDEODEVICEINFO_LIST & _videoDevList);
    virtual unsigned     ReleaseCamera(VideoDevice*_pCamDevice);
    virtual VideoDevice* GetDevice(unsigned _index);
};
///////////////////////////////////////////////////////////////////////////////
}}     // namespace tv::ip
#endif // DEVCAM_H
