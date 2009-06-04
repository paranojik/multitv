/*
 * G.723.1 (MPC-MLQ - ACELP)/G.728 (LD-CELP)/G.729a (ACELP) Voice audio codec
 * Copyright (c) 2007 Dario Andrade.
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <ippdefs.h>
#include <limits.h>
#include "avcodec.h"
#include "common.h"
#include "bitstream.h"
#include "intel_ipp/speech-codecs/codec/speech/ippusc.h"
#include "intel_ipp/speech-codecs/codec/speech/g729i/include/g729api.h"
#include "intel_ipp/speech-codecs/codec/speech/g711/include/g711api.h"

/**
 * @author Dario Andrade
 * Using Intel USC codec samples (part of Intel IPP)
 * Intel® Integrated Performance Primitives 5.1
 * Check http://developer.intel.com/software/products for more info
 */

/* Interfacing to the libavcodec */

//#define DUMP_DATA

#ifdef DUMP_DATA

#define le_int(_) ((int)(_))
#define le_short(_) ((short)(_))
#define be_int(x) (unsigned int)(((x) << 24) | (((x)&0xff00) << 8) | (((x) >>8)&0xff00) | ((x&0xff000000) >> 24))
#define be_short(x) (unsigned short)(((unsigned short)(x) << 8) | ((unsigned short)(x) >>8 ))

static void write_wav_header(FILE *file, int rate, int channels, int reg, int bitrate, int size)
{
    char ch[5];
	int itmp;
    short stmp;

	ch[4]=0;

	fwrite ("RIFF", 4, 1, file);

	if (!size) size = 0x7fffffff;
	//itmp = 0x7fffffff;
	itmp = le_int(size);
	fwrite(&itmp, 4, 1, file);

	fwrite ("WAVEfmt ", 8, 1, file);

	itmp = le_int(16);
	fwrite(&itmp, 4, 1, file);

	stmp = le_short(reg);
	fwrite(&stmp, 2, 1, file);

	stmp = le_short(channels);
	fwrite(&stmp, 2, 1, file);

	itmp = le_int(rate);
	fwrite(&itmp, 4, 1, file);

	// if pcm
	if (reg == 1)
		itmp = le_int(rate*channels*2);
	else
		itmp = le_int((bitrate + 7 ) / 8);
	fwrite(&itmp, 4, 1, file);

	stmp = le_short(2*channels);
	fwrite(&stmp, 2, 1, file);

	stmp = le_short(16);
	fwrite(&stmp, 2, 1, file);

	itmp = 0;
	fwrite(&stmp, 4, 1, file);

	fwrite ("data", 4, 1, file);

	//itmp = le_int(0x7fffffff);
	itmp = le_int(size - 50);
	fwrite(&itmp, 4, 1, file);
}

/*
* 0               1               2               3               4       	        5
* 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7
* -----------------------------------------------------------------------------------------------
* |      Bitrate                                  |Frame type     |  Frame length               |
* -----------------------------------------------------------------------------------------------
*/

#define G7XX_HEADER_SIZE 6

typedef struct G7XX_HEADER {
	int bitrate:24;
	int frametype:8;
	int framelen:16;
} G729_HEADER;

#else // #ifdef DUMP_DATA
typedef struct G7XX_HEADER {} G7XX_HEADER;
#define G7XX_HEADER_SIZE 0
#endif // #ifdef DUMP_DATA

#define USC_MAX_NAME_LEN 64

typedef struct AVUSCContext {
	char codecName[USC_MAX_NAME_LEN];
	USC_Fxns *codec;
	USC_Handle handle;
	int nbanks;
	USC_MemBank* membanks;
	USC_CodecInfo info;
#ifdef DUMP_DATA
	FILE* fcod;
	FILE* fpcm;
#endif // DUMP_DATA
} AVUSCContext;

static int usc_init(AVCodecContext * avctx)
{
    AVUSCContext* c = (AVUSCContext*)avctx->priv_data;
    USC_Status s;
    int iInfo, i;

	switch (avctx->codec->id)
	{
	case CODEC_ID_PCM_ALAW:
	c->codec = GetUSC_G711A_Fxns();
	break;
	case CODEC_ID_PCM_MULAW:
	c->codec = GetUSC_G711U_Fxns();
	break;
	case CODEC_ID_LDCELP_G728:
	c->codec = GetUSC_G728_Fxns();
	break;
    case CODEC_ID_G723:
	c->codec = GetUSC_G723_Fxns();
	break;
    case CODEC_ID_ACELP_G729A:
	c->codec = GetUSC_G729A_Fxns();
	break;
	}

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: getting info size\n");
	c->codec->std.GetInfoSize(&iInfo);

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: getting info\n");
	s = c->codec->std.GetInfo(NULL, &c->info);
	if (s != USC_NoError)
    	return -1;

    if (avctx->channels != 1) {
        av_log(avctx, AV_LOG_ERROR, "Intel USC: unsupported audio bitrate\n");
        return -1;
    }

    if (avctx->sample_rate != c->info.pPcmTypesTbl->sample_frequency
			|| avctx->sample_fmt != SAMPLE_FMT_S16
			|| avctx->block_align != 2
			|| avctx->channels != 1) {
        av_log(avctx, AV_LOG_ERROR, "Intel USC: unsupported audio sample format (must be 16 bits with correct samplerate, block_align and sample_fmt must be set\n");
        return -1;
    }
    avctx->coded_frame = avcodec_alloc_frame();
    if (!avctx->coded_frame)
        return AVERROR(ENOMEM);
    avctx->coded_frame->key_frame = 1;

	if (avctx->codec->encode)
	{
		c->info.params.direction = 0;
		c->info.params.nModes = 0;
		c->info.params.modes.vad = 0;
		/*
		c->info.params.modes.pf = 1;
		c->info.params.modes.hpf = 1;
		*/
#ifdef DUMP_DATA
		c->fpcm=fopen("avcodec.encode.g7xx.pcm.wav","wb");
		write_wav_header(c->fpcm, avctx->sample_rate, 1, 1, 0, 100000);
		c->fcod=fopen("avcodec.encode.g7xx.bin.wav","wb");
		write_wav_header(c->fcod, avctx->sample_rate, 1, WAVE_FORMAT_IPP_G729A, avctx->bit_rate, 15000);
#endif // DUMP_DATA
	}
	else
	{
		c->info.params.direction = 1;

#ifdef DUMP_DATA
		c->fpcm=fopen("avcodec.decode.g7xx.pcm.wav","wb");
		write_wav_header(c->fpcm, avctx->sample_rate, 1, 1, 0, 100000);
		c->fcod=fopen("avcodec.decode.g7xx.bin.wav","wb");
		write_wav_header(c->fcod, avctx->sample_rate, 1, WAVE_FORMAT_IPP_G729A, avctx->bit_rate, 15000);
#endif // DUMP_DATA
	}

	c->info.params.modes.bitrate = avctx->bit_rate;

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: discovering how much memory to allocate, numalloc\n");
	s = c->codec->std.NumAlloc(&c->info.params, &c->nbanks);

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: allocating memory %d bytes\n", sizeof(USC_MemBank) * c->nbanks);
	c->membanks = (USC_MemBank*) av_malloc(sizeof(USC_MemBank) * c->nbanks);
	if (!c->membanks)
		return -1;

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: calling memalloc\n");
	s = c->codec->std.MemAlloc(&c->info.params, c->membanks);
	if (s != USC_NoError)
		return -1;

	for (i = 0; i < c->nbanks; i++)
	{
	    av_log(avctx, AV_LOG_DEBUG, "Intel USC: allocating each bank %d bytes\n", c->membanks[i].nbytes);
		c->membanks[i].pMem = (char*) av_malloc(c->membanks[i].nbytes);
	}

	for (i = 0; i < c->nbanks; i++)
	{
		if (!c->membanks[i].pMem)
			return -1;
	}

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: initializing codec\n");
	s = c->codec->std.Init(&c->info.params, c->membanks, &c->handle);

	if (s != USC_NoError)
    	return -1;

	// usc framesize is in bytes (ugh)
	avctx->frame_size = c->info.params.framesize / avctx->block_align;
	
    av_log(avctx, AV_LOG_INFO, "%s: encoder is set to %d hz audio (%d channels) at %d bps (framesize: %d samples)\n",
        avctx->codec->name,
        avctx->sample_rate,
        avctx->channels,
        avctx->bit_rate,
        avctx->frame_size);
            
    return 0;
}

static int usc_close(AVCodecContext *avctx)
{
	int i;
	AVUSCContext* c = (AVUSCContext*)avctx->priv_data;

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: closing codec\n");

#ifdef DUMP_DATA
	fclose(c->fpcm);
	fclose(c->fcod);
#endif // DUMP_DATA

    av_log(avctx, AV_LOG_DEBUG, "Intel USC: freeing frame %p\n", avctx->coded_frame);
    av_freep(&avctx->coded_frame);

	if (c->membanks)
	{
		for (i = 0; i < c->nbanks; i++)
		{
			if (c->membanks[i].pMem)
			{
		    	av_log(avctx, AV_LOG_DEBUG, "Intel USC: freeing membanks %p\n", c->membanks[i].pMem);
				av_freep(&c->membanks[i].pMem);
			}
		}

    	av_log(avctx, AV_LOG_DEBUG, "Intel USC: freeing membanks %p\n", c->membanks);
	    av_freep(&c->membanks);
	}

	c->nbanks = 0;

    return 0;
}

#ifdef CONFIG_ENCODERS
static int usc_encode_frame(AVCodecContext *avctx,
                            uint8_t *dst, int buf_size, void *data)
{
    USC_Status s;
    AVUSCContext* c = (AVUSCContext*)avctx->priv_data;
    USC_PCMStream pcmin;
    USC_Bitstream streamout;
    G7XX_HEADER* header = (G7XX_HEADER*) dst;

    pcmin.pBuffer = data;
    pcmin.pcmType.sample_frequency = avctx->sample_rate;
	pcmin.pcmType.bitPerSample = avctx->block_align * 8;
    pcmin.nbytes = avctx->frame_size * avctx->block_align;
    pcmin.bitrate = avctx->bit_rate;
    
    streamout.pBuffer = dst + G7XX_HEADER_SIZE;
    
	s = c->codec->Encode(c->handle, &pcmin, &streamout);

	switch (s)
	{
		case USC_NoError:
#ifdef DUMP_DATA
		    header->bitrate = be_int(streamout.bitrate);
		    header->frametype = be_short(streamout.frametype);
		    header->framelen = be_short(streamout.nbytes);

			fwrite(data, pcmin.nbytes, 1, c->fpcm);
			fwrite(dst, streamout.nbytes + G7XX_HEADER_SIZE, 1, c->fcod);
#endif // DUMP_DATA

		    return streamout.nbytes + G7XX_HEADER_SIZE;
	    case USC_StateNotChanged:
			return 0;
		default:
			return s;
	}
}
#endif

static int usc_decode_frame(AVCodecContext *avctx,
                             void *data, int *data_size,
                             uint8_t *buf, int buf_size)
{
    AVUSCContext *c = avctx->priv_data;
    USC_Status s;
    USC_Bitstream streamin;
    USC_PCMStream pcmout;

	if (buf)
	{
#ifdef DUMP_DATA
	    G7XX_HEADER* header = (G7XX_HEADER*) buf;
		streamin.frametype = header->frametype;
		streamin.bitrate = header->bitrate;
#else // DUMP_DATA
		switch (avctx->codec->id)
		{
		    case CODEC_ID_ACELP_G729A:
		   		streamin.frametype = buf ? G729_Voice_Frame : G729_Untransmitted_Frame;
				break;
			case CODEC_ID_PCM_MULAW:
			case CODEC_ID_PCM_ALAW:
				streamin.frametype = buf ? G711_Voice_Frame : G711_Untransmitted_Frame;
				break;
			case CODEC_ID_LDCELP_G728:
		    case CODEC_ID_G723:
	    	default:
	   			streamin.frametype = 0;
				break;
		}
		streamin.bitrate = avctx->bit_rate;
#endif // DUMP_DATA

		streamin.pBuffer = buf + G7XX_HEADER_SIZE;
	}

	pcmout.pBuffer = data;

    s = c->codec->Decode(c->handle, buf ? &streamin : NULL, &pcmout);

	if (s != USC_NoError)
	{
		*data_size = 0;
		return -1;
	}

	*data_size = pcmout.nbytes;

#ifdef DUMP_DATA
	fwrite(data, pcmout.nbytes, 1, c->fpcm);
	fwrite(buf, streamin.nbytes + G7XX_HEADER_SIZE, 1, c->fcod);
#endif // DUMP_DATA

	return buf ? (streamin.nbytes + G7XX_HEADER_SIZE) : 0;
}

#ifdef CONFIG_ENCODERS
AVCodec g728_encoder = {
    "g728",
    CODEC_TYPE_AUDIO,
    CODEC_ID_LDCELP_G728,
    sizeof(AVUSCContext),
    usc_init,
    usc_encode_frame,
    usc_close,
    NULL,
};
#endif //CONFIG_ENCODERS

AVCodec g728_decoder = {
    "g728",
    CODEC_TYPE_AUDIO,
    CODEC_ID_LDCELP_G728,
    sizeof(AVUSCContext),
    usc_init,
    NULL,
    usc_close,
    usc_decode_frame,
};

#ifdef CONFIG_ENCODERS
AVCodec g729a_encoder = {
    "g729a",
    CODEC_TYPE_AUDIO,
    CODEC_ID_ACELP_G729A,
    sizeof(AVUSCContext),
    usc_init,
    usc_encode_frame,
    usc_close,
    NULL,
};
#endif //CONFIG_ENCODERS

AVCodec g729a_decoder = {
    "g729a",
    CODEC_TYPE_AUDIO,
    CODEC_ID_ACELP_G729A,
    sizeof(AVUSCContext),
    usc_init,
    NULL,
    usc_close,
    usc_decode_frame,
};

#ifdef CONFIG_ENCODERS
AVCodec g7231_encoder = {
    "g723",
    CODEC_TYPE_AUDIO,
    CODEC_ID_G723,
    sizeof(AVUSCContext),
    usc_init,
    usc_encode_frame,
    usc_close,
    NULL,
};
#endif //CONFIG_ENCODERS

AVCodec g7231_decoder = {
    "g723",
    CODEC_TYPE_AUDIO,
    CODEC_ID_G723,
    sizeof(AVUSCContext),
    usc_init,
    NULL,
    usc_close,
    usc_decode_frame,
};

#ifdef CONFIG_ENCODERS
AVCodec g711alaw_encoder = {
    "g711alaw",
    CODEC_TYPE_AUDIO,
    CODEC_ID_PCM_ALAW,
    sizeof(AVUSCContext),
    usc_init,
    usc_encode_frame,
    usc_close,
    NULL,
};
#endif //CONFIG_ENCODERS

AVCodec g711alaw_decoder = {
    "g711alaw",
    CODEC_TYPE_AUDIO,
    CODEC_ID_PCM_ALAW,
    sizeof(AVUSCContext),
    usc_init,
    NULL,
    usc_close,
    usc_decode_frame,
};

#ifdef CONFIG_ENCODERS
AVCodec g711ulaw_encoder = {
    "g711ulaw",
    CODEC_TYPE_AUDIO,
    CODEC_ID_PCM_MULAW,
    sizeof(AVUSCContext),
    usc_init,
    usc_encode_frame,
    usc_close,
    NULL,
};
#endif //CONFIG_ENCODERS

AVCodec g711ulaw_decoder = {
    "g711ulaw",
    CODEC_TYPE_AUDIO,
    CODEC_ID_PCM_MULAW,
    sizeof(AVUSCContext),
    usc_init,
    NULL,
    usc_close,
    usc_decode_frame,
};
