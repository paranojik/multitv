/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright(c) 2006-2007 Intel Corporation. All Rights Reserved.
//
//  Intel(R) Integrated Performance Primitives Audio Processing
//  Sample for Windows*
//
*/

#include <stdio.h>
#include <stdlib.h>

#if defined (WIN32) || defined _WIN32_WCE
#include <windows.h>
#endif

#include <ippcore.h>
#include <ipps.h>
#include <ippsc.h>

#include "util.h"

#define _USC_TONES
#include "usc_objects_decl.h"

#define COPYRIGHT_STRING "Copyright (c) 2006-2007 Intel Corporation. All Rights Reserved."

#define OPEN_FILE_ERROR     -1   /* return code on file open error */
#define MEMORY_ERROR        -2   /* return code on memory allocation error */
#define USC_ERROR           -3   /* return code on error USC operation error*/

#define ASSERT_MSGBOX(cond, msg, title, exitcode, pObjects)   \
{                                                             \
  if (!(cond))                                                \
  {                                                           \
    USC_OutputString(Objects.f_log, "%s: %s\n", title, msg);  \
    ReleaseUsedObjects(pObjects);                             \
    exit(exitcode);                                           \
  }                                                           \
}

#define FILE_ASSERT_MSGBOX(cond, msg, title, exitcode, pObjects)   \
{                                                             \
  if (!(cond))                                                \
  {                                                           \
    USC_OutputString(Objects.f_log, "%s: TD example. Cannot open %s file!\n", title, msg);  \
    ReleaseUsedObjects(pObjects);                             \
    exit(exitcode);                                           \
  }                                                           \
}

#if defined( _WIN32_WCE )
#define WINCE_CMDLINE_SIZE 512
#define WINCE_EXENAME_SIZE 128
#define WINCE_NCMD_PARAMS   16
int parseCmndLine( char* exename, const char* cmndline, char* line, int linelen, char** argv, int argvlen ) {
   int i;
   char* token;
   char* seps = " ,";                     /* argement separators */
   int argc = 1;                          /* number of parameters */
   for (i=0; i<argvlen; i++) argv[i] = NULL;
   argv[0] = exename;                     /* the first standard argument */
   memset( line, 0, linelen );
   strncpy( line, cmndline, linelen-1 );
   token = strtok( line, seps );          /* the first true argument */
   while( token != NULL && argc <= argvlen ) {
      argv[argc++] = token;
      token = strtok( NULL, seps );
   }
   return argc;
}

int WINAPI WinMain( HINSTANCE hinst, HINSTANCE xxx, LPWSTR lpCmdLine, int yyy )
{
   char line[WINCE_CMDLINE_SIZE];                     /* to copy command line */
   char* argvv[WINCE_NCMD_PARAMS];
   char** argv=argvv;

   wchar_t wexename[WINCE_EXENAME_SIZE];
   char exename[WINCE_EXENAME_SIZE];
   char cmdline[WINCE_CMDLINE_SIZE];

   /* simulate argc and argv parameters */
   int argc;
#else /*Other OS*/
int main(int argc, char *argv[])
{
#endif /*_WIN32_WCE*/
   USC_TD_Fxns *pUSC_TD = NULL;
   USC_Handle hUSCTD;
   USC_Status uscRes;
   USC_PCMStream in;

   CommandLineParams clParams;
   UsedObjects Objects;

   unsigned int     cSamplesRead;                       /* number of samples read from input file */
   int i, infoSize=0, modeReq=-1;

#if defined( _WIN32_WCE )

   GetModuleFileName( hinst, wexename, WINCE_EXENAME_SIZE );
   sprintf( exename, "%ls", wexename );
   sprintf( cmdline, "%ls", lpCmdLine );
   argc = parseCmndLine( exename, cmdline, line, WINCE_CMDLINE_SIZE, argv, WINCE_NCMD_PARAMS );

#endif

   ippStaticInit();

   SetCommandLineByDefault(&clParams);
   InitUsedObjects(&Objects);

   ReadCommandLine(&clParams, argc, argv);

   if(clParams.puttologfile) {
      Objects.f_log=fopen(clParams.logFileName,"a");
      if(Objects.f_log==NULL) {
         printf("Cannot open %s log file for writing\n",clParams.logFileName);
         printf("Log file ignored.\n");
         clParams.puttologfile = 0;
      }
   }

   if(clParams.optionReport) {
      const  IppLibraryVersion *verIppSC;
      verIppSC = ippscGetLibVersion();
      USC_OutputString(Objects.f_log, "%s\n",COPYRIGHT_STRING);
      USC_OutputString(Objects.f_log, "Intel IPP, Unified Speech Codec interface based Tone detector sample\n");
      USC_OutputString(Objects.f_log, "The Intel(R) IPPSC library used:  %d.%d.%d Build %d, name %s\n",
             verIppSC->major,verIppSC->minor,verIppSC->majorBuild,verIppSC->build,verIppSC->Name);
   }

   if((!clParams.inputFileName[0]) ||(!clParams.outputFileName[0])) {
      PrintUsage(argv[0], Objects.f_log);
      ReleaseUsedObjects(&Objects);
      return 0;
   }

   if(clParams.type == ANS) {
      pUSC_TD = &USC_ANSs_Fxns;
   } else {
      USC_OutputString(Objects.f_log, "Error: Unsupported type.\n");
      PrintUsage(argv[0], Objects.f_log);
      ReleaseUsedObjects(&Objects);
      return 0;
   }

   /* Open input and output files */
   Objects.f_in = fopen(clParams.inputFileName,"rb");
   FILE_ASSERT_MSGBOX(Objects.f_in != NULL, clParams.inputFileName, "Error", OPEN_FILE_ERROR, &Objects);
   Objects.f_out = fopen(clParams.outputFileName,"wb");
   FILE_ASSERT_MSGBOX(Objects.f_in != NULL, clParams.outputFileName, "Error", OPEN_FILE_ERROR, &Objects);

   pUSC_TD->std.GetInfoSize(&infoSize);
   Objects.pInfo = (USC_TD_Info*)ippsMalloc_8u(infoSize);
   ASSERT_MSGBOX(Objects.pInfo != NULL, "Out of memory!\n", "Error", MEMORY_ERROR, &Objects);

   /* Get the TD info */
   uscRes = pUSC_TD->std.GetInfo((USC_Handle)NULL, Objects.pInfo);
   ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC GetInfo failed!\n", "Error", USC_ERROR, &Objects);

   for(i=0;i<Objects.pInfo->nOptions;i++) {
      if(Objects.pInfo->params[i].pcmType.sample_frequency==clParams.sample_frequency) {
         modeReq = i;
      }
   }

   ASSERT_MSGBOX(modeReq != -1, "TD example. Unsupported PCM type!\n", "Error", USC_ERROR, &Objects);

   /* Learn how many memory block needed  for the encoder */
   uscRes = pUSC_TD->std.NumAlloc(&Objects.pInfo->params[modeReq], &Objects.nBanks);
   ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC NumAlloc failed!\n", "Error", USC_ERROR, &Objects);

   /* allocate memory for memory bank table */
   Objects.pBanks = (USC_MemBank*)ippsMalloc_8u(sizeof(USC_MemBank)*Objects.nBanks);
   ASSERT_MSGBOX(Objects.pBanks != NULL, "Out of memory!\n", "Error", MEMORY_ERROR, &Objects);

   /* Query how big has to be each block */
   uscRes = pUSC_TD->std.MemAlloc(&Objects.pInfo->params[modeReq], Objects.pBanks);
   ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC MemAlloc failed!\n", "Error", USC_ERROR, &Objects);

   /* allocate memory for each block */
   for(i=0; i<Objects.nBanks;i++){
      Objects.pBanks[i].pMem = NULL;
      Objects.pBanks[i].pMem = (char*)ippsMalloc_8u(Objects.pBanks[i].nbytes);
   }

   for(i=0; i<Objects.nBanks;i++){
      ASSERT_MSGBOX(Objects.pBanks[i].pMem != NULL, "Out of memory!\n", "Error", MEMORY_ERROR, &Objects);
   }

   /* Create encoder instance */
   uscRes = pUSC_TD->std.Init(&Objects.pInfo->params[modeReq], Objects.pBanks, &hUSCTD);
   ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC Init failed!\n", "Error", USC_ERROR, &Objects);

   uscRes = pUSC_TD->std.GetInfo(hUSCTD, Objects.pInfo);
   ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC GetInfo failed!\n", "Error", USC_ERROR, &Objects);

   Objects.pInputBuffer = (char*)ippsMalloc_8u(Objects.pInfo->params[0].framesize);
   ASSERT_MSGBOX(Objects.pInputBuffer != NULL, "Out of memory!\n", "Error", MEMORY_ERROR, &Objects);

   USC_OutputString(Objects.f_log, "\nStart processing.\n");
   USC_OutputString(Objects.f_log, "Input  file: %s\n",clParams.inputFileName);
   USC_OutputString(Objects.f_log, "Output file: %s\n",clParams.outputFileName);

   cSamplesRead = 1;
   while(cSamplesRead > 0)
   {
      USC_ToneID toneID;
      cSamplesRead = (unsigned int)fread(Objects.pInputBuffer, 1, Objects.pInfo->params[0].framesize, Objects.f_in);
#if defined _BIG_ENDIAN
      SwapBytes_16u((unsigned short *)Objects.pInputBuffer,cSamplesRead>>1);
#endif

      in.bitrate = Objects.pInfo->params[0].pcmType.sample_frequency * Objects.pInfo->params[0].pcmType.bitPerSample;
      in.nbytes = Objects.pInfo->params[0].framesize;
      in.pBuffer = Objects.pInputBuffer;
      in.pcmType.bitPerSample = Objects.pInfo->params[0].pcmType.bitPerSample;
      in.pcmType.nChannels = Objects.pInfo->params[0].pcmType.nChannels;
      in.pcmType.sample_frequency = Objects.pInfo->params[0].pcmType.sample_frequency;

      uscRes = pUSC_TD->DetectTone(hUSCTD, &in, &toneID);
      ASSERT_MSGBOX(uscRes == USC_NoError, "TD example. USC DetectTone failed!\n", "Error", USC_ERROR, &Objects);
      if(toneID!=USC_NoTone) {
         char buffer[128];
         USC_ToneId2Char(toneID, buffer);
         fprintf(Objects.f_out,"%s\n",buffer);
         printf("%s\n",buffer);
      }
   }

   USC_OutputString(Objects.f_log, "\nFinish.\n");

   /* Close all objects */
   ReleaseUsedObjects(&Objects);

   return 0;
}

