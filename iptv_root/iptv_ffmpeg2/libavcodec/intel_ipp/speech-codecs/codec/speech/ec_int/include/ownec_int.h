/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2005-2007 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives EC Sample
//
//  By downloading and installing this sample, you hereby agree that the
//  accompanying Materials are being provided to you under the terms and
//  conditions of the End User License Agreement for the Intel(R) Integrated
//  Performance Primitives product previously accepted by you. Please refer
//  to the file ippEULA.rtf or ippEULA.txt located in the root directory of your Intel(R) IPP
//  product installation for more information.
//
//
//  Purpose: Echo Canceller header file.
//
*/

#ifndef __OWNEC_INT_H__
#define __OWNEC_INT_H__

#if defined(__ICC) || defined( __ICL ) || defined ( __ECL )
#define __INLINE static __inline
#elif defined( __GNUC__ )
#define __INLINE static __inline__
#else
#define __INLINE static
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /* common section */

#define COEF_SF 14
#define COEF_ONE (1 << COEF_SF)

#define F_SF 20

#define ALIGN(s) (((s) + 15) & ~15)
#define ADDWEIGHTED_INT(val, add, coef) ((val) = SCALE(((val) * (coef) + (COEF_ONE - coef) * (add)), COEF_SF))

#define SAT_32S_16S(v)((v) > 32767 ? 32767 : ((v) < -32768 ? -32768 : (v)))
#define SCALE(v, f) (((v) + (1<<((f)-1))) >> (f))

#define ADAPTATION_ENABLED (state->mode & 1)
#define NLP_ENABLED (state->mode & 2)
#define TD_ENABLED (state->mode & 4)
#define AH_ENABLED (state->mode & 16)
#define AH_ENABLED3 (state->mode & 64)

    /* fullband section */

    /* maximum size of frame */
    /* size of frame */
#define FB_FRAME_ORDER 3
#define FB_FRAME_SIZE (1<<FB_FRAME_ORDER)
#define FB_FRAME_SIZE_MAX (1<<(FB_FRAME_ORDER+1))
#define FIR_COFF_TYPE Ipp16s

    typedef struct __ifbECState {
        ScratchMem_Obj Mem;
        IppsFullbandControllerState_EC_16s *controller; /* controller */
        FIR_COFF_TYPE *firf_coef;        /* fixed filter coeffs */
        FIR_COFF_TYPE *fira_coef;        /* adaptive filter coeffs */
        Ipp16s *rin_hist;         /* history buffer */
        Ipp32s *pErrToWeights;
        ahState_int *ah;
        shiftupCosState_int *shiftupCos;
        //fdState_int *fd;
        //Ipp32s td_coeff;/* ToneDisabler coeffs */
        //Ipp64s td_thres;/* ToneDisabler coeffs */
        int tap;                  /* number of filter coeffs */
        int tap_m;                /* size of history buffer */
        int mode;                 /* current operating mode */
        int hist_buf;             /* history buffer */
        int pos;                  /* current position in history buffer */
        int td_mode;              /* mode before activate ToneDisabler */
        //int td_resr;              /* used in ToneDisabler */
        //int td_ress;              /* used in ToneDisabler */
        int frameSize;            /* size of frame */
        int frameNum;
        int freq;
        Ipp32s scratch_mem_size;
        Ipp16s err;               /* error of NLMS */
        Ipp16s sgain;             /* NLP gain */
        Ipp16s nlp;
        Ipp16s cng;
        //Ipp16s  samplingRate,td_sil_time,td_sil_thresh;
    } _ifbECState;

    /* subband section */

#define SB_FRAME_ORDER 6
#define SB_FRAME_SIZE (1<<SB_FRAME_ORDER)
#define SB_FFT_ORDER  (SB_FRAME_ORDER+1)
#define SB_FFT_LEN    (1<<SB_FFT_ORDER)
#define SB_WIN_LEN    SB_FFT_LEN
#define SB_SUBBANDS   ((1<<(SB_FFT_ORDER-1))+1)
#define INIT_SEED     11111

    typedef struct __isbECState {
        ScratchMem_Obj Mem;
        IppsSubbandControllerState_EC_16s *controller; /* subband controller */
        IppsToneDetectState_EC_16s *tdr, *tds; /* ToneDetect state structures */
        IppsFFTSpec_R_16s32s* spec_fft; /* FFT structure */
        IppsFFTSpec_R_32s* spec_fft32;  /* FFT structure */
        IppsFFTSpec_C_32sc* spec_fftC;  /* FFT structure */
        Ipp32sc **ppRinSubbandHistory; /* receive-in subband history */
        Ipp32sc **ppAdaptiveCoefs;     /* adaptive coeffs */
        Ipp32sc **ppFixedCoefs;        /* fixed coeffs */
        ahState_int *ah;
        shiftupCosState_int *shiftupCos;
        Ipp8u    *pBuf;                   /* FFT work buffer */
        //Ipp32s td_coeff;/* ToneDisabler coeffs */
        //Ipp64s td_thres;/* ToneDisabler coeffs */
        int    FFTSize, frameSize, windowLen;
        int    numSegments;               /* number of filter segments */
        int    numSubbands;               /* number of subbands */
        int    mode;                      /* current operating mode */
        int    td_mode;                   /* mode before activate ToneDisabler */
        Ipp32s scratch_mem_size;
        //int    td_resr, td_ress;          /* used in ToneDisabler */
        int    constrainSubbandNum,frameNum;
#ifndef AEC_SB_8ms_interface
        int    instackLenSout,instackLenRin,instackLenSin;
        int    instackCurLenSout,instackCurLenRin,instackCurLenSin;
        Ipp16s instackRin[FRAME_SIZE_10ms+FRAME_SIZE_6ms];
        Ipp16s instackSin[FRAME_SIZE_10ms+FRAME_SIZE_6ms];
        Ipp16s instackSout[FRAME_SIZE_10ms+FRAME_SIZE_6ms];
#endif
        Ipp16s pRinHistory[SB_WIN_LEN];   /* receive-in signal history */
        Ipp16s sgain;                     /* NLP gain */
        Ipp16s silence[SB_FRAME_SIZE];
        Ipp16s nlp;
        Ipp16s cng;
        short  seed;
    } _isbECState;

    /* subbandfast section */

#define SBF_FRAME_ORDER 4
    //#define SBF_20

#if 0

#define SBF_SUBBANDS 17
#define SBF_FRAME_SIZE 24
#define SBF_FFT_LEN 32
#define SBF_FFT_ORDER 5
#define SBF_WIN_LEN 128

#else
    /*
    #define SBF_SUBBANDS 33
    #define SBF_FRAME_SIZE 44
    #define SBF_FFT_LEN 64
    #define SBF_FFT_ORDER 6
    #define SBF_WIN_LEN 256
    */
#if !defined(SBF_20) && !defined(SBF_40)
#define SBF_SUBBANDS 33
#define SBF_FRAME_SIZE 16
#define SBF_FFT_LEN 64
#define SBF_FFT_ORDER 6
#define SBF_WIN_LEN 64
#endif

#ifdef SBF_20
#define SBF_SUBBANDS 33
#define SBF_FRAME_SIZE 20
#define SBF_FFT_LEN 64
#define SBF_FFT_ORDER 6
#define SBF_WIN_LEN 64
#endif

#ifdef SBF_40
#define SBF_SUBBANDS 65
#define SBF_FRAME_SIZE 40
#define SBF_FFT_LEN 128
#define SBF_FFT_ORDER 7
#define SBF_WIN_LEN 128
#endif
#endif

#define NOISE_GEN_LEVEL 10
#define SAFETY_BUFFER_SIZE 64 /* must be pow of 2 */

    //#define ADAPT_NOISE

    typedef struct __t_fp {
        int e, m;
    } t_fp;

    typedef struct __isbfECState {
        ScratchMem_Obj Mem;
        Ipp64s r_in_pwr;               /* receive-in signal power */
        Ipp64s s_in_pwr;               /* send-in signal power */
        Ipp64s rinPow;               /* receive-in signal power */
        Ipp64s sinPow;               /* send-in signal power */
        Ipp64s soutPow;
        Ipp32s coupling;
        Ipp32s es_off;
        Ipp32s conv;
        Ipp32s start_time;
        Ipp16s rinBuf[SBF_WIN_LEN];
        Ipp16s sinBuf[SBF_WIN_LEN];
        /* receive-in subband analysis structure */
        IppsSubbandProcessState_16s *state_ana_rin;
        /* send-in subband analysis structure */
        IppsSubbandProcessState_16s *state_ana_sin;
        /* subband synthesis structure */
        IppsSubbandProcessState_16s *state_syn;
        Ipp32sc **ppRinSubbandHistory; /* receive-in subband history */
        Ipp32sc **ppAdaptiveCoefs;     /* adaptive coeffs */
        Ipp32sc **ppFixedCoefs;        /* fixed coeffs */
        IppsSubbandControllerDTState_EC_16s *controller; /* subband controller */
//        struct _SubbandControllerDTState_EC_16s *controller2; /* subband controller 2*/
        IppsToneDetectState_EC_16s *tdr, *tds; /* ToneDetect state structures */
        ahState_int *ah;
        shiftupCosState_int *shiftupCos;
        Ipp8u *pBuf;                   /* FFT work buffer */
        //Ipp32s td_coeff;/* ToneDisabler coeffs */
        //Ipp64s td_thres;/* ToneDisabler coeffs */
        int FFTSize, frameSize, windowLen;
        int numSegments;               /* number of filter segments */
        int numSubbands;               /* number of subbands */
        int mode;                      /* current operating mode */
        int td_mode;                   /* mode before activate ToneDisabler */
        //int td_resr, td_ress;          /* used in ToneDisabler */
        int dt, dt1;                        /* Double talk */
        int frame;
        int upCounter;
        Ipp16s sgain;                  /* NLP gain */
        Ipp16s nlp;
        Ipp16s cng;
        Ipp32s smoothSin, smoothSout;
        Ipp32s shortTermSin, shortTermSout, longTermSout;
        Ipp32s nlp_mult;
        Ipp32s noise_seed;
#ifdef ADAPT_NOISE
        Ipp32s noise_idx;
        Ipp32s save_idx;
#endif
        Ipp32s noise_lev;
        Ipp32s save_noise_lev[5];
        Ipp32s iirDly[6];
        Ipp16s saved_sin[48];
        Ipp32s saved_sin_idx;
        t_fp ns_lambda_d[SBF_SUBBANDS];
        t_fp ns_x_pwr[SBF_SUBBANDS];
#ifdef ADAPT_NOISE
        t_fp saved_noise_pow[SAFETY_BUFFER_SIZE];
        t_fp noise_pow;
#endif
        t_fp ns_one, ns_beta, ns_beta1m, ns_alpha, ns_alpha1m, ns_gamma, ns_gamma1m;
        t_fp ns_al, ns_al1m;
        Ipp32s scratch_mem_size;
        //Ipp16s  samplingRate,td_sil_time,td_sil_thresh;
// new members
        Ipp64s acpwr;
        Ipp64s cdlt;
        int fixCfgInit;
        Ipp64s rinSubbandPow[SBF_SUBBANDS];

    } _isbfECState;
#define AEC_MASK1S 2
#ifdef __cplusplus
}
#endif

#endif /* __OWNEC_H__ */
