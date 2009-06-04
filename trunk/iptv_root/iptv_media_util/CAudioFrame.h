#ifndef CAUDIO_FRAME_H
#define CAUDIO_FRAME_H

#include "CFrame.h"

class CAudioFrame : public CFrame
{
private:

    ULONG m_ulChannels,
          m_ulSampleRate,
          m_ulSampleSize;

public:
	
	CAudioFrame(BYTE *_pData, 
				ULONG _ulDatalen, 
				ULONG _ulTimestamp, 
				ULONG _ulChannels, 
				ULONG _ulSampleRate, 
				ULONG _ulSampleSize) : CFrame(AUDIO, _pData, _ulDatalen, _ulTimestamp)
	{
        m_ulChannels   = _ulChannels;
        m_ulSampleRate = _ulSampleRate;	
        m_ulSampleSize = _ulSampleSize;
	}
	
    CAudioFrame() : CFrame(AUDIO)
    {
        m_ulChannels   = 0;
        m_ulSampleRate = 0;	
        m_ulSampleSize = 0;
    }

	virtual ~CAudioFrame() {}

    ULONG GetAudioChannels(ULONG *_pChannels);
    ULONG GetSampleRate(ULONG *_pSampleRate);
    ULONG GetSampleSize(ULONG *_pSampleSize);
    ULONG SetData(BYTE *_pData, ULONG _ulDatalen, ULONG _ulTimestamp, ULONG  _ulChannels, ULONG _ulSampleRate, ULONG _ulSampleSize);

};

#endif
