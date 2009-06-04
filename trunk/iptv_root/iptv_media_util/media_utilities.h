#ifndef MEDIA_UTILITIES_H
#define MEDIA_UTILITIES_H

#include <string>
#include <deque>
#include "mediapkt.h"

struct FrameDimension
{

    FrameDimension(unsigned _width, unsigned _height)
    {
        uWidth  = _width;
        uHeight = _height; 
    }

    FrameDimension()
    {
        uWidth  = 0;
        uHeight = 0;
    }

    unsigned uWidth,
             uHeight;
};

struct EncodingHeader
{
    bool  MPKT_FlagKeyframe;
    unsigned long MPKT_SubSeq;  
    unsigned char MPKT_Type;
 
    unsigned width, 
             height,
             framerate;

};

// macro to check the return value of a function
#define CHECK_ERROR(retCode, ok) if ((retCode) != (ok)) return retCode;
#define INFINITE_TIME   1000000


struct IRM_PKT
{
	MediaPkt  header;
	BYTE	  payload[1];
};


// currently supported media types 
enum MediaSpec {VIDEO, 
                AUDIO,  
                ALL, 
                NONE, 
                SUBTITLE,
                UNKNOWN};

// struct for debugging media
struct MediaDbgInfo
{
    ULONG ulMediaId,
          ulPktRcv,
          ulJitterPkts,
          ulVideoPkts,
          ulAudioPkts,
          ulBufVideoFrames,
          ulBufAudioFrames,
          ulProcVideoFrames,
          ulProcAudioFrames,
          ulVideoTS,
          ulAudioTS,
          ulCurTS,
          ulBufferingPercent;
    
    BOOL  bVideoStarted,
          bAudioStarted,
          bVideoPaused,
          bAudioPaused,
          bBuffering;
};

// function to invert a bitmap upside-down
ULONG TopDownToBottomUp(BYTE *_pData, ULONG _ulDatalen, ULONG _ulHeight);

// function to print a message to console
void   PrintDbgMsg(const std::string & _functionName, const std::string & _msg, const std::string & _dbgStr, int errorCode);

unsigned SepareStrings(std::string & _inStr, std::deque<std::string> & _stringList, char _token);
unsigned ReallocIfRequired(unsigned char **_pBufPtr, unsigned & _uBufPtrLen, unsigned _uRequiredBufLen);

#endif 
