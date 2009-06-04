/*
 * LGPL
 */

/**
 * @file opts.c
 * options parser.
 * typical parsed command line:
 * msmpeg4:bitrate=720000:qmax=16
 *
 */

#include "avcodec.h"

#ifndef offsetof
# define offsetof(T,F) ((unsigned int)((char *)&((T *)0)->F))
#endif

#define _AVOPTION_CODEC_BOOL(name, help, field) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_BOOL }
#define _AVOPTION_CODEC_DOUBLE(name, help, field, minv, maxv, defval) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_DOUBLE, minv, maxv, defval }
#define _AVOPTION_CODEC_FLAG(name, help, field, flag, defval) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_FLAG, flag, 0, defval }
#define _AVOPTION_CODEC_INT(name, help, field, minv, maxv, defval) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_INT, minv, maxv, defval }
#define _AVOPTION_CODEC_STRING(name, help, field, str, val) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_STRING, .defval = val, .defstr = str }
#define _AVOPTION_CODEC_RCOVERRIDE(name, help, field) \
    { name, help, offsetof(AVCodecContext, field), _FF_OPT_TYPE_RCOVERRIDE, .defval = 0, .defstr = NULL }
#define _AVOPTION_SUB(ptr) { .name = NULL, .help = (const char*)ptr }
#define _AVOPTION_END() _AVOPTION_SUB(NULL)

#define _AVOPTION_FILTER_BOOL(name, help, field) \
    { name, help, offsetof(AVFilterContext, field), _FF_OPT_TYPE_BOOL }
#define _AVOPTION_FILTER_DOUBLE(name, help, field, minv, maxv, defval) \
    { name, help, offsetof(AVFilterContext, field), _FF_OPT_TYPE_DOUBLE, minv, maxv, defval }
#define _AVOPTION_FILTER_FLAG(name, help, field, flag, defval) \
    { name, help, offsetof(AVFilterContext, field), _FF_OPT_TYPE_FLAG, flag, 0, defval }
#define _AVOPTION_FILTER_INT(name, help, field, minv, maxv, defval) \
    { name, help, offsetof(AVFilterContext, field), _FF_OPT_TYPE_INT, minv, maxv, defval }

/**
 * AVOption.
 */
typedef struct _AVOption {
    /** options' name */
    const char *name; /* if name is NULL, it indicates a link to next */
    /** short English text help or const struct AVOption* subpointer */
    const char *help; //	const struct AVOption* sub;
    /** offset to context structure where the parsed value should be stored */
    int offset;
    /** options' type */
    int type;
#define _FF_OPT_TYPE_BOOL 1      ///< boolean - true,1,on  (or simply presence)
#define _FF_OPT_TYPE_DOUBLE 2    ///< double
#define _FF_OPT_TYPE_INT 3       ///< integer
#define _FF_OPT_TYPE_STRING 4    ///< string (finished with \0)
#define _FF_OPT_TYPE_MASK 0x1f	///< mask for types - upper bits are various flags
//#define _FF_OPT_TYPE_EXPERT 0x20 // flag for expert option
#define _FF_OPT_TYPE_FLAG (_FF_OPT_TYPE_BOOL | 0x40)
#define _FF_OPT_TYPE_RCOVERRIDE (_FF_OPT_TYPE_STRING | 0x80)
    /** min value  (min == max   ->  no limits) */
    double min;
    /** maximum value for double/int */
    double max;
    /** default boo [0,1]l/double/int value */
    double defval;
    /**
     * default string value (with optional semicolon delimited extra option-list
     * i.e.   option1;option2;option3
     * defval might select other then first argument as default
     */
    const char *defstr;
#define _FF_OPT_MAX_DEPTH 10
} _AVOption;

static const _AVOption _avoptions_common[] = {
    _AVOPTION_CODEC_FLAG("bit_exact", "use only bit-exact stuff", flags, CODEC_FLAG_BITEXACT, 0),
    _AVOPTION_CODEC_FLAG("mm_force", "force mm flags", dsp_mask, FF_MM_FORCE, 0),
#ifdef HAVE_MMX
    _AVOPTION_CODEC_FLAG("mm_mmx", "mask MMX feature", dsp_mask, FF_MM_MMX, 0),
    _AVOPTION_CODEC_FLAG("mm_3dnow", "mask 3DNow feature", dsp_mask, FF_MM_3DNOW, 0),
    _AVOPTION_CODEC_FLAG("mm_mmxext", "mask MMXEXT (MMX2) feature", dsp_mask, FF_MM_MMXEXT, 0),
    _AVOPTION_CODEC_FLAG("mm_sse", "mask SSE feature", dsp_mask, FF_MM_SSE, 0),
    _AVOPTION_CODEC_FLAG("mm_sse2", "mask SSE2 feature", dsp_mask, FF_MM_SSE2, 0),
#endif
    _AVOPTION_END()
};

static const _AVOption _avoptions_video_common[] = {
    _AVOPTION_CODEC_INT("bit_rate", "desired codec bitrate (bits/sec)", bit_rate, 4, 240000000, 800000),
    _AVOPTION_CODEC_INT("bit_rate_tolerance", "number of bits the bitstream is allowed to diverge from the reference, can be CBR (for CBR pass1) or VBR (for pass2)", bit_rate_tolerance, 4, 240000000, 8000000),
    _AVOPTION_CODEC_INT("gop", "group of pictures (max number of frames with one keyframe)", gop_size, 0, 600, 50),
    _AVOPTION_CODEC_INT("rc_min", "rate control minimum bitrate (bits/sec)", rc_min_rate, 4, 24000000, 0),	
    _AVOPTION_CODEC_INT("rc_max", "rate control maximum bitrate (bits/sec)", rc_max_rate, 4, 24000000, 0),
    _AVOPTION_CODEC_INT("rc_buffer_size", "rate control vbv buffer size (in bits)", rc_buffer_size, 0, 24000000, 0),
    _AVOPTION_CODEC_DOUBLE("rc_buffer_aggressivity", "rate control buffer aggressivity", rc_buffer_aggressivity, 4, 24000000, 0),
    _AVOPTION_CODEC_INT("rc_initial_buffer_occupancy", "rate control vbv buffer occupancy (in bits)", rc_initial_buffer_occupancy, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("qmin", "minimum quantizer", qmin, 1, 51, 2),
    _AVOPTION_CODEC_INT("qmax", "maximum quantizer", qmax, 1, 51, 31),
    _AVOPTION_CODEC_FLAG("psnr", "calculate PSNR of compressed frames", flags, CODEC_FLAG_PSNR, 0),
    _AVOPTION_CODEC_INT("noise_reduction", "noise reduction strength", noise_reduction, 0, 256, 0),
    _AVOPTION_CODEC_FLAG("ime", "force interlaced motion estimation", flags, CODEC_FLAG_INTERLACED_ME, 0),
    _AVOPTION_CODEC_FLAG("idct", "force interlaced discrete cosine transform", flags, CODEC_FLAG_INTERLACED_DCT, 0),
    _AVOPTION_CODEC_FLAG("closedgop", "closed gop", flags, CODEC_FLAG_CLOSED_GOP, 0),
    _AVOPTION_CODEC_INT("qdiff", "maximum quantizer difference between frames", max_qdiff, 1, 16, 3),
    _AVOPTION_CODEC_DOUBLE("qsquish", "ratecontrol qmin qmax limiting method. 0-> clipping, 1-> use a nice continous function to limit qscale wthin qmin/qmax", rc_qsquish, 0., 1., 0.),
    _AVOPTION_CODEC_DOUBLE("qcomp", "video quantizer scale compression (VBR)", qcompress, 0., 1., 0.5),    
    // [encoder,streams[video],codec_properties[h263p]]
    _AVOPTION_CODEC_FLAG("umv", "unlimited motion vector", flags, CODEC_FLAG_H263P_UMV, 0),
    _AVOPTION_CODEC_FLAG("ssm", "h263 slice structure", flags, CODEC_FLAG_H263P_SLICE_STRUCT , 0),
    _AVOPTION_CODEC_FLAG("aiv", "H263 Alternative inter vlc", flags, CODEC_FLAG_H263P_AIV, 0),
    _AVOPTION_CODEC_FLAG("obmc", "OBMC", flags, CODEC_FLAG_OBMC, 0),
    // [encoder,streams[video],codec_properties[mpeg4]]
    _AVOPTION_CODEC_FLAG("4mv", "4 MV per MB allowed / Advanced prediction for H263", flags, CODEC_FLAG_4MV, 0),
    _AVOPTION_CODEC_FLAG("part", "use data partitioning", flags, CODEC_FLAG_PART, 0),
    _AVOPTION_CODEC_FLAG("gmc", "use GMC (Global Motion Compensation", flags, CODEC_FLAG_GMC, 0),
    _AVOPTION_CODEC_FLAG("qpel", "use qpel MC", flags, CODEC_FLAG_QPEL, 0),
    _AVOPTION_CODEC_FLAG("mv0", "always try a MB with MV=<0,0>", flags, CODEC_FLAG_MV0, 0),
    // [encoder,streams[video],codec_properties[h263p|mpeg4]]
    _AVOPTION_CODEC_FLAG("acpred", "H263 Advanced intra coding / MPEG4 AC prediction", flags, CODEC_FLAG_AC_PRED, 0),
    // [encoder,streams[video],codec_properties[h263p|mpeg2|mpeg4|h264]]
    _AVOPTION_CODEC_INT("bframes", "maximum number of b frames between non b frames. note: the output will be delayed by max_b_frames+1 relative to the input", max_b_frames, 0, FF_MAX_B_FRAMES, 0),
    _AVOPTION_CODEC_FLAG("trellis", "use trellis quantization", flags, CODEC_FLAG_TRELLIS_QUANT, 0),
    _AVOPTION_CODEC_FLAG("alt", "use alternate scan", flags, CODEC_FLAG_ALT_SCAN, 0),
    _AVOPTION_CODEC_FLAG("loop", "loop filter", flags, CODEC_FLAG_LOOP_FILTER, 0),
    // [encoder,streams[video],codec_properties[h264]]
    _AVOPTION_CODEC_DOUBLE("crf", "enables constant quality mode, and selects the quality (x264)", crf, 0., 51., 0.),
    _AVOPTION_CODEC_INT("cqp", "constant quantization parameter rate control method", cqp, INT_MIN, INT_MAX, -1),
    _AVOPTION_CODEC_INT("coder", "coder type. 0-> vlc, 1-> ac", coder_type, 0, 1, 0),
    _AVOPTION_CODEC_INT("thread_count", "# encoding threads", thread_count, 1, 8, 1),
    _AVOPTION_CODEC_INT("mpeg_quant", "0-> h263 quant 1-> mpeg quant", mpeg_quant, 0, 1, 0),
    _AVOPTION_CODEC_STRING("rc_eq", "rate control equation", rc_eq, "tex^qComp,option1,options2", 0),
    _AVOPTION_CODEC_DOUBLE("rc_initial_cplx", "initial complexity for pass1 ratecontrol", rc_initial_cplx, 0., 9999999., 0),
    _AVOPTION_CODEC_DOUBLE("i_quant_factor", "qscale factor between p and i frames", i_quant_factor, 0., 0., 0),
    _AVOPTION_CODEC_DOUBLE("i_quant_offset", "qscale offset between p and i frames", i_quant_factor, -999999., 999999., 0),
    _AVOPTION_CODEC_INT("dct_algo", "dct alghorithm", dct_algo, 0, 5, 0), // fixme - "Auto,FastInt,Int,MMX,MLib,Altivec"
    _AVOPTION_CODEC_DOUBLE("lumi_masking", "luminance masking", lumi_masking, 0., 999999., 0),
    _AVOPTION_CODEC_DOUBLE("temporal_cplx_masking", "temporary complexity masking", temporal_cplx_masking, 0., 999999., 0),
    _AVOPTION_CODEC_DOUBLE("spatial_cplx_masking", "spatial complexity masking", spatial_cplx_masking, 0., 999999., 0),
    _AVOPTION_CODEC_DOUBLE("p_masking", "p block masking", p_masking, 0., 999999., 0),
    _AVOPTION_CODEC_DOUBLE("dark_masking", "darkness masking", dark_masking, 0., 999999., 0),
    _AVOPTION_CODEC_INT("idct_algo", "idct alghorithm", idct_algo, 0, 8, 0), // fixme - "Auto,Int,Simple,SimpleMMX,LibMPEG2MMX,PS2,MLib,ARM,Altivec"
    _AVOPTION_CODEC_INT("mb_qmin", "minimum MB quantizer", mb_qmin, 0, 8, 0),
    _AVOPTION_CODEC_INT("mb_qmax", "maximum MB quantizer", mb_qmin, 0, 8, 0),
    _AVOPTION_CODEC_INT("me_cmp", "ME compare function", me_cmp, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("me_sub_cmp", "subpixel ME compare function", me_sub_cmp, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("dia_size", "ME diamond size & shape", dia_size, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("last_predictor_count", "amount of previous MV predictors", last_predictor_count, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("pre_me", "pre pass for ME", pre_me, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("me_pre_cmp", "ME pre pass compare function", me_pre_cmp, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("me_range", "maximum ME search range", me_range, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("pre_dia_size", "ME pre pass diamod size & shape", pre_dia_size, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("me_subpel_quality", "subpel ME quality", me_subpel_quality, 0, 24000000, 0),
    _AVOPTION_CODEC_INT("me_range", "maximum ME search range", me_range, 0, 24000000, 0),
    _AVOPTION_CODEC_RCOVERRIDE("rc_override", "ratecontrol override (=startframe,endframe,qscale,quality_factor)", rc_override),    
    _AVOPTION_SUB(_avoptions_common),
    _AVOPTION_END()
};

static const _AVOption _avoptions_audio_encode[] =
{    
    _AVOPTION_CODEC_INT("quality", "encoding global quality", global_quality, 0, 9, 0),
    _AVOPTION_CODEC_INT("bit_rate", "desired audio bitrate (only useful in vbr)", bit_rate, 2000, 20000000, 0),
    _AVOPTION_CODEC_FLAG("dtx", "discontinous transmission (DTX)", flags2, CODEC_FLAG2_SPEEX_DTX, 0), 
    _AVOPTION_CODEC_FLAG("cng", "confort noise generation (CNG) (codec vad)", flags2, CODEC_FLAG2_SPEEX_CNG, 0), 
    _AVOPTION_CODEC_FLAG("vbr", "variable bitrate (VBR)", flags, CODEC_FLAG_QSCALE, 0), 
    _AVOPTION_CODEC_INT("abr", "average bitrate (ABR)", rc_max_rate, 0, 44000, 0), 
    _AVOPTION_END()
};

static const _AVOption _avoptions_audio_decode[] =
{
    _AVOPTION_CODEC_FLAG("enh", "perceptual enhancement (ENH)", flags2, CODEC_FLAG2_SPEEX_ENH, 0),
    _AVOPTION_END()
};

static const _AVOption _avoptions_workaround_bug[] = {
    _AVOPTION_CODEC_FLAG("bug_autodetect", "workaround bug autodetection", workaround_bugs, FF_BUG_AUTODETECT, 1),
    _AVOPTION_CODEC_FLAG("bug_old_msmpeg4", "workaround old msmpeg4 bug", workaround_bugs, FF_BUG_OLD_MSMPEG4, 0),
    _AVOPTION_CODEC_FLAG("bug_xvid_ilace", "workaround XviD interlace bug", workaround_bugs, FF_BUG_XVID_ILACE, 0),
    _AVOPTION_CODEC_FLAG("bug_ump4", "workaround ump4 bug", workaround_bugs, FF_BUG_UMP4, 0),
    _AVOPTION_CODEC_FLAG("bug_no_padding", "workaround padding bug", workaround_bugs, FF_BUG_NO_PADDING, 0),
    _AVOPTION_CODEC_FLAG("bug_ac_vlc", "workaround ac VLC bug", workaround_bugs, FF_BUG_AC_VLC, 0),
    _AVOPTION_CODEC_FLAG("bug_qpel_chroma", "workaround qpel chroma bug", workaround_bugs, FF_BUG_QPEL_CHROMA, 0),
    _AVOPTION_CODEC_FLAG("bug_std_qpel", "workaround std qpel bug", workaround_bugs, FF_BUG_STD_QPEL, 0),
    _AVOPTION_CODEC_FLAG("bug_qpel_chroma2", "workaround qpel chroma2 bug", workaround_bugs, FF_BUG_QPEL_CHROMA2, 0),
    _AVOPTION_CODEC_FLAG("bug_direct_blocksize", "workaround direct blocksize bug", workaround_bugs, FF_BUG_DIRECT_BLOCKSIZE, 0),
    _AVOPTION_END()
};

static const _AVOption _avoptions_filters[] = {
    _AVOPTION_FILTER_FLAG("vad", "voice activity detection (VAD) preprocess", flags, FILTER_FLAG_SPEEX_VAD, 0), 
    _AVOPTION_FILTER_FLAG("denoise", "denoising preprocess", flags, FILTER_FLAG_SPEEX_DENOISER, 0),
    _AVOPTION_FILTER_FLAG("agc", "agc (automatic gain control) preprocess", flags, FILTER_FLAG_SPEEX_AGC, 0),
    _AVOPTION_FILTER_INT("agc_level", "agc level", agc_level, 0, 32768, 8000),
    _AVOPTION_END()
};	

static const _AVOption _all_options[] =
{
    _AVOPTION_SUB(_avoptions_video_common),
    _AVOPTION_SUB(_avoptions_audio_encode),
    _AVOPTION_SUB(_avoptions_audio_decode),
    _AVOPTION_SUB(_avoptions_workaround_bug),
    _AVOPTION_END()    
};	

/* avoid compatibility problems by redefining it */
static int av_strcasecmp(const char *s1, const char *s2)
{
    signed char val;
    
    for(;;) {
        val = toupper(*s1) - toupper(*s2);
        if (val != 0)
            break;
        if (*s1 != '\0')
            break;
        s1++;
        s2++;
    }
    return val;
}


static int parse_bool(const _AVOption *c, char *s, int *var)
{
    int b = 1; /* by default -on- when present */
    if (s) {
	if (!av_strcasecmp(s, "off") || !av_strcasecmp(s, "false")
	    || !strcmp(s, "0"))
	    b = 0;
	else if (!av_strcasecmp(s, "on") || !av_strcasecmp(s, "true")
		 || !strcmp(s, "1"))
	    b = 1;
	else
	    return -1;
    }

    if (c->type == _FF_OPT_TYPE_FLAG) {
	if (b)
	    *var |= (int)c->min;
	else
            *var &= ~(int)c->min;
    } else
	*var = b;
    return 0;
}

static int parse_double(const _AVOption *c, char *s, float *var)
{
    float d;
    if (!s)
        return -1;
    d = atof(s);
    if (c->min != c->max) {
	if (d < c->min || d > c->max) {
	    av_log(NULL, AV_LOG_ERROR, "Option: %s double value: %f out of range <%f, %f>\n",
		    c->name, d, c->min, c->max);
	    return -1;
	}
    }
    *var = d;
    return 0;
}

static int parse_int(const _AVOption* c, char* s, int* var)
{
    int i;
    if (!s)
        return -1;
    i = atoi(s);
    if (c->min != c->max) {
	if (i < (int)c->min || i > (int)c->max) {
	    av_log(NULL, AV_LOG_ERROR, "Option: %s integer value: %d out of range <%d, %d>\n",
		    c->name, i, (int)c->min, (int)c->max);
	    return -1;
	}
    }
    *var = i;
    return 0;
}

static int parse_string(const _AVOption *c, char *s, void* strct, char **var)
{
    if (!s)
	return -1;

    if (c->type == _FF_OPT_TYPE_RCOVERRIDE) {
	int sf, ef, qs;
	float qf;
	if (sscanf(s, "%d,%d,%d,%f", &sf, &ef, &qs, &qf) == 4 && sf < ef) {
	    AVCodecContext *avctx = (AVCodecContext *) strct;
	    RcOverride *o;
	    avctx->rc_override = av_realloc(avctx->rc_override,
					    sizeof(RcOverride) * (avctx->rc_override_count + 1));
	    o = avctx->rc_override + avctx->rc_override_count++;
	    o->start_frame = sf;
	    o->end_frame = ef;
	    o->qscale = qs;
	    o->quality_factor = qf;

	    //printf("parsed Rc:  %d,%d,%d,%f  (%d)\n", sf,ef,qs,qf, avctx->rc_override_count);
	} else {
	    av_log(NULL, AV_LOG_ERROR, "incorrect/unparsable Rc: \"%s\"\n", s);
	}
    } else
	*var = av_strdup(s);
    return 0;
}

int av_opt_parse(void* strct, const char *opts, int codec)
{
    int r = 0;
    char* dopts = av_strdup(opts);
    if (dopts) {
        char *str = dopts;

	while (str && *str && r == 0) {
	    const _AVOption *stack[_FF_OPT_MAX_DEPTH];
	    const _AVOption *c = codec ? _all_options : _avoptions_filters;
	    int depth = 0;
	    char* e = strchr(str, ':');
	    char* p;
	    if (e)
		*e++ = 0;

	    p = strchr(str, '=');
	    if (p)
		*p++ = 0;

            // going through option structures
	    for (;;) {
		if (!c->name) {
		    if (c->help) {
			stack[depth++] = c;
			c = (const _AVOption*) c->help;
			assert(depth > _FF_OPT_MAX_DEPTH);
		    } else {
			if (depth == 0)
			    break; // finished
			c = stack[--depth];
                        c++;
		    }
		} else {
		    if (!strcmp(c->name, str)) {
			void* ptr = (char*)strct + c->offset;

			av_log (strct, AV_LOG_DEBUG, "setting property %s = %s\n", c->name, p);
			
			switch (c->type & _FF_OPT_TYPE_MASK) {
			case _FF_OPT_TYPE_BOOL:
			    r = parse_bool(c, p, (int*)ptr);
			    break;
			case _FF_OPT_TYPE_DOUBLE:
			    r = parse_double(c, p, (float*)ptr);
			    break;
			case _FF_OPT_TYPE_INT:
			    r = parse_int(c, p, (int*)ptr);
			    break;
			case _FF_OPT_TYPE_STRING:
			    r = parse_string(c, p, strct, (char**)ptr);
			    break;
			default:
			    assert(0 == 1);
			}
		    }
		    c++;
		}
	    }
	    str = e;
	}
	av_free(dopts);
    }
    return r;
}
