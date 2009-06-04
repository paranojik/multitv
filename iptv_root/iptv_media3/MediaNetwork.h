#ifndef MEDIA_NETWORK_H
#define MEDIA_NETWORK_H

#include <string>
#include "ISendKernelNotify.h"
#include "media_utilities.h"
#include "semaphore.h"

struct CodecParam
{
    unsigned char MPKT_type;
};

struct VideoCodecParam : CodecParam
{
    bool MPKT_keyFrame;
};

struct AudioCodecParam : CodecParam
{
};

class CFrame;
class MediaCodec;
class MediaNetwork
{
protected:

    bool        m_Init;
    MediaCodec *m_pCodec;
    ULONG       m_ulSubSeq;

    unsigned CheckParameters(std::string _transmission, std::string _network, std::string _bitrate);

public:

    MediaNetwork()
    {
        m_Init     = false;
        m_pCodec   = NULL;
        m_ulSubSeq = 0;
    }

    bool Ok() {return m_Init; }

    MediaCodec *GetCodec() {return m_pCodec; }

    virtual unsigned Init(std::string _transmission, 
                          std::string _network, 
                          std::string _bitrate,
                          std::string _codec) = 0;

    virtual unsigned EncodePkt(CFrame & _frame, 
                               unsigned _uMediaId, 
                               unsigned _uSeq, 
                               BYTE *_pBuf, 
                               unsigned & _uBufLen, 
                               unsigned & _uHeaderLen) = 0;
};

class AudioNetworkEncoder : public MediaNetwork
{

public:

    unsigned Init(std::string _transmission, 
                  std::string _network, 
                  std::string _bitrate,
                  std::string _codec);

    unsigned EncodePkt(CFrame & _frame, 
                       unsigned _uMediaId, 
                       unsigned _uSeq, 
                       BYTE *_pBuf, 
                       unsigned & _uBufLen, 
                       unsigned & _uHeaderLen);
};

class VideoNetworkEncoder : public MediaNetwork
{

public:

    unsigned Init(std::string _transmission, 
                  std::string _network, 
                  std::string _bitrate,
                  std::string _codec);

    unsigned EncodePkt(CFrame & _frame, 
                       unsigned _uMediaId, 
                       unsigned _uSeq, 
                       BYTE *_pBuf, 
                       unsigned & _uBufLen, 
                       unsigned & _uHeaderLen);
};


class IFrameSrc
{

public:
    BOOL OnMediaNotify(MediaParam *_pParameters) {return TRUE; }
    
};

class IThread;

class MediaEncoder : public ISendKernelNotify
{
private:

    void EncodeAndTransmit(bool & _mediaRunning, 
                           MediaParamRcvFrame & rcvFrame, 
                           MediaParamSndPkt & sndPkt, 
                           MediaNetwork & _mediaNetwork);
    
    VideoNetworkEncoder m_VideoEncoder;
    AudioNetworkEncoder m_AudioEncoder;

    IFrameSrc *m_pFrameSrc;

    IThread *m_pVideoThread,
            *m_pAudioThread;

    _SEMAPHORE m_Semaph;

    bool m_flVideoRunning,
         m_flAudioRunning;

    unsigned m_uMediaId;

    static int VideoEncoderThread(IThread *_pThread, void *_pParam);
    static int AudioEncoderThread(IThread *_pThread, void *_pParam);

public:

    MediaEncoder(unsigned _uMediaId)
    {
        m_pFrameSrc = NULL;
        m_uMediaId  = _uMediaId;

        m_flVideoRunning = false;
        m_flAudioRunning = false;
    }

    ~MediaEncoder() {}

    unsigned SetFrameSrc(IFrameSrc *_pFrameSrc);

    unsigned SetEncoder(std::string _transmission, 
                        std::string _network, 
                        std::string _bitrate, 
                        std::string _codec, 
                        MediaSpec _mediaSpec);

    unsigned StartEncoding(MediaSpec _mediaSpec);

};

#endif
