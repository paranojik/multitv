/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the tems of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the tems of that agreement.
//          Copyright(c) 2006-2007 Intel Corporation. All Rights Reserved.
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
//  Purpose: NLP, NR
//
*/

#include <stdio.h>
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
#define tapsFactor 14
#define COEFF_15 15
Ipp16u aec_i_sqrt(Ipp32u x)
{
    Ipp16u r=0;
    Ipp16u i;
    for(i=IPP_MAX_16U; i>0; i>>=1)
    {
        r = r + i;
        if( ((Ipp32u)(r*r)) > x)
            r = r - i;
    }
    return (Ipp16u)r;
}
Ipp16u aec_i_order(Ipp16u x)
{
    Ipp16u i;
    for(i=0;i<16; i++)
    {
        if(!x)
            break;
        x>>=1;
    }
    return i;
}
static void saveHist(Ipp16s *histSout,int histSoutLen,int *histSoutCurLen,
                        Ipp16s *sout,int len){
    if((*histSoutCurLen) < histSoutLen){
        ippsCopy_16s(sout,histSout+(*histSoutCurLen),len);
        (*histSoutCurLen)=((*histSoutCurLen)+len);
        return;
    }
    if(histSoutLen>len){
        ippsMove_16s(histSout+len,histSout,histSoutLen-len);
    }
    ippsCopy_16s(sout,histSout+histSoutLen-len,len);
    return;
}
static int nna(ahState_int *state,Ipp16s range,Ipp16s *sout,int len){
    ippsRandUniform_Direct_16s(sout,len,-range,range,&state->seed);
    return 1;
}
static int na(ahState_int *state,Ipp16s range,Ipp16s *sout,int len){
    LOCAL_ALIGN_ARRAY(32,Ipp16s,pRnd,MASKING_FRAME_SIZE,state);
    ippsRandUniform_Direct_16s(pRnd,len,-range,range,&state->seed);
    ippsAdd_16s_I(pRnd, sout, len);
    LOCAL_ALIGN_ARRAY_FREE(32,Ipp16s,pRnd,MASKING_FRAME_SIZE,state);
    return 1;
}
int cnr(ahState_int *state,Ipp16s *rin,Ipp16s *sin,Ipp16s *sout,int len,int nlp,int cng,
        int coeff,int off){
    Ipp16s  sqrtNoisePwr=((11*11)>>5);
    int     i,noiseFlag,sinNoiseFlag,outNoisePwr;
    Ipp64s   noiseCurMagn[AEC_FFT_ORDER7_LEN];
    saveHist(state->histSout,state->histSoutLen,&state->histSoutCurLen,sout,len);
    saveHist(state->histSin,state->histLen,&state->histSinCurLen,sin,len);
    saveHist(state->histRin,state->histLen,&state->histRinCurLen,rin,len);
    //_ippsNR_16s(state->nr,sout,len,&noiseFlag);
    _ippsSubbandNoiseDetect_EC_16s(state->nrSin,sin,len,noiseCurMagn,&sinNoiseFlag,&state->sinNoisePwr);
    if ( off ) {
        if ( state->histSoutCurLen-off-len>=0 ) {
            _ippsSubbandNoiseDetect_EC_16s(state->nr,
                state->histSout+state->histSoutCurLen-2*len+off,
                len,noiseCurMagn,&noiseFlag,&outNoisePwr);
            if (sinNoiseFlag) {
                ippsCopy_64s(noiseCurMagn,state->noiseCurMagn,AEC_FFT_ORDER7_LEN);
            }
        }
    }else{
        _ippsSubbandNoiseDetect_EC_16s(state->nr,sout,len,noiseCurMagn,&noiseFlag,&outNoisePwr);
        if (sinNoiseFlag) {
          //  printf("call=%d pwr=%d \n",state->frameNum,state->sinNoisePwr);state->frameNum++;
            ippsCopy_64s(noiseCurMagn,state->noiseCurMagn,AEC_FFT_ORDER7_LEN);
        }
    }
    _ippsSubbandNoiseSub_EC_16s(state->nr,state->noiseCurMagn,sout,len);
    if (nlp==1) {
        if (state->histSoutCurLen-off-len>=0) {
                for (i=0;i<len;i+=MASKING_FRAME_SIZE) {
                    if (state->initHowl>0) {
                        if (cng==1)
                            nna(state,sqrtNoisePwr,&sout[i],MASKING_FRAME_SIZE);
                        else
                            ippsZero_16s(&sout[i], MASKING_FRAME_SIZE);
                        state->initHowl-=MASKING_FRAME_SIZE;
                    }else{
                        soutProc(&state->nlpObj,state->histSout+state->histSoutCurLen-len+i-off,
                            &sout[i], state->histRin+state->histRinCurLen-len+i-off,
                            state->histSin+state->histSinCurLen-len+i-off, coeff,state->sinNoisePwr);
                    }
                }
        }else{
            if (cng==1)
                nna(state,sqrtNoisePwr,sout,len);
            else
                ippsZero_16s(sout, len);
            state->initHowl=(Ipp16s)(state->initHowl-len);
            return 0;
        }
    }
    if (cng==1) {
        na(state,sqrtNoisePwr,sout,len);
    }
    return 0;
}
