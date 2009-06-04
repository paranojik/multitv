#include "compat.h"
#include <stdio.h>
#include "alc.h"
#include "al.h"
#include "ithread.h"
#include <deque>
#include "log.h"
#include "AudioCapture.h"
#include "CAudioFrame.h"
#include "global_error.h"
#include <iostream>

#define BUF_LEN_MS         100
#define BUF_FREQUENCY      44100
#define SAMPLES_TO_PROCESS 3200

using namespace std;

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

    while(1)
    {
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


int main(int argc, char **argv)
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

    //do
    //{
    audioCapture.GetDevice(enumDevList[devPos], STEREO16, BUF_FREQUENCY, BUF_LEN_MS, &pAudioCapDevice);
    //    devPos++;

    //} while( !pAudioCapDevice->DeviceOK() && (devPos < enumDevList.size()) );

    if (pAudioCapDevice->DeviceOK())
        CaptureAudio(pAudioCapDevice);
    else
        g_FileLog.SetLog("Could not find audio device!");

    g_FileLog.SetLog("Leaving main...");

    return 0;
}

