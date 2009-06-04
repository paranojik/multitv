#ifndef VIDEO_ENCODER_H
#define VIDEO_ENCODER_H

#include "Codec.h"

class CParseString;
class VideoEncoder : public Encoder, public IMediaVideo
{
protected:

    AVFrame *m_pYuvPicture, 
            *m_pRgbPicture;
 
    BYTE    *m_pYuvPicBuf, 
            *m_pRgbPicBuf;

    virtual unsigned InitCommonParameters(CParseString & _parseString);
    virtual unsigned InitFormat(DsCScriptValue & _parameters);
    unsigned InitVideoEncoder();

public:

    VideoEncoder(std::string _transmission, 
                 std::string _network, 
                 std::string _bitrate);
    
    virtual ~VideoEncoder() = 0;

    virtual unsigned Encode(unsigned char *_pBufIn, 
                            unsigned _bufInLen, 
                            unsigned char *_pBufOut, 
                            unsigned & _bufOutLen, 
                            void *_pHeaderData);

// IMediaVideo implementation

    virtual unsigned GetWidth();
    virtual unsigned GetHeight();
    virtual unsigned GetFrameSize();
    virtual double GetFramerate();
};

//----------------------------------------------//
// MPEG4 encoder class declaration
//----------------------------------------------//

class Mpeg4Encoder : public VideoEncoder
{
public:

    Mpeg4Encoder(std::string _transmission, 
                 std::string _network, 
                 std::string _bitrate) : VideoEncoder(_transmission, _network, _bitrate)
    {
        SetCodec("mpeg4");
    }

    virtual unsigned Init();
};

//----------------------------------------------//
// H264 encoder class declaration
//----------------------------------------------//

class H264Encoder : public VideoEncoder
{
public:

    H264Encoder(std::string _transmission, 
                std::string _network, 
                std::string _bitrate) : VideoEncoder(_transmission, _network, _bitrate)
    {
        SetCodec("h264");
    }

    virtual unsigned Init();
};




#endif
