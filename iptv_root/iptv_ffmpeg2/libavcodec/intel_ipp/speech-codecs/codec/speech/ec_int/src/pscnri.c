/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the tems of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the tems of that agreement.
//          Copyright(c)  2005-2007 Intel Corporation. All Rights Reserved.
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
//  Purpose: noise reduction
//
*/

#include <stdio.h>
#include <ipps.h>
#include <ippsc.h>
#include "_ippnr_int.h"
static void _ippsMagSquared_32sc64s(Ipp32sc *src,Ipp64s *dst,int len){
    int j;
    for (j = 0; j < len; j++) {
        dst[j] = SCALE(((Ipp64s)src[j].re*src[j].re+(Ipp64s)src[j].im*src[j].im),PSD_SF);
    }
    return;
}
int _ippsSubbandNoiseReduceGetSize_EC_16s(int *size)
{
    int specSize2,bufSize2,initSize2,gsize;
    ippsFFTGetSize_R_16s32s(FFT_ORDER7, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize2, &initSize2, &bufSize2);
    gsize = initSize2;
    if (gsize < bufSize2)
        gsize = bufSize2;
    *size = ALIGN(sizeof(IppsSubbandNoise_Reduce_AEC_16s))+
        ALIGN(specSize2)+ALIGN(gsize);
    return 0;
}
IppStatus _ippsSubbandNoiseReduceInit_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state){
    int  bufSize2, specSize2, initSize2,gsize;
    IppStatus status=ippStsNoErr;
    Ipp8u *pInit2,*pInit = (Ipp8u *)state;
    pInit += ALIGN(sizeof(IppsSubbandNoise_Reduce_AEC_16s));
    ippsZero_64s(state->noiseCurMagn,AEC_FFT_ORDER7_LEN);
    ippsZero_64s(state->curMagn,AEC_FFT_ORDER7_LEN);
    ippsZero_64s(state->coeffs,AEC_FFT_ORDER7_LEN);
    ippsZero_32s(state->cfs,AEC_FFT_ORDER7_LEN);
    ippsZero_16s(state->histSout,FFT_ORDER10_SRC_LEN);
    status=ippsFFTGetSize_R_16s32s(FFT_ORDER7, IPP_FFT_DIV_INV_BY_N, ippAlgHintAccurate,
        &specSize2, &initSize2, &bufSize2);
    gsize = initSize2;
    if (gsize < bufSize2)
        gsize = bufSize2;
    pInit2 = pInit;
    pInit += ALIGN(specSize2);
    status=ippsFFTInit_R_16s32s(&(state->FFTspec7), FFT_ORDER7, IPP_FFT_DIV_INV_BY_N,
        ippAlgHintAccurate, pInit2, pInit);
    state->pBuf=pInit;
    pInit += ALIGN(gsize);
    state->histSoutCurLen=0;
    state->histSoutLen = FFT_ORDER10_SRC_LEN;
    state->ouTstableSignal=0;
    state->ouTstabThresh=IPP_AEC_STAB_THRESH;
    state->ouTwbNoisePwr=0;
    state->ouTtrueNoise=0;
    state->ouTorder6Pwr=0;
    state->scfs=0;
    state->nsi_pwr=0;
    state->portion=0;
    state->dpwr=state->trueNoise=0;
    state->freq[0]=AEC_FFT_ORDER7_LEN/5+1;
    state->freq[1]=state->freq[0]*3+6;
    return status;
}
static void saveHistOut(Ipp16s *histSout,int histSoutLen,int *histSoutCurLen,
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
    ippsCopy_16s(sout,histSout+histSoutLen-len,len);
    return;
}
static IppStatus cr(const Ipp16s* pSrc, int lenSrc, Ipp32s* pDst)
{
    int    i;
    Ipp32s sum=0;
    for(i=0;i<lenSrc-1;i++){
        if(pSrc[i]*pSrc[i+1]<0)
            sum++;
    }
    *pDst = sum;
    return ippStsNoErr;
}
static void rollSumMed(Ipp64s *src,int len,int width,Ipp64s *med,Ipp64s *max,Ipp64s *total){
    int    i;
    Ipp64s sum=0,lmed,lmax,ltotal;
    if (len<=width) {
        width=len-1;
    }
    for (i=0;i<width;i++) {
        sum+=src[i];
    }
    lmax=lmed=sum;
    ltotal=sum;
    for (i=0;i<len-width;i++) {
        sum=sum+src[width+i]-src[i];
        ltotal+=src[width+i];
        lmed+=sum;
        if (lmax<sum) {
            lmax=sum;
        }
    }
    *med=lmed/(len-width);
    *max=lmax;
    *total=ltotal;
}
#define IPP_AEC_ROLL_WDTH 4
#define IPP_AEC_NDIAP 3
static int nmnr(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp16s *src,Ipp64s *noiseMagn ){
    Ipp64s   disp[3],noise_disp;
    Ipp64s   medRol[3],maxRol[3],*pMagn;
    int      flen=AEC_FFT_ORDER7_LEN,fsign,rat[3];
    int      ret=0,i;
    int      diap[3];
    diap[0]=state->freq[0];
    diap[1]=state->freq[1]-state->freq[0];
    diap[2]=flen-state->freq[1];
    pMagn=state->curMagn;
    ippsFFTFwd_RToCCS_16s32s_Sfs((const Ipp16s *)src,
        (Ipp32s *)state->spectrum,state->FFTspec7,-8,state->pBuf);
    _ippsMagSquared_32sc64s(state->spectrum,state->curMagn,flen);
    if (noiseMagn) {
        ippsCopy_64s(state->curMagn,noiseMagn,AEC_FFT_ORDER7_LEN);
    }
    for (i=0;i<IPP_AEC_NDIAP;i++) {
        rollSumMed(pMagn,diap[i],IPP_AEC_ROLL_WDTH,&medRol[i],&maxRol[i],&disp[i]);
        medRol[i]++;
        disp[i]++;
        disp[i]/=diap[i];
        rat[i]=(int)(maxRol[i]/medRol[i]);
        pMagn+=diap[i];
    }
    noise_disp=((disp[0]+disp[2])*15)>>4;
    if(
        (noise_disp>disp[1])
        ){
            ret= 1;
        }
    if(
        (rat[0]>2 || rat[1]>2 || rat[2]>2)
        ){
            ret= 0 ;
        }
    cr((const Ipp16s *)src,FFT_ORDER7_SRC_LEN,&fsign);
    if (fsign>42) {
        ret=0;
    }
    if (fsign<6) {
            ret=1;
    }
    return ret;
}
static void noiseDetect(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp64s *noiseMagn,int *flag,int *noisePwr)
{
  Ipp16s  *longIn;
  Ipp16s  *shortIn;
  Ipp32s   longIn_pwr=1,shortIn_pwr=1,t,i,curLen;
  Ipp64s   spwr=0;
  Ipp64s  *noiseCurMagn=state->noiseCurMagn;
    if (noiseMagn) {
        noiseCurMagn=noiseMagn;
    }
    *flag=0;
    curLen=state->histSoutCurLen;
    shortIn = state->histSout+curLen-FFT_ORDER7_SRC_LEN;
    if(curLen<FFT_ORDER10_SRC_LEN){
        for (i=0;i<curLen;i++) {
            spwr += state->histSout[i] * state->histSout[i];
        }
        spwr/=curLen;
        if(!spwr)
            spwr=1;
        longIn_pwr=shortIn_pwr=(Ipp32s)spwr;
    }
    if(!shortIn_pwr)
        shortIn_pwr=1;
    if(curLen>=FFT_ORDER10_SRC_LEN){
        longIn = state->histSout+curLen-FFT_ORDER10_SRC_LEN;
        ippsDotProd_16s32s_Sfs(longIn,longIn,FFT_ORDER10_SRC_LEN,
            &longIn_pwr,FFT_ORDER10);
        if(!longIn_pwr)
            longIn_pwr=1;
    }
    if(curLen>=FFT_ORDER7_SRC_LEN){
        if(nmnr(state,shortIn,noiseCurMagn)){
            if (!(state->ouTstableSignal)){
                (state->ouTorder6Pwr)=shortIn_pwr;
            }
            if (
                (((Ipp64s)(state->ouTorder6Pwr)*3)>=(Ipp64s)shortIn_pwr) &&
                ((Ipp64s)(state->ouTorder6Pwr)<=((Ipp64s)shortIn_pwr*3))
                )
            {
                ++(state->ouTstableSignal);
                if((state->ouTstableSignal)==(state->ouTstabThresh)){
                    if((state->ouTtrueNoise) &&
                        ((state->ouTstabThresh)==(IPP_AEC_STAB_THRESH)) &&
                        ((state->ouTwbNoisePwr)*14<((Ipp64s)longIn_pwr*10))
                        )
                    {
                        t=longIn_pwr/(state->ouTwbNoisePwr);
                        t=t>10?10:t;
                        (state->ouTstabThresh)*=t;
                    }else{
                        *flag=1;
                        (state->ouTtrueNoise)=1;
                        (state->ouTstabThresh)=IPP_AEC_STAB_THRESH;
                        (state->ouTwbNoisePwr)=longIn_pwr;
                        (state->ouTstableSignal)=0;
                    }
                }
            }else{
                (state->ouTstableSignal)=0;
                (state->ouTstabThresh)=IPP_AEC_STAB_THRESH;
            }
        }else{
            (state->ouTstableSignal)=0;
            (state->ouTstabThresh)=IPP_AEC_STAB_THRESH;
        }
    }
    *noisePwr=state->ouTwbNoisePwr;
}
#define ND_INIT_SIZE (MASKING_FRAME_SIZE)
IppStatus _ippsSubbandNoiseDetect_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp16s  *sin,
                          int len, Ipp64s *noiseCurMagn,int *dstFlag,int *noisePwr)
{
    int      i,flag=0;
    *dstFlag=0;
    if (!state->trueNoise && len>=ND_INIT_SIZE) {
        for (i=ND_INIT_SIZE;i<=len;i+=ND_INIT_SIZE) {// find intensively at the beginning
                saveHistOut(state->histSout,state->histSoutLen,&state->histSoutCurLen,
                    &sin[i-ND_INIT_SIZE],ND_INIT_SIZE);
                noiseDetect(state,state->magn,&flag,noisePwr);
                if (flag) {
                    state->trueNoise=1;
                    *dstFlag=1;
                    ippsCopy_64s(state->magn,noiseCurMagn,AEC_FFT_ORDER7_LEN);
                }
        }
        i-=ND_INIT_SIZE;
        if (len-i) {
            saveHistOut(state->histSout,state->histSoutLen,&state->histSoutCurLen,
                &sin[i],len-i);
            noiseDetect(state,state->magn,&flag,noisePwr);
            if (flag) {
                state->trueNoise=1;
                *dstFlag=1;
                ippsCopy_64s(state->magn,noiseCurMagn,AEC_FFT_ORDER7_LEN);
            }
        }
    }else{
        for (i=ND_INIT_SIZE;i<=len;i+=ND_INIT_SIZE) {
            saveHistOut(state->histSout,state->histSoutLen,&state->histSoutCurLen,
                &sin[i-ND_INIT_SIZE],ND_INIT_SIZE);
            state->portion+=ND_INIT_SIZE;
            if (state->portion==NR_TEST_PERIOD) {
                noiseDetect(state,state->magn,&flag,noisePwr);
                if (flag) {
                    state->trueNoise=1;
                    *dstFlag=1;
                    ippsCopy_64s(state->magn,noiseCurMagn,AEC_FFT_ORDER7_LEN);
                }
                state->portion=0;
            }
        }
        i-=ND_INIT_SIZE;
         if (len-i) {
            saveHistOut(state->histSout,state->histSoutLen,&state->histSoutCurLen,
            &sin[i],len-i);
            state->portion+=len-i;
        }
    }
    return ippStsNoErr;
}
#define NS_MAX_DIFF_ORDER   8
#define NS_MAX_DIFF         (1<<NS_MAX_DIFF_ORDER)
#define NS_ONE_ORDER        14
#define NS_ONE     (1<<NS_ONE_ORDER)
#define NS_COEFF   (NS_ONE * 880 / 1000)
#define ANS_COEFF  (NS_ONE-NS_COEFF)
#define NS_COEFF_SCFS   (NS_ONE * 980 / 1000)
#define ANS_COEFF_SCFS  (NS_ONE-NS_COEFF_SCFS)
IppStatus _ippsSubbandNoiseSub_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp64s *noiseMagn,Ipp16s *sout,
                                     int len){
    int      flen=AEC_FFT_ORDER7_LEN;
    int      i,cfs,scfs=0;
    Ipp16s  *src=state->histSout+state->histSoutCurLen-FFT_ORDER7_SRC_LEN;
    Ipp64s  *noiseCurMagn=state->noiseCurMagn;
    if (state->histSoutCurLen>=FFT_ORDER7_SRC_LEN) {
        if (noiseMagn) {
            noiseCurMagn=noiseMagn;
        }
        ippsFFTFwd_RToCCS_16s32s_Sfs((const Ipp16s *)src,(Ipp32s *)state->spectrum,
            state->FFTspec7,-8,state->pBuf);
        _ippsMagSquared_32sc64s(state->spectrum,state->curMagn,flen);
        for (i=0;i<AEC_FFT_ORDER7_LEN;i++) {
            state->coeffs[i]=state->curMagn[i]*(1<<(NS_ONE_ORDER-NS_MAX_DIFF_ORDER))/(noiseCurMagn[i]+1);
            if (state->coeffs[i]>(IPP_MAX_16S)*(1<<(NS_ONE_ORDER-NS_MAX_DIFF_ORDER))) {
                state->coeffs[i]=(IPP_MAX_16S)*(1<<(NS_ONE_ORDER-NS_MAX_DIFF_ORDER));
            }
            state->cfs[i] = (Ipp32s)(((Ipp64s)ANS_COEFF*(Ipp64s)state->cfs[i] +
                (Ipp64s)NS_COEFF *state->coeffs[i])>>NS_ONE_ORDER);
            if (state->cfs[i]>(1<<NS_ONE_ORDER)) {
                scfs+=(1<<NS_ONE_ORDER);
            }else{
                scfs+=state->cfs[i];
            }
        }
        scfs/=AEC_FFT_ORDER7_LEN;
        state->scfs = (Ipp32s)(((Ipp64s)ANS_COEFF_SCFS*(Ipp64s)state->scfs +
            (Ipp64s)NS_COEFF_SCFS *scfs)>>NS_ONE_ORDER);
        cfs=state->scfs;
        //printf("%d \n",(int)cfs);
        if (cfs<(1<<NS_ONE_ORDER)) {
            for (i=0;i<AEC_FFT_ORDER7_LEN;i++) {
                    state->spectrum[i].re=(Ipp32s)(((Ipp64s)state->spectrum[i].re*cfs)>>(NS_ONE_ORDER));
                    state->spectrum[i].im=(Ipp32s)(((Ipp64s)state->spectrum[i].im*cfs)>>(NS_ONE_ORDER));
            }
        }
        ippsFFTInv_CCSToR_32s16s_Sfs((Ipp32s *)state->spectrum,(Ipp16s *)state->sBuf,
            state->FFTspec7,8,state->pBuf);
        ippsCopy_16s(state->sBuf+FFT_ORDER7_SRC_LEN-len,sout,len);
    }
    return ippStsNoErr;
}
int _ippsSubbandNoiseReduce_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp16s *srcDst,int len){
    int n;
    int flag=0;
    Ipp64s   noiseCurMagn[AEC_FFT_ORDER7_LEN];
    _ippsSubbandNoiseDetect_EC_16s(state,srcDst,len,noiseCurMagn,&flag,&n);
    if (flag) {
        ippsCopy_64s(noiseCurMagn,state->noiseCurMagn,AEC_FFT_ORDER7_LEN);
    }
    _ippsSubbandNoiseSub_EC_16s(state,state->noiseCurMagn,srcDst,len);
    return 0;
}
