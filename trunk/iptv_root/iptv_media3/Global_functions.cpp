#include "compat.h"
#include "global_error.h"
#include "mediapkt.h"
#include "media_utilities.h"
#include "DemuxedMediaType.h"

#ifdef _LINUX
    #include <time.h>
    #include <string.h>
    #include <sys/times.h> 
    #include <unistd.h>
#endif

ULONG GetFrameDimensions(BYTE _type, ULONG *_pulWidth, ULONG *_pulHeight)
{
    if (!_pulWidth || !_pulHeight)
        return RET_INVALID_ARG;

    switch(_type)
    {
        case MTYPE_MEDIA_VIDEO_H264_QCIF:
        case MTYPE_MEDIA_VIDEO_MPEG4_176x144:
            *_pulHeight = 144;
            *_pulWidth  = 176;
        break;

        case MTYPE_MEDIA_VIDEO_H264_QVGA:      
        case MTYPE_MEDIA_VIDEO_MPEG4_320x240:	     
            *_pulHeight = 240;
            *_pulWidth  = 320;
        break;

        case MTYPE_MEDIA_VIDEO_H264_D1:
            *_pulHeight = 576;
            *_pulWidth  = 720;
        break;

        default:
            return RET_INVALID_COMPRESSION_TYPE;

    }

    return RET_OK;
}

ULONG GetMediaSpec(BYTE _type, MediaSpec * _pMediaSpec)
{
    if (!_pMediaSpec)
        return RET_INVALID_ARG;

    switch(_type)
    {

        case MTYPE_MEDIA_AUDIO_SPEEX_3F:
        case MTYPE_MEDIA_AUDIO_G729A_6F:
        case MTYPE_MEDIA_AUDIO_G723_2F:
        case MTYPE_MEDIA_AUDIO_G728_4F:
        case MTYPE_MEDIA_AUDIO_G711_ALAW:
        case MTYPE_MEDIA_AUDIO_G711_ULAW:
        case MTYPE_MEDIA_AUDIO_AAC_48:
        case MTYPE_MEDIA_AUDIO_AAC_128:
        case ASF_DEMUXED_AUDIO:

            *_pMediaSpec = AUDIO;
    
        break;

        case MTYPE_MEDIA_VIDEO_MPEG4_320x240:
        case MTYPE_MEDIA_VIDEO_MPEG4_176x144:
        case MTYPE_MEDIA_VIDEO_H264_QCIF:
        case MTYPE_MEDIA_VIDEO_H264_QVGA:
        case ASF_DEMUXED_VIDEO:

            *_pMediaSpec = VIDEO;

        break;

        default:
            return RET_INVALID_COMPRESSION_TYPE;
    }

    return RET_OK;
}

ULONG GetTime(ULONG *_pulTime)
{
    if (!_pulTime)
        return RET_INVALID_ARG;


#ifdef WIN32
    *_pulTime = GetTickCount();
    //*_pulTime = timeGetTime();
#endif

#ifdef _LINUX
    tms tm;
    *_pulTime = (times(&tm) * 1000.0) / sysconf(_SC_CLK_TCK);
#endif

    return RET_OK;
}
