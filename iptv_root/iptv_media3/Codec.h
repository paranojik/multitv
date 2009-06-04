#ifndef MEDIA_CODEC_H
#define MEDIA_CODEC_H

#include "avcodec.h"
#include "CodecConfig.h"
#include "mediapkt.h"

class MediaCodec : public CodecConfig
{
private:

    static bool m_avcodecInited;
    std::string m_codec;

protected:

    bool m_Init;
    unsigned char m_MediaType;

    AVCodecContext *m_pCodecCtx;
    AVCodec        *m_pCodec;       

    void SetCodec(std::string _codec) {m_codec = _codec; }
    std::string & GetCodec() {return m_codec; }

    virtual AVCodec * InitContext(std::string _codec) = 0;

public:

    MediaCodec() 
    {
        m_Init      = false;

        m_MediaType = MTYPE_NONE;

        m_pCodecCtx = NULL;
        m_pCodec    = NULL;

        if (!m_avcodecInited)
        {
            avcodec_init();
            avcodec_register_all();

            m_avcodecInited = true;
        }
    }
   
    virtual ~MediaCodec()
    {
        if (m_pCodecCtx)
            avcodec_close(m_pCodecCtx);
    }

    bool CodecOk() 
    {
       return m_Init; 
    }

    unsigned char GetMediaType() {return m_MediaType; }

    virtual unsigned Init() = 0;
};

class CParseString;
class Encoder : public MediaCodec
{
private:
    std::string m_transmission,
                m_network,
                m_bitrate;

protected:

    virtual unsigned InitCommonParameters(CParseString & _parseString) = 0;
    virtual unsigned InitFormat(DsCScriptValue & _parameters) = 0;

public:

    Encoder(std::string _transmission, 
            std::string _network, 
            std::string _bitrate)
    {
        m_transmission = _transmission;
        m_network      = _network;
        m_bitrate      = _bitrate;
    }
    
    virtual ~Encoder() {}

    std::string & GetTransmission() {return m_transmission; }
    std::string & GetNetwork()      {return m_network;      }
    std::string & GetBitrate()      {return m_bitrate;      }

    virtual AVCodec * InitContext(std::string _codec);
    virtual unsigned SetEncoder(std::string _codecType, DsCScriptValue & _tableParam);

    virtual unsigned Encode(unsigned char *_pBufIn, 
                            unsigned _bufInLen, 
                            unsigned char *_pBufOut, 
                            unsigned & _bufOutLen, 
                            void *_pHeaderData) = 0;
};

class Decoder : public MediaCodec
{

public:
    
    Decoder(unsigned char _type)
    {
        m_MediaType = _type;
    }

    virtual ~Decoder() {}

    virtual AVCodec * InitContext(std::string _codec);
    virtual unsigned SetDecoder(DsCScriptValue & _tableParam);

    virtual unsigned Decode(unsigned char *_pBufIn, 
                            unsigned _bufInLen, 
                            unsigned char *_pBufOut, 
                            unsigned & _bufOutLen, 
                            void *_pHeaderData) = 0;
};

class IMediaCodecType
{
private:
    
    std::string m_codecType;

public:

    virtual ~IMediaCodecType() {}

    void SetCodecType(std::string _codecType) {m_codecType = _codecType; }
    std::string & GetCodecType() {return m_codecType; }
};

class IMediaVideo : public IMediaCodecType
{

public:

    virtual unsigned GetWidth()     = 0;
    virtual unsigned GetHeight()    = 0;
    virtual unsigned GetFrameSize() = 0;
    virtual double GetFramerate()   = 0;

    IMediaVideo()
    {
        SetCodecType("video");
    }

};

class IMediaAudio : public IMediaCodecType
{

public:

    IMediaAudio()
    {
        SetCodecType("audio");
    }

    virtual unsigned GetFramesPkt()    = 0;
    virtual unsigned GetSampleRate()   = 0;
    virtual unsigned GetSampleSize()   = 0;
    virtual unsigned GetFrameSamples() = 0;
    virtual unsigned GetChannels()     = 0;
};

#endif
