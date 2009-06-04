#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <string>
#include <deque>
#include "alc.h"


enum sampleFmt{MONO8, MONO16, STEREO8, STEREO16};
enum captureState{PLAYING, STOPPED};

class CAudioCapDevice;
class CAudioFrame;

class CAudioCapture
{
private:
    std::deque<CAudioCapDevice *> m_devList;

public:
    CAudioCapture() {}
    ~CAudioCapture();

    unsigned EnumDevices(std::deque<std::string> & _capDevList);
    
    unsigned GetDevice(std::string & _device, 
                       sampleFmt _fmt, 
                       unsigned _frequency, 
                       unsigned _bufLenMs, 
                       CAudioCapDevice **_ppCapDevice);


    unsigned DestroyDevice(CAudioCapDevice *_pDevice);
};

class CAudioCapDevice
{
private:

    bool m_deviceOK;

    ALCdevice * m_capDevice;

    ALCenum  m_sampleFmt;

    captureState m_state;

    unsigned m_frequency,
             m_capBufSize,
             m_channels,
             m_devId;

    static unsigned m_createdDevices;

    unsigned checkLastError();
    unsigned GetSampleSize(unsigned & _sampleSize);

    CAudioCapDevice(std::string & _device, sampleFmt _fmt, unsigned _frequency, unsigned _bufLenMs);

public:
    friend class CAudioCapture;

    ~CAudioCapDevice();

    unsigned GetSamplesCaptured(unsigned & _samples);
    unsigned GetMsCaptured(unsigned & _ms);
    unsigned GetData(CAudioFrame & _audioFrame, unsigned _samples);
    unsigned GetData(unsigned char *_pBuf, unsigned _bufSize, unsigned _samples);
    unsigned FreeBuffer();   
 
    unsigned StartCapture();
    unsigned StopCapture();

    captureState GetCaptureState() {return m_state; }

    bool     DeviceOK()    {return m_deviceOK; }
    unsigned GetDeviceId() {return m_devId;    }
};

#endif
