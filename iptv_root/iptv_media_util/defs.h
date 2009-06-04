#ifndef DEFS_H
#define DEFS_H

#include "compat.h"

#define MC_ASF_DECODER_BUFFER_PLUS                 11000
#define MC_ASF_SIMPLE_INDEX_ASF_ENTRIES_MAX        2000000
#define MC_MAX_IRM_RAW_PACKETSIZE                  (128*1024)
#define MC_IRM_HEADER_SIZE                         12
#define MC_MAX_IRM_PACKETSIZE                      (MC_MAX_IRM_RAW_PACKETSIZE-MC_IRM_HEADER_SIZE)
#define MC_ENCODER_ASF_INTERVAL_BETWEEN_PACKETS    3

#ifdef _LINUX
    #include <inttypes.h>
    typedef int64_t QWORD;
#else
    typedef __int64 QWORD;
#endif


typedef DWORD  *LPDWORD;
typedef QWORD  LONGLONG;

typedef struct _GUID_W {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID_W;

#ifdef _LINUX
#include <inttypes.h>

typedef uint64_t ULONGLONG;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

#endif

#endif



