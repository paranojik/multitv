/*////////////////////////////////////////////////////////////////////////
//
// INTEL CORPORATION PROPRIETARY INFORMATION
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Intel Corporation and may not be copied
// or disclosed except in accordance with the terms of that agreement.
// Copyright (c) 2005-2007 Intel Corporation. All Rights Reserved.
//
//   Intel(R)  Integrated Performance Primitives
//
//     USC speech codec sample
//
// Purpose: Auxiliary functions file.
//
////////////////////////////////////////////////////////////////////////*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "util.h"

#include <ippcore.h>
#include <ipps.h>

void SetCommandLineByDefault(CommandLineParams *params)
{
   ippsZero_8u((Ipp8u*)params,sizeof(CommandLineParams));
   params->inputFileName[0] = '\0';
   params->logFileName[0] = '\0';
   params->optionReport = 0;
   params->outputFileName[0] = '\0';
   params->puttologfile = 0;
   params->sample_frequency = 8000;
   params->type = Empty;
}
void ReadCommandLine(CommandLineParams *params, int argc, char *argv[])
{
   argc--;
   while(argc-- > 0){ /*process command line */
      argv++;
      if ('-' == (*argv)[0]) {
         if (!strcmp(*argv,"-n")){ /* check option report mode */
            params->optionReport = 1;
            continue;
         }else if (!strcmp(*argv,"-type")){ /* get TD type */
            argv++;
            argc--;
            if(strcmp(*argv,"ANS")==0) {
               params->type = ANS;
            }
            continue;
         }else if (!strncmp(*argv,"-f",2)){ /* check if sample frequency specified */
            params->sample_frequency = atol(*argv+2);
            continue;
         }else if (!strcmp(*argv,"-o")){ /* check if log output is specified */
            params->puttologfile=1;
            argv++;
            argc--;
            strcpy(params->logFileName, *argv);
            continue;
         }
      }else{ /* unknown option */
         argc++; /* skip */
         argv--;
         break;
      }
   }

   if(argc-->0){ /* check if input file name is specified */
      argv++;
      strcpy(params->inputFileName, *argv);
   }

   if(argc-->0){ /* check if output file name is specified */
      argv++;
      strcpy(params->outputFileName, *argv);
   }
   return;
}

void PrintUsage(char* pAppName, FILE *flog)
{
   if(flog) {
      fprintf(flog,"Usage : %s -type ANS <options> <inFile> <outFile>\n",pAppName);
      fprintf(flog,"  options:\n");
      fprintf(flog,"   [-f]              Sample frequency (8000 or 16000) .\n");
      fprintf(flog,"   [-n]              Option reports (optional) .\n");
      fprintf(flog,"   [-o] <logFile>    Print out to <logFile> file (optional).\n");
      fprintf(flog,"  <inFile> <outFile> - 8000 kHz  or 16000 kHz 16 bps PCM files .\n");
   } else {
      printf("Usage : %s -type ANS <options> <inFile> <outFile>\n",pAppName);
      printf("  options:\n");
      printf("   [-f]              Sample frequency (8000 or 16000) .\n");
      printf("   [-n]              Option reports (optional) .\n");
      printf("   [-o] <logFile>    Print out to <logFile> file (optional).\n");
      printf("  <inFile> <outFile> - 8000 kHz  or 16000 kHz 16 bps PCM files .\n");
   }

   return;
}

#define MAX_LINE_LEN 1024

void USC_OutputString(FILE *flog, char *format,...)
{
   char  line[MAX_LINE_LEN];
   va_list  args;

   va_start(args, format);
   vsprintf(line, format, args);
   va_end(args);

   if(flog) {
      fprintf(flog,"%s", line);
   } else {
      printf("%s", line);
   }

   return;
}

void InitUsedObjects(UsedObjects *pObjects)
{
   pObjects->f_in = NULL;
   pObjects->f_out = NULL;
   pObjects->f_log = NULL;
   pObjects->pInputBuffer = NULL;
   pObjects->nBanks = 0;
   pObjects->pBanks = NULL;
   pObjects->pInfo = NULL;
}
void ReleaseUsedObjects(UsedObjects *pObjects)
{
   if(pObjects->f_in) {
      fclose(pObjects->f_in);
      pObjects->f_in = NULL;
   }
   if(pObjects->f_out) {
      fclose(pObjects->f_out);
      pObjects->f_out = NULL;
   }
   if(pObjects->f_log) {
      fclose(pObjects->f_log);
      pObjects->f_log = NULL;
   }
   if(pObjects->pInputBuffer) {
      ippsFree(pObjects->pInputBuffer);
      pObjects->pInputBuffer = NULL;
   }
   if(pObjects->pBanks) {
      int i;

      for(i=0; i<pObjects->nBanks;i++){
         ippsFree(pObjects->pBanks[i].pMem);
         pObjects->pBanks[i].pMem = NULL;
      }
      ippsFree(pObjects->pBanks);
      pObjects->pBanks = NULL;
      pObjects->nBanks=0;
   }
   if(pObjects->pInfo) {
      ippsFree(pObjects->pInfo);
      pObjects->pInfo = NULL;
   }
}

void USC_ToneId2Char(USC_ToneID tID, char *pDstString)
{
   if(tID==USC_NoTone) {
      pDstString[0] = '\0';
      return;
   }
   switch(tID) {
      case USC_Tone_0:
         pDstString[0] = '0';
         pDstString[1] = '\0';
         break;
      case USC_Tone_1:
         pDstString[0] = '1';
         pDstString[1] = '\0';
         break;
      case USC_Tone_2:
         pDstString[0] = '2';
         pDstString[1] = '\0';
         break;
      case USC_Tone_3:
         pDstString[0] = '3';
         pDstString[1] = '\0';
         break;
      case USC_Tone_4:
         pDstString[0] = '4';
         pDstString[1] = '\0';
         break;
      case USC_Tone_5:
         pDstString[0] = '5';
         pDstString[1] = '\0';
         break;
      case USC_Tone_6:
         pDstString[0] = '6';
         pDstString[1] = '\0';
         break;
      case USC_Tone_7:
         pDstString[0] = '7';
         pDstString[1] = '\0';
         break;
      case USC_Tone_8:
         pDstString[0] = '8';
         pDstString[1] = '\0';
         break;
      case USC_Tone_9:
         pDstString[0] = '9';
         pDstString[1] = '\0';
         break;
      case USC_Tone_ASTERISK:
         pDstString[0] = '*';
         pDstString[1] = '\0';
         break;
      case USC_Tone_HASH:
         pDstString[0] = '#';
         pDstString[1] = '\0';
         break;
      case USC_Tone_A:
         pDstString[0] = 'A';
         pDstString[1] = '\0';
         break;
      case USC_Tone_B:
         pDstString[0] = 'B';
         pDstString[1] = '\0';
         break;
      case USC_Tone_C:
         pDstString[0] = 'C';
         pDstString[1] = '\0';
         break;
      case USC_Tone_D:
         pDstString[0] = 'D';
         pDstString[1] = '\0';
         break;
      case USC_ANS:
         strcpy(pDstString,"ANS");
         break;
      case USC_slashANS:
         strcpy(pDstString,"\\ANS");
         break;
      case USC_ANSam:
         strcpy(pDstString,"ANSam");
         break;
      case USC_slashANSam:
         strcpy(pDstString,"\\ANSam");
         break;
   }
   return;
}
#if defined (_BIG_ENDIAN)
void SwapBytes_16u(unsigned short *pSrc,int len)
{
   int i;
   for(i=0;i<len;i++) {
      pSrc[i] = (((pSrc[i]) << 8) | ((pSrc[i]) >> 8 ));
   }
   return;
}
#endif
