#ifndef IKERNEL_NOTIFY_H
#define IKERNEL_NOTIFY_H


enum MediaCodeNotify{MN_READ_PACKET, 
                     MN_WRITE_PACKET,
                     MN_READ_VIDEO_FRAME,
                     MN_READ_AUDIO_FRAME,
                     MN_VIDEO_ALIVE, 
                     MN_AUDIO_ALIVE,
                     MN_LOCALE_SET_ENGLISH,
                     MN_LOCALE_RESTORE,
                     MN_BUFFERING_START, 
                     MN_BUFFERING_STOP,
                     MN_ERROR};


struct MediaParam
{
    ULONG id;
    MediaCodeNotify code;
};

struct MediaParamExtBuf : MediaParam
{
    ULONG ulPercent;
};

class CFrame;
struct MediaParamRcvFrame : MediaParam
{
    CFrame * pFrame;
};

struct MediaParamSndPkt : MediaParam
{
    BYTE * buf;
    ULONG  uBufLen;
};

struct MediaParamExtError : MediaParam
{
    ULONG ulErrorCode;
};

class IMediaKernelNotify
{

public:
    virtual ~IMediaKernelNotify() {}
    virtual BOOL OnMediaNotify(MediaParam *_pParameters) = 0;
};

#endif
