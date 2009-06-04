#include "compat.h"
#include "LinuxCamCapture.h"
#include "ScreenCaptureLinux.h"
#include "gui.h"
#include "media_utilities.h"
#include <deque>
#include "pixel_conversion.h"

#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <iostream>
using namespace std;

//#include "alc.h"
//#include "al.h"
#include "ithread.h"
#include "log.h"
//#include "AudioCapture.h"
#include "CAudioFrame.h"

// video settings
#define MAX_FRAME_DIMENSION 2048

// audio settings
#define BUF_LEN_MS         100
#define BUF_FREQUENCY      44100
#define SAMPLES_TO_PROCESS 3200

unsigned printStandardInfo(AnagVideoStd _std_id);
unsigned TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd);
unsigned printDevCapabilities(std::string _dev);
//unsigned CaptureAudio(CAudioCapDevice *_pAudioCapDevice);
//int GrabCaptureCardAudio(IThread *_pThread, void *_pCtx);

int main(int argc, char **argv)
{
    if (argc != 2)
    {    
        printf("Usage: %s [video device index]\n", argv[0]);
        return 0;
    }

    gtk_init(&argc, &argv);

	if (atoi(argv[1]) < 0)
	{
		ScreenDevice dev;
		dev.Init(1);
        CGui gui(dev);
        gui.CreateWindow();
        return 0;
	}

    CamAbstractionLayerV4L cam;
   	cam.Init(atoi(argv[1]));

    CDevice *pDevice = cam.GetDevice();

    if (!pDevice)
    {    
        printf("Cannot get device!\n");
        return 0;
    }

    std::string deviceName = pDevice->GetDevId();
    printf("device: %s selected\n", deviceName.c_str());

    printDevCapabilities(deviceName);

    std::deque<DeviceInput> inputList;
    cam.GetInputList(inputList);

    printf("\n");
    printf("This device has %u inputs\n", inputList.size());
    printf("\n");
    
    for (int index=0; index<inputList.size(); index++)     
        printf("index: %u - name: %s\n", inputList[index].index, inputList[index].name.c_str());   
    
    printf("\n");
    int selectedInput = 0;

    if (inputList.size() > 1)
    {
        printf("\n");
        char szInput[10];

        printf("\n");
        printf("Choose input:\n");
        printf("\n");  
        printf("Input: ");  scanf("%s", szInput);
    
        selectedInput = atoi(szInput);
        cam.SelectInput(selectedInput);
    }

    printf("\"%s\" selected\n", inputList[selectedInput].name.c_str());

    std::deque<PixFmt> pixFmtList;
    std::deque<FrameDimension> dimensionList; 
        
    std::string pixFmtDesc;

    pDevice->GetSupportedPixelFmt(pixFmtList);
    printf("\n");
    printf("This device supports %u types of pixel format\n", pixFmtList.size());    
    printf("We list them below:\n");
    printf("\n");

    unsigned uMenuItem = 0;

    for (unsigned i=0; i < pixFmtList.size(); i++)
    {
        pDevice->GetSupportedDimensions(pixFmtList[i], dimensionList);
        GetPixFmtDesc(pixFmtList[i], pixFmtDesc);         

        for (unsigned j=0; j < dimensionList.size(); j++)    
            printf("%u. %s - %ux%u\n", ++uMenuItem, pixFmtDesc.c_str(), dimensionList[j].uWidth, dimensionList[j].uHeight);
            
        printf("\n");
    }

    unsigned standardsCount=0;
    cam.GetAnagStdCount(standardsCount);

    printf("\n");
    printf("This device has %u standards\n", standardsCount);
    printf("\n");

    if (standardsCount)
    {
        int stdCounter=0;
        std::deque<AnagVideoStd> allAvailableStds;

        for (int i=0; i<standardsCount; i++)
        {
            printf("Index: %u\n", i);

            std::deque<AnagVideoStd> videoStdList;         
            cam.EnumAnagStd(i, videoStdList);

            for (int j=0; j<videoStdList.size(); j++)
            {    
                printf("%u -", stdCounter);            
                printStandardInfo(videoStdList[j]);
                allAvailableStds.push_back(videoStdList[j]);
                stdCounter++;
            }
            printf("\n");
        }

        unsigned selectedStd=0;
        char szSelectedStd[10];
        printf("\n");
        printf("Choose analog video format: (0 - %u)\n", stdCounter-1);
        printf("\n");
        printf("Format: "); scanf("%s", szSelectedStd);
 
        selectedStd = atoi(szSelectedStd);

        printf("You selected: ");
        printStandardInfo(allAvailableStds[selectedStd]);
        printf("\n");

        cam.SelectAnagStd(allAvailableStds[selectedStd]);
    }

    char szWidth[10],
         szHeight[10];

    unsigned uWidth  = 0,
             uHeight = 0;

    printf("\n");
    printf("Choose capture frame dimensions:\n");
    printf("\n");
  
    printf("Width: ");  scanf("%s", szWidth);
    printf("Height: "); scanf("%s", szHeight);

    uWidth  = atoi(szWidth);
    uHeight = atoi(szHeight);

    if ( (uWidth > 0) && (uWidth < MAX_FRAME_DIMENSION) && (uHeight > 0) && (uHeight < MAX_FRAME_DIMENSION))
    {
        cam.SelectFormat(uWidth, uHeight, RGB24);
        cam.StartCapture();

        CGui gui(cam);
        gui.CreateWindow();
    
        cam.StopCapture();
        cam.Close();
    }
    else
        printf("Error: Width and Height should be in range 0 - %u\n", MAX_FRAME_DIMENSION);

    return 0;
}


#if 0
int main_x()
{
    GrabCaptureCardAudio(0, 0);
    return 0;
}

int GrabCaptureCardAudio(IThread *_pThread, void *_pCtx)
{
    g_FileLog.SetLog("Entering main...");

    if (alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT") != AL_TRUE)
        return RET_ERROR;
    
    CAudioCapture audioCapture;

    std::deque<std::string> enumDevList;
    audioCapture.EnumDevices(enumDevList);

    cout << "Capture device listing:" << endl;

    for (unsigned index=0; index < enumDevList.size(); index++)
    {
        g_FileLog.SetLog(enumDevList[index].c_str());
        cout << index << ". " << enumDevList[index].c_str() << endl;
    }

    // MONO8
    // MONO16
    // STEREO8
    // STEREO16

    cout << endl;
    cout << "Select device: ";
    unsigned devPos;
    cin >> devPos;
    cout << endl;

    // if overflow then select the last one
    if(devPos >= enumDevList.size())
        devPos = (unsigned)(enumDevList.size() - 1);

    cout << "Device " << devPos << " selcted." << endl;

    CAudioCapDevice *pAudioCapDevice;
    audioCapture.GetDevice(enumDevList[devPos], STEREO16, BUF_FREQUENCY, BUF_LEN_MS, &pAudioCapDevice);

    if (pAudioCapDevice->DeviceOK())
        CaptureAudio(pAudioCapDevice);
    else
        g_FileLog.SetLog("Could not find audio device!");

    g_FileLog.SetLog("Leaving main...");

    return 0;
}

unsigned CaptureAudio(CAudioCapDevice *_pAudioCapDevice)
{
    if (!_pAudioCapDevice)
        return RET_INVALID_ARG;

    const ALCchar *renderDevices,
                  *defaultRenderDevice;

    bool renderOK = false;

    ALCcontext * pAudioContext = NULL;
    ALCdevice  * pRenderDevice = NULL;

    std::deque<int> freeBuffersList;

    ALuint  source,
			buffers[10];

    renderDevices       = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    defaultRenderDevice = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    pRenderDevice = alcOpenDevice(defaultRenderDevice);
    
    if (pRenderDevice) 
    {
        pAudioContext = alcCreateContext(pRenderDevice, NULL);
        
        if (pAudioContext)
        {
            if (alcMakeContextCurrent(pAudioContext))
                renderOK = true;
        }
    }

    alGenSources(1, &source);

    if (alGetError() == AL_NO_ERROR)
    {
        alGenBuffers(10, buffers);
        
        if (alGetError() != AL_NO_ERROR)
            return RET_ERROR;

        for(ULONG i=0; i<10; i++)
            freeBuffersList.push_back(buffers[i]); 
    }

    _pAudioCapDevice->StartCapture();

    unsigned  availableSamples;
    CAudioFrame audioFrame;

    int initialTime = GetTickCount();

    while(1)
    {
        if (GetTickCount() - initialTime > 6000)
            break;

        CHECK_ERROR(_pAudioCapDevice->GetSamplesCaptured(availableSamples), RET_OK)

	printf("%d\n", availableSamples);
        
        if (availableSamples >= SAMPLES_TO_PROCESS)
        {
            CHECK_ERROR(_pAudioCapDevice->GetData(audioFrame, SAMPLES_TO_PROCESS), RET_OK)

            ALint iBuffersProcessed;
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);
        
            if (alGetError() != AL_NO_ERROR)
                return RET_ERROR;

            if (iBuffersProcessed)
            {
                ALuint *pUnqueuedBuffers = new ALuint[iBuffersProcessed];
                alSourceUnqueueBuffers(source, iBuffersProcessed, pUnqueuedBuffers);

                if (alGetError() != AL_NO_ERROR)
                    return RET_ERROR;

                for (ULONG i=0; i<(ULONG)iBuffersProcessed; i++)
			        freeBuffersList.push_back(pUnqueuedBuffers[i]);
		        
	           delete pUnqueuedBuffers;
	        }

            if (freeBuffersList.size())
            {
                ALuint bid = freeBuffersList[0];
        
                unsigned long sampleRate,
                              datalen,
                              channels,
                              sampleSize;

                unsigned char *pData;

                audioFrame.GetSampleRate(&sampleRate);
                audioFrame.GetFrameData(&pData, &datalen);

                audioFrame.GetAudioChannels(&channels);
                audioFrame.GetSampleSize(&sampleSize);

                ALenum audioFmt;

                switch(channels)
                {
                    case 1:
                        switch(sampleSize)
                        {
                            case 1:
                                audioFmt = AL_FORMAT_MONO8;
                            break;

                            case 2:
                                audioFmt = AL_FORMAT_MONO16;
                            break;
                        }
                    break;

                    case 2:
                        switch(sampleSize)
                        {
                            case 1:
                               audioFmt = AL_FORMAT_STEREO8;
                            break;

                            case 2:
                                audioFmt = AL_FORMAT_STEREO16;
                            break;
                        }
                    break;
                }

                alBufferData(bid, audioFmt, pData, datalen, sampleRate);
                
                if (alGetError() != AL_NO_ERROR)
                    return RET_ERROR;

                alSourceQueueBuffers(source, 1, &bid);

                if (alGetError() != AL_NO_ERROR)
                    return RET_ERROR;                

                freeBuffersList.pop_front();
            }
            
            ALint iSourceState;

            alGetSourcei(source, AL_SOURCE_STATE, &iSourceState);
            
            if (alGetError() != AL_NO_ERROR)
                return RET_ERROR;

            if (iSourceState != AL_PLAYING) 
            {   
                alSourcePlay(source);

                if (alGetError() != AL_NO_ERROR)
                    return RET_ERROR;
            }
        }

        Wait(10);
    }

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, AL_NONE);
    alDeleteBuffers(10, buffers);
    alDeleteSources(1, &source);

	ALCcontext *pContext = alcGetCurrentContext();
	ALCdevice  *pDevice  = alcGetContextsDevice(pContext);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);

    return RET_OK;
}
#endif

unsigned printStandardInfo(AnagVideoStd _std_id)
{
        switch(_std_id)
        {
            case PAL_B:
                printf("PAL_B\n");
            break;

            case PAL_B1:
                printf("PAL_B1\n");
            break;
            case PAL_G:
                printf("PAL_G\n");
            break;
            case PAL_H:
                printf("PAL_H\n");
            break;
            case PAL_I:
                printf("PAL_I\n");
            break;
            case PAL_D:
                printf("PAL_D\n");
            break;
            case PAL_D1:
                printf("PAL_D1\n");
            break;
            case PAL_K:
                printf("PAL_K\n");
            break;
            case PAL_M:
                printf("PAL_M\n");
            break;
            case PAL_N:
                printf("PAL_N\n");
            break;
            case PAL_Nc:
                printf("PAL_Nc\n");
            break;
            case PAL_60:
                printf("PAL_60\n");
            break;
            case NTSC_M:
                printf("NTSC_M\n");
            break;
            case NTSC_M_JP:
                printf("NTSC_M_JP\n");
            break;
            case NTSC_443:
                printf("NTSC_443\n");
            break;
            case NTSC_M_KR:
                printf("NTSC_M_KR\n");
            break;
            case SECAM_B:
                printf("SECAM_B\n");
            break;
            case SECAM_D:
                printf("SECAM_D\n");
            break;
            case SECAM_G:
                printf("SECAM_G\n");
            break;
            case SECAM_H:
                printf("SECAM_H\n");
            break;
            case SECAM_K:
                printf("SECAM_K\n");
            break;
            case SECAM_K1:
                printf("SECAM_K1\n");
            break;
            case SECAM_L:
                printf("SECAM_L\n");
            break;
            case SECAM_LC:
                printf("SECAM_LC\n");
            break;
            case ATSC_8_VSB:
                printf("ATSC_8_VSB\n");
            break;
            case ATSC_16_VSB:
                printf("ATSC_16_VSB\n");
            break;

            default:
                printf("Unrecognized standard!\n");
                return RET_ERROR;
        }

        return RET_OK;
}

unsigned TranslateToAnagStd(v4l2_std_id std_id, AnagVideoStd & _videoStd)
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

unsigned printDevCapabilities(std::string _dev)
{
    unsigned ret = RET_OK;
    int fd;

    if ( (fd = open(_dev.c_str(), O_RDWR)) != -1)
    {
        v4l2_capability cap;
        memset(&cap, 0, sizeof(cap));

        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) != -1)
        {
            printf("\n");
            printf("Device %s information:\n", _dev.c_str());
            printf("\n");

            printf("cap.driver: %s\n", cap.driver);
            printf("cap.card: %s\n", cap.card);
            printf("cap.bus_info: %s\n", cap.bus_info);
            printf("cap.version: %u\n", cap.version);

            printf("\n");
            printf("Printing %s capabilities\n", _dev.c_str());
            printf("\n");

            if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
                printf("V4L2_CAP_VIDEO_CAPTURE\n");

            if (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
                printf("V4L2_CAP_VIDEO_OUTPUT\n");

            if (cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
                printf("V4L2_CAP_VIDEO_OVERLAY\n");

            if (cap.capabilities & V4L2_CAP_VBI_CAPTURE)
                printf("V4L2_CAP_VBI_CAPTURE\n");

            if (cap.capabilities & V4L2_CAP_VBI_OUTPUT)
                printf("V4L2_CAP_VBI_OUTPUT\n");

            if (cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE)
                printf("V4L2_CAP_SLICED_VBI_CAPTURE\n");

            if (cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT)
                printf("V4L2_CAP_SLICED_VBI_OUTPUT\n");

            if (cap.capabilities & V4L2_CAP_RDS_CAPTURE)
                printf("V4L2_CAP_RDS_CAPTURE\n");

            if (cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY)
                printf("V4L2_CAP_VIDEO_OUTPUT_OVERLAY\n");

            if (cap.capabilities & V4L2_CAP_TUNER)
                printf("V4L2_CAP_TUNER\n");

            if (cap.capabilities & V4L2_CAP_AUDIO)
                printf("V4L2_CAP_AUDIO\n");

            if (cap.capabilities & V4L2_CAP_RADIO)
                printf("V4L2_CAP_RADIO\n");

            if (cap.capabilities & V4L2_CAP_READWRITE)
                printf("V4L2_CAP_READWRITE\n");

            if (cap.capabilities & V4L2_CAP_ASYNCIO)
                printf("V4L2_CAP_ASYNCIO\n");

            if (cap.capabilities & V4L2_CAP_STREAMING)
                printf("V4L2_CAP_STREAMING\n");
        }
        else
        {
            printf("%s - VIDIOC_QUERYCAP error!\n", _dev.c_str());
        }
    }
    else
    {
        printf("Error trying to open %s\n", _dev.c_str());
        ret = RET_ERROR;
    }

    return ret;
}

