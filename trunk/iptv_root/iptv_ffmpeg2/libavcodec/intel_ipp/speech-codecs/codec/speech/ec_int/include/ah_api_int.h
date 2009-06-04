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
//  Purpose: AH header file.
//
*/


#ifndef __AH_API_INT_H__
#define __AH_API_INT_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef struct __ahState_int ahState_int;
typedef struct __shiftupCosState_int shiftupCosState_int;

/* set mode */
int ah_SetMode_int(ahState_int *state, AHOpcode op);

/* Returns size of buffer for state structure */
int ah_GetSize_int(int frameSize, int *size,int freq,int *scratchMemSize);

/* Initializes state structure */
int ah_Init_int(ahState_int *state,int samplingRate,int frameSize,int nlpOff);

/* Process one frame */
AHOpcode ah_ProcessFrame_int(ahState_int *state, Ipp16s *sin, Ipp16s *sout,int frameSize);
void formFFTin(ahState_int *state, Ipp16s *sout, int len);
void shiftupFft(ahState_int *state, Ipp16s *out,int frameSize);
int saveHistRS(ahState_int *state, Ipp16s *rin, Ipp16s *sin);
Ipp16u aec_i_sqrt(Ipp32u x);
Ipp16u aec_i_order(Ipp16u x);
extern Ipp16s IPP_AEC_dBtbl[];
int cnr(ahState_int *state,Ipp16s *rin,Ipp16s *sin,Ipp16s *sout,int len,int nlp,int cng,int c,int off);
int shiftupCos_GetSize(int frameSize, int *size);
int shiftupCos_Init(shiftupCosState_int *state, int frameSize);
IppStatus shiftupCos(shiftupCosState_int *state, Ipp16s *in1, Ipp16s *out1);
#ifdef __cplusplus
}
#endif
#endif /* __AH_API_INT_H__ */
