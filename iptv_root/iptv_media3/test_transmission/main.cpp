#include "compat.h"
#include "mediapkt.h" 
//#include "CapCam.h"
#include "AudioCapture.h"
#include "socket\proxyparam.h"
#include "socket\sockmwbuffs.h"
#include "VideoEncoder.h"
#include "AudioEncoder.h"
#include "ithread.h"
#include "CAudioFrame.h"
#include "MediaNetwork.h"
#include "global_error.h"
#include "CVideoFrame.h"
#include "iptv_kernel/MediaCapture.h"


#define BUF_LEN_MS             500
#define MAX_ENCODED_AUDIO_SIZE 10000
#define MAX_ENCODED_VIDEO_SIZE 20000

void StartTransmission();

//CapCam            g_CapVideo;
MediaCapture		g_CapVideo;
CAudioCapture		g_CapAudio;
CAudioCapDevice *	g_pAudioCapDevice;

AudioNetworkEncoder g_AudioNetwork;
VideoNetworkEncoder g_VideoNetwork;

VideoEncoder * g_pVideoEncoder;
AudioEncoder * g_pAudioEncoder;


unsigned g_MediaId = 1470416,
         g_SamplesToProcess,
         g_captureFmt,
		 g_miliSecInAFrame;


SockMWBuffs g_Socket(0, 0, 6000, FALSE, FALSE, // não buferiza o audio
                                        MW_DEFRAGMODE_PKT | MW_DEFRAGMODE_PKTEXT);



// video codecs

//std::string g_VideoCodec = MPEG4;
std::string g_VideoCodec = H264;


// audio codecs

//std::string g_AudioCodec = SPEEX_LO;
std::string g_AudioCodec =  SPEEX_HI;
//std::string g_AudioCodec =  G711A;
//std::string g_AudioCodec =  G711U;
//std::string g_AudioCodec =  G729A;
//std::string g_AudioCodec =  G723_53;
//std::string g_AudioCodec =  G723_63;
//std::string g_AudioCodec =  G728_96;
//std::string g_AudioCodec =  G728_128;
//std::string g_AudioCodec =  G728_160;
//std::string g_AudioCodec =  AAC_48;
//std::string g_AudioCodec =  AAC_128;


int AudioThread(IThread *_pThread, void *_pParam)
{
    unsigned samplesCaptured;
 
    BYTE encodedData[MAX_ENCODED_AUDIO_SIZE];
    
    unsigned uEncodedDataLen,
             uSubSeq,
             uHeaderLen,
             uBytesWritten;

    CAudioFrame audioFrame;

    uSubSeq = 0;

    while (1)
    {
        g_pAudioCapDevice->GetSamplesCaptured(samplesCaptured);

        if (samplesCaptured >= g_SamplesToProcess)
        {
            if (g_pAudioCapDevice->GetData(audioFrame, g_SamplesToProcess) == RET_OK)
            {
                uEncodedDataLen = MAX_ENCODED_AUDIO_SIZE;
                if (g_AudioNetwork.EncodePkt(audioFrame, g_MediaId, uSubSeq, encodedData, uEncodedDataLen, uHeaderLen) == RET_OK)
                {
                    MediaPktExt *pMediaPktExt;
                    pMediaPktExt = (MediaPktExt *)encodedData;
                    
                    g_Socket.Write(encodedData, uEncodedDataLen, (ULONG *)&uBytesWritten);
                    uSubSeq++;
                }
            }
        }
        else
            Wait((unsigned long)g_miliSecInAFrame);

    }
    return 0;
}

int VideoThread(IThread *_pThread, void *_pParam)
{
	bool videoInitialized = g_CapVideo.InitializeVideoCapture(-1);

	if(videoInitialized)
	{
		CVideoFrame videoFrame;
	 
		BYTE encodedData[MAX_ENCODED_VIDEO_SIZE];

		unsigned uEncodedDataLen,
				 uSubSeq,
				 uHeaderLen,
				 uBytesWritten;

		uSubSeq = 0;

		while(1)
		{
			if (g_CapVideo.GrabVideoFrame())
			{
				videoFrame.SetData(g_CapVideo.GetVideoData(), g_CapVideo.GetVideoWidth() * g_CapVideo.GetVideoHeight() * 3, GetTickCount(), g_CapVideo.GetVideoWidth(), g_CapVideo.GetVideoHeight(), RGB24);
				uEncodedDataLen = MAX_ENCODED_VIDEO_SIZE;

				if (g_VideoNetwork.EncodePkt(videoFrame, g_MediaId, uSubSeq, encodedData, uEncodedDataLen, uHeaderLen) == RET_OK)
				{
					MediaPktExt *pMediaPktExt = (MediaPktExt *)encodedData;

					g_Socket.Write(encodedData, uEncodedDataLen, (ULONG *)&uBytesWritten);
					uSubSeq++;
	  
					Wait((unsigned)(1000.0 / g_pVideoEncoder->GetFramerate()));
				}
			}
			else
				Wait(10);
		}
	}

    return 0;
}


int main()
{
    std::string strTransmission, 
                strNetwork,
                strBitrate,
                strVideoCodec,
                strAudioCodec;

    strTransmission = "conf";
    strNetwork      = "internet"; 
    strBitrate      = "128k";

    g_AudioNetwork.Init(strTransmission, strNetwork, strBitrate, g_AudioCodec);
    g_VideoNetwork.Init(strTransmission, strNetwork, strBitrate, g_VideoCodec);

    g_pVideoEncoder = (VideoEncoder *) g_VideoNetwork.GetCodec();
    g_pAudioEncoder = (AudioEncoder *) g_AudioNetwork.GetCodec();
	g_miliSecInAFrame = g_pAudioEncoder->GetFrameSamples() * 1000 / g_pAudioEncoder->GetSampleRate();

//"192.168.7.55"
    g_Socket.SetUrl("UDP", "146.164.42.218", 8001, NULL, FALSE, NULL); 
    g_Socket.OpenUrl();

    StartTransmission();

    while(1)
        Wait(1000);

    return 0;
}

void StartTransmission()
{
    std::deque<std::string> enumDevList;
    g_CapAudio.EnumDevices(enumDevList);    
    
    g_SamplesToProcess = g_pAudioEncoder->GetFrameSamples()*g_pAudioEncoder->GetFramesPkt();
    g_pAudioEncoder->GetChannels() == 1 ? g_captureFmt = MONO16 : g_captureFmt = STEREO16;

    int devPos = 0;
    
    do
    {
        g_CapAudio.GetDevice(enumDevList[devPos], 
                             (sampleFmt)g_captureFmt, 
                             g_pAudioEncoder->GetSampleRate(), 
                             BUF_LEN_MS, 
                             &g_pAudioCapDevice); 
        devPos++;

    } while( !g_pAudioCapDevice->DeviceOK() && (devPos < (int)enumDevList.size()) );

    g_pAudioCapDevice->StartCapture();

    CreateIThread(AudioThread, NULL, TRUE);
	CreateIThread(VideoThread, NULL, TRUE);
}