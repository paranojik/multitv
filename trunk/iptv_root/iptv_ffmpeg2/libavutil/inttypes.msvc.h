
#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__)
#    define CONFIG_WIN32
#endif

#if defined(WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__) && !defined(EMULATE_INTTYPES)
#    define EMULATE_INTTYPES
#endif


#ifdef CONFIG_WIN32
    typedef signed char  int8_t;
    typedef signed short int16_t;
    typedef signed int   int32_t;
    typedef unsigned char  uint8_t;
    typedef unsigned short uint16_t;
    typedef unsigned int   uint32_t;

    typedef signed __int64   int64_t;
    typedef unsigned __int64 uint64_t;
#else /* other OS */
    typedef signed long long   int64_t;
    typedef unsigned long long uint64_t;
#endif /* other OS */

#ifndef INT16_MIN
#define INT16_MIN       (-0x7fff-1)
#endif

#ifndef INT16_MAX
#define INT16_MAX       0x7fff
#endif

#ifndef INT64_MIN
#define INT64_MIN       (-0x7fffffffffffffffLL-1)
#endif

#ifndef INT64_MAX
#define INT64_MAX int64_t_C(9223372036854775807)
#endif

#ifndef UINT64_MAX
#define UINT64_MAX uint64_t_C(0xFFFFFFFFFFFFFFFF)
#endif

/* Macros for minimum-width integer constant expressions */

#define INT8_C(x) x
#define INT16_C(x) x
#define INT32_C(x) x ## L
#define INT64_C(x) x ## LL

#define UINT8_C(x) x
#define UINT16_C(x) x
#define UINT32_C(x) x ## UL
#define UINT64_C(x) x ## ULL

/* Macros for greatest-width integer constant expressions */

#define INTMAX_C(x) x ## LL
#define UINTMAX_C(x) x ## ULL 
