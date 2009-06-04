/*
** mm.h
** Waldemar Celes Filho
** Sep 16, 1992
** Last update: Feb 1999
*/

#define _MM_


#ifndef mm_h
#define mm_h

#include <stdlib.h>
#include <string.h>

#ifdef _MM_

/* Define macros if not compiling mm source file */
#ifndef MM_SOURCEFILE
#define malloc(s)	_MmMalloc(s,__FILE__,__LINE__)
#define calloc(n,s)	_MmCalloc(n,s,__FILE__,__LINE__)
#define realloc(a,s)	_MmRealloc(a,s,__FILE__,__LINE__,#a)
#define free(a)		_MmFree(a,__FILE__,__LINE__,#a)
#define strdup(s)	_MmStrdup(s,__FILE__,__LINE__)
#endif

typedef void (*Ferror) (char *);

/* Exported functions */

#ifdef __cplusplus
extern "C" {
#endif

void *_MmMalloc (unsigned size, char *file, int line);
void *_MmCalloc (unsigned n, unsigned size, char *file, int line);
void  _MmFree (void *a, char *file, int line, char *var);
void *_MmRealloc (void *old, unsigned size, char *file, int line, char *var);
char *_MmStrdup (char *s, char *file, int line);

 /* Utility functions */
void  MmInit (Ferror f, Ferror w);
unsigned MmGetBytes (void);
void MmListAllocated (void);
void MmCheck (void);
void MmStatistics (void);
void MmModuleStatistics (char *module);

#ifdef __cplusplus
}  // close 'extern "C"'

int _MmSetLine (char* file, int line);

// define macros if not compiling mm source file
#ifndef MM_SOURCEFILE
#define new _MmSetLine(__FILE__,__LINE__)?0:new
#define delete _MmSetLine(__FILE__,__LINE__),delete
#endif

#endif

#else

/* Desable utilies when not debugging */
#define MmInit(f,w)
#define MmGetBytes()
#define MmListAllocated()
#define MmCheck()
#define MmStatistics()
#define MmModuleStatistics(module)

#endif
#endif

