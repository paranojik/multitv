#include "compat.h"
#include <stdio.h>
#include <deque>
#include "alc.h"
#include "al.h"
#include "ithread.h"
#include "log.h"
#include "AudioCapture.h"
#include "CAudioFrame.h"
#include "global_error.h"
#include "AudioEncoder.h"
#include "AudioDecoder.h"
#include "MediaNetwork.h"
#include "Play.h"
#include "Capture.h"


#define AUDIO_ENCODED_BUF  1000
#define AUDIO_DECODED_BUF  3840


//-------------- CODEC TO USE --------------//

//int g_MediaType = MTYPE_MEDIA_AUDIO_SPEEX_3F;
//int g_MediaType = MTYPE_MEDIA_AUDIO_G711_ALAW; 
//int g_MediaType = MTYPE_MEDIA_AUDIO_G711_ULAW;
//int g_MediaType = MTYPE_MEDIA_AUDIO_G723_2F;
int g_MediaType = MTYPE_MEDIA_AUDIO_G728_4F;
//int g_MediaType = MTYPE_MEDIA_AUDIO_G729A_6F;
//int g_MediaType = MTYPE_MEDIA_AUDIO_AAC_48;
//int g_MediaType = MTYPE_MEDIA_AUDIO_AAC_128;

unsigned g_SamplesToProcess,
         g_captureFmt,
         g_playFmt;

AudioDecoder   *g_pAudioDecoder;
AudioEncoder   *g_pAudioEncoder;
DsCScriptValue  g_tableParam;

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

    CAudioFrame audioFrame;

    BYTE encodedBuf[AUDIO_ENCODED_BUF],
         decodedBuf[AUDIO_DECODED_BUF];

    unsigned uEncodedBufSize,
             uDecodedBufSize,
             uDatalen,
             uAvailableSamples;
    
    int iSamples;

    while(1)
    {
        CHECK_ERROR(_pAudioCapDevice->GetSamplesCaptured(uAvailableSamples), RET_OK)

        if (uAvailableSamples >= g_SamplesToProcess)
        {
            CHECK_ERROR(_pAudioCapDevice->GetData(audioFrame, g_SamplesToProcess), RET_OK)

            BYTE *pData;
            audioFrame.GetFrameData(&pData, (ULONG *)&uDatalen);

            AudioCodecParam audioParam;
            memset(&audioParam, 0, sizeof(audioParam));

            uEncodedBufSize = AUDIO_ENCODED_BUF;
            g_pAudioEncoder->Encode(pData, uDatalen, encodedBuf, uEncodedBufSize, &audioParam);

            uDecodedBufSize = AUDIO_DECODED_BUF;
            g_pAudioDecoder->Decode(encodedBuf, uEncodedBufSize, decodedBuf, uDecodedBufSize, NULL);

            Play(source, freeBuffersList, decodedBuf, uDecodedBufSize);
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

int main(int argc, char **argv)
{    

    g_FileLog.SetLog("Entering main...");

    DsCScriptEngine m_luaScriptConfig;

    if (m_luaScriptConfig.Execute("mc.cfg") == DsS_OK)
    {
        g_tableParam = m_luaScriptConfig.GetGlobalValue("profile_av_streams_types").GetTableValue(g_MediaType);

        std::string transmission, 
                    network, 
                    bitrate;

        transmission = "conf";
        network      = "internet"; 
        bitrate      = "33k";

        switch (g_MediaType)
        {
            case MTYPE_MEDIA_VOICE_SPEEX_9NB:
            case MTYPE_MEDIA_VOICE_SPEEX_4UWB:
            case MTYPE_MEDIA_VOICE_SPEEX_2UWB:
            case MTYPE_MEDIA_VOICE_SPEEX_2NB:
            case MTYPE_MEDIA_AUDIO_SPEEX_6F:
            case MTYPE_MEDIA_AUDIO_SPEEX_3F:
            case MTYPE_MEDIA_AUDIO_EX_SPEEX_6F:
            case MTYPE_MEDIA_AUDIO_EX_SPEEX_3F:
                g_pAudioDecoder = new SpeexDecoder(g_MediaType);  
                g_pAudioEncoder = new SpeexHIEncoder(transmission, network, bitrate);
            break;

            case MTYPE_MEDIA_AUDIO_G711_ALAW:
                g_pAudioDecoder = new G711Decoder(g_MediaType);
                g_pAudioEncoder = new G711ALawEncoder(transmission, network, bitrate);
            break;

            case MTYPE_MEDIA_AUDIO_G711_ULAW:
                g_pAudioDecoder = new G711Decoder(g_MediaType);
                g_pAudioEncoder = new G711ULawEncoder(transmission, network, bitrate);
            break;

            case MTYPE_MEDIA_AUDIO_G723_2F:
                g_pAudioDecoder = new G723Decoder(g_MediaType);
                //g_pAudioEncoder = new G723_53_Encoder(transmission, network, bitrate);
                g_pAudioEncoder = new G723_63_Encoder(transmission, network, bitrate);
            break;

            case MTYPE_MEDIA_AUDIO_G728_4F:
                g_pAudioDecoder = new G728Decoder(g_MediaType);
                //g_pAudioEncoder = new G728_96_Encoder(transmission, network, bitrate);
                //g_pAudioEncoder = new G728_128_Encoder(transmission, network, bitrate); 
                g_pAudioEncoder = new G728_160_Encoder(transmission, network, bitrate);          
            break;


            case MTYPE_MEDIA_AUDIO_G729A_6F:
                g_pAudioDecoder = new G729Decoder(g_MediaType);
                g_pAudioEncoder = new G729Encoder(transmission, network, bitrate);        
            break;

            case MTYPE_MEDIA_AUDIO_AAC_48:
                g_pAudioDecoder = new AACDecoder(g_MediaType);
                g_pAudioEncoder = new AAC_48_Encoder(transmission, network, bitrate);     
            break;

            case MTYPE_MEDIA_AUDIO_AAC_128:
                g_pAudioDecoder = new AACDecoder(g_MediaType);
                g_pAudioEncoder = new AAC_128_Encoder(transmission, network, bitrate);          
            break;

            default:
                return 0;
        }

        CHECK_ERROR(g_pAudioDecoder->Init(), RET_OK)
        CHECK_ERROR(g_pAudioEncoder->Init(), RET_OK)

        g_SamplesToProcess = g_pAudioEncoder->GetFrameSamples()*g_pAudioEncoder->GetFramesPkt();
        g_pAudioEncoder->GetChannels() == 1 ? g_captureFmt = MONO16 : g_captureFmt = STEREO16;

        if (alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT") != AL_TRUE)
            return RET_ERROR;
    
        InitCapture();
    }

    g_FileLog.SetLog("Leaving main...");

    return 0;
}