/*/////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//          Copyright(c) 2006-2007 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives
//     USC - Unified Speech Codec interface library
//
// By downloading and installing USC codec, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ippEULA.rtf or ippEULA.txt located in the root directory of your Intel(R) IPP
// product installation for more information.
//
//
//
// Purpose: Acoustic Echo Canceller: USC functions.
//
*/

#include <ipps.h>
#include <ippsc.h>
#include "ec_api_int.h"
#include "ecusc_int.h"

#define  SUBBAND_FRAME_SIZE    64
#define  FULLBAND_FRAME_SIZE    8
#define  FASTSBBAND_FRAME_SIZE 44
#define  SB_MAX_LEN_TAIL      200
#define  FB_MAX_LEN_TAIL      200
#define  MAX_BLOCK_SIZE       128

static USC_Status GetInfoSize(Ipp32s *pSize);
static USC_Status GetInfo(USC_Handle handle, USC_EC_Info *pInfo);
static USC_Status NumAlloc(const USC_EC_Option *options, Ipp32s *nbanks);
static USC_Status MemAlloc(const USC_EC_Option *options, USC_MemBank *pBanks);
static USC_Status Init(const USC_EC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle);
static USC_Status Reinit(const USC_EC_Modes *modes, USC_Handle handle );
static USC_Status Control(const USC_EC_Modes *modes, USC_Handle handle );
static USC_Status CancelEcho(USC_Handle handle, Ipp16s *pSin, Ipp16s *pRin, Ipp16s *pSout);

typedef struct {
   ec_GetFrameSizeI_ptr ec_GetFrameSize;
   ec_GetSizeI_ptr ec_GetSize;
   ec_InitI_ptr ec_Init;
   ec_ModeOpI_ptr ec_ModeOp;
   ec_GetInfoI_ptr ec_GetInfo;
   ec_GetSendPathDelayI_ptr ec_GetSendPathDelay;
   ec_ProcessFrameI_ptr ec_ProcessFrame;
   ec_setNRI_ptr ec_setNR;
   ec_setCNGI_ptr ec_setCNG;
} ecint_fun;

typedef struct {
   USC_EC_Algs   algType;
   USC_PCMType   pcmType;
   Ipp32s        echotail;
   USC_AdaptType adapt;
   Ipp32s        zeroCoeff;
   Ipp32s        cng;
   Ipp32s        nlp;
   Ipp32s        td;
   Ipp32s        ah;
   Ipp32s        frameSize;
   ecint_fun     fun;
   int           reserve[3];
} ECINT_Handle_Header;


/* global usc vector table */
USCFUN USC_EC_Fxns USC_ECINT_Fxns=
{
    {
        USC_EC,
        (GetInfoSize_func) GetInfoSize,
        (GetInfo_func)     GetInfo,
        (NumAlloc_func)    NumAlloc,
        (MemAlloc_func)    MemAlloc,
        (Init_func)        Init,
        (Reinit_func)      Reinit,
        (Control_func)     Control
    },
    CancelEcho

};
static USC_PCMType pcmTypesTbl_ECint[2]={
   {8000,16},{16000,16}
};

static USC_Status GetInfoSize(Ipp32s *pSize)
{
    if (pSize == NULL)
        return USC_BadDataPointer;
    *pSize = sizeof(USC_EC_Info);
    return USC_NoError;
}
static USC_Status GetInfo(USC_Handle handle, USC_EC_Info *pInfo)
{
    ECINT_Handle_Header *ecint_header;
    if (pInfo == NULL)
        return USC_BadDataPointer;
    pInfo->name = "IPP_EC_INT";
    if (handle == NULL) {
#ifdef AEC_SB_8ms_interface
        pInfo->params.framesize = SUBBAND_FRAME_SIZE*sizeof(Ipp16s);
#else
        pInfo->params.framesize = FRAME_SIZE_10ms*sizeof(Ipp16s);
#endif
      pInfo->params.algType = EC_SUBBAND;
      pInfo->params.pcmType.sample_frequency = 8000;
      pInfo->params.pcmType.bitPerSample = 16;
      pInfo->params.pcmType.nChannels = 1;
      pInfo->params.echotail = 16;
      pInfo->params.modes.adapt = AD_FULLADAPT;
      pInfo->params.modes.zeroCoeff = 0;
      pInfo->params.modes.nlp = 1;
      pInfo->params.modes.cng = 1;
      pInfo->params.modes.td = 1;
      pInfo->params.modes.ah = 0;         /* AH off*/
    } else {
      ecint_header = (ECINT_Handle_Header*)handle;
      if(ecint_header->algType == EC_SUBBAND) {
        pInfo->params.algType = EC_SUBBAND;
#ifdef AEC_SB_8ms_interface
        pInfo->params.framesize = SUBBAND_FRAME_SIZE*sizeof(Ipp16s);
#else
        pInfo->params.framesize = FRAME_SIZE_10ms*sizeof(Ipp16s);
#endif
      } else if(ecint_header->algType == EC_FULLBAND) {
        pInfo->params.algType = EC_FULLBAND;
        pInfo->params.framesize = FRAME_SIZE_10ms*sizeof(Ipp16s);
      } else {
        pInfo->params.algType = EC_FASTSUBBAND;
        pInfo->params.framesize = FRAME_SIZE_10ms*sizeof(Ipp16s);
      }
      pInfo->params.pcmType.sample_frequency = ecint_header->pcmType.sample_frequency;
      pInfo->params.pcmType.bitPerSample = ecint_header->pcmType.bitPerSample;
      pInfo->params.pcmType.nChannels = ecint_header->pcmType.nChannels;
      pInfo->params.echotail = ecint_header->echotail;
      pInfo->params.modes.adapt = ecint_header->adapt;
      pInfo->params.modes.zeroCoeff = ecint_header->zeroCoeff;
      pInfo->params.modes.nlp = ecint_header->nlp;
      pInfo->params.modes.cng = ecint_header->cng;
      pInfo->params.modes.td = ecint_header->td;
      pInfo->params.modes.ah = ecint_header->ah;
      pInfo->params.nModes = sizeof(USC_EC_Modes)/sizeof(Ipp32s);
    }
    pInfo->nPcmTypes = 2;
    pInfo->pPcmTypesTbl = pcmTypesTbl_ECint;
    return USC_NoError;
}

static USC_Status NumAlloc(const USC_EC_Option *options, Ipp32s *nbanks)
{
   if(options==NULL) return USC_BadDataPointer;
   if(nbanks==NULL) return USC_BadDataPointer;
   *nbanks = 3;
   return USC_NoError;
}
#define ALIGN(s) (((s) + 15) & ~15)
static USC_Status MemAlloc(const USC_EC_Option *options, USC_MemBank *pBanks)
{
   Ipp32s nbytes,nbytes2,scratch_size,taptime_ms;
   IppPCMFrequency freq;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(options->pcmType.bitPerSample != 16) return USC_UnsupportedPCMType;
   switch(options->pcmType.sample_frequency) {
     case 8000:  freq = IPP_PCM_FREQ_8000;  break;
     case 16000: freq = IPP_PCM_FREQ_16000; break;
     default: return USC_UnsupportedPCMType;
   }

   pBanks[0].pMem = NULL;
   pBanks[0].align = 32;
   pBanks[0].memType = USC_OBJECT;
   pBanks[0].memSpaceType = USC_NORMAL;

   pBanks[1].pMem = NULL;
   pBanks[1].align = 32;
   pBanks[1].memType = USC_OBJECT;
   pBanks[1].memSpaceType = USC_NORMAL;

   pBanks[2].pMem = NULL;
   pBanks[2].align = 32;
   pBanks[2].memType = USC_OBJECT;
   pBanks[2].memSpaceType = USC_NORMAL;

   if(options->algType == EC_SUBBAND) {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_isb_GetSize(freq, taptime_ms,&nbytes,&nbytes2,&scratch_size);
   } else if(options->algType == EC_FULLBAND) {
     if((options->echotail > 0) && (options->echotail <= FB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_ifb_GetSize(freq, taptime_ms,&nbytes,&nbytes2,&scratch_size);
   } else {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ec_isbf_GetSize(freq, taptime_ms,&nbytes,&nbytes2,&scratch_size);
   }
   pBanks[0].nbytes = nbytes + ALIGN(sizeof(ECINT_Handle_Header)); /* room for USC header */
   pBanks[1].nbytes = nbytes2;
   pBanks[2].nbytes = scratch_size;
   return USC_NoError;
}

static USC_Status Init(const USC_EC_Option *options, const USC_MemBank *pBanks, USC_Handle *handle)
{
   ECINT_Handle_Header *ecint_header;
   IppPCMFrequency freq;
   Ipp32s taptime_ms;
   void *ahObj;
   void* scratchbuff = NULL;
   USC_Handle *obj_ec;

   if(options==NULL) return USC_BadDataPointer;
   if(pBanks==NULL) return USC_BadDataPointer;
   if(pBanks[0].pMem==NULL) return USC_NotInitialized;
   if(pBanks[0].nbytes<=0) return USC_NotInitialized;
   if(pBanks[1].pMem==NULL) return USC_NotInitialized;
   if(pBanks[1].nbytes<=0) return USC_NotInitialized;
   if(pBanks[2].pMem==NULL) return USC_NotInitialized;
   if(pBanks[2].nbytes<=0) return USC_NotInitialized;
   if(handle==NULL) return USC_InvalidHandler;
   if(options->pcmType.bitPerSample != 16) return USC_UnsupportedPCMType;
   switch(options->pcmType.sample_frequency) {
     case 8000:  freq = IPP_PCM_FREQ_8000;  break;
     case 16000: freq = IPP_PCM_FREQ_16000; break;
     default: return USC_UnsupportedPCMType;
   }
   if(options->pcmType.nChannels<=0)
     return USC_UnsupportedPCMType;
   if(options->echotail<0)
       return USC_UnsupportedEchoTail;
   if(options->modes.zeroCoeff<0)
       return USC_BadArgument;
   if(options->modes.ah<0)
       return USC_BadArgument;
   if(options->modes.cng<0)
       return USC_BadArgument;
   if(options->modes.nlp<0)
       return USC_BadArgument;
   if(options->modes.td<0)
       return USC_BadArgument;

   *handle = (USC_Handle*)pBanks[0].pMem;
   ahObj = (USC_Handle*)pBanks[1].pMem;
   scratchbuff = (void*)pBanks[2].pMem;
   ecint_header = (ECINT_Handle_Header*)*handle;
   obj_ec = (USC_Handle*)((Ipp8s*)*handle + ALIGN(sizeof(ECINT_Handle_Header)));

   ecint_header->algType = options->algType;
   ecint_header->pcmType.sample_frequency = options->pcmType.sample_frequency;
   ecint_header->pcmType.bitPerSample = options->pcmType.bitPerSample;
   ecint_header->pcmType.nChannels = options->pcmType.nChannels;
   if(ecint_header->algType == EC_SUBBAND) {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecint_header->fun.ec_GetFrameSize = (ec_GetFrameSizeI_ptr)ec_isb_GetFrameSize;
     ecint_header->fun.ec_GetSize = (ec_GetSizeI_ptr)ec_isb_GetSize;
     ecint_header->fun.ec_Init = (ec_InitI_ptr)ec_isb_Init;
     ecint_header->fun.ec_ModeOp = (ec_ModeOpI_ptr)ec_isb_ModeOp;
     ecint_header->fun.ec_GetInfo = (ec_GetInfoI_ptr)ec_isb_GetInfo;
     ecint_header->fun.ec_GetSendPathDelay = (ec_GetSendPathDelayI_ptr)ec_isb_GetSendPathDelay;
     ecint_header->fun.ec_ProcessFrame = (ec_ProcessFrameI_ptr)ec_isb_ProcessFrame;

     ecint_header->fun.ec_setNR= (ec_setNRI_ptr)ec_isb_setNR;
     ecint_header->fun.ec_setCNG= (ec_setCNGI_ptr)ec_isb_setCNG;
   } else if(ecint_header->algType == EC_FULLBAND) {
     if((options->echotail > 0) && (options->echotail <= FB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecint_header->fun.ec_GetFrameSize = (ec_GetFrameSizeI_ptr)ec_ifb_GetFrameSize;
     ecint_header->fun.ec_GetSize = (ec_GetSizeI_ptr)ec_ifb_GetSize;
     ecint_header->fun.ec_Init = (ec_InitI_ptr)ec_ifb_Init;
     ecint_header->fun.ec_ModeOp = (ec_ModeOpI_ptr)ec_ifb_ModeOp;
     ecint_header->fun.ec_GetInfo = (ec_GetInfoI_ptr)ec_ifb_GetInfo;
     ecint_header->fun.ec_GetSendPathDelay = (ec_GetSendPathDelayI_ptr)ec_ifb_GetSendPathDelay;
     ecint_header->fun.ec_ProcessFrame = (ec_ProcessFrameI_ptr)ec_ifb_ProcessFrame;

     ecint_header->fun.ec_setNR= (ec_setNRI_ptr)ec_ifb_setNR;
     ecint_header->fun.ec_setCNG= (ec_setCNGI_ptr)ec_ifb_setCNG;
   } else {
     if((options->echotail > 0) && (options->echotail <= SB_MAX_LEN_TAIL)) taptime_ms = options->echotail;
     else return USC_UnsupportedEchoTail;
     ecint_header->fun.ec_GetFrameSize = (ec_GetFrameSizeI_ptr)ec_isbf_GetFrameSize;
     ecint_header->fun.ec_GetSize = (ec_GetSizeI_ptr)ec_isbf_GetSize;
     ecint_header->fun.ec_Init = (ec_InitI_ptr)ec_isbf_Init;
     ecint_header->fun.ec_ModeOp = (ec_ModeOpI_ptr)ec_isbf_ModeOp;
     ecint_header->fun.ec_GetInfo = (ec_GetInfoI_ptr)ec_isbf_GetInfo;
     ecint_header->fun.ec_GetSendPathDelay = (ec_GetSendPathDelayI_ptr)ec_isbf_GetSendPathDelay;
     ecint_header->fun.ec_ProcessFrame = (ec_ProcessFrameI_ptr)ec_isbf_ProcessFrame;

     ecint_header->fun.ec_setNR= (ec_setNRI_ptr)ec_isbf_setNR;
     ecint_header->fun.ec_setCNG= (ec_setCNGI_ptr)ec_isbf_setCNG;
   }
   ecint_header->echotail = taptime_ms;
   ecint_header->adapt = options->modes.adapt;
   ecint_header->zeroCoeff = options->modes.zeroCoeff;
   ecint_header->nlp = options->modes.nlp;
   ecint_header->cng = options->modes.cng;
   ecint_header->td = options->modes.td;
   ecint_header->ah = options->modes.ah;

   if (ecint_header->fun.ec_Init(obj_ec, ahObj,freq, ecint_header->echotail)){
       return USC_NoOperation;
   }
   if(ecint_header->algType == EC_SUBBAND) {
       if (ec_isb_InitBuff(obj_ec, scratchbuff)){
           return USC_NoOperation;
       }
   } else if(ecint_header->algType == EC_FULLBAND) {
       if (ec_ifb_InitBuff(obj_ec, scratchbuff)){
           return USC_NoOperation;
       }
   }else{
       if (ec_isbf_InitBuff(obj_ec, scratchbuff)){
           return USC_NoOperation;
       }
   }
   if(ecint_header->zeroCoeff) ecint_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
   if((ecint_header->adapt == AD_FULLADAPT) || (ecint_header->adapt == AD_LITEADAPT)) {
     if(ecint_header->adapt == AD_FULLADAPT) ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
     else ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE_LITE);
   } else {
     ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
     ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE_LITE);
   }
   if(ecint_header->nlp) ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
   else ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
   if(ecint_header->td) ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
   else ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);

    switch(ecint_header->ah) {
    case 0:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        break;
    case 1:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE);
        break;
//    case 2:
  //      ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE3);
    //    break;
    default:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
    }
   ecint_header->fun.ec_GetFrameSize(IPP_PCM_FREQ_8000,0,&ecint_header->frameSize );
   ecint_header->fun.ec_setNR(obj_ec, ecint_header->nlp);
   ecint_header->fun.ec_setCNG(obj_ec, ecint_header->cng);

   return USC_NoError;
}

static USC_Status Reinit(const USC_EC_Modes *modes, USC_Handle handle )
{
    ECINT_Handle_Header *ecint_header;
    USC_Handle *obj_ec;

    if(modes==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    ecint_header = (ECINT_Handle_Header*)handle;
    obj_ec = (USC_Handle*)((Ipp8s*)handle + ALIGN(sizeof(ECINT_Handle_Header)));
    if(modes->zeroCoeff<0)
        return USC_BadArgument;
    if(modes->ah<0)
        return USC_BadArgument;
    if(modes->cng<0)
        return USC_BadArgument;
    if(modes->nlp<0)
        return USC_BadArgument;
    if(modes->td<0)
        return USC_BadArgument;

    ecint_header->adapt = modes->adapt;
    ecint_header->zeroCoeff = modes->zeroCoeff;
    ecint_header->nlp = modes->nlp;
    ecint_header->cng = modes->cng;
    ecint_header->td = modes->td;
    ecint_header->ah = modes->ah;

    if(ecint_header->zeroCoeff) ecint_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
    if((ecint_header->adapt == AD_FULLADAPT) || (ecint_header->adapt == AD_LITEADAPT)) {
      if(ecint_header->adapt == AD_FULLADAPT) ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
      else ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE_LITE);
    } else {
      ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
      ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE_LITE);
    }
    if(ecint_header->nlp>0) ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
    else ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
    if(ecint_header->td) ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
    else ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);

    switch(ecint_header->ah) {
    case 0:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        break;
    case 1:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE);
        break;
    default:
        ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
    }
    ecint_header->fun.ec_setNR(obj_ec, ecint_header->nlp);
    ecint_header->fun.ec_setCNG(obj_ec, ecint_header->cng);

    return USC_NoError;
}

static USC_Status Control(const USC_EC_Modes *modes, USC_Handle handle )
{

    ECINT_Handle_Header *ecint_header;
    USC_Handle *obj_ec;

    if(modes==NULL) return USC_BadDataPointer;
    if(handle==NULL) return USC_InvalidHandler;

    ecint_header = (ECINT_Handle_Header*)handle;
    obj_ec = (USC_Handle*)((Ipp8s*)handle + ALIGN(sizeof(ECINT_Handle_Header)));
    if(modes->zeroCoeff<0)
        return USC_BadArgument;
    if(modes->ah<0)
        return USC_BadArgument;
    if(modes->cng<0)
        return USC_BadArgument;
    if(modes->nlp<0)
        return USC_BadArgument;
    if(modes->td<0)
        return USC_BadArgument;

    ecint_header->zeroCoeff = modes->zeroCoeff;
    if(ecint_header->zeroCoeff) ecint_header->fun.ec_ModeOp(obj_ec, EC_COEFFS_ZERO);
    if(ecint_header->adapt != modes->adapt) {
      ecint_header->adapt = modes->adapt;
      if((ecint_header->adapt == AD_FULLADAPT) || (ecint_header->adapt == AD_LITEADAPT)) {
        if(ecint_header->adapt == AD_FULLADAPT) ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE);
        else ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_ENABLE_LITE);
      } else {
        ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE);
        ecint_header->fun.ec_ModeOp(obj_ec, EC_ADAPTATION_DISABLE_LITE);
      }
    }
    if(ecint_header->nlp != modes->nlp) {
      ecint_header->nlp = modes->nlp;
      if(ecint_header->nlp) ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_ENABLE);
      else ecint_header->fun.ec_ModeOp(obj_ec, EC_NLP_DISABLE);
    }
    if(ecint_header->td != modes->td) {
      ecint_header->td = modes->td;
      if(ecint_header->td) ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_ENABLE);
      else ecint_header->fun.ec_ModeOp(obj_ec, EC_TD_DISABLE);
    }
    if(ecint_header->ah != modes->ah) {
        ecint_header->ah = modes->ah;
        switch(ecint_header->ah) {
        case 0:
            ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
            break;
        case 1:
            ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_ENABLE);
            break;
        default:
            ecint_header->fun.ec_ModeOp(obj_ec, EC_AH_DISABLE);
        }
    }
    if(ecint_header->nlp != modes->nlp)
        ecint_header->nlp = modes->nlp;
    ecint_header->fun.ec_setNR(obj_ec, modes->nlp);
    if(ecint_header->cng != modes->cng)
        ecint_header->cng = modes->cng;
    ecint_header->fun.ec_setCNG(obj_ec, modes->cng);
    return USC_NoError;
}

static USC_Status CancelEcho(USC_Handle handle, Ipp16s *pSin, Ipp16s *pRin, Ipp16s *pSout)
{
   ECINT_Handle_Header *ecint_header;
   USC_Handle *obj_ec;
   //Ipp32s framesize;
   //Ipp32f r_in_32f_cur[MAX_BLOCK_SIZE];
   //Ipp32f s_in_32f_cur[MAX_BLOCK_SIZE];
   //Ipp32f s_out_32f_cur[MAX_BLOCK_SIZE];

   if(handle==NULL) return USC_InvalidHandler;
   if(pSin==NULL) return USC_BadDataPointer;
   if(pRin==NULL) return USC_BadDataPointer;
   if(pSout==NULL) return USC_BadDataPointer;

   ecint_header = (ECINT_Handle_Header*)handle;
   obj_ec = (USC_Handle*)((Ipp8s*)handle + ALIGN(sizeof(ECINT_Handle_Header)));
   //if(ecint_header->algType == EC_SUBBAND) framesize = SUBBAND_FRAME_SIZE;
   //else if(ecint_header->algType == EC_FULLBAND) framesize = FULLBAND_FRAME_SIZE;
   //else framesize = FASTSBBAND_FRAME_SIZE;

   //ippsConvert_16s32f_Sfs((Ipp16s *)pRin, r_in_32f_cur, framesize, 0);
   //ippsConvert_16s32f_Sfs((Ipp16s *)pSin, s_in_32f_cur, framesize, 0);
   ecint_header->fun.ec_ProcessFrame(obj_ec, pRin, pSin, pSout);
   //ippsConvert_32f16s_Sfs(s_out_32f_cur, (Ipp16s *)pSout, framesize, ippRndZero, 0);

   return USC_NoError;

}


