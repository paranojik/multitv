#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

#include <string>
#include "Codec.h"
#include "media_codecs.h"

#define MIN_AUDIO_BUF_TO_ENCODE_SIZE 1000
#define MIN_VIDEO_BUF_TO_ENCODE_SIZE 5000

class AudioEncoder : public Encoder, public IMediaAudio
{
protected:

    unsigned m_uFramesPkt,
             m_uFrameSize;

public:

    AudioEncoder(std::string _transmission, 
                 std::string _network, 
                 std::string _bitrate) : Encoder(_transmission, _network, _bitrate)
    {
        m_uFramesPkt = 0;
        m_uFrameSize = 0;
    }
    
    virtual ~AudioEncoder() {}

    virtual unsigned InitCommonParameters(CParseString & _parseString);
    virtual unsigned InitFormat(DsCScriptValue & _parameters);

    virtual unsigned Encode(unsigned char *_pBufIn, 
                            unsigned _bufInLen, 
                            unsigned char *_pBufOut, 
                            unsigned & _bufOutLen, 
                            void *_pHeaderData);

// IMediaAudio implementation

    virtual unsigned GetFramesPkt() {return m_uFramesPkt; }
    virtual unsigned GetSampleRate();
    virtual unsigned GetSampleSize();
    virtual unsigned GetFrameSamples();
    virtual unsigned GetChannels();

};

//----------------------------------------------//
// Speex encoder class declaration
//----------------------------------------------//

class SpeexEncoder : public AudioEncoder
{
public:

    SpeexEncoder(std::string & _transmission, 
                 std::string & _network, 
                 std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate){}

    virtual ~SpeexEncoder() {}

    virtual unsigned Init();
};

class SpeexLOEncoder : public SpeexEncoder
{

public:
    SpeexLOEncoder(std::string & _transmission, 
                   std::string & _network, 
                   std::string & _bitrate) : SpeexEncoder(_transmission, _network, _bitrate)
    {
        SetCodec(SPEEX_LO);
    }
};

class SpeexHIEncoder : public SpeexEncoder
{

public:
    SpeexHIEncoder(std::string & _transmission, 
                   std::string & _network, 
                   std::string & _bitrate) : SpeexEncoder(_transmission, _network, _bitrate)
    {
        SetCodec(SPEEX_HI);
    }

};

//----------------------------------------------//
// G711 encoder class declaration
//----------------------------------------------//

class G711Encoder : public AudioEncoder
{
public:

    G711Encoder(std::string & _transmission, 
                std::string & _network, 
                std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate){}

    virtual ~G711Encoder() {}

    virtual unsigned Init();
};

class G711ALawEncoder : public G711Encoder
{

public:
    G711ALawEncoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : G711Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G711A);
    }
};

class G711ULawEncoder : public G711Encoder
{

public:
    G711ULawEncoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : G711Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G711U);
    }
};

class G723Encoder : public AudioEncoder
{
public:

    G723Encoder(std::string & _transmission, 
                std::string & _network, 
                std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate){}

    virtual ~G723Encoder() {}

    virtual unsigned Init();
};

class G723_53_Encoder : public G723Encoder
{
public:
    G723_53_Encoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : G723Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G723_53);
    }
};

class G723_63_Encoder : public G723Encoder
{
public:
    G723_63_Encoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : G723Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G723_63);
    }

};

class G728Encoder : public AudioEncoder
{
public:

    G728Encoder(std::string & _transmission, 
                std::string & _network, 
                std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate){}

    virtual ~G728Encoder() {}

    virtual unsigned Init();
};

class G728_96_Encoder : public G728Encoder
{
public:
    G728_96_Encoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : G728Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G728_96);
    }
};

class G728_128_Encoder : public G728Encoder
{
public:
    G728_128_Encoder(std::string & _transmission, 
                     std::string & _network, 
                     std::string & _bitrate) : G728Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G728_128);
    }
};

class G728_160_Encoder : public G728Encoder
{
public:
    G728_160_Encoder(std::string & _transmission, 
                     std::string & _network, 
                     std::string & _bitrate) : G728Encoder(_transmission, _network, _bitrate)
    {
        SetCodec(G728_160);
    }
};


class G729Encoder : public AudioEncoder
{
public:

    G729Encoder(std::string & _transmission, 
                std::string & _network, 
                std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate)
    {
        SetCodec(G729A);
    }

    virtual ~G729Encoder() {}

    virtual unsigned Init();
};

class AACEncoder : public AudioEncoder
{
public:

    AACEncoder(std::string & _transmission, 
               std::string & _network, 
               std::string & _bitrate) : AudioEncoder(_transmission, _network, _bitrate){}

    virtual ~AACEncoder() {}

    virtual unsigned Init();
};

class AAC_48_Encoder : public AACEncoder
{
public:
    AAC_48_Encoder(std::string & _transmission, 
                   std::string & _network, 
                   std::string & _bitrate) : AACEncoder(_transmission, _network, _bitrate)
    {
        SetCodec(AAC_48);
    }
};

class AAC_128_Encoder : public AACEncoder
{
public:
    AAC_128_Encoder(std::string & _transmission, 
                    std::string & _network, 
                    std::string & _bitrate) : AACEncoder(_transmission, _network, _bitrate)
    {
        SetCodec(AAC_128);
    }
};

#endif
