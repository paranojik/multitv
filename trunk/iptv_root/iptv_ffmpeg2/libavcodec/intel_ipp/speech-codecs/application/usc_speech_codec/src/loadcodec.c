/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2006-2007 Intel Corporation. All Rights Reserved.
//
//   Intel(R)  Integrated Performance Primitives
//
//     USC speech codec sample
//
// By downloading and installing this sample, you hereby agree that the
// accompanying Materials are being provided to you under the terms and
// conditions of the End User License Agreement for the Intel(R) Integrated
// Performance Primitives product previously accepted by you. Please refer
// to the file ippEULA.rtf or ippEULA.txt located in the root directory of your Intel(R) IPP
// product installation for more information.
//
// Purpose: Plug-ins operation functions.
//
////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "usc.h"

#ifdef _USC_ALL
#  undef _USC_ALL
#  define _USC_CODECS
#endif

#include "usc_objects_decl.h"
#include "util.h"
#include "loadcodec.h"
#include "uscfmt.h"

extern usc_fmt_info_Fxns USC_FORMAT_INFO_FXNS;


#define MAXLENSTR       256

typedef struct _StaticCodecs {
   Ipp8s codecName[USC_MAX_NAME_LEN];
   void *funcPtr;
}StaticCodecs;

StaticCodecs LinkedCodecs[] = {
#if (defined _USC_G723 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G723.1",&USC_G723_Fxns,
#endif
#if (defined _USC_G726 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G726",&USC_G726_Fxns,
#endif
#if (defined _USC_AMRWB || defined _USC_CODECS || defined _USC_ALL)
   "IPP_AMRWB",&USC_AMRWB_Fxns,
#endif
#if (defined _USC_GSMAMR || defined _USC_CODECS || defined _USC_ALL)
   "IPP_GSMAMR",&USC_GSMAMR_Fxns,
#endif
#if (defined _USC_GSMFR || defined _USC_CODECS || defined _USC_ALL)
   "IPP_GSMFR",&USC_GSMFR_Fxns,
#endif
#if (defined _USC_G728 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G728",&USC_G728_Fxns,
#endif
#if (defined _USC_G729 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G729I",&USC_G729I_Fxns,
   "IPP_G729A",&USC_G729A_Fxns,
#endif
#if (defined _USC_G722 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G722.1",&USC_G722_Fxns,
#endif
#if (defined _USC_G729FP || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G729A_FP",&USC_G729AFP_Fxns,
   "IPP_G729I_FP",&USC_G729IFP_Fxns,
#endif
#if (defined _USC_G722SB || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G722",&USC_G722SB_Fxns,
#endif
#if (defined _USC_G711 || defined _USC_CODECS || defined _USC_ALL)
   "IPP_G711A",&USC_G711A_Fxns,
   "IPP_G711U",&USC_G711U_Fxns,
#endif
#if (defined _USC_AMRWBE || defined _USC_CODECS || defined _USC_ALL)
   "IPP_AMRWBE",&USC_AMRWBE_Fxns,
#endif
   "END",NULL
};

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        EnumerateStaticLinkedCodecs
//  Purpose:     Enumerate all static linked codecs by name.
//  Returns:     None.
//  Parameters:
*/
void EnumerateStaticLinkedCodecs(FILE *f_log)
{
   Ipp32s i = 0;
   printf("Following codecs are linked:\n");
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      USC_OutputString(f_log, "           %s\n",LinkedCodecs[i].codecName);
      i++;
   }

   return;
}

void *TryToFindStatic(const Ipp8s *nameCodec)
{
   void *pAddr = NULL;
   Ipp32s i=0;
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      if(strcmp(nameCodec, LinkedCodecs[i].codecName) == 0) {
         pAddr = LinkedCodecs[i].funcPtr;
      }
      i++;
   }
   return pAddr;
}

Ipp32s GetNumLinkedCodecs()
{
   Ipp32s i = 0;
   while(strcmp(LinkedCodecs[i].codecName,"END")!=0) {
      i++;
   }

   return i;
}

void *Get_ith_StaticLinkedCodecs(Ipp32s index, Ipp8s *nameCodec)
{
   strcpy(nameCodec,LinkedCodecs[index].codecName);
   return LinkedCodecs[index].funcPtr;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        LoadUSCCodecByName
//  Purpose:     Load USC codec function table by name.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec     pointer to the input/output codec params structure
*/
Ipp32s LoadUSCCodecByName(LoadedCodec *codec,FILE *f_log)
{
   codec->uscParams.USC_Fns = NULL;

   codec->uscParams.USC_Fns = (USC_Fxns*)TryToFindStatic(codec->codecName);
   if(codec->uscParams.USC_Fns) {
      USC_OutputString(f_log, " Staticly loaded codec: %s\n",codec->codecName);
      return 0;
   }

   return -1;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        GetUSCCodecParamsByFormat
//  Purpose:     getting codec format description from the format library by name or by format tags.
//  Returns:     0 if success, -1 if fails.
//  Parameters:
//     codec       pointer to the input/output codec params structure
//     lQuery      Flag of search mode
//                 BY_FORMATTAG - search by format tag
//                 BY_NAME      - search by codec name
*/
Ipp32s GetUSCCodecParamsByFormat(LoadedCodec *codec, Ipp32s lQuery,FILE *f_log)
{
   Ipp32s CallResult;
   Ipp32s version;

   version = GetFormatsInfoVersion();
   version = (version>>8)&0xFF;
   /* Check version of the format library. We estimate that ig greater or equal than 1.0*/
   if(version >= 1) {
      if(lQuery&BY_FORMATTAG) {
         CallResult = USC_FORMAT_INFO_FXNS.GetNameByFormatTag(codec->lFormatTag, (Ipp8s *)codec->codecName);
         if(CallResult==1) {
            USC_FORMAT_INFO_FXNS.GetFormatDetailsByFormatTag(codec->lFormatTag, &codec->lIsVad);
            codec->lIsVad = GET_VAD_FROM_FORMAT_DET(codec->lIsVad);
            USC_OutputString(f_log, " Loaded %s format description\n", codec->codecName);
            return 0;
         }
      } else if(lQuery&BY_NAME) {
         Ipp32s det = USC_MAKE_FORMAT_DET(codec->lIsVad,codec->uscParams.pInfo->params.modes.hpf,
                     codec->uscParams.pInfo->params.modes.pf,codec->uscParams.pInfo->params.modes.truncate,0);
         CallResult = USC_FORMAT_INFO_FXNS.GetFormatTagByNameAndByDetails(codec->codecName, det, &codec->lFormatTag);
         if(CallResult==1) {
            USC_OutputString(f_log, " Loaded %s format description\n", codec->codecName);
            return 0;
         }
      }
   }
   return -1;
}

/* /////////////////////////////////////////////////////////////////////////////
//  Name:        FreeUSCSharedObjects
//  Purpose:     Release all USC shared objects.
//  Returns:     None.
//  Parameters:
//     codec     pointer to the input/output codec params structure
*/
void FreeUSCSharedObjects(LoadedCodec *codec)
{
   codec = codec;/*Just to avoid varning*/
   return;
}
