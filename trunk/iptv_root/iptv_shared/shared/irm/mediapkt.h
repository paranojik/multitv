#ifndef _MEDIAPKT_STR
#define _MEDIAPKT_STR


// MEDIAPKT_FLAGS

#define PKT_KEYFRAME           0x01
#define PKT_MULTICAST          0x02
#define PKT_REALTIME           0x04
#define PKT_VIDEO              0x08
#define PKT_AUDIO              0x10
#define PKT_SEGMENTED          0x20
#define PKT_RESENT             0x40
#define PKT_AUDIO_SILENCE      0x40
#define PKT_FLAGS_EX           0x80


enum MEDIAPKT_TYPES
{
    MTYPE_MEDIA                         = 1,
    MTYPE_MEDIA_VOICE                   = 2,
    MTYPE_PING                          = 3,
    MTYPE_PONG                          = 4,
    MTYPE_CONNECT                       = 5,
    MTYPE_AUTH                          = 6,
    MTYPE_FILE                          = 7,
    MTYPE_MEDIA_CONFERENCE              = 8,
    MTYPE_MEDIA_VOICE_SPEEX_9NB         = 9,  // Speex quality 9 narrow band
    MTYPE_MEDIA_VIDEO_MPEG4_320x240     = 10,
    MTYPE_MEDIA_VOICE_SPEEX_4UWB        = 11, // Speex quality 4 ultra wide band
    MTYPE_MEDIA_VIDEO_MPEG4_176x144     = 12,
    MTYPE_MEDIA_VOICE_SPEEX_2UWB        = 13, // Speex quality 2 ultra wide band
    MTYPE_MEDIA_VOICE_SPEEX_2NB         = 14, // Speex quality 2 ultra wide band
    MTYPE_MEDIA_VIDEO_H264_QCIF         = 15, // H264 QCIF
    MTYPE_MEDIA_VIDEO_H264_QVGA         = 16, // H264 QVGA
    MTYPE_MEDIA_VIDEO_H264_D1           = 17, // H264 D1
    MTYPE_MEDIA_AUDIO_SPEEX_6F          = 18, // Speex 6 packets per frame
    MTYPE_MEDIA_AUDIO_SPEEX_3F          = 19, // Speex 3 packets per frame
    MTYPE_MEDIA_AUDIO_EX_SPEEX_6F       = 20, // Speex 6 packets per frame
    MTYPE_MEDIA_AUDIO_EX_SPEEX_3F       = 21, // Speex 3 packets per frame
    MTYPE_MEDIA_AUDIO_G729A_6F          = 22, // G729A 3 frames per packet
    MTYPE_MEDIA_APPSHARING              = 23, // Application sharing
    MTYPE_MEDIA_AUDIO_G723_2F           = 24, // G723.1 LPC 2 frames per packet
    MTYPE_MEDIA_AUDIO_G728_4F           = 25, // G728 CELP 4 frames per packet
    MTYPE_MEDIA_AUDIO_VORBIS            = 26, // VORBIS 1 frame per packet
    MTYPE_MEDIA_AUDIO_G711_ALAW         = 27, // G711 alaw 3 frames per packet
    MTYPE_MEDIA_AUDIO_G711_ULAW         = 28, // G711 ulaw 3 frame per packet
    MTYPE_MEDIA_AUDIO_AAC_48            = 29, // AAC 48kbps 1 frame per packet
    MTYPE_MEDIA_AUDIO_AAC_128           = 30, // AAC 128kbps 1 frame per packet
    MTYPE_MEDIA_LAST                    = 31,
};


/*
enum MEDIAPKTEX_VERSIONS
{
    MEDIAPKTEX_VER_1                    = 1,
};
*/

// No caso de UDPPING id representa a porta

#pragma pack(1)

struct MediaPkt
{
    ULONG  id,
           seq;
    USHORT datalen;
    BYTE   type,
           flags;

    MediaPkt  (void) { reset(); }
    void reset(void) { id=0; seq=0; datalen=0; type=0; flags=0; }
};


struct LMediaPkt
{
    ULONG id,
          seq;
    ULONG datalen;
    BYTE  type,
          flags;

    LMediaPkt (void) { reset(); }
    void reset(void) { id=0; seq=0; datalen=0; type=0; flags=0; }
};


//class CMCAVBasePacket

struct MediaPktExt : MediaPkt
{
    ULONG ulSubSeq;
    DWORD dwMsTimestamp;

    MediaPktExt(void) { reset(); }
    void reset (void) { ulSubSeq=0; dwMsTimestamp=0; MediaPkt::reset(); }
};


struct LMediaPktExt : LMediaPkt
{
    ULONG ulSubSeq;
    DWORD dwMsTimestamp;

    LMediaPktExt(void) { reset(); }
    void reset  (void) { ulSubSeq=0; dwMsTimestamp=0; LMediaPkt::reset(); }
};


struct MediaPktExtSeg : MediaPktExt
{
    BYTE  nSegments;

    MediaPktExtSeg(void) { reset(); }
    void reset    (void) { nSegments=0; MediaPktExt::reset(); }
};

struct LMediaPktExtSeg : MediaPktExt
{
    ULONG nSegments;

    LMediaPktExtSeg(void) { reset(); }
    void reset     (void) { nSegments=0; MediaPktExt::reset(); }
};

//class CMCAVVideoPacket : public CMCAVBasePacket

struct MediaPktExtVideo : MediaPktExtSeg
{
	BYTE  pEncodedBuf[1];
};

//class CMCAVAudioPacket : public CMCAVBasePacket

struct MediaPktExtAudio : MediaPktExt
{
	BYTE  pEncodedBuf[1];
};

/*
struct MediaPktEx1 : MediaPkt
{
    BYTE   xversion:4;
    USHORT xflags:12;

};
*/

#pragma pack()


#endif  // _MEDIAPKT_STR

