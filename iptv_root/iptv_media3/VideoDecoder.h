#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include "Codec.h"
#include "DemuxedMediaType.h"
#include "global_error.h"

class VideoDecoder : public Decoder, public IMediaVideo
{
protected:

    bool     m_keyFrameFound;

    unsigned m_uDecodedFrameLen,
             m_uOutputFrameLen;

    PixelFormat m_outputPixFmt;

    unsigned char * m_pDecodedFrameBuf;
    
    AVFrame * m_pDecodedFrame,
            * m_pOutputFrame;

    unsigned InitBuffers();

    virtual unsigned ConvertToOutputFmt(AVPicture *_pDecodedFrame, unsigned char *_pDstBuf, unsigned  &_uDstBufLen);

public:

    VideoDecoder(unsigned char _type) : Decoder(_type)
    {
        m_outputPixFmt     = PIX_FMT_RGB24;

        m_uDecodedFrameLen = 0;
        m_uOutputFrameLen  = 0;

        m_pDecodedFrameBuf = NULL;
        m_pDecodedFrame    = NULL;
        m_pOutputFrame     = NULL;

        m_keyFrameFound    = false;
    }

    virtual ~VideoDecoder()
    {
        if (m_pDecodedFrameBuf)
            delete m_pDecodedFrameBuf;
        
        if (m_pDecodedFrame)
            av_free(m_pDecodedFrame);

        if (m_pOutputFrame)
            av_free(m_pOutputFrame);
    }

    virtual unsigned Init();

    virtual unsigned Decode(unsigned char *_pBufIn, 
                            unsigned _bufInLen, 
                            unsigned char *_pBufOut, 
                            unsigned & _bufOutLen, 
                            void *_pHeaderData);

// IMediaVideo implementation

    virtual unsigned GetWidth();
    virtual unsigned GetHeight();
    virtual unsigned GetFrameSize();
    virtual double   GetFramerate();
};

class ASFVideoDecoder : public VideoDecoder
{

public:

    ASFVideoDecoder(AVCodecContext *_pCodecCtx) : VideoDecoder(ASF_DEMUXED_VIDEO) 
    {
        m_pCodecCtx = _pCodecCtx;
    }

    ~ASFVideoDecoder() {}

    virtual unsigned Init();
//    virtual unsigned Decode();

};

#endif
