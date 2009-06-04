#include "iptv_shared/shared/compat.h"
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "iptv_media_util/pixel_conversion.h"
#include "iptv_media_util/mjpeg_decoder.h"
#include "iptv_media_util/CVideoFrame.h"
#include "CamCaptureLinux.h"
#include "ScreenCaptureLinux.h"
///////////////////////////////////////////////////////////////////////////////
using namespace tv::ip;
///////////////////////////////////////////////////////////////////////////////
// CameraCaptureDeviceLinux implementation
CameraCaptureDeviceLinux::CameraCaptureDeviceLinux()
{
    m_pixFmt = NO_PIX_FMT;
    m_width  = 0;
    m_height = 0;
}
///////////////////////////////////////////////////////////////////////////////
CameraCaptureDeviceLinux::~CameraCaptureDeviceLinux()
{
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::SelectInput(unsigned _inputIndex)
{
    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice->V4L2Device())
    {
        ret = pDevice->SelectInput(_inputIndex);
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::GetInputList(DEVICEINPUT_LIST& _inputList)
{
    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    unsigned ret = RET_OK;

    DeviceInput devInput;
    devInput.index = 0;
    devInput.name  = "default input\n";
    _inputList.push_back(devInput);

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice->V4L2Device())
    {
        ret = pDevice->GetInputList(_inputList);
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList)
{
    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice->V4L2Device())
    {
        ret = pDevice->EnumAnagStd(_index, _videoStdList);
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::GetAnagStdCount(unsigned & standardsCount)
{
    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice->V4L2Device())
    {
        ret = pDevice->GetAnagStdCount(standardsCount);
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::SelectAnagStd(AnagVideoStd _videoStd)
{
    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice->V4L2Device())
    {
        ret = pDevice->SelectAnagStd(_videoStd);
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::Init(unsigned _index)
{
    unsigned ret;
    char camIndex = '0' + _index;

    std::string camDevice = "/dev/video";
    camDevice.push_back(camIndex);

    if ( (ret = m_devGrabber.Init(camDevice)) == RET_OK)
    {
        CDevice *pDevice = m_devGrabber.GetDevicePtr();

        if (pDevice) 
        {    
            if ( (ret = pDevice->Open()) == RET_OK)
                m_index = _index;  
        }
    }

    if (ret != RET_OK)
        PrintDbgMsg("CamAbstractionLayer::CamAbstractionLayer", "Initialization error", "", 0);

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::SelectFormat(unsigned _width, unsigned _height, PixFmt _pixFmt)
{

    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    if ((_pixFmt != RGB24) && (_pixFmt != YUV420P))
        return RET_INVALID_ARG;

    if (!_width || !_height)
        return RET_INVALID_ARG;

    m_pixFmt = _pixFmt;
    m_width  = _width;
    m_height = _height;

    unsigned uSelectedWidth   = 0, 
             uSelectedHeight  = 0,
             uSmallerSizeDiff = 0,
             uCurSizeDiff     = 0;

    unsigned ret = RET_OK;

    PixFmt selectedFmt = NO_PIX_FMT;

    bool flFmtDone = false;

    std::deque<PixFmt> pixFmtList;
    std::deque<FrameDimension> dimensionList;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();
    CHECK_ERROR(pDevice->GetSupportedPixelFmt(pixFmtList), RET_OK)

    for (unsigned pixFmtIndex = 0; ( pixFmtIndex < pixFmtList.size() ) && !flFmtDone; pixFmtIndex++)
    {
        if ( ( pixFmtList[pixFmtIndex] == RGB24   ) || 
             ( pixFmtList[pixFmtIndex] == BGR24   ) || 
             ( pixFmtList[pixFmtIndex] == RGBA32  ) ||
             ( pixFmtList[pixFmtIndex] == YUV420P ) ||
             ( pixFmtList[pixFmtIndex] == YUYV422 ) ||
             ( pixFmtList[pixFmtIndex] == RGB565  ) ||
             ( pixFmtList[pixFmtIndex] == RGB555  ) || // to be tested
             ( pixFmtList[pixFmtIndex] == MJPEG   ) )
        {
            pDevice->GetSupportedDimensions(pixFmtList[pixFmtIndex], dimensionList);

            for (unsigned dimensionIndex = 0; dimensionIndex < dimensionList.size(); dimensionIndex++)    
            {
                if (dimensionList[dimensionIndex].uWidth > m_width)
                    uCurSizeDiff = dimensionList[dimensionIndex].uWidth - _width;
                else
                    uCurSizeDiff = _width - dimensionList[dimensionIndex].uWidth;    

                if (dimensionList[dimensionIndex].uHeight > m_height)
                    uCurSizeDiff += dimensionList[dimensionIndex].uHeight - _height;
                else
                    uCurSizeDiff += _height - dimensionList[dimensionIndex].uHeight;

                if ( ( (dimensionIndex == 0) && (pixFmtIndex == 0) ) || (uCurSizeDiff < uSmallerSizeDiff))
                { 
                    uSelectedWidth   = dimensionList[dimensionIndex].uWidth;
                    uSelectedHeight  = dimensionList[dimensionIndex].uHeight;
            
                    uSmallerSizeDiff = uCurSizeDiff;
                    selectedFmt = pixFmtList[pixFmtIndex];
                }
             
                if (uSmallerSizeDiff == 0)
                {    
                    flFmtDone = true;
                    break;
                }
            }
        }
    }

    if ( (selectedFmt != NO_PIX_FMT) && uSelectedWidth && uSelectedHeight )
        pDevice->SetFormat(uSelectedWidth, uSelectedHeight, selectedFmt);
    else
        ret = RET_ERROR;

#ifdef _DEBUG

    unsigned uWidth,
             uHeight;

    PixFmt   camPixFmt;

    std::string selectedFmtDesc,
                camFmtDesc,
                userFmtDesc;

    pDevice->GetFormat(uWidth, uHeight, camPixFmt);
  
    // selected format
    GetPixFmtDesc(selectedFmt, selectedFmtDesc);  
    // cam format
    GetPixFmtDesc(camPixFmt, camFmtDesc);
    // user format
    GetPixFmtDesc(m_pixFmt, userFmtDesc);

    printf("\nDebug info:\n\n");
    printf("pDevice->SetFormat: %s\nCamPixFmt: %s - %ux%u\nUserPixFmt: %s - %ux%u\n", selectedFmtDesc.c_str(),
                                                                                      camFmtDesc.c_str(),
                                                                                      uSelectedWidth, uSelectedHeight,
                                                                                      userFmtDesc.c_str(),
                                                                                      _width, _height);
    printf("\n");

#endif

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::StartCapture()
{
    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        pDevice->StartCapture();
    else
        ret = RET_INIT_ERROR;    

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::StopCapture()
{
    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        pDevice->StopCapture();
    else
        ret = RET_INIT_ERROR;    

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::Close()
{
    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        pDevice->Close();
    else
        ret = RET_INIT_ERROR; 

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::GrabFrame(CVideoFrame & _videoFrame)
{

    if (!m_devGrabber.deviceGrabbed())
        return RET_INIT_ERROR;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    unsigned ret = pDevice->GetLastFrame(_videoFrame, m_width, m_height, m_pixFmt);

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::SelectBestFmt(unsigned & _uSelectedWidth, 
                                                 unsigned & _uSelectedHeight,
                                                 PixFmt   & _uSelectedPixFmt)
{
	return RET_ERROR;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::SetControlValue(int _controlId, int _value)
{
    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        pDevice->SetControlValue(_controlId, _value);
    else
        ret = RET_ERROR;

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CameraCaptureDeviceLinux::GetControlValue(int _controlId, int & _value)
{
    unsigned ret = RET_OK;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        pDevice->GetControlValue(_controlId, _value);
    else
        ret = RET_ERROR;

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
bool CameraCaptureDeviceLinux::NewFrame()
{
    bool ret = false;

    CDevice *pDevice = m_devGrabber.GetDevicePtr();

    if (pDevice)
        ret = pDevice->NewFrame();

    return ret;    
}
///////////////////////////////////////////////////////////////////////////////
CDevice * CameraCaptureDeviceLinux::GetDevice()
{
    return m_devGrabber.GetDevicePtr();
}
///////////////////////////////////////////////////////////////////////////////
CDeviceGrabber::CDeviceGrabber()
{
    m_pDevice = NULL;
}
///////////////////////////////////////////////////////////////////////////////
CDeviceGrabber::CDeviceGrabber(std::string & _devId)
{
    m_pDevice = NULL;
    Init(_devId);
}
///////////////////////////////////////////////////////////////////////////////
CDeviceGrabber::~CDeviceGrabber()
{
    if (m_pDevice)
        delete m_pDevice;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceGrabber::Init(std::string & _devId)
{
    if (m_pDevice)
        delete m_pDevice;

    unsigned ret = RET_OK;

    int fd;
    if ( (fd = open(_devId.c_str(), O_RDWR)) != -1)
    {
        v4l2_capability v4l2_cap;
        memset(&v4l2_cap, 0, sizeof(v4l2_cap));

        if ( ioctl (fd, VIDIOC_QUERYCAP, &v4l2_cap) < 0 ) 
        {
            video_capability v4l_cap;
            memset(&v4l_cap, 0, sizeof(v4l_cap));

            if (ioctl(fd, VIDIOCGCAP, &v4l_cap) != -1) 
            {
                m_pDevice = new CDeviceV4L(_devId);
            
                if (!m_pDevice)
                {    
                    PrintDbgMsg("CDeviceGrabber::CDeviceGrabber", "NO_MEMORY", "", 0);
                    ret = RET_LOW_MEMORY;
                } 
           }
           else
           {    
               PrintDbgMsg("CDeviceGrabber::CDeviceGrabber", "VIDIOCGCAP", strerror(errno), errno);
               ret = RET_ERROR;
           }
        }
        else
        {
            m_pDevice = new CDeviceV4L2(_devId);
            
            if (!m_pDevice)
            { 
                PrintDbgMsg("CDeviceGrabber::CDeviceGrabber", "NO_MEMORY", "", 0);
                ret = RET_LOW_MEMORY;
            }
        }

        close(fd);
    }    
    else 
    {    
        PrintDbgMsg("CDeviceGrabber::CDeviceGrabber", "Cannot open device", strerror(errno), errno);
        ret = RET_ERROR;
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
CDevice::CDevice(std::string & _devId)
{
    m_DevId = _devId;
    m_Fd    = -1;

    m_pGrabFrames       = NULL;
    m_pCurFrame         = NULL;

    m_CurFrameLen       = 0;
    m_CurFrameTimestamp = 0;

    m_bNewFrame         = false;

    m_DevOpened         = false;
    m_ReadyToCapture    = false;
    m_ThreadRunning     = false;
}
///////////////////////////////////////////////////////////////////////////////
CDevice::~CDevice()
{
    if (m_DevOpened)
        Close();

    if (m_pGrabFrames)
        delete m_pGrabFrames;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDevice::Open()
{
    if (m_DevOpened)
        return RET_DEVICE_ALREADY_OPENED;

    m_Fd = open(m_DevId.c_str(), O_RDWR);

    if (-1 == m_Fd) 
    {
        PrintDbgMsg("CCamDevice::Open", "Cannot open device", strerror(errno), errno);
        return RET_DEVICE_NOT_OPENED;
    }

    m_DevOpened = true;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDevice::Close()
{
    if (!m_DevOpened)
        return RET_DEVICE_NOT_OPENED;

    if (m_ThreadRunning)
        StopCapture();

    if (-1 == close(m_Fd)) 
    {
        PrintDbgMsg("CDevice::Close", "Cannot close device", strerror(errno), errno);
        return RET_DEVICE_NOT_CLOSED;
    }

    m_Fd = -1;
    m_DevOpened = false;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDevice::GetFd(int & _fd)
{
    if (m_Fd == -1)
        return RET_DEVICE_NOT_OPENED;

    _fd = m_Fd;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDevice::GetLastFrame(CVideoFrame & videoFrame, unsigned _dstWidth, unsigned _dstHeight, PixFmt _dstPixFmt)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    if ((_dstPixFmt != YUV420P) && (_dstPixFmt != RGB24))
        return RET_ERROR;

    if (!m_ThreadRunning)
        return RET_NOT_CAPTURING;

    unsigned ret = RET_OK;

    m_Semaph.Wait();

    if (m_pCurFrame && m_CurFrameLen)
    {
        unsigned char srcFrameData[MAX_FRAME_SIZE], 
                      dstFrameData[MAX_FRAME_SIZE];

        unsigned width, height;
        PixFmt curPixFmt;

        GetFormat(width, height, curPixFmt);

        if (curPixFmt == MJPEG)
        {
            if (jpeg_decode(srcFrameData, m_pCurFrame, (int *)&width, (int *)&height) < 0)
            {
                PrintDbgMsg("CDevice::GetLastFrame", "jpeg_decode error", "", 0);
                m_Semaph.Signal();
	        return RET_ERROR; 
            }

            curPixFmt     = YUYV422;
            m_CurFrameLen = width*(height+8)*2;
            m_pCurFrame   = srcFrameData;
        }

        if ((width != _dstWidth) || (height != _dstHeight))
        {
            unsigned dstFrameLen = MAX_FRAME_SIZE;

            if ((ret = StretchPicture(m_pCurFrame, m_CurFrameLen, width, height, curPixFmt, dstFrameData, dstFrameLen, _dstWidth, _dstHeight)) == RET_OK)
            {
                width     = _dstWidth;
                height    = _dstHeight;
                curPixFmt = YUV420P;

                memcpy(srcFrameData, dstFrameData, dstFrameLen);

                m_pCurFrame   = srcFrameData;
                m_CurFrameLen = dstFrameLen;
            }
            else
            {
                PrintDbgMsg("CDevice::GetLastFrame", "Stretch picture error", "", ret);
            }
        }

        if (curPixFmt != _dstPixFmt)
        {
            unsigned newFmtDatalen = MAX_FRAME_SIZE;

            switch(_dstPixFmt)
            {
                case YUV420P:
                    ConvertToYUV420(m_pCurFrame, m_CurFrameLen, curPixFmt, dstFrameData, newFmtDatalen, width, height);
                break;

                case RGB24:
                    ConvertToRGB24(m_pCurFrame, m_CurFrameLen, curPixFmt, dstFrameData, newFmtDatalen, width, height);
                break;
            }

            m_CurFrameLen = newFmtDatalen;
            m_pCurFrame   = dstFrameData;
        }

        videoFrame.SetData(m_pCurFrame, m_CurFrameLen, m_CurFrameTimestamp, width, height, _dstPixFmt);
    }
    else
        ret = RET_ERROR;

    m_bNewFrame = false;
    m_Semaph.Signal();

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
int CDevice::GrabFrames(IThread *_pThread, void *_pParam)
{
    CDevice *pDevice = (CDevice *) _pParam; 

    pDevice->m_Semaph.Wait();    
    pDevice->m_ThreadRunning = true;

    unsigned ret,
             index;

    ret = RET_OK;
    
    while (pDevice->m_ThreadRunning)
    {
        if ( (ret = pDevice->SetFramePtr(index) ) == RET_OK)
        {
            pDevice->m_CurFrameTimestamp = GetTickCount();
            pDevice->m_bNewFrame = true;

            pDevice->m_Semaph.Signal();
            pDevice->m_Semaph.Wait();

            if ( (ret = pDevice->ReleaseFramePtr(index)) != RET_OK)
                break;
        }
        else
            break;
    }

    pDevice->m_Semaph.Signal();

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
// Video for Linux 2 device implementation
///////////////////////////////////////////////////////////////////////////////
CDeviceV4L2::CDeviceV4L2(std::string & _device) : CDevice(_device)
{
    m_DevType = V4L2_DEVICE_CAPTURE;
}
///////////////////////////////////////////////////////////////////////////////
CDeviceV4L2::~CDeviceV4L2()
{
    if (m_ThreadRunning)
        StopCapture();
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetInputList(DEVICEINPUT_LIST& _inputList)
{
    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    unsigned ret = RET_OK;

    _inputList.clear();

    v4l2_input inputs;
    memset(&inputs, 0, sizeof(inputs));

    inputs.index = 0;

    while (ioctl(fd, VIDIOC_ENUMINPUT, &inputs) == 0)
    {
    	DeviceInput devInput;
    	devInput.index = inputs.index;
    	devInput.name  = (const char*)inputs.name;
        _inputList.push_back(devInput);
        inputs.index++;
    }

    if (!inputs.index)
    {   
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L2::GetInputList", "VIDIOC_ENUMINPUT - Error", strerror(errno), errno);
        #endif
        ret = RET_ERROR;
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SelectInput(unsigned _inputIndex)
{
    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;

    unsigned ret = RET_OK;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    if (ioctl(fd, VIDIOC_S_INPUT, &_inputIndex) < 0)
    {    
        PrintDbgMsg("CDeviceV4L2::SelectInput", "VIDIOC_S_INPUT - Error", strerror(errno), errno);
        ret = RET_ERROR;
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetAnagStdCount(unsigned & standardsCount)
{
    standardsCount = 0;

    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;    

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_standard standard;
    memset(&standard, 0, sizeof(standard));

    standard.index = 0;

    while (ioctl(fd, VIDIOC_ENUMSTD, &standard) == 0)
    {
        standard.index++;
        standardsCount++;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::EnumAnagStd(unsigned _index, ANALOGVIDEO_LIST & _videoStdList)
{
    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;    

    _videoStdList.clear();

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_standard standard;
    memset(&standard, 0, sizeof(standard));

    standard.index = _index;

    if (ioctl(fd, VIDIOC_ENUMSTD, &standard) == 0)
    {
        if (standard.id & V4L2_STD_PAL_B)
            _videoStdList.push_back(PAL_B);
        
        if (standard.id & V4L2_STD_PAL_B1)
            _videoStdList.push_back(PAL_B1);
        
        if (standard.id & V4L2_STD_PAL_G)
            _videoStdList.push_back(PAL_G);

        if (standard.id & V4L2_STD_PAL_H)
            _videoStdList.push_back(PAL_H);

        if (standard.id & V4L2_STD_PAL_I)
            _videoStdList.push_back(PAL_I);

        if (standard.id & V4L2_STD_PAL_D)
            _videoStdList.push_back(PAL_D);

        if (standard.id & V4L2_STD_PAL_D1)
            _videoStdList.push_back(PAL_D1);

        if (standard.id & V4L2_STD_PAL_K)
            _videoStdList.push_back(PAL_K);

        if (standard.id & V4L2_STD_PAL_M)
            _videoStdList.push_back(PAL_M);

        if (standard.id & V4L2_STD_PAL_N)
            _videoStdList.push_back(PAL_N);

        if (standard.id & V4L2_STD_PAL_Nc)
            _videoStdList.push_back(PAL_Nc);

        if (standard.id & V4L2_STD_PAL_60)
            _videoStdList.push_back(PAL_60);

        if (standard.id & V4L2_STD_NTSC_M)
            _videoStdList.push_back(NTSC_M);

        if (standard.id & V4L2_STD_NTSC_M_JP)
            _videoStdList.push_back(NTSC_M_JP);

        if (standard.id & V4L2_STD_NTSC_443)
            _videoStdList.push_back(NTSC_443);

        if (standard.id & V4L2_STD_NTSC_M_KR)
            _videoStdList.push_back(NTSC_M_KR);

        if (standard.id & V4L2_STD_SECAM_B) 
            _videoStdList.push_back(SECAM_B);

        if (standard.id & V4L2_STD_SECAM_D)
            _videoStdList.push_back(SECAM_D);

        if (standard.id & V4L2_STD_SECAM_G)
            _videoStdList.push_back(SECAM_G);

        if (standard.id & V4L2_STD_SECAM_H)
            _videoStdList.push_back(SECAM_H);

        if (standard.id & V4L2_STD_SECAM_K)
            _videoStdList.push_back(SECAM_K);

        if (standard.id & V4L2_STD_SECAM_K1)
            _videoStdList.push_back(SECAM_K1);

        if (standard.id & V4L2_STD_SECAM_L)
            _videoStdList.push_back(SECAM_L);

        if (standard.id & V4L2_STD_SECAM_LC)
            _videoStdList.push_back(SECAM_LC);

        if (standard.id & V4L2_STD_ATSC_8_VSB)
            _videoStdList.push_back(ATSC_8_VSB);

        if (standard.id & V4L2_STD_ATSC_16_VSB)
            _videoStdList.push_back(ATSC_16_VSB);
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
#if 0        
        printf("standard.name: %s\n", standard.name);
        printf("standard.frameperiod: %u/%u\n", standard.frameperiod.numerator, 
                                                standard.frameperiod.denominator);

        printf("standard.framelines: %u\n", standard.framelines);
        printStandardInfo(standard.id);
        printf("\n");
#endif
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SelectAnagStd(AnagVideoStd _videoStd)
{
    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;    

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_std_id std_id;
    TranslateFromAnagStd(_videoStd, std_id);

    unsigned ret = RET_OK;

    if (ioctl(fd, VIDIOC_S_STD, &std_id) < 0)
    {
        PrintDbgMsg("CDeviceV4L2::SelectAnagStd", "VIDIOC_S_STD - error", strerror(errno), errno);
        ret = RET_ERROR;
    }

    return ret;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd)
{
    switch (std_id)
    {
        case V4L2_STD_PAL_B:
            _videoStd = PAL_B;
        break;
        case V4L2_STD_PAL_B1:
            _videoStd = PAL_B1;
        break;
        case V4L2_STD_PAL_G:
            _videoStd = PAL_G;
        break;
        case V4L2_STD_PAL_H:
            _videoStd = PAL_H;
        break;
        case V4L2_STD_PAL_I:
            _videoStd = PAL_I ;
        break;
        case V4L2_STD_PAL_D:
            _videoStd = PAL_D;
        break;
        case V4L2_STD_PAL_D1:
            _videoStd = PAL_D1;
        break;
        case V4L2_STD_PAL_K:
            _videoStd = PAL_K;
        break;
        case V4L2_STD_PAL_M:
            _videoStd = PAL_M;
        break;
        case V4L2_STD_PAL_N:
            _videoStd = PAL_N;
        break;
        case V4L2_STD_PAL_Nc:
            _videoStd = PAL_Nc;
        break;
        case V4L2_STD_PAL_60:
            _videoStd = PAL_60;
        break;
        case V4L2_STD_NTSC_M:
            _videoStd = NTSC_M;
        break;
        case V4L2_STD_NTSC_M_JP:
            _videoStd = NTSC_M_JP;
        break;
        case V4L2_STD_NTSC_443:
            _videoStd = NTSC_443;
        break;
        case V4L2_STD_NTSC_M_KR:
            _videoStd = NTSC_M_KR;
        break;
        case V4L2_STD_SECAM_B: 
            _videoStd = SECAM_B;
        break;
        case V4L2_STD_SECAM_D:
            _videoStd = SECAM_D;
        break;
        case V4L2_STD_SECAM_G:
            _videoStd = SECAM_G;
        break;
        case V4L2_STD_SECAM_H:
            _videoStd = SECAM_H;
        break;
        case V4L2_STD_SECAM_K:
            _videoStd = SECAM_K;
        break;
        case V4L2_STD_SECAM_K1:
            _videoStd = SECAM_K1;
        break;
        case V4L2_STD_SECAM_L:
            _videoStd = SECAM_L;
        break;
        case V4L2_STD_SECAM_LC:
            _videoStd = SECAM_LC;
        break;
        case V4L2_STD_ATSC_8_VSB:
            _videoStd = ATSC_8_VSB;
        break;
        case V4L2_STD_ATSC_16_VSB:
            _videoStd = ATSC_16_VSB;
        break;

        default:
            return RET_ERROR;
    }
 
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::TranslateFromAnagStd(AnagVideoStd _videoStd, v4l2_std_id & std_id)
{
    switch(_videoStd)
    {
    case PAL_B:
        std_id = V4L2_STD_PAL_B;
    break;

    case PAL_B1:
        std_id = V4L2_STD_PAL_B1;
    break;

    case PAL_G: 
        std_id = V4L2_STD_PAL_G;
    break;

    case PAL_H:
        std_id = V4L2_STD_PAL_H;
    break;

    case PAL_I:
        std_id = V4L2_STD_PAL_I;
    break;

    case PAL_D:
        std_id = V4L2_STD_PAL_D;
    break;
   
    case PAL_D1:
        std_id = V4L2_STD_PAL_D1;
    break;

    case PAL_K:
        std_id = V4L2_STD_PAL_K;
    break;

    case PAL_M:
        std_id = V4L2_STD_PAL_M;
    break;

    case PAL_N:
        std_id = V4L2_STD_PAL_N;
    break;

    case PAL_Nc:
        std_id = V4L2_STD_PAL_Nc;
    break;

    case PAL_60:
        std_id = V4L2_STD_PAL_60;
    break;

    case NTSC_M:
        std_id = V4L2_STD_NTSC_M;
    break;

    case NTSC_M_JP:
        std_id = V4L2_STD_NTSC_M_JP;
    break;
 
    case NTSC_443:
        std_id = V4L2_STD_NTSC_443;
    break;

    case NTSC_M_KR:
        std_id = V4L2_STD_NTSC_M_KR;
    break;

    case SECAM_B:
        std_id = V4L2_STD_SECAM_B;
    break; 

    case SECAM_D:
        std_id = V4L2_STD_SECAM_D;
    break;

    case SECAM_G:
        std_id = V4L2_STD_SECAM_G;
    break;

    case SECAM_H:
        std_id = V4L2_STD_SECAM_H;
    break;

    case SECAM_K:
        std_id = V4L2_STD_SECAM_K;
    break;

    case SECAM_K1:
        std_id = V4L2_STD_SECAM_K1;
    break;

    case SECAM_L:
        std_id = V4L2_STD_SECAM_L;
    break;

    case SECAM_LC:
        std_id = V4L2_STD_SECAM_LC;
    break;

    case ATSC_8_VSB:
        std_id = V4L2_STD_ATSC_8_VSB;
    break;

    case ATSC_16_VSB:
        std_id = V4L2_STD_ATSC_16_VSB;
    break;

    default:
        return RET_ERROR;    
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::StartCapture()
{
    if ( !DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));

    if ( ioctl (fd, VIDIOC_QUERYCAP, &cap) < 0 ) 
    {
        PrintDbgMsg("CDeviceV4L2::StartCapture", "this is not a V4L2 device", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) 
    {
        PrintDbgMsg("CDeviceV4L2::StartCapture", "this is not a video capture device", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) 
    {
        PrintDbgMsg("CDeviceV4L2::StartCapture", "this device does not support streaming io", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;

    memset (&cropcap, 0, sizeof(cropcap));
    memset (&crop,    0, sizeof(crop));

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_CROPCAP, &cropcap) == 0) 
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c    = cropcap.defrect;

    #ifdef _DEBUG
        if (ioctl(fd, VIDIOC_S_CROP, &crop) < 0);
            PrintDbgMsg("CCamDevice::StartCapture", "VIDIOC_S_CROP: Error setting crop", strerror(errno), errno);
    #endif
    } 
    else
    { 
    #ifdef _DEBUG
        PrintDbgMsg("CCamDevice::StartCapture", "VIDIOC_CROPCAP: Error getting crop capabilities", strerror(errno), errno);
    #endif
    }
 
    CHECK_ERROR(InitMMap(), RET_OK)

    m_pGrabFrames = CreateIThread(CDevice::GrabFrames, this, TRUE);

    if (!m_pGrabFrames)
	    return RET_ERROR;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::TranslateFromPixFmt(PixFmt _srcPixFmt, unsigned & _v4l2DstPixFmt)
{
    switch(_srcPixFmt)
    {
        case MJPEG:  
            _v4l2DstPixFmt = V4L2_PIX_FMT_MJPEG;
        break;

        case YUV420P:
            _v4l2DstPixFmt = V4L2_PIX_FMT_YUV420;
        break;
  
        case YUYV422:
            _v4l2DstPixFmt = V4L2_PIX_FMT_YUYV;
        break;
            
        case UYVY422:
            _v4l2DstPixFmt = V4L2_PIX_FMT_UYVY; 
        break;

        case RGB565:
            _v4l2DstPixFmt = V4L2_PIX_FMT_RGB565;
        break;

        case RGB555:
            _v4l2DstPixFmt = V4L2_PIX_FMT_RGB555;
        break;

        case RGB24:
            _v4l2DstPixFmt = V4L2_PIX_FMT_RGB24;
        break;

        case BGR24:
            _v4l2DstPixFmt = V4L2_PIX_FMT_BGR24;
        break;

        case PWC2:
           _v4l2DstPixFmt = V4L2_PIX_FMT_PWC2;
        break;

        default:
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L2::TranslateFromPixFmt", "UNSUPPORTED_PIXEL_FORMAT", "", 0);
        #endif
            return RET_UNSUPPORTED_PIXEL_FORMAT;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::TranslateToPixFmt(unsigned _v4l2SrcPixFmt, PixFmt & _dstPixFmt)
{
    switch(_v4l2SrcPixFmt)
    {
        case V4L2_PIX_FMT_MJPEG:  
            _dstPixFmt = MJPEG;
        break;    

        case V4L2_PIX_FMT_YUV420:
            _dstPixFmt = YUV420P;
        break;

        case V4L2_PIX_FMT_YUYV:
            _dstPixFmt = YUYV422;
        break;
    
        case V4L2_PIX_FMT_UYVY:
            _dstPixFmt = UYVY422;
        break;

        case V4L2_PIX_FMT_RGB565:
            _dstPixFmt = RGB565;
        break;

        case V4L2_PIX_FMT_RGB555:
            _dstPixFmt = RGB555;
        break;

        case V4L2_PIX_FMT_RGB24:
            _dstPixFmt = RGB24;
        break;

        case V4L2_PIX_FMT_BGR24:
            _dstPixFmt = BGR24;
        break;

        case V4L2_PIX_FMT_PWC2:
           _dstPixFmt = PWC2;
        break;
        
        default:
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L2::TranslateToPixFmt", "UNSUPPORTED_PIXEL_FORMAT", "", 0);
        #endif        
            return RET_UNSUPPORTED_PIXEL_FORMAT;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::StopCapture()
{
    if (m_ThreadRunning)
    {
        m_ThreadRunning = false;
        m_pGrabFrames->WaitForMe(INFINITE_TIME);
        FinishStreaming();
        FinishMMap();
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::InitMMap()
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    struct v4l2_requestbuffers req;
    memset (&req, 0, sizeof(req));

    req.count  = BUFFERS_TO_MAP;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if ( ioctl (fd, VIDIOC_REQBUFS, &req) < 0 ) 
    {
        PrintDbgMsg("CDeviceV4L2::InitMMap", "VIDIOC_REQBUFS", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    if (req.count < 2) 
    {
        PrintDbgMsg("CDeviceV4L2::InitMMap", "VIDIOC_REQBUFS: insuficient buffer memory", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    V4L2_VideoBufferInfo bufInfo;
    v4l2_buffer buf;

    for (unsigned index=0; index<req.count; index++) 
    {
        memset(&buf,     0, sizeof(buf));
        memset(&bufInfo, 0, sizeof(bufInfo));

        buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory  = V4L2_MEMORY_MMAP;
        buf.index   = index;

        if ( ioctl (fd, VIDIOC_QUERYBUF, &buf) < 0) 
        {
            PrintDbgMsg("CDeviceV4L2::InitMMap", "VIDIOC_QUERYBUF: error querying buffer", strerror(errno), errno);
            return RET_DEVICE_ERROR;
        }

        bufInfo.pStartAddress  = (unsigned char *)mmap (NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        bufInfo.index          = index;
        bufInfo.bufLength      = buf.length; 

        if (bufInfo.pStartAddress == MAP_FAILED) 
        {
            PrintDbgMsg("CDeviceV4L2::InitMMap", "Error mapping buffer", strerror(errno), errno);
            return RET_DEVICE_ERROR;
        }

        if( ioctl (fd, VIDIOC_QBUF, &buf) < 0 ) 
        {
            PrintDbgMsg("CDeviceV4L2::InitMMap", "VIDIOC_QBUF: Error queuing buffer", strerror(errno), errno);
            return RET_DEVICE_ERROR;
        }

        m_BufferList.push_back(bufInfo);
    }

    CHECK_ERROR(StartStreaming(), RET_OK)

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::FinishMMap() 
{
    while(m_BufferList.size()) 
    {
        if (-1 == munmap (m_BufferList[0].pStartAddress, m_BufferList[0].bufLength)) 
        {
            PrintDbgMsg("CStreamingIO::FinishMMap", "Error unmapping buffer", strerror(errno), errno);
            return RET_DEVICE_ERROR;
        }

        m_BufferList.pop_front();
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::StartStreaming() 
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) 
    {
        PrintDbgMsg("CDeviceV4L2::StartStreaming", "VIDIOC_STREAMON: error starting capture", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::FinishStreaming() 
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) 
    {
        PrintDbgMsg("CDeviceV4L2::StartStreaming", "VIDIOC_STREAMOFF: error finishing capture", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::TranslateCtrlId(int _controlId, int & _v4l2ControlId)
{
    switch(_controlId)
    {
        case BRIGHTNESS:
            _v4l2ControlId = V4L2_CID_BRIGHTNESS;
        break;

        case CONTRAST:
            _v4l2ControlId = V4L2_CID_CONTRAST;
        break;

        case HUE:
            _v4l2ControlId = V4L2_CID_HUE;
        break;

        case SATURATION:
            _v4l2ControlId = V4L2_CID_SATURATION;
        break;

        default:
            return RET_UNSUPPORTED_CTRL;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetControlValue(int _controlId, int & _value)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));

    int v4l2ControlId;
    CHECK_ERROR(TranslateCtrlId(_controlId, v4l2ControlId), RET_OK)

    ctrl.id = v4l2ControlId;

    if ( ioctl(fd, VIDIOC_G_CTRL, &ctrl) < 0 ) 
    {
        PrintDbgMsg("CDeviceV4L2::GetControlValue", "VIDIOC_G_CTRL", strerror(errno), errno);
        return RET_ERROR;
    }

    _value = ctrl.value;
    
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SetControlValue(int _controlId, int _value)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    int v4l2ControlId;
    CHECK_ERROR(TranslateCtrlId(_controlId, v4l2ControlId), RET_OK)

    v4l2_control ctrl;
    memset(&ctrl, 0, sizeof(ctrl));

    ctrl.id 	= v4l2ControlId;
    ctrl.value	= _value;

    if ( ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0 ) 
    {
        PrintDbgMsg("CDeviceV4L2::SetControlValue", "VIDIOC_S_CTRL", strerror(errno), errno);
        return RET_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    _pixFmtList.clear();

    bool     enumEnd = false;
    unsigned index   = 0;

    v4l2_fmtdesc fmtDesc;

    do
    {
        memset(&fmtDesc, 0, sizeof(fmtDesc));

        fmtDesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmtDesc.index = index;        

        if (ioctl(fd, VIDIOC_ENUM_FMT, &fmtDesc) == 0)
        {    
            PixFmt pixFmt;

            if (TranslateToPixFmt(fmtDesc.pixelformat, pixFmt) == RET_OK)
                _pixFmtList.push_back(pixFmt);

            index++;
        }
        else
            enumEnd = true;

    }while(!enumEnd);

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensionsList)
{
    if (!DeviceOpened() )
        return RET_DEVICE_NOT_OPENED;

    _dimensionsList.clear();

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    unsigned v4l2_pixelformat;
    CHECK_ERROR(TranslateFromPixFmt(_pixFmt, v4l2_pixelformat), RET_OK)

    v4l2_frmsizeenum frameSizeEnum;
    memset(&frameSizeEnum, 0, sizeof(frameSizeEnum));

    frameSizeEnum.index = 0;
    frameSizeEnum.pixel_format = v4l2_pixelformat;

    if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frameSizeEnum) == 0)
    {
        FrameDimension dimension;

        switch(frameSizeEnum.type)
        {
            case V4L2_FRMSIZE_TYPE_DISCRETE:

                do
                {
                    dimension.uWidth  = frameSizeEnum.discrete.width;
                    dimension.uHeight = frameSizeEnum.discrete.height;
                    
                    _dimensionsList.push_back(dimension);
                    frameSizeEnum.index++;
                }   
                while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frameSizeEnum) == 0);
            
            break;
            
            case V4L2_FRMSIZE_TYPE_CONTINUOUS: 
            break;

            case V4L2_FRMSIZE_TYPE_STEPWISE: 
            break;
        }
    }
    else
    {
        FrameDimension dimension[COMMON_FRAME_DIMENSIONS_COUNT] = 
        {    
            FrameDimension(160, 120),
            FrameDimension(176, 144),
            FrameDimension(320, 240),
            FrameDimension(352, 288),
            FrameDimension(640, 360),
            FrameDimension(640, 480),
            FrameDimension(800, 600),
            FrameDimension(1024, 768),
            FrameDimension(1600, 1200),
        };

        v4l2_format format,
                    first_format;

        GetV4L2Fmt(format);
        first_format = format;

        for (int i=0; i<COMMON_FRAME_DIMENSIONS_COUNT; i++)
        {
            format.fmt.pix.width  = dimension[i].uWidth;
            format.fmt.pix.height = dimension[i].uHeight;

            ioctl(fd, VIDIOC_S_FMT, &format);

            memset(&format, 0, sizeof(format));
            format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (ioctl(fd, VIDIOC_G_FMT, &format) == 0)
            {
                if ((format.fmt.pix.width  == dimension[i].uWidth) && 
                    (format.fmt.pix.height == dimension[i].uHeight))
                {
                    _dimensionsList.push_back(dimension[i]);
                }
            }
            else
                printf("VIDIOC_G_FMT ERROR !\n");
        }

        SetV4L2Fmt(first_format);
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    v4l2_format fmt;
    CHECK_ERROR(GetV4L2Fmt(fmt), RET_OK)

    fmt.fmt.pix.width  = _width;
    fmt.fmt.pix.height = _height;
  
    CHECK_ERROR(TranslateFromPixFmt(_pixFmt, fmt.fmt.pix.pixelformat), RET_OK) 

    CHECK_ERROR(SetV4L2Fmt(fmt), RET_OK)

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SetV4L2Fmt(v4l2_format & fmt)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) 
    {
        PrintDbgMsg("CDeviceV4L2::SetV4L2Fmt", "VIDIOC_S_FMT", strerror(errno), errno);
        return RET_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetV4L2Fmt(v4l2_format & fmt)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) 
    {
	PrintDbgMsg("CDeviceV4L2::GetV4L2Fmt", "VIDIOC_G_FMT", strerror(errno), errno);
        return RET_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    v4l2_format fmt;
    CHECK_ERROR(GetV4L2Fmt(fmt), RET_OK)

    _width  = fmt.fmt.pix.width;
    _height = fmt.fmt.pix.height;

    CHECK_ERROR(TranslateToPixFmt(fmt.fmt.pix.pixelformat, _pixFmt), RET_OK)

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::SetFramePtr(unsigned & _index)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_buffer buf;
    memset (&buf, 0, sizeof(buf));

    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0 ) 
    {
        PrintDbgMsg("CDeviceV4L2::SetFramePtr", "VIDIOC_DQBUF: cannot dequeue buffer", strerror(errno), errno);
	return RET_DEVICE_ERROR;
    }

    m_CurFrameLen = buf.bytesused;
    m_pCurFrame   = (unsigned char *)m_BufferList[buf.index].pStartAddress;

    _index = buf.index;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L2::ReleaseFramePtr(unsigned _index)
{
    m_CurFrameLen = 0;
    m_pCurFrame   = NULL;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    v4l2_buffer buf;
    memset (&buf, 0, sizeof(buf));

    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index  = _index;

    if (ioctl (fd, VIDIOC_QBUF, &buf) <  0) 
    {
        PrintDbgMsg("CDevice::GrabFrames", "VIDIOC_QBUF: cannot enqueue buffer", strerror(errno), errno);
	return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
// Video for Linux device implementation
///////////////////////////////////////////////////////////////////////////////
CDeviceV4L::V4LPixFmt CDeviceV4L::v4lPixFmtList[V4L_PIXFMT_COUNT] =
{ 
    {VIDEO_PALETTE_GREY,      "VIDEO_PALETTE_GREY"},
    //{VIDEO_PALETTE_HI240,     "VIDEO_PALETTE_HI240"},
    {VIDEO_PALETTE_RGB565,    "VIDEO_PALETTE_RGB565"},
    {VIDEO_PALETTE_RGB24,     "VIDEO_PALETTE_RGB24"},
    //{VIDEO_PALETTE_RGB32,     "VIDEO_PALETTE_RGB32"},
    //{VIDEO_PALETTE_RGB555,    "VIDEO_PALETTE_RGB555"},
    {VIDEO_PALETTE_YUV422,    "VIDEO_PALETTE_YUV422"},
    {VIDEO_PALETTE_YUYV,      "VIDEO_PALETTE_YUYV"},
    {VIDEO_PALETTE_UYVY,      "VIDEO_PALETTE_UYVY"},
    {VIDEO_PALETTE_YUV420,    "VIDEO_PALETTE_YUV420"},
    //{VIDEO_PALETTE_YUV411,    "VIDEO_PALETTE_YUV411"},
    //{VIDEO_PALETTE_RAW,       "VIDEO_PALETTE_RAW"},
    //{VIDEO_PALETTE_YUV422P,   "VIDEO_PALETTE_YUV422P"},
    //{VIDEO_PALETTE_YUV411P,   "VIDEO_PALETTE_YUV411P"},
    {VIDEO_PALETTE_YUV420P,   "VIDEO_PALETTE_YUV420P"},
    //{VIDEO_PALETTE_YUV410P,   "VIDEO_PALETTE_YUV410P"},
    //{VIDEO_PALETTE_PLANAR,    "VIDEO_PALETTE_PLANAR"},
    //{VIDEO_PALETTE_COMPONENT, "VIDEO_PALETTE_COMPONENT"}
};
///////////////////////////////////////////////////////////////////////////////
FrameDimension CDeviceV4L::CommonFrameDimensions[COMMON_FRAME_DIMENSIONS_COUNT] =
{    
    FrameDimension(160, 120),
    FrameDimension(176, 144),
    FrameDimension(320, 240),
    FrameDimension(352, 288),
    FrameDimension(640, 360),
    FrameDimension(640, 480),
    FrameDimension(800, 600),
    FrameDimension(1024, 768),
    FrameDimension(1600, 1200),
};
///////////////////////////////////////////////////////////////////////////////
CDeviceV4L::CDeviceV4L(std::string & _device) : CDevice(_device)
{
    m_DevType = V4L_DEVICE_CAPTURE;

    memset(&m_VideoInfo, 0, sizeof(m_VideoInfo));
}
///////////////////////////////////////////////////////////////////////////////
CDeviceV4L::~CDeviceV4L()
{
    if (m_ThreadRunning)
        StopCapture();
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::StartCapture()
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    video_channel videoChannel;
    memset(&videoChannel, 0, sizeof(videoChannel));

    videoChannel.channel = 0;

    if (ioctl(fd, VIDIOCGCHAN, &videoChannel) < 0)
    {
        PrintDbgMsg("CDeviceV4L::StartCapture", "VIDIOCGCHAN", "", 0);
        return RET_DEVICE_ERROR;
    }

    unsigned uSelectedChannel = 0;
    if ( ioctl(fd, VIDIOCSCHAN, &uSelectedChannel) < 0)
    {
        PrintDbgMsg("CDeviceV4L::StartCapture", "VIDIOCSCHAN", "", 0);
        return RET_DEVICE_ERROR;
    }

    video_window clipArea;
    CHECK_ERROR(GetV4LDimensions(clipArea), RET_OK)

    m_VideoInfo.index = 0;
    m_VideoInfo.dimension.uHeight = clipArea.height;
    m_VideoInfo.dimension.uWidth  = clipArea.width;

    video_picture picture;
    CHECK_ERROR(GetV4LPixFmt(picture), RET_OK)

    m_VideoInfo.pixFmt = picture.palette;

    unsigned pixFmtDepth;
    CHECK_ERROR(GetV4LPixFmtDepth(picture.palette, pixFmtDepth), RET_OK)

    m_VideoInfo.frameSize = ( clipArea.width*clipArea.height*pixFmtDepth )/8;

    if (ioctl(fd, VIDIOCGMBUF, &m_VideoInfo.mbuf) < 0)
    {
        PrintDbgMsg("CDeviceV4L::StartCapture", "VIDIOCGMBUF", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    CHECK_ERROR(InitMMap(), RET_OK)
 
    m_pGrabFrames = CreateIThread(CDevice::GrabFrames, this, TRUE);

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::StopCapture()
{
    if (m_ThreadRunning)
    {
        m_ThreadRunning = false;
        m_pGrabFrames->WaitForMe(INFINITE_TIME);
        FinishMMap();
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetControlValue(int _controlId, int & _value)
{

    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    video_picture picture;
    CHECK_ERROR(GetV4LPixFmt(picture), RET_OK)

    switch(_controlId)
    {
        case CONTRAST:
            _value = picture.contrast;
        break;

        case HUE:
            _value = picture.hue;
        break;

        case SATURATION:
            _value = picture.colour;
        break;

        case BRIGHTNESS:
            _value = picture.brightness;
        break;

        default:
            PrintDbgMsg("CDeviceV4L::GetControlValue", "UNSUPPORTED_CTRL", "", 0);
            return RET_UNSUPPORTED_CTRL;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::SetControlValue(int _controlId, int _value)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    video_picture picture;
    CHECK_ERROR(GetV4LPixFmt(picture), RET_OK)

    switch(_controlId)
    {
        case CONTRAST:
            picture.contrast = _value;
        break;

        case HUE:
            picture.hue = _value;
        break;

        case SATURATION:
            picture.colour = _value;
        break;

        case BRIGHTNESS:
            picture.brightness = _value;
        break;

        default:
            PrintDbgMsg("CDeviceV4L::SetControlValue", "UNSUPPORTED_CTRL", "", 0);
            return RET_UNSUPPORTED_CTRL;
    }

    CHECK_ERROR(SetV4LPixFmt(picture), RET_OK)

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetSupportedPixelFmt(std::deque<PixFmt> & _pixFmtList)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    _pixFmtList.clear();

    video_picture first_picture;
    CHECK_ERROR(GetV4LPixFmt(first_picture), RET_OK)

    video_picture picture = first_picture;

    for (unsigned index=0; index < V4L_PIXFMT_COUNT; index++)
    {
        picture.palette = v4lPixFmtList[index].v4lPixFmt;
        
        unsigned v4lPixFmtDepth;

        CHECK_ERROR(GetV4LPixFmtDepth(picture.palette, v4lPixFmtDepth), RET_OK)
        picture.depth = v4lPixFmtDepth;

        if (SetV4LPixFmt(picture) == RET_OK)
        {
            PixFmt pixFmt;

            if ( TranslateToPixFmt(picture.palette, pixFmt) == RET_OK)
                _pixFmtList.push_back(pixFmt);
        }
    }

    CHECK_ERROR(SetV4LPixFmt(first_picture), RET_OK)
 
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetSupportedDimensions(PixFmt _pixFmt, std::deque<FrameDimension> & _dimensionsList)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    _dimensionsList.clear();

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    video_capability cap;
    memset(&cap, 0, sizeof(cap));

    if (ioctl(fd, VIDIOCGCAP, &cap) == 0) 
    {          
        video_window firstClipArea;
        CHECK_ERROR(GetV4LDimensions(firstClipArea), RET_OK)

        video_window clipArea = firstClipArea;

        clipArea.width  = cap.minwidth;
        clipArea.height = cap.minheight;

        if (SetV4LDimensions(clipArea) == RET_OK)
            _dimensionsList.push_back(FrameDimension(clipArea.width, clipArea.height));

        for (int index=0; index < COMMON_FRAME_DIMENSIONS_COUNT; index++)
        {
            if ( ((CommonFrameDimensions[index].uWidth != (unsigned)cap.minwidth) || (CommonFrameDimensions[index].uHeight != (unsigned)cap.minheight)) &&
                 ((CommonFrameDimensions[index].uWidth != (unsigned)cap.maxwidth) || (CommonFrameDimensions[index].uHeight != (unsigned)cap.maxheight)) )
            {

                clipArea.width  = CommonFrameDimensions[index].uWidth;
                clipArea.height = CommonFrameDimensions[index].uHeight;

                if (SetV4LDimensions(clipArea) == RET_OK)
                    _dimensionsList.push_back(FrameDimension(clipArea.width, clipArea.height));
            }
        }

        clipArea.width  = cap.maxwidth;
        clipArea.height = cap.maxheight;

        if (SetV4LDimensions(clipArea) == RET_OK)
            _dimensionsList.push_back(FrameDimension(clipArea.width, clipArea.height));

        CHECK_ERROR(SetV4LDimensions(firstClipArea), RET_OK)
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetV4LPixFmtDepth(unsigned _pixFmt, unsigned & _depth)
{
    switch(_pixFmt)
    {
        case VIDEO_PALETTE_RGB24:
            _depth = 24;
        break;

        case VIDEO_PALETTE_YUV422:
        case VIDEO_PALETTE_UYVY:
        case VIDEO_PALETTE_YUYV:
        case VIDEO_PALETTE_RGB565:
            _depth = 16;
        break;

        case VIDEO_PALETTE_YUV420:
        case VIDEO_PALETTE_YUV420P:
            _depth = 12;
        break;

        case VIDEO_PALETTE_GREY:
            _depth = 8;
        break;

        default:
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L::GetV4LPixFmtDepth", "UNSUPPORTED_PIXEL_FORMAT", "", 0);
        #endif        
            return RET_UNSUPPORTED_PIXEL_FORMAT;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetV4LPixFmt(video_picture & _picture)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    memset(&_picture, 0, sizeof(_picture));

    if (ioctl(fd, VIDIOCGPICT, &_picture) < 0)
    {
    #ifdef _DEBUG
        PrintDbgMsg("CDeviceV4L::GetV4LPixFmt", "VIDIOCGPICT", strerror(errno), errno);
    #endif
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::SetV4LPixFmt(video_picture & _picture)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    if (ioctl(fd, VIDIOCSPICT, &_picture) < 0)
    {
    #ifdef _DEBUG
        PrintDbgMsg("CDeviceV4L::SetV4LPixFmt", "VIDIOCSPICT", strerror(errno), errno);
    #endif
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetV4LDimensions(video_window & _clipArea)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    memset(&_clipArea, 0, sizeof(_clipArea));

    if (ioctl(fd, VIDIOCGWIN, &_clipArea) < 0)
    {
    #ifdef _DEBUG
        PrintDbgMsg("CDeviceV4L::GetV4LDimensions", "VIDIOCGWIN", strerror(errno), errno);
    #endif
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::SetV4LDimensions(video_window & _clipArea)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    if (ioctl(fd, VIDIOCSWIN, &_clipArea) < 0)
    {
    #ifdef _DEBUG
        PrintDbgMsg("CDeviceV4L::SetV4LDimensions", "VIDIOCSWIN", strerror(errno), errno);
    #endif    
        return RET_DEVICE_ERROR;    
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::SetFormat(unsigned _width, unsigned _height, PixFmt _pixFmt)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    video_window clipArea;
    CHECK_ERROR(GetV4LDimensions(clipArea), RET_OK)

    clipArea.width  = _width;
    clipArea.height = _height;

    CHECK_ERROR(SetV4LDimensions(clipArea), RET_OK)

    video_picture picture;
    CHECK_ERROR(GetV4LPixFmt(picture), RET_OK)

    unsigned v4lPixFmt;
    CHECK_ERROR(TranslateFromPixFmt(_pixFmt, v4lPixFmt), RET_OK)

    unsigned pixDepth;
    CHECK_ERROR(GetV4LPixFmtDepth(v4lPixFmt, pixDepth), RET_OK)

    picture.palette = v4lPixFmt;
    picture.depth   = pixDepth;

    CHECK_ERROR(SetV4LPixFmt(picture), RET_OK)

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::GetFormat(unsigned & _width, unsigned & _height, PixFmt & _pixFmt)
{
    if (!DeviceOpened())
        return RET_DEVICE_NOT_OPENED;

    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    video_picture picture;
    CHECK_ERROR(GetV4LPixFmt(picture), RET_OK)

    CHECK_ERROR(TranslateToPixFmt(picture.palette, _pixFmt), RET_OK)

    video_window clipArea;    
    CHECK_ERROR(GetV4LDimensions(clipArea), RET_OK)

    _width  = clipArea.width;
    _height = clipArea.height;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::SetFramePtr(unsigned & _index)
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    video_mmap mmap;
    memset(&mmap, 0, sizeof(mmap));  

    mmap.frame  = m_VideoInfo.index;
    mmap.format = m_VideoInfo.pixFmt;
    mmap.width  = m_VideoInfo.dimension.uWidth;
    mmap.height = m_VideoInfo.dimension.uHeight;
    
    if (ioctl(fd, VIDIOCMCAPTURE, &mmap) < 0) 
    {
        PrintDbgMsg("CDeviceV4L::SetFramePtr", "VIDIOCMCAPTURE", strerror(errno), errno); 
        return RET_DEVICE_ERROR;
    }
        
    if (ioctl(fd, VIDIOCSYNC, &m_VideoInfo.index) < 0)
    {
        PrintDbgMsg("CDeviceV4L::SetFramePtr", "VIDIOCSYNC", strerror(errno), errno); 
        return RET_DEVICE_ERROR;
    }

    m_pCurFrame   = m_VideoInfo.pStartAddress + m_VideoInfo.mbuf.offsets[m_VideoInfo.index];
    m_CurFrameLen = m_VideoInfo.frameSize;

    m_VideoInfo.index = (m_VideoInfo.index+1) % m_VideoInfo.mbuf.frames;

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::ReleaseFramePtr(unsigned _index)
{
    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::InitMMap()
{
    int fd;
    CHECK_ERROR( GetFd(fd), RET_OK)

    m_VideoInfo.pStartAddress = (unsigned char *) mmap(0, m_VideoInfo.mbuf.size, 
                                                       PROT_READ|PROT_WRITE, 
                                                       MAP_SHARED, fd, 0);
    
    if (m_VideoInfo.pStartAddress == MAP_FAILED) 
    {
        PrintDbgMsg("CDeviceV4L::InitMMap", "Error mapping buffer", strerror(errno), errno);
        return RET_DEVICE_ERROR;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::FinishMMap()
{
     if ( munmap(m_VideoInfo.pStartAddress, m_VideoInfo.mbuf.size) < 0)  
     {
         PrintDbgMsg("CDeviceV4L::FinishMMap()", "Error unmapping buffer", strerror(errno), errno);
         return RET_DEVICE_ERROR;
     }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::TranslateFromPixFmt(PixFmt _srcPixFmt, unsigned & _v4lDstPixFmt)
{

    switch(_srcPixFmt)
    {
        case GREY:
            _v4lDstPixFmt = VIDEO_PALETTE_GREY;
        break;

        case YUV422:
            _v4lDstPixFmt = VIDEO_PALETTE_YUV422;
        break;

        case YUV420P:
            _v4lDstPixFmt = VIDEO_PALETTE_YUV420;
        break;
  
        case YUV420:
            _v4lDstPixFmt = VIDEO_PALETTE_YUV420P;
        break;

        case YUYV422:
            _v4lDstPixFmt = VIDEO_PALETTE_YUYV;
        break;
            
        case UYVY422: 
            _v4lDstPixFmt = VIDEO_PALETTE_UYVY; 
        break;

        case RGB565:
            _v4lDstPixFmt = VIDEO_PALETTE_RGB565; 
        break;

        case RGB555:
            _v4lDstPixFmt = VIDEO_PALETTE_RGB555; 
        break;

        case BGR24:
            _v4lDstPixFmt = VIDEO_PALETTE_RGB24;
        break;

        default:
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L::TranslateFromPixFmt", "UNSUPPORTED_PIXEL_FORMAT", "", 0);
        #endif
            return RET_UNSUPPORTED_PIXEL_FORMAT;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned CDeviceV4L::TranslateToPixFmt(unsigned _v4lSrcPixFmt, PixFmt & _dstPixFmt)
{
    switch(_v4lSrcPixFmt)
    {
        case VIDEO_PALETTE_GREY:
           _dstPixFmt = GREY;
        break;

        case VIDEO_PALETTE_YUV422:
            _dstPixFmt = YUV422;
        break;

        case VIDEO_PALETTE_YUV420:
            _dstPixFmt = YUV420P;
        break;

        case VIDEO_PALETTE_YUV420P:
            _dstPixFmt = YUV420;
        break;
  
        case VIDEO_PALETTE_YUYV:
            _dstPixFmt = YUYV422;
        break;

        case VIDEO_PALETTE_UYVY:
            _dstPixFmt = UYVY422;
        break;

        case VIDEO_PALETTE_RGB565:
            _dstPixFmt = RGB565;
        break;

        case VIDEO_PALETTE_RGB555:
            _dstPixFmt = RGB555;
        break;

        case VIDEO_PALETTE_RGB24:
            _dstPixFmt = BGR24; // VIDEO_PALETTE_RGB24 is in reality a BGR24 pixel format 
        break;

        default:
        #ifdef _DEBUG
            PrintDbgMsg("CDeviceV4L::TranslateToPixFmt", "UNSUPPORTED_PIXEL_FORMAT", "", 0);
        #endif 
           return RET_UNSUPPORTED_PIXEL_FORMAT;
    }

    return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
// LinuxCamDeviceCtrl implementation
std::string LinuxCamDeviceCtrl::GetDeviceName(unsigned index)
{
	int fd;
	char deviceName[MAX_PATH_LENGTH];
	struct v4l2_capability v4l2caps;
	struct video_capability v4lcaps;
	std::string name;
	
	snprintf(deviceName, MAX_PATH_LENGTH, DEVICE_NAME_STR, index);
	
	fd = open(deviceName, O_RDWR | O_NONBLOCK);
	if(fd != -1)
	{
		// Try v4l2 method
		if(ioctl(fd, VIDIOC_QUERYCAP, &v4l2caps) != -1)
		{
			name = (char *)(v4l2caps.card);
			name += " [V4L2]";
			close(fd);
			return name;
		}
		// V4L2 failed, try v4l1 method
		if(ioctl(fd, VIDIOCGCAP, &v4lcaps) != -1)
		{
			name = (char *)(v4lcaps.name);
			name += " [V4L]";
			close(fd);
			return name;
		}
		// V4L1 failed too - error
		close(fd);
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
unsigned LinuxCamDeviceCtrl::GetVideoDeviceList(VIDEODEVICEINFO_LIST & _videoDevList)
{
	DIR *dir;
	struct dirent *dirEntry;
	std::string strDeviceFullPath;
	char *endp;
	unsigned index;
		
	// Search for /dev/video# devices
	dir = opendir(DEVICE_PATH);
	if(!dir)
		return RET_ERROR;
	do
	{
		dirEntry = readdir(dir);
		if(dirEntry)
		{
			// Check if file name begins with "video" (see defines above)
			if(!strncmp(dirEntry->d_name, DEVICE_NAME_PREFIX, DEVICE_NAME_PREFIX_LENGTH))
			{
				// Try to get the index, based on the nondigit character after "video"
				index = (unsigned)strtoul(dirEntry->d_name + DEVICE_NAME_PREFIX_LENGTH, &endp, 10);
				// if the input string isn't the empty string and *endp is the empty string, it means the input string is entirely valid
				if((dirEntry->d_name[DEVICE_NAME_PREFIX_LENGTH] != '\0') && (*endp == '\0'))
				{
					// Device found. Get it's name and add the info to the list
					VideoDeviceInfo deviceInfo;
					deviceInfo.index = index;
					deviceInfo.name  = this->GetDeviceName(index);
					_videoDevList.push_back(deviceInfo);
				}
			}
		}
	} while(dirEntry);
	
	closedir(dir);

	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////
unsigned LinuxCamDeviceCtrl::ReleaseCamera(VideoDevice* _pCamDevice)
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
VideoDevice* LinuxCamDeviceCtrl::GetDevice(unsigned _index)
{
	VideoDevice* videoDev = NULL;

	// Camera Device: ID >=0
	if (_index >= 0)
	{
		videoDev = new CameraCaptureDeviceLinux();
		videoDev->Init(_index);
	}
	// Screen Device: ID < 0
	else
	{
		videoDev = new ScreenDeviceLinux();
		videoDev->Init(_index); // Screen Devices ignores index
	}

	return videoDev;
}
///////////////////////////////////////////////////////////////////////////////

