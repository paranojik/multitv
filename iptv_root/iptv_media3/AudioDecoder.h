#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

#include "Codec.h"
#include "DemuxedMediaType.h"
#include "global_error.h"

class AudioDecoder : public Decoder, public IMediaAudio
{
protected:

    unsigned m_uFramesPkt,
             m_uFrameDuration;

public:

    AudioDecoder(unsigned char _type) : Decoder(_type)
    {
        m_uFramesPkt     = 0;
        m_uFrameDuration = 0;
    }

    virtual ~AudioDecoder() {}

    unsigned Decode(unsigned char *_pBufIn, 
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
// Speex decoder class declaration
//----------------------------------------------//

class SpeexDecoder : public AudioDecoder
{
public:

    SpeexDecoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~SpeexDecoder() {}

    virtual unsigned Init();
};

//----------------------------------------------//
// G711 decoder class declaration
//----------------------------------------------//

class G711Decoder : public AudioDecoder
{
public:

    G711Decoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~G711Decoder() {}

    virtual unsigned Init();
};

//----------------------------------------------//
// G723 decoder class declaration
//----------------------------------------------//

class G723Decoder : public AudioDecoder
{
public:

    G723Decoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~G723Decoder() {}

    virtual unsigned Init();
};

//----------------------------------------------//
// G728 decoder class declaration
//----------------------------------------------//

class G728Decoder : public AudioDecoder
{
public:

    G728Decoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~G728Decoder() {}

    virtual unsigned Init();
};

//----------------------------------------------//
// G729 decoder class declaration
//----------------------------------------------//

class G729Decoder : public AudioDecoder
{
public:

    G729Decoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~G729Decoder() {}

    virtual unsigned Init();
};

//----------------------------------------------//
// AAC decoder class declaration
//----------------------------------------------//

class AACDecoder : public AudioDecoder
{
public:

    AACDecoder(unsigned char _type) : AudioDecoder(_type){}

    virtual ~AACDecoder() {}

    virtual unsigned Init();
};


//----------------------------------------------//
// ASF decoder class declaration
//----------------------------------------------//

class ASFAudioDecoder : public AudioDecoder
{

public:

    ASFAudioDecoder(AVCodecContext *_pCodecCtx) : AudioDecoder(ASF_DEMUXED_AUDIO) 
    {
        m_pCodecCtx = _pCodecCtx;
    }
 
    ~ASFAudioDecoder() {}

    virtual unsigned Init()
    {
        unsigned ret = RET_ERROR;

        if (m_pCodecCtx) 
        {    
            if (m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id))
            {
                if (avcodec_open(m_pCodecCtx, m_pCodec) == 0)
                {
                    m_Init       = true;

                    m_uFramesPkt = 1;
                    ret = RET_OK;
                }
            }
        }

        return ret;
    }
};

#endif
