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
//  Purpose: NLP
//
*/

#include <ipps.h>
#include <ippsc.h>
#include "nlp_api.h"
#define E_COEFF (((1<<14) * 981 / 1000))
#define AE_COEFF ((1<<14)-E_COEFF)

void initNLP(nlpState *state){
    state->noiseAdapted=3000;
    state->si_pwr=0;
    state->sil_pwr=0;
    state->srin_pwr=state->sin_pwr2=0;
    state->suppStop=0;
}
void soutProc(nlpState *state,Ipp16s *h,Ipp16s *inout,Ipp16s *rin,Ipp16s *sin,
                    int coef,Ipp32s noisePwr){
    Ipp64s   sum,sumi,sumr;
    Ipp32s   s_out_pwr=0,i,l,s_in_pwr,s_rin_pwr,attn;
    Ipp16s   *psout,*psin,*psh,*psr;
    for (i=0;i<(1<<(MASKING_FRAME_ORDER-3));i++) {
        psr=&rin[i*(MASKING_FRAME_SIZE>>1)];
        psh=&h[i*(MASKING_FRAME_SIZE>>1)];
        psout=&inout[i*(MASKING_FRAME_SIZE>>1)];
        psin=&sin[i*(MASKING_FRAME_SIZE>>1)];
        for (sumi=sumr=sum=0,l=0;l<(MASKING_FRAME_SIZE>>1);l++) {
            sum+=psh[l]*psh[l];
            sumi+=psin[l]*psin[l];
            sumr+=psr[l]*psr[l];
        }
        s_out_pwr=(Ipp32s)(sum>>(MASKING_FRAME_ORDER-1));
        s_in_pwr=(Ipp32s)(sumi>>(MASKING_FRAME_ORDER-1));
        s_rin_pwr=(Ipp32s)(sumr>>(MASKING_FRAME_ORDER-1));
        state->si_pwr = (Ipp32s)(((Ipp64s)16302* (Ipp64s)state->si_pwr +
            (Ipp64s)82 * (Ipp64s)s_in_pwr)>>14);
        state->sil_pwr = (Ipp32s)(((Ipp64s)16302* (Ipp64s)state->sil_pwr +
            (Ipp64s)82 * (Ipp64s)s_out_pwr)>>14);
        state->srin_pwr = (Ipp32s)(((Ipp64s)E_COEFF* (Ipp64s)state->srin_pwr +
            (Ipp64s)AE_COEFF * (Ipp64s)s_rin_pwr)>>14);
        state->sin_pwr2 = (Ipp32s)(((Ipp64s)E_COEFF* (Ipp64s)state->sin_pwr2 +
            (Ipp64s)AE_COEFF * (Ipp64s)s_in_pwr)>>14);

        if (
            ((Ipp64s)state->sin_pwr2*10 > (Ipp64s)state->srin_pwr*15) &&
            ((Ipp64s)state->sin_pwr2*10>(Ipp64s)noisePwr*12)
            ) {
                state->suppStop=(1<<11);
            }
        if (state->suppStop) {
            --state->suppStop;
            if(((Ipp64s)state->srin_pwr*10>(Ipp64s)state->sin_pwr2*30)&&
                ((Ipp64s)state->sin_pwr2*10>(Ipp64s)noisePwr*12)){
                    attn=(Ipp32s)((Ipp64s)state->sin_pwr2*(1<<14)/(state->srin_pwr+1))
                        +state->suppStop
                        ;
                    if(attn<(1<<14))
                        ippsMulC_NR_16s_ISfs((Ipp16s)attn,psout,MASKING_FRAME_SIZE>>1,14);
                }
        }else
            if(((Ipp64s)state->srin_pwr*10>(Ipp64s)state->sin_pwr2*30)&&
                ((Ipp64s)state->sin_pwr2*10>(Ipp64s)noisePwr*12)){
                    attn=(Ipp32s)((Ipp64s)state->sin_pwr2*(1<<14)/(state->srin_pwr+1));
                    if(attn<(1<<14))
                        ippsMulC_NR_16s_ISfs((Ipp16s)attn,psout,MASKING_FRAME_SIZE>>1,14);
                }

        else{
            if((Ipp64s)state->si_pwr*10>(Ipp64s)state->sil_pwr*coef){
                ippsMulC_NR_16s_ISfs((Ipp16s)((Ipp64s)state->sil_pwr*(1<<8)/state->si_pwr),
                    psout,MASKING_FRAME_SIZE>>1,8);
            }
        }
        {
            if(((Ipp64s)state->si_pwr*10<(Ipp64s)noisePwr*30) && state->noiseAdapted){
                ippsMulC_NR_16s_ISfs(1,psout,MASKING_FRAME_SIZE>>1,4);
            }
        }
        if (state->noiseAdapted)
            --state->noiseAdapted;
    }
}
