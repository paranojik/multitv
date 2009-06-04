/*
 * Speex voice codec support via libspeex (Dario Andrade <dario@sinistro.net>
 * http://www.speex.org
 * Copyright (c) 2003 The ffmpeg Project.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file: speex.c
 * Speex voice codec support via libspeex
 * http://www.speex.org
 * @author Dario Andrade <dario@ip.tv>
 */
 
/*#define SPEEX_PREPROCESS_SET_AGC_FIXED 18
#define SPEEX_PREPROCESS_GET_AGC_FIXED 19*/


#include "avcodec.h"
#include <speex/speex.h>
#include <speex/speex_stereo.h>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>

#define SPEEX_COMPLEXITY_DEFAULT    		3

typedef struct SpeexContext {
    int complexity, lookahead;
    void *st;
    SpeexBits bits;
    SpeexStereoState stereo;
} SpeexContext;

typedef struct SpeexFilterContext {
    SpeexPreprocessState* preproc;
} SpeexFilterContext;

typedef struct SpeexAecContext {
    SpeexEchoState* aec;
#ifdef DUMP_PCM    
    FILE*f;
#endif // DUMP_PCM
} SpeexAecContext;

static int speex_encode_init(AVCodecContext *avctx)
{
    SpeexContext *s = avctx->priv_data;
    SpeexMode* mode = NULL;
    int opt = 1;
    int vbr_enabled;

    /* default speex complexity (see speexenc.c) */
    s->complexity = SPEEX_COMPLEXITY_DEFAULT;
    
    switch (avctx->codec->id) 
    {
    case CODEC_ID_SPEEX:
        if (avctx->sample_rate > 48000)
        {
            av_log(avctx, AV_LOG_ERROR, "Error: sampling rate too high: %d Hz, try down-sampling\n", avctx->sample_rate);
            return -1;
        } 
        else if (avctx->sample_rate > 25000)
        {
            mode = (SpeexMode*) &speex_uwb_mode;
        } 
        else if (avctx->sample_rate > 12500)
        {
            mode = (SpeexMode*) &speex_wb_mode;
        } 
        else if (avctx->sample_rate >= 6000)
        {
            mode = (SpeexMode*) &speex_nb_mode;
        } 
        else 
        {
            av_log(avctx, AV_LOG_ERROR, "Error: sampling rate too low: %d Hz\n", avctx->sample_rate);
            return -1;
        }
        
        if (avctx->sample_rate != 8000 && avctx->sample_rate != 16000 && avctx->sample_rate != 32000)
             av_log(avctx, AV_LOG_INFO, "Warning: speex is only optimized for 8, 16 and 32 kHz. It will still work at %d Hz but your mileage may vary\n", avctx->sample_rate); 
        
        if (avctx->channels < 1 || avctx->channels > 2) 
        {
            av_log(avctx, AV_LOG_ERROR, "Error: invalid number of channels: %d, only mono or stereo is supported\n", avctx->channels);
            return -1;
        }
        
        /* we support encoding more than a frame in a single call, but is has to be multiple of speex native frame_size */
        speex_mode_query(mode, SPEEX_MODE_FRAME_SIZE, &avctx->frame_size);

        /*Initialize Speex encoder*/
        speex_bits_init(&s->bits);
        s->st = speex_encoder_init(mode);
        
        speex_encoder_ctl(s->st, SPEEX_SET_COMPLEXITY, &s->complexity);
        speex_encoder_ctl(s->st, SPEEX_SET_SAMPLING_RATE, &avctx->sample_rate);
        speex_encoder_ctl(s->st, SPEEX_GET_LOOKAHEAD, &s->lookahead);
                        
        vbr_enabled = (avctx->flags & CODEC_FLAG_QSCALE) ? 1 : 0;
        
        if (avctx->global_quality) 
        {
            if (vbr_enabled)
            {
                float fquality = avctx->global_quality;
                
                speex_encoder_ctl(s->st, SPEEX_SET_VBR_QUALITY, &fquality);
            }
            else
            {
                speex_encoder_ctl(s->st, SPEEX_SET_QUALITY, &avctx->global_quality);
            }
        }
        
        if (avctx->bit_rate && vbr_enabled)
        {
            if (avctx->global_quality)
                av_log(avctx, AV_LOG_INFO, "Warning: vbr: bitrate option is overriding global_quality\n");
            
            speex_encoder_ctl(s->st, SPEEX_SET_BITRATE, &avctx->bit_rate);
        }
        
        if (!avctx->global_quality)
        {
            if (vbr_enabled)
            {
                float fquality;
                
                speex_encoder_ctl(s->st, SPEEX_GET_VBR_QUALITY, &fquality);
                
                avctx->global_quality = (int) fquality;
            }
        }
        
        if (vbr_enabled)
        {
            speex_encoder_ctl(s->st, SPEEX_SET_VBR, &opt);
            
            if (avctx->rc_max_rate)
            {
                speex_encoder_ctl(s->st, SPEEX_SET_ABR, &avctx->rc_max_rate);
            }
        } 
        else if (avctx->flags2 & CODEC_FLAG2_SPEEX_CNG)
        {
            speex_encoder_ctl(s->st, SPEEX_SET_VAD, &opt);
        }
        
        if (avctx->flags2 & CODEC_FLAG2_SPEEX_DTX)
        {
            if ((avctx->flags2 & CODEC_FLAG2_SPEEX_CNG) || vbr_enabled)
            {
                speex_encoder_ctl(s->st, SPEEX_SET_DTX, &opt);
            }
            else
            {
                av_log(avctx, AV_LOG_INFO, "Warning: dtx is useless without cng or vbr\n");
            }
        } 
        else if (vbr_enabled && (avctx->flags2 & CODEC_FLAG2_SPEEX_CNG))
        {
            av_log(avctx, AV_LOG_INFO, "Warning: cng is already implied by vbr\n");
        }

        av_log(avctx, AV_LOG_INFO, "Speex: encoder is set to %d hz audio (%d channels) at %d bps (vbr: %d, abr: %d) using (%s) mode (quality %d, framesize: %d samples, lookahead: %d, cng: %d, dtx: %d)\n", 
            avctx->sample_rate,
            avctx->channels,
            avctx->bit_rate,
            vbr_enabled,
            avctx->rc_max_rate,
            mode->modeName,
            avctx->global_quality,
            avctx->frame_size,
            s->lookahead,
            avctx->flags2 & CODEC_FLAG2_SPEEX_CNG ? 1 : 0,
            avctx->flags2 & CODEC_FLAG2_SPEEX_DTX ? 1 : 0);
        
        break;

    default:
        return -1;
    } // end switch codec type

    return 0;
}

static int speex_encode_frame(AVCodecContext *avctx,
                      unsigned char *frame, int buf_size, void *data)
{
    SpeexContext *s = avctx->priv_data;
    int bytes_written = 0;
    
    if (avctx->channels == 2)
        speex_encode_stereo_int(data, avctx->frame_size, &s->bits);
        
    speex_encode_int(s->st, data, &s->bits);

    //speex_bits_insert_terminator(&s->bits);
        
    if (!(avctx->flags2 & CODEC_FLAG2_NO_OUTPUT))
    {
    	bytes_written = speex_bits_write(&s->bits, frame, buf_size);
    }

	speex_bits_reset(&s->bits);

    if (avctx->debug & FF_DEBUG_BITSTREAM)
    {    
        av_log(avctx, AV_LOG_DEBUG, "Speex: encoded speex frame (%d bytes total, framesize: %d)\n", 
            bytes_written, avctx->frame_size);
    }
    
    return bytes_written;
}

static int speex_encode_close(AVCodecContext *avctx)
{
    SpeexContext *s = avctx->priv_data;

    speex_encoder_destroy(s->st);
    speex_bits_destroy(&s->bits);

    return 0;
}

static int speex_decode_init(AVCodecContext *avctx)
{
    SpeexContext *s = avctx->priv_data;
    SpeexMode* mode = NULL;
    SpeexStereoState stereo = SPEEX_STEREO_STATE_INIT;
    int opt = 1;

    switch (avctx->codec->id) {

    case CODEC_ID_SPEEX:        
        if (avctx->sample_rate > 48000)
        {
            av_log(avctx, AV_LOG_ERROR, "Error: sampling rate too high: %d Hz, speex does not support it\n", avctx->sample_rate);
            return -1;
        } 
        else if (avctx->sample_rate > 25000)
        {
            mode = (SpeexMode*) &speex_uwb_mode;
        } 
        else if (avctx->sample_rate > 12500)
        {
            mode = (SpeexMode*) &speex_wb_mode;
        } 
        else if (avctx->sample_rate >= 6000)
        {
            mode = (SpeexMode*) &speex_nb_mode;
        } 
        else 
        {
            av_log(avctx, AV_LOG_ERROR, "Error: sampling rate too low: %d Hz\n", avctx->sample_rate);
            return -1;
        }

        if (avctx->channels < 1 || avctx->channels > 2) 
        {
            av_log(avctx, AV_LOG_ERROR, "Error: invalid number of channels: %d, only mono or stereo is supported\n", avctx->channels);
            return -1;
        }
        
        /*Initialize Speex encoder*/
        speex_bits_init(&s->bits);
        s->st = speex_decoder_init(mode);

        memcpy(&s->stereo, &stereo, sizeof(stereo));

        speex_decoder_ctl(s->st, SPEEX_GET_FRAME_SIZE, &avctx->frame_size);
        speex_decoder_ctl(s->st, SPEEX_SET_SAMPLING_RATE, &avctx->sample_rate);
        
        /* we don't know that yet */
        avctx->bit_rate = 0;
        
        if (avctx->flags2 & CODEC_FLAG2_SPEEX_ENH)
        {
            speex_decoder_ctl(s->st, SPEEX_SET_ENH, &opt);
        }

        av_log(avctx, AV_LOG_INFO, "Speex: decoder is set to %d hz audio (%d channels) using (%s) mode (framesize: %d samples)\n", 
            avctx->sample_rate,
            avctx->channels,
            mode->modeName,
            avctx->frame_size);
            
        break;
                
        default:
            return -1;
    } // end switch codec type

    return 0;
}

static int speex_decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        uint8_t* buf, int buf_size)
{
    SpeexContext *s = avctx->priv_data;
    int ret, bits_remaining, used = 0, bits_remaining_before;

	av_log(avctx,AV_LOG_DEBUG,"speexdec: out: %p %p(%d), in: %p, %d\n",
		data, data_size, data_size?*data_size:0,buf,buf_size);
	if (data)
	{    
	    /* we'll save this info to log later */
	    bits_remaining_before = bits_remaining = speex_bits_remaining(&s->bits);
	    
	    *data_size = 0;
	
	    /* We are not interpolating a frame */
	    if (buf)
	    {    
	        if (bits_remaining > 0)
	        {
	            speex_bits_read_whole_bytes(&s->bits, (char*) buf, buf_size);
	        }
	        else
	        {
	            /*Copy buf to Speex bitstream*/
	            speex_bits_read_from(&s->bits, (char*) buf, buf_size);
	        }
	    } // end if we are not interpolating a frame
	        
	    ret = speex_decode_int(s->st, buf ? &s->bits : NULL, data);
	
	    switch (ret)
	    {
	    case -2:
	        av_log(avctx, AV_LOG_ERROR, "Error: corrupted speex stream\n");
	        speex_bits_reset(&s->bits);
	        return -1;
	
	    case -1:
	        /* no frame has been decoded, return bytes used */
	        //av_log(avctx, AV_LOG_INFO, "Warning: no samples have been decoded (end of stream)\n");
	        speex_bits_rewind(&s->bits);
	        return buf_size;
	
	    default:    
	
	        if (buf)
	        {
	            /* Calculate how many bytes we've used */
	            bits_remaining = speex_bits_remaining(&s->bits);
	        
	            if (bits_remaining < 0)
	            {
	                //av_log(avctx, AV_LOG_DEBUG, "Warning: decoding overflow, need more data (%d used)\n", buf_size);
	                
	                speex_bits_rewind(&s->bits);
	                
	                return buf_size;
	            }
	                
	            used = buf_size - (bits_remaining / 8);
	        }
	        else
	        {
	            av_log(avctx, AV_LOG_DEBUG, "Info: interpolating a lost frame\n");
	        }            
	        
	        *data_size = avctx->frame_size * avctx->channels * sizeof(short);
	            
	        if (avctx->channels == 2)
	            speex_decode_stereo_int(data, avctx->frame_size, &s->stereo);
	        
	        if (avctx->debug & FF_DEBUG_BITSTREAM)
	        {
	            int bitrate;
	            speex_decoder_ctl(s->st, SPEEX_GET_BITRATE, &bitrate);
	            
	            av_log(avctx, AV_LOG_DEBUG, "Speex: decoder used: %d/%d bytes, output: %d bytes, bitrate: %d bps (remaining in stream: %.1f bytes after %d bits)\n", 
	                    used, buf_size, *data_size, bitrate, (float) bits_remaining / 8, bits_remaining_before);
	        }
	        
	        speex_bits_reset(&s->bits);
	        
	        break;
	    }
    }
    else
    {
		speex_bits_reset(&s->bits);    	
    }
               
    return used;
}

static int speex_decode_close(AVCodecContext *avctx)
{
    SpeexContext *s = avctx->priv_data;

    speex_decoder_destroy(s->st);
    speex_bits_destroy(&s->bits);

    return 0;
}

int speex_filter_init(AVFilterContext *avf)
{
    SpeexFilterContext *s = avf->priv_data;
	int opt = 1;

	if (avf->avctx->channels == 2) {
		av_log(avf, AV_LOG_ERROR, "Speex error: preprocessor does not support stereo encoding\n");
		return -1;
	}
	s->preproc = speex_preprocess_state_init(avf->avctx->frame_size, avf->avctx->sample_rate);
	
    if (avf->flags & FILTER_FLAG_SPEEX_VAD) {
        speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_VAD, &opt);
    }
    
    if (avf->flags & FILTER_FLAG_SPEEX_AGC) {
        float flevel = avf->agc_level;
    	speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_AGC, &opt);
		speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_AGC_LEVEL, &flevel);
	}

    opt = (avf->flags & FILTER_FLAG_SPEEX_DENOISER) ? 1 : 0;

    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_DENOISE, &opt);

	av_log(avf, AV_LOG_INFO, "Speex preproc: using preprocessor (vad: %d, agc level: %d, denoiser: %d)\n",
		(avf->flags & FILTER_FLAG_SPEEX_VAD) ? 1 : 0,
		avf->agc_level,
		(avf->flags & FILTER_FLAG_SPEEX_DENOISER) ? 1 : 0);
	
	return 0;
};

int speex_filter_close(AVFilterContext *avf)
{
    SpeexFilterContext *s = avf->priv_data;

    speex_preprocess_state_destroy(s->preproc);

    return 0;	
}

int speex_filter_proc(AVFilterContext *avf, AVFilterContext *prev, void *in)
{
    SpeexFilterContext *s = avf->priv_data;

    float* spectral = prev && prev->specific && prev->active ? prev->specific : NULL;

    int opt;
    float flevel;

	// agc different?
    int newopt = (avf->flags & FILTER_FLAG_SPEEX_AGC) ? 1 : 0;

    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_GET_AGC, &opt);
    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_GET_AGC_LEVEL, &flevel);

    if (newopt ^ opt) {
		av_log(avf, AV_LOG_INFO, "Speex: changing AGC: %d\n", newopt);
	    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_AGC, &newopt);
	}

	if ((int) flevel != avf->agc_level)
	{
		av_log(avf, AV_LOG_INFO, "Speex: changing AGC level: %d\n", avf->agc_level);
		flevel = avf->agc_level;
		speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_AGC_LEVEL, &flevel);
	}

	// agc fixed different?
	/*newopt = (avf->flags & FILTER_FLAG_SPEEX_AGC_FIXED) ? 1 : 0;
    
	speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_GET_AGC_FIXED, &opt);

    if (newopt ^ opt) {
		av_log(avf, AV_LOG_INFO, "Changing AGC fixed: %d\n", newopt);
	    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_AGC_FIXED, &newopt);
	}*/

	// denoise different?
	newopt = (avf->flags & FILTER_FLAG_SPEEX_DENOISER) ? 1 : 0;
    
	speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_GET_DENOISE, &opt);

    if (newopt ^ opt) {
		av_log(avf, AV_LOG_INFO, "Changing Denoiser: %d\n", newopt);
	    speex_preprocess_ctl(s->preproc, SPEEX_PREPROCESS_SET_DENOISE, &newopt);
	}

    avf->vad = speex_preprocess(s->preproc, in, spectral);
    avf->loudness = (int) s->preproc->loudness2;
    
    av_log(avf, AV_LOG_DEBUG, "Speex preproc: current speex frame has (vad: %d, loudness: %d)\n",
        avf->vad,
        (int) avf->loudness);
    	
    return 0;	
}

int speex_aec_init(AVFilterContext *avf)
{
    SpeexAecContext *s = avf->priv_data;

	if (avf->echo_tail <= 0 || avf->echo_tail > 64000)
	{
		av_log(avf, AV_LOG_ERROR, "Speex aec error: echo tail is invalid (%d)\n", avf->echo_tail);
		return -1;
	}
	
	avf->specific = av_mallocz(sizeof(float) * avf->avctx->frame_size);
	avf->out = av_mallocz(sizeof(short) * avf->avctx->frame_size);
	avf->extra = av_mallocz(sizeof(short) * avf->avctx->frame_size);
	
	if (!avf->specific || !avf->out || !avf->extra)
	{
		av_log(avf, AV_LOG_ERROR, "Speex error: out of memory\n");
		return -1;
	}
	
	s->aec = speex_echo_state_init(avf->avctx->frame_size, avf->echo_tail);
	
	av_log(avf, AV_LOG_INFO, "Speex aec: using echo cancellation (tail length: %d samples)\n",
		avf->echo_tail);
	
#ifdef DUMP_PCM	
	s->f=fopen("avcodec.speexaec.pcm","wb");
#endif // DUMP_PCM

	return 0;
};

int speex_aec_close(AVFilterContext *avf)
{
    SpeexAecContext *s = avf->priv_data;

    speex_echo_state_destroy(s->aec);
    
    if (avf->specific)
    	av_free(avf->specific);

	if (avf->out)
		av_free(avf->out);

	if (avf->extra)
		av_free(avf->extra);
	
#ifdef DUMP_PCM	
	if (s->f)
		fclose(s->f);
#endif // DUMP_PCM
		
    return 0;	
}

int speex_aec_proc(AVFilterContext *avf, AVFilterContext *prev, void *in)
{
    SpeexAecContext *s = avf->priv_data;
    
#ifdef DUMP_PCM    
    ///////////////////////////////////////
	int i;
	short*mic = (short*) in;
	short*echo = (short*) avf->extra;

	struct STEREO {
		short echo;
		short mic;
	} st[4096];
	for (i=0;i<avf->avctx->frame_size;i++) {
		st[i].mic = mic[i];
		st[i].echo = echo[i];
	}
	fwrite(st,avf->avctx->frame_size,sizeof(struct STEREO),s->f);
	//////////////////////////////////////
#endif // DUMP_PCM	
	
	av_log(avf, AV_LOG_DEBUG, "Speex aec: cancelling frame\n");
	
	speex_echo_cancel(s->aec, in, avf->extra, avf->out, (float *) avf->specific);

	/* Reset the echo canceller state */
	//speex_echo_state_reset(s->aec); 
    	
    return 0;	
}

AVCodec speex_encoder = {
    "speex",
    CODEC_TYPE_AUDIO,
    CODEC_ID_SPEEX,
    sizeof(SpeexContext),
    speex_encode_init,
    speex_encode_frame,
    speex_encode_close
};

AVCodec speex_decoder = {
    "speex",
    CODEC_TYPE_AUDIO,
    CODEC_ID_SPEEX,
    sizeof(SpeexContext),
    speex_decode_init,
    NULL,
    speex_decode_close,
    speex_decode_frame
};

AVFilter speex_preproc_filter = {
	"speexpreproc",
	CODEC_TYPE_AUDIO,
	sizeof(SpeexFilterContext),
	speex_filter_init,
	speex_filter_proc,
	speex_filter_close
};

AVFilter speex_aec_filter = {
	"speexaec",
	CODEC_TYPE_AUDIO,
	sizeof(SpeexAecContext),
	speex_aec_init,
	speex_aec_proc,
	speex_aec_close
};
