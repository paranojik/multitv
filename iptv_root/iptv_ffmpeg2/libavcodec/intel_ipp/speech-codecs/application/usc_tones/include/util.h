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
// Purpose: Auxiliary structures and functions header file.
//
////////////////////////////////////////////////////////////////////////*/
#ifndef __UTIL_H__
#define __UTIL_H__

#include "usc_td.h"

#define MAX_FILENAME_LEN 256

typedef enum {
   Empty = -32768,
   DTMF=-1,
   ANS=0
} TD_Type;

typedef struct _CommandLineParams {
   char inputFileName[MAX_FILENAME_LEN];
   char outputFileName[MAX_FILENAME_LEN];
   char logFileName[MAX_FILENAME_LEN];
   int  optionReport;
   int  puttologfile;
   int  sample_frequency;
   TD_Type  type;
}CommandLineParams;

typedef struct _UsedObjects {
   FILE *f_in;
   FILE *f_out;
   FILE *f_log;
   char *pInputBuffer;
   int nBanks;
   USC_MemBank* pBanks;
   USC_TD_Info *pInfo;
}UsedObjects;

void PrintUsage(char* pAppName, FILE *flog);

void ReadCommandLine(CommandLineParams *params, int argc, char *argv[]);
void SetCommandLineByDefault(CommandLineParams *params);
void USC_OutputString(FILE *flog, char *format,...);

void InitUsedObjects(UsedObjects *pObjects);
void ReleaseUsedObjects(UsedObjects *pObjects);

void USC_ToneId2Char(USC_ToneID tID, char *pDstString);

#if defined (_BIG_ENDIAN)
void SwapBytes_16u(unsigned short *pSrc,int len);
#endif

#endif/* __UTIL_H__*/
