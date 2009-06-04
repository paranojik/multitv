#include "compat.h"
#include "AudioCapture.h"
#include "AudioEncoder.h"
#include "log.h"
#include "global_error.h"

extern AudioEncoder *g_pAudioEncoder;
extern unsigned g_captureFmt;

#define BUF_LEN_MS 500

unsigned CaptureAudio(CAudioCapDevice *_pAudioCapDevice);

unsigned InitCapture()
{
    CAudioCapture audioCapture;

    std::deque<std::string> enumDevList;
    audioCapture.EnumDevices(enumDevList);

    for (unsigned index=0; index < enumDevList.size(); index++)
        g_FileLog.SetLog(enumDevList[index].c_str());

    unsigned devPos = 0;
    CAudioCapDevice *pAudioCapDevice;

    do
    {
        audioCapture.GetDevice(enumDevList[devPos], 
                               (sampleFmt)g_captureFmt, 
                               g_pAudioEncoder->GetSampleRate(), 
                               BUF_LEN_MS, 
                               &pAudioCapDevice);    
        devPos++;

    } while( !pAudioCapDevice->DeviceOK() && (devPos < enumDevList.size()) );

    if (!pAudioCapDevice->DeviceOK())
    {    
        g_FileLog.SetLog("Could not find audio device!");
        return RET_ERROR;
    }
    else
        CaptureAudio(pAudioCapDevice);

    return RET_OK;
}