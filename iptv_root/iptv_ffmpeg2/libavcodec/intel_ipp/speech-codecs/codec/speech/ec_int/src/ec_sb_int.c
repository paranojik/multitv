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
//  Purpose: Echo Canceller, subband algorithm
//
*/

#include <stdlib.h>
#include <ipps.h>
#include <ippsc.h>
#include "scratchmem.h"
#include "nlp_api.h"
#include "ec_api_int.h"
#include "ec_td_int.h"
#include "_ippnr_int.h"
#include "ah_api_int.h"
#include "ownah_int.h"
#include "ownec_int.h"

#define COEF_SF 14
#define COEF_ONE (1 << COEF_SF)
#define COEF_0_5 (COEF_ONE / 2)
#define COEF_0_2 (COEF_ONE / 5)
#define COEF_0_77 (COEF_ONE * 10 / 13)
#define COEF_0_35 (COEF_ONE * 7 / 20)
#define COEF_0_48 (COEF_ONE * 12 / 25)
#define COEF_0_66 (COEF_ONE * 2 / 3)
#define COEF_0_8 (COEF_ONE * 4 / 5)
#define COEF_Q15_ONE ((1 << 15) - 1)

#define PSD_SF   10          /* scale factor for input PSD */
#define WEIGHTED_INT(val, add, coef) SCALE(((val) * (coef) + (COEF_ONE - coef) * (add)), COEF_SF)
#define CMUL(x, v) SCALE((x) * (v), COEF_SF)

int ec_isb_setTDThreshold(void *stat,int val)
{
    _isbECState *state=(_isbECState *)stat;
    if (val>-30) {
        return 1;
    }
    if (val<-89) {
        val=-89;
    }
    state->ah->td.td_thres=(Ipp64s)(IPP_AEC_dBtbl[-val-10]*IPP_AEC_dBtbl[-val-10]);
    return 0;
}
int ec_isb_setTDSilenceTime(void *stat,int val)
{
    _isbECState *state=(_isbECState *)stat;
    if (val<100) {
        return 1;
    }
    state->ah->td.td_sil_thresh=(Ipp16s)val;
    return 0;
}
int ec_isb_setNR(void *stat, int n){
    int status=0;
    _isbECState *state=(_isbECState *)stat;
    state->nlp = (Ipp16s)n;
    return status;
}
int ec_isb_setCNG(void *stat, int n){
    int status=0;
    _isbECState *state=(_isbECState *)stat;
    state->cng = (Ipp16s)n;
    return status;
}
/* Returns size of frame */
/* init scratch memory buffer */
int ec_isb_InitBuff(void * stat, char *buff) {
    _isbECState *state=(_isbECState *)stat;
    if (state==NULL) {
        return 1;
    }
    if(NULL != buff)
        state->Mem.base = buff;
    else
        if (NULL == state->Mem.base)
            return 1;
    state->Mem.CurPtr = state->Mem.base;
    state->Mem.VecPtr = (int *)(state->Mem.base+state->scratch_mem_size);
    return 0;
}
int ec_isb_GetInfo(void *stat,ECOpcode *isNLP,ECOpcode *isTD,
                   ECOpcode *isAdapt,ECOpcode *isAdaptLite, ECOpcode *isAH)
{
    _isbECState *state=(_isbECState *)stat;
    if(state->mode & 2)
        *isNLP = EC_NLP_ENABLE;
    else
        *isNLP = EC_NLP_DISABLE;

    if(state->mode & 16)
        *isAH = EC_AH_ENABLE;
    //*isAH = EC_AH_ENABLE1;
    //else
    //    *isAH = EC_AH_DISABLE;

    //if(state->mode & 32)
    //    *isAH = EC_AH_ENABLE2;
    //else
    //    *isAH = EC_AH_DISABLE;

    if(state->mode & 4)
        *isTD = EC_TD_ENABLE;
    else
        *isTD = EC_TD_DISABLE;

    *isAdaptLite = EC_ADAPTATION_DISABLE_LITE;
    if(state->mode & 1){
        *isAdapt = EC_ADAPTATION_ENABLE;
        if(state->mode & 8)
            *isAdaptLite = EC_ADAPTATION_ENABLE_LITE;
    } else
        *isAdapt = EC_ADAPTATION_DISABLE;

    return 0;
}

#ifdef AEC_SB_8ms_interface
/* Returns size of frame */
int ec_isb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    *s = SB_FRAME_SIZE+(int)freq-(int)freq+taptime_ms-taptime_ms;
    return 0;
}
#else
int ec_isb_GetFrameSize(IppPCMFrequency freq, int taptime_ms, int *s)
{
    *s = FRAME_SIZE_10ms+(int)freq-(int)freq+taptime_ms-taptime_ms;
    return 0;
}
#endif
int ec_isb_GetSegNum(void * stat)
{
    _isbECState *state=(_isbECState *)stat;
    return state->numSegments;
}
int ec_isb_GetSubbandNum(void * stat)
{
    _isbECState *state=(_isbECState *)stat;
    return state->numSubbands;
}

/* Returns size of buffer for state structure */
int ec_isb_GetSize(IppPCMFrequency freq, int taptime_ms, int *s, int *s1, int *scratch_size)
{
    int size = 0, numSegments;//,csize1;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;

    size += ALIGN(sizeof(_isbECState));

    ippsFFTGetSize_R_16s32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);

    size += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32sc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    size += ALIGN(pSizeSpec);

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTGetSize_R_32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    size += ALIGN(pSizeSpec);

    if (csize < pSizeInit1)
        csize = pSizeInit1;

    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    size += ALIGN(csize);

    if (freq == IPP_PCM_FREQ_8000) {
        numSegments = (taptime_ms * 8 - 1) / SB_FRAME_SIZE + 1;
    } else if (freq == IPP_PCM_FREQ_16000) {
        numSegments = (taptime_ms * 16 - 1) / SB_FRAME_SIZE + 1;
    } else {
        return 1;
    }

    size += ALIGN(3 * numSegments * sizeof(Ipp32sc *));

    size += ALIGN(3 * numSegments * SB_SUBBANDS * sizeof(Ipp32sc));

    ippsSubbandControllerGetSize_EC_16s(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);// freq don't used

    size += ALIGN(csize);

    *s = ALIGN(size+31);

    ah_GetSize_int(SB_FRAME_SIZE, &csize,freq,scratch_size);
    //shiftupCos_GetSize(FRAME_SIZE_10ms, &csize1);
    //csize += ALIGN(csize1);
    *s1 = ALIGN(csize+31);

    return 0;
}

/* Returns delay introduced to send-path */
int ec_isb_GetSendPathDelay(int *delay)
{
    *delay = SB_OFF;
    return 0;
}

/* acquire NLP gain coeff */
int ec_isb_GetNLPGain(void * stat)
{
    _isbECState *state=(_isbECState *)stat;
    return state->sgain;
}

/* Initializes state structure */
int ec_isb_Init(void * stat,void *ahObj, IppPCMFrequency freq, int taptime_ms)
{
    int i, numSegments,ret;
    int pSizeSpec, pSizeInit, pSizeBuf;
    int pSizeInit1, pSizeBuf1, csize;
    Ipp8u *ptr, *ptr0, *ptr1, *ptr2, *buf;
    _isbECState *state=(_isbECState *)stat;
    if (state==0 || ahObj==0) {
        return 1;
    }
    ptr = (Ipp8u *)state;

    ptr += ALIGN(sizeof(_isbECState));


    ippsFFTGetSize_R_16s32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit, &pSizeBuf);

    ptr0 = ptr;
    ptr += ALIGN(pSizeSpec);

    ippsFFTGetSize_C_32sc(SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    ptr1 = ptr;
    ptr += ALIGN(pSizeSpec);

    csize = pSizeInit;
    if (csize < pSizeInit1)
        csize = pSizeInit1;
    if (csize < pSizeBuf)
        csize = pSizeBuf;
    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTGetSize_R_32s(SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &pSizeSpec, &pSizeInit1, &pSizeBuf1);

    ptr2 = ptr;
    ptr += ALIGN(pSizeSpec);

    if (csize < pSizeInit1)
        csize = pSizeInit1;

    if (csize < pSizeBuf1)
        csize = pSizeBuf1;

    ippsFFTInit_R_16s32s(&(state->spec_fft), SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        ptr0, ptr);
    ippsFFTInit_R_32s(&(state->spec_fft32), SB_FFT_ORDER, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        ptr1, ptr);
    ippsFFTInit_C_32sc(&(state->spec_fftC), SB_FFT_ORDER, IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate,
        ptr2, ptr);

    /* Work buffer for FFT */
    state->pBuf = ptr;

    ptr += ALIGN(csize);

    state->FFTSize = (1<<SB_FFT_ORDER);
    state->numSubbands = SB_SUBBANDS;
    state->frameSize = SB_FRAME_SIZE;
    state->windowLen = SB_WIN_LEN;

    if (freq == IPP_PCM_FREQ_8000) {
        numSegments = (taptime_ms * 8 - 1) / state->frameSize + 1;
    } else if (freq == IPP_PCM_FREQ_16000) {
        numSegments = (taptime_ms * 16 - 1) / state->frameSize + 1;
    } else {
        return 1;
    }

    state->numSegments = numSegments;

    /* receive-in subband history */
    state->ppRinSubbandHistory = (Ipp32sc **)ptr;
    ptr += ALIGN(3 * numSegments * sizeof(Ipp32sc *));
    /* adaptive coeffs */
    state->ppAdaptiveCoefs = state->ppRinSubbandHistory + numSegments;
    /* fixed coeffs */
    state->ppFixedCoefs = state->ppRinSubbandHistory + numSegments * 2;

    buf = ptr;
    ptr += ALIGN(3 * numSegments * (state->numSubbands) * sizeof(Ipp32sc));

    /* Zero coeffs buffer and history buffer */
    ippsZero_32sc((Ipp32sc *)buf, 3 * numSegments * (state->numSubbands));

    /* Initialize receive-in subband history array */
    for (i = 0; i < numSegments; i++) {
        (state->ppRinSubbandHistory)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32sc);

    /* Initialize adaptive coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppAdaptiveCoefs)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }
    buf += numSegments * (state->numSubbands) * sizeof(Ipp32sc);

    /* Initialize fixed coeffs array */
    for (i = 0; i < numSegments; i++) {
        (state->ppFixedCoefs)[i] = (Ipp32sc *)buf + i * (state->numSubbands);
    }

    /* Initialize subband controller */
    state->controller = (IppsSubbandControllerState_EC_16s *)ptr;
    if (ippsSubbandControllerInit_EC_16s(state->controller, state->numSubbands, state->frameSize, numSegments, freq) != ippStsOk)
        return 1;

    ippsSubbandControllerGetSize_EC_16s(SB_SUBBANDS, SB_FRAME_SIZE, numSegments, freq, &csize);

    ptr += ALIGN(csize);
    state->ah = (ahState_int *)IPP_ALIGNED_PTR(ahObj,16);
    ret=ah_Init_int(state->ah, freq, SB_FRAME_SIZE,SB_OFF);
    if (ret) {
        return ret;
    }
    ah_GetSize_int(SB_FRAME_SIZE, &csize, freq,&state->scratch_mem_size);
    ptr += ALIGN(csize);
    //state->shiftupCos = (shiftupCosState_int *)IPP_ALIGNED_PTR((Ipp8u*)ahObj + ALIGN(csize), 16);
    //shiftupCos_Init(state->shiftupCos, FRAME_SIZE_10ms);
    //ah_GetSize_int(SB_FRAME_SIZE, &csize);
    //ptr += ALIGN(csize);

    /* zero receive-in history buffer */
    ippsZero_16s(state->pRinHistory, state->windowLen);

    /* enable adaptation */
    state->mode = 1;
    state->sgain = IPP_MAX_16S;
    state->constrainSubbandNum=0;
    state->seed=INIT_SEED;
    state->nlp=1;
#ifndef AEC_SB_8ms_interface
    state->instackLenSout=state->instackLenRin=state->instackLenSin=FRAME_SIZE_10ms+FRAME_SIZE_6ms;
    state->instackCurLenSout=0;
    state->instackCurLenRin=state->instackCurLenSin=FRAME_SIZE_6ms;
    ippsZero_16s(state->instackRin, FRAME_SIZE_10ms+FRAME_SIZE_6ms);
    ippsZero_16s(state->instackSin, FRAME_SIZE_10ms+FRAME_SIZE_6ms);
    ippsZero_16s(state->instackSout, FRAME_SIZE_10ms+FRAME_SIZE_6ms);
#endif
    //mprintf("Intel(R) IPP AEC. SUB-BAND algorithm. Version %d\n",IPP_AEC_VER );
    return 0;
}

/* Do operation or set mode */
int ec_isb_ModeOp(void * stat, ECOpcode op)
{
    int i, j;
    _isbECState *state=(_isbECState *)stat;
    if (state == NULL)
        return 1;
    switch (op) {
    case (EC_COEFFS_ZERO):      /* Zero coeffs of filters */
        {
            for (i = 0; i < state->numSegments; i++)
                for (j = 0; j < state->numSubbands; j++)
                    state->ppAdaptiveCoefs[i][j].re = state->ppAdaptiveCoefs[i][j].im =
                    state->ppFixedCoefs[i][j].re = state->ppFixedCoefs[i][j].im = 0;
        }
        break;
    case(EC_ADAPTATION_ENABLE): /* Enable adaptation */
        if (!(state->mode & 1))
            ippsSubbandControllerReset_EC_16s(state->controller);
        state->mode |= 1;
        break;
    case(EC_ADAPTATION_ENABLE_LITE): /* Enable adaptation */
        if (!(state->mode & 1))
            ippsSubbandControllerReset_EC_16s(state->controller);
        state->mode |= 9; /* Enable adaptation + no constrain */
        break;
    case(EC_ADAPTATION_DISABLE): /* Disable adaptation */
        state->mode &= ~1;
        break;
    case(EC_ADAPTATION_DISABLE_LITE): /* Disable adaptation */
        state->mode &= ~8;
        break;
    case(EC_NLP_ENABLE):    /* Enable NLP */
        state->mode |= 2;
        break;
    case(EC_NLP_DISABLE):   /* Disable NLP */
        state->mode &= ~2;
        break;
    case(EC_TD_ENABLE):     /* Enable ToneDisabler */
        state->mode |= 4;
        break;
    case(EC_TD_DISABLE):    /* Disable ToneDisabler */
        state->mode &= ~4;
        break;
    case(EC_AH_ENABLE):     /* Enable howling control */
        //case(EC_AH_ENABLE1):     /* Enable howling control */
        ah_SetMode_int(state->ah,AH_ENABLE1);// set anti-howling on
        state->mode |= 16;
        break;
        //case(EC_AH_ENABLE2):     /* Enable howling control */
        //    ah_SetMode_int(state->ah,AH_ENABLE2);// set anti-howling on
        //    state->mode |= 32;
        //    break;
    case(EC_AH_DISABLE):    /* Disable howling control */
        ah_SetMode_int(state->ah,AH_DISABLE);// set anti-howling off
        state->mode &= ~16;
        state->mode &= ~32;
        break;
    default:
        break;
    }

    return 0;
}


static void isb_ownConstrainCoeff(_isbECState *state, int subbandNum){
    int k,s;
    LOCAL_ALIGN_ARRAY(32,Ipp32s,bsrc,SB_SUBBANDS * 4,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32s,bsrc1,SB_SUBBANDS * 4,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32s,bdst,SB_FFT_LEN,state);
    ippsFFTInv_CCSToR_32s_Sfs((Ipp32s *)(state->ppAdaptiveCoefs[subbandNum]), bdst, state->spec_fft32, 0, state->pBuf);
    ippsZero_32s(bsrc1,SB_SUBBANDS * 4);
    for (k = 0; k < SB_FRAME_SIZE; k++) {
        bsrc1[2 * k] = SB_FFT_LEN * bdst[k];
    }

    ippsFFTFwd_CToC_32sc_Sfs((const Ipp32sc *)bsrc1, (Ipp32sc *)bsrc, state->spec_fftC, 0, state->pBuf);

    for (s = 0; s < SB_SUBBANDS; s++) {
        state->ppAdaptiveCoefs[subbandNum][s].re = bsrc[2 * s];
        state->ppAdaptiveCoefs[subbandNum][s].im = bsrc[2 * s + 1];
    }
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bdst,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bsrc1,SB_SUBBANDS * 4,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s,bsrc,SB_SUBBANDS * 4,state);

}
/* Process one frame */

//static int isignal_calc(Ipp16s *srcDst, int len, int cmul){
//    int j,r;
//    for (j = 0; j < len; j++) {
//        r = (srcDst[j] * cmul)>>14;
//        srcDst[j] = S16_SAT(r);
//    }
//    return 0;
//}
static int processFrame(_isbECState *state, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout)
{
    int i;
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,fira_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32sc,firf_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY(32,Ipp32s_EC_Sfs,pStepSize,SB_SUBBANDS*2,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,pTimeFiltOut,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,pTimeError,SB_FFT_LEN,state);
    Ipp32sc* pSegment; /* Pointer to segment of input history */
    int      iSegment;
    int      numSegments;
    Ipp16s  *pRinHistory;
    int      windowLen;
    int      frameSize;
#ifdef AEC_TAB_TUNE
    char pString[1024];
#endif

    if ((state == NULL) || (rin==NULL) || (sin==NULL) || (sout==NULL))
        return 1;
    numSegments = state->numSegments;
    pRinHistory = state->pRinHistory;
    windowLen = state->windowLen;
    frameSize = state->frameSize;

    /* update receive-in subband history buffer */
    pSegment = state->ppRinSubbandHistory[state->numSegments-1];
    for(iSegment = state->numSegments-1; iSegment > 0; iSegment--){
        state->ppRinSubbandHistory[iSegment] = state->ppRinSubbandHistory[iSegment-1];
    }
    state->ppRinSubbandHistory[0] = pSegment;

    /* update receive-in history buffer */
    ippsMove_16s(&pRinHistory[frameSize],pRinHistory,windowLen - frameSize);
    ippsMove_16s(rin,&pRinHistory[windowLen - frameSize],frameSize);
    /* Apply FFT to receive-in signal */
    ippsFFTFwd_RToCCS_16s32s_Sfs(pRinHistory, (Ipp32s *)(state->ppRinSubbandHistory[0]), state->spec_fft, 0, state->pBuf);

    /* Do filtering with fixed coeffs */
    ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppFixedCoefs, firf_out,
        numSegments, state->numSubbands, F_SF);

    /* Get fixed filter output in time domain: apply inverse FFT */
    ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)firf_out, pTimeFiltOut, state->spec_fft, 0, state->pBuf);

    /*set to zero the first half of errors*/
    ippsZero_16s(pTimeError, SB_FRAME_SIZE);
    /* Subtract  adaptive filter output from microphone signal*/
    for (i = 0; i < SB_FRAME_SIZE; i++) {
        pTimeError[SB_FRAME_SIZE + i] = SAT_32S_16S(sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i]);
        sout[i] = pTimeError[SB_FRAME_SIZE + i];
    }
    if (ADAPTATION_ENABLED ){ // Adaptation Enabled && (!state->ah->dtFlag)
        /* Get fixed filter error in frequency domain */
        ippsFFTFwd_RToCCS_16s32s_Sfs(pTimeError, (Ipp32s *)firf_err, state->spec_fft, 0, state->pBuf);
        /* Do filtering with adaptive coeffs */
        ippsFIRSubband_EC_32sc_Sfs(state->ppRinSubbandHistory, state->ppAdaptiveCoefs, fira_out,
            numSegments, state->numSubbands, F_SF);
        /* Get adaptive filter output in time domain */
        ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)fira_out, pTimeFiltOut, state->spec_fft, 0, state->pBuf);
        /* Subtract  adaptive filter output from microphone signal*/
        for (i = 0; i < SB_FRAME_SIZE; i++) {
            pTimeError[SB_FRAME_SIZE + i] = SAT_32S_16S(sin[i] - pTimeFiltOut[SB_FRAME_SIZE + i]);
        }
        ippsFFTFwd_RToCCS_16s32s_Sfs(pTimeError, (Ipp32s *)fira_err, state->spec_fft, 0, state->pBuf);
        /* Update subband controller */
        ippsSubbandControllerUpdate_EC_16s(rin, sin, (const Ipp32sc **)(state->ppRinSubbandHistory),
            0, pStepSize, state->controller);
        /* Update adaptive coeffs */
        ippsFIRSubbandCoeffUpdate_EC_32sc_I(pStepSize, (const Ipp32sc **)(state->ppRinSubbandHistory), fira_err,
            state->ppAdaptiveCoefs, numSegments, state->numSubbands, F_SF);
        if(state->mode & 8){ /* lite */
            /* Constrain coeffs each 10th frame. */
            if(!(state->constrainSubbandNum)) {
                for (i = 0; i < numSegments; i++) {
                    isb_ownConstrainCoeff(state,i);
                }
            }
            state->constrainSubbandNum++;
            if(state->constrainSubbandNum >= 10) state->constrainSubbandNum=0;
        }else{
            /* Constrain coeffs. */
            for (i = 0; i < numSegments; i++) {
                isb_ownConstrainCoeff(state,i);
            }
        }
        /* Apply subband controller */
        ippsSubbandController_EC_16s(fira_err, firf_err, state->ppAdaptiveCoefs, state->ppFixedCoefs,
            &state->sgain,state->controller);
    }

    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,pTimeError,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,pTimeFiltOut,SB_FFT_LEN,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32s_EC_Sfs,pStepSize,SB_SUBBANDS*2,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_out,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,firf_err,SB_SUBBANDS,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp32sc,fira_err,SB_SUBBANDS,state);

    return 0;
}
#ifdef AEC_SB_8ms_interface
int ec_isb_ProcessFrame(void * stat, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout)
{
    _isbECState *state=(_isbECState *)stat;
    int     ret=0,nlpOff=0;
    //state->ah->Mem=state->Mem;
    state->ah->Mem.base=state->Mem.base;
    state->ah->Mem.CurPtr=state->Mem.CurPtr;
    state->ah->Mem.VecPtr=state->Mem.VecPtr;
    state->ah->Mem.offset=state->Mem.offset;
    if (TD_ENABLED){
        nlpOff=toneDisabler(rin, sin, state->ah->samplingRate, SB_FRAME_SIZE,
            &state->ah->td);
    }
    if(nlpOff){
        ippsCopy_16s(sin,sout,SB_FRAME_SIZE);
    }else{
        ret=processFrame(state, rin, sin, sout);
        if (ret)
            return ret;
        //if (AH_ENABLED || AH_ENABLED3) {
        //    if (AH_ENABLED) {
        //        formFFTin(state->ah,sout,SB_FRAME_SIZE);
        //        shiftupFft(state->ah,sout,SB_FRAME_SIZE);
        //    } else if (AH_ENABLED3) {
        //        ippsCopy_16s(sout,sout2,SB_FRAME_SIZE);
        //        ippsCopy_16s(sout,sout2+SB_FRAME_SIZE,FRAME_SIZE_10ms-SB_FRAME_SIZE);
        //        shiftupCos(state->shiftupCos, sout2, sout2);
        //        ippsCopy_16s(sout2,sout,SB_FRAME_SIZE);
        //    }
        //}
        cnr(state->ah,rin,sin,sout,SB_FRAME_SIZE,state->nlp,state->cng,23,SB_OFF);
    }
    return ret;
}
#else
static void put_frame(Ipp16s *histSout,int histSoutLen,int *histSoutCurLen,
                        Ipp16s *sout,int len){
    if(((*histSoutCurLen)+len) <= histSoutLen){
        ippsCopy_16s(sout,histSout+(*histSoutCurLen),len);
        (*histSoutCurLen)=((*histSoutCurLen)+len);
        return;
    }else{
        if((*histSoutCurLen) < histSoutLen){
            ippsMove_16s(histSout+(len-(histSoutLen-(*histSoutCurLen))),histSout,
                (*histSoutCurLen)-(len-(histSoutLen-(*histSoutCurLen))));
            ippsCopy_16s(sout,histSout+histSoutLen-len,len);
            (*histSoutCurLen)=histSoutLen;
            return;
        }
    }
    if(histSoutLen>len){
        ippsMove_16s(histSout+len,histSout,
            histSoutLen-len);
    }
    ippsCopy_16s(sout,histSout+histSoutLen-len,
        len);
    return;
}
static int get_frame(Ipp16s *histSout,int *histSoutCurLen,
                        Ipp16s *sout,int len){
    if((*histSoutCurLen) >= len){
        ippsCopy_16s(histSout,sout,len);
        (*histSoutCurLen)=((*histSoutCurLen)-len);
        if (*histSoutCurLen) {
            ippsMove_16s(histSout+len,histSout,*histSoutCurLen);
        }
        return 1;
    }
    return 0;
}
int ec_isb_ProcessFrame(void * stat, Ipp16s *rin, Ipp16s *sin, Ipp16s *sout)
{
    _isbECState *state=(_isbECState *)stat;
    int     ret=0,nlpOff=0;
    LOCAL_ALIGN_ARRAY(32,Ipp16s,rin_sb,SB_FRAME_SIZE,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,sin_sb,SB_FRAME_SIZE,state);
    LOCAL_ALIGN_ARRAY(32,Ipp16s,sout_sb,SB_FRAME_SIZE,state);
    //state->ah->Mem=state->Mem;
    state->ah->Mem.base=state->Mem.base;
    state->ah->Mem.CurPtr=state->Mem.CurPtr;
    state->ah->Mem.VecPtr=state->Mem.VecPtr;
    state->ah->Mem.offset=state->Mem.offset;
    put_frame(state->instackRin,state->instackLenRin,&state->instackCurLenRin,
        rin,FRAME_SIZE_10ms);
    put_frame(state->instackSin,state->instackLenSin,&state->instackCurLenSin,
        sin,FRAME_SIZE_10ms);
    while (get_frame(state->instackRin,&state->instackCurLenRin,rin_sb,SB_FRAME_SIZE)) {
        get_frame(state->instackSin,&state->instackCurLenSin,sin_sb,SB_FRAME_SIZE);
        if (TD_ENABLED){
            nlpOff=toneDisabler(rin_sb, sin_sb, state->ah->samplingRate, SB_FRAME_SIZE,
                &state->ah->td);
        }
        if(nlpOff){
            ippsCopy_16s(sin_sb,sout_sb,SB_FRAME_SIZE);
        }else{
            ret=processFrame(state, rin_sb, sin_sb, sout_sb);
            if (ret)
                break;

            cnr(state->ah,rin_sb,sin_sb,sout_sb,SB_FRAME_SIZE,state->nlp,state->cng,23,SB_OFF);
        }
        put_frame(state->instackSout,state->instackLenSout,&state->instackCurLenSout,
            sout_sb,SB_FRAME_SIZE);
    }
    get_frame(state->instackSout,&state->instackCurLenSout,sout,FRAME_SIZE_10ms);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,sout_sb,SB_FRAME_SIZE,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,sin_sb,SB_FRAME_SIZE,state);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,rin_sb,SB_FRAME_SIZE,state);
    return ret;
}
#endif
