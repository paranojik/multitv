#include "compat.h"
#include "AudioCapture.h"
#include "global_error.h"
#include "al.h"
#include "log.h"
#include "media_utilities.h"
#include "CAudioFrame.h"

#ifdef _LINUX
	#include "shared/compat.h"
#endif

CAudioCapture::~CAudioCapture()
{
    while(m_devList.size())
    {
        delete m_devList[0];
        m_devList.pop_front();
    }
}


// enumerate all available devices, the first one is the default device
unsigned CAudioCapture::EnumDevices(std::deque<std::string> & _capDevList)
{
    const ALCchar * capDevices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    
	if(!capDevices)	// No capture devices present...?
		return RET_ERROR;

    bool enumEnd = false;

    unsigned beginPos = 0,
             index    = 0;

    while (!enumEnd)
    {
        if (capDevices[index] == NULL)
        {    
            std::string devName = &capDevices[beginPos];
            _capDevList.push_back(devName);

            beginPos = index+1;

            if (capDevices[beginPos] == NULL)
                enumEnd = true;
        }

        index++;
    }

    return RET_OK;
}
    
unsigned CAudioCapture::GetDevice(std::string & _device, sampleFmt _fmt, unsigned _frequency, unsigned _bufLenMs, CAudioCapDevice **_ppCapDevice)
{
    if (!_ppCapDevice)
        return RET_INVALID_ARG;

    *_ppCapDevice = NULL;

    if ((_fmt != MONO8) && (_fmt != MONO16) && (_fmt != STEREO8) && (_fmt != STEREO16))
        return RET_INVALID_ARG;

    std::deque<std::string> devList;
    EnumDevices(devList);

    bool devFound = false;

    for (unsigned index=0; index < devList.size(); index++)
    {
        if (devList[index] == _device)
        {    
            devFound = true;
            break;
        }
    }

    unsigned ret = RET_OK;

    if (devFound)
    {
        CAudioCapDevice * pAudioCapDevice = new CAudioCapDevice(_device, _fmt, _frequency, _bufLenMs);

        if (pAudioCapDevice)
        {
            if (pAudioCapDevice->DeviceOK() )   
            {   
                m_devList.push_back(pAudioCapDevice);
                *_ppCapDevice = pAudioCapDevice;
            }
            else
            {    
                delete pAudioCapDevice;
                ret = RET_DEVICE_ERROR;
            }
        }
        else
            ret = RET_LOW_MEMORY;
    }
    else
        ret = RET_INVALID_ARG;

    return ret;
}


unsigned CAudioCapture::DestroyDevice(CAudioCapDevice *_pDevice)
{
    if (!_pDevice)
        return RET_INVALID_ARG;

    std::deque<CAudioCapDevice *>::iterator it = m_devList.begin();
    
    bool devFound = false;

	unsigned index=0;
    for (; index < m_devList.size(); index++)
    {
        if (m_devList[index]->GetDeviceId() == _pDevice->GetDeviceId())
        {
            delete m_devList[index];
            devFound = true;
			break;
        }
    }

    if (devFound)
	{
		it += index;
		m_devList.erase(it);
	}
	else
        return RET_DEVICE_NOT_FOUND;

    return RET_OK;
}

unsigned CAudioCapDevice::m_createdDevices = 0;

CAudioCapDevice::CAudioCapDevice(std::string & _device, sampleFmt _fmt, unsigned _frequency, unsigned _bufLenMs)
{
    unsigned bytesPerSample;

    m_deviceOK = false;
    m_devId    = 0;
    
    switch(_fmt)
    {
        case MONO8:
            m_sampleFmt    = AL_FORMAT_MONO8;
            m_channels     = 1;    
            bytesPerSample = 1;
        break;

        case MONO16:
            m_sampleFmt    = AL_FORMAT_MONO16;
            m_channels     = 1;
            bytesPerSample = 2;
        break;

        case STEREO8:
            m_sampleFmt    = AL_FORMAT_STEREO8;
            m_channels     = 2;
            bytesPerSample = 1;
        break;

        case STEREO16:
            m_sampleFmt    = AL_FORMAT_STEREO16;
            m_channels     = 2;
            bytesPerSample = 2;
        break;
    }

    m_state      = STOPPED;

    m_frequency  = (unsigned)_frequency;
    m_capBufSize = (unsigned)(m_frequency * (_bufLenMs / 1000.0));

    m_capDevice  = alcCaptureOpenDevice(_device.c_str(), m_frequency, m_sampleFmt, m_capBufSize);

    if (m_capDevice && (checkLastError() == RET_OK))
    {    
        m_devId    = m_createdDevices;
        m_deviceOK = true;
        m_createdDevices++;
    }
}


CAudioCapDevice::~CAudioCapDevice()
{
   alcCaptureStop(m_capDevice);
   alcCaptureCloseDevice(m_capDevice);
}

unsigned CAudioCapDevice::checkLastError()
{
    if (!m_capDevice)
        return RET_ERROR;

    ALenum error;

    if ((error = alcGetError(m_capDevice)) != ALC_NO_ERROR)
    {        
    #ifdef _DEBUG
        
        char errorBuf[50];
        sprintf(errorBuf, "CAudioCapDevice::checkLastError, OpenAL error: %d\n", error);
        
        g_FileLog.SetLog(errorBuf);
    
    #endif
        
        return RET_ERROR;
    }

    return RET_OK;
}

unsigned CAudioCapDevice::GetSampleSize(unsigned & _sampleSize)
{
    switch(m_sampleFmt)
    {
        case AL_FORMAT_MONO8:
        case AL_FORMAT_STEREO8:

            _sampleSize = 1;    
        
        break;

        case AL_FORMAT_MONO16:
        case AL_FORMAT_STEREO16:

            _sampleSize = 2;
        
        break;
    }

    return RET_OK;
}

unsigned CAudioCapDevice::GetSamplesCaptured(unsigned & _samples)
{
    alcGetIntegerv(m_capDevice, ALC_CAPTURE_SAMPLES, sizeof(_samples), (ALCint *)&_samples);
    
    CHECK_ERROR(checkLastError(), RET_OK)

    return RET_OK;
}

unsigned CAudioCapDevice::GetMsCaptured(unsigned & _ms)
{
    unsigned samples;
    CHECK_ERROR(GetSamplesCaptured(samples), RET_OK)

    _ms = (unsigned)((samples * 1000.0) / m_frequency);

    return RET_OK;
}

unsigned CAudioCapDevice::GetData(unsigned char *_pBuf, unsigned _bufSize, unsigned _samples)
{
    if (!_pBuf)
        return RET_INVALID_ARG;

    unsigned sampleSize;
    GetSampleSize(sampleSize);

    unsigned datalen = _samples * sampleSize * m_channels;

    if (_bufSize < datalen)
        return RET_BUFFER_TOO_SMALL;

    alcCaptureSamples(m_capDevice, _pBuf, _samples);
    CHECK_ERROR(checkLastError(), RET_OK)

    return RET_OK;
}

unsigned CAudioCapDevice::GetData(CAudioFrame & _audioFrame, unsigned _samples)
{
    unsigned samplesCaptured;
    CHECK_ERROR(GetSamplesCaptured(samplesCaptured), RET_OK)

    if (samplesCaptured < _samples)
        return RET_INVALID_ARG;

    unsigned sampleSize;
    GetSampleSize(sampleSize);
    
    unsigned bufferSize = _samples*sampleSize*m_channels;

    unsigned char *sampleBuffer = new unsigned char[bufferSize];

    if (sampleBuffer)
    {    
        alcCaptureSamples(m_capDevice, sampleBuffer, _samples);
        _audioFrame.SetData(sampleBuffer, bufferSize, GetTickCount(), m_channels, m_frequency, sampleSize);

        delete [] sampleBuffer;

        CHECK_ERROR(checkLastError(), RET_OK)
    }
    else
        return RET_LOW_MEMORY;

    return RET_OK;
}

unsigned CAudioCapDevice::StartCapture()
{
    if (m_state = STOPPED)
    {
        alcCaptureStart(m_capDevice);
        CHECK_ERROR(checkLastError(), RET_OK)

        m_state = PLAYING;
    }

    return RET_OK;
}

unsigned CAudioCapDevice::StopCapture()
{
    if (m_state == PLAYING)
    {
        alcCaptureStop(m_capDevice);
        CHECK_ERROR(checkLastError(), RET_OK)

        m_state = STOPPED;
    }

    return RET_OK;
}

unsigned CAudioCapDevice::FreeBuffer()
{
    unsigned samples;
    CHECK_ERROR(GetSamplesCaptured(samples), RET_OK)

    unsigned sampleSize;
    GetSampleSize(sampleSize);
    
    unsigned bufferSize = samples * sampleSize * m_channels;

    unsigned char *pBuf = new unsigned char[bufferSize];

    if (pBuf)
    {
        GetData(pBuf, bufferSize, samples);
        delete  pBuf;
    }
    else
        return RET_LOW_MEMORY;

    return RET_OK;
}
