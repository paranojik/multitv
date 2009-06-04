/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
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
//  Purpose: AEC NR header file.
//
*/


#ifndef __AEC_NR_API_INT_H__
#define __AEC_NR_API_INT_H__
#include <ipps.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __AEC_NR_API_H__
#define FFT_ORDER11         8
#define FFT_ORDER10         10
#define FFT_ORDER6          6
#define AEC_FFT_ORDER11_LEN (((1<<FFT_ORDER11)+2)>>1)
#define AEC_FFT_ORDER10_LEN (((1<<FFT_ORDER10)+2)>>1)
#define AEC_FFT_ORDER6_LEN  (((1<<FFT_ORDER6)+2)>>1)
#define AEC_FFT_ORDER6_LEN2 ((1<<FFT_ORDER6)+2)
#define FFT_ORDER7          7
#define AEC_FFT_ORDER7_LEN  (((1<<FFT_ORDER7)+2)>>1)
#define FFT_ORDER7_SRC_LEN  (1<<FFT_ORDER7)
#define FFT_ORDER10_SRC_LEN (1<<FFT_ORDER10)
#define FFT_ORDER11_SRC_LEN (1<<FFT_ORDER11)
#define FRAME_SIZE_10ms     80
#define SCALE(v, f) (((v) + (1<<((f)-1))) >> (f))
#define PSD_SF 10
#define ALIGN(s) (((s) + 15) & ~15)
#define IPP_AEC_STAB_THRESH 2
#define MASKING_FRAME_ORDER 4
#define MASKING_FRAME_SIZE  (1<<MASKING_FRAME_ORDER)
#define NR_TEST_PERIOD 80
#endif /* __AEC_NR_API_H__ */

typedef struct __nr_int{
    IppsFFTSpec_R_16s32s *FFTspec7;
    Ipp64s   noiseCurMagn[AEC_FFT_ORDER7_LEN];
    Ipp64s   curMagn[AEC_FFT_ORDER7_LEN];
    Ipp64s   coeffs[AEC_FFT_ORDER7_LEN];
    Ipp64s   magn[AEC_FFT_ORDER7_LEN];
    Ipp32sc  spectrum[AEC_FFT_ORDER7_LEN];
    Ipp32s   cfs[AEC_FFT_ORDER7_LEN];
    Ipp32s   portion;
    Ipp16s   sBuf[FFT_ORDER7_SRC_LEN];
    Ipp16s   histSout[FFT_ORDER10_SRC_LEN];
    Ipp8u    *pBuf;
    Ipp32s   ouTstableSignal,
        ouTstabThresh,ouTwbNoisePwr,ouTtrueNoise,
        ouTorder6Pwr,freq[2],histSoutLen,histSoutCurLen,scfs,nsi_pwr,dpwr,trueNoise;
} IppsSubbandNoise_Reduce_AEC_16s;

IppStatus _ippsSubbandNoiseReduce_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *nr,Ipp16s *srcDst,int len );
IppStatus _ippsSubbandNoiseReduceGetSize_EC_16s(int *size);
IppStatus _ippsSubbandNoiseReduceInit_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state);
IppStatus _ippsSubbandNoiseDetect_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp16s *src,int len,
                     Ipp64s *noiseCurMagn,int *flag,int *noisePwr);
IppStatus _ippsSubbandNoiseSub_EC_16s(IppsSubbandNoise_Reduce_AEC_16s *state,Ipp64s *noiseCurMagn,Ipp16s *srcDst,int len);
#ifdef __cplusplus
}
#endif
#endif /* __AEC_NR_API_INT_H__ */
