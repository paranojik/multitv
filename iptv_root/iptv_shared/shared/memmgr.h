#ifndef __MEM_MGR__
#define __MEM_MGR__


#ifdef _MM_


void  _MmFree   (void *ptr, const char *filename, ULONG line);
void *_MmMalloc (ULONG size, const char *filename, ULONG line);
void *_MmCalloc (ULONG n, ULONG _size, const char *filename, ULONG line);
char *_MmStrdup (char *s, const char *filename, ULONG line);
void *_MmRealloc(void *old, ULONG size, const char *filename, ULONG line);


/* Define macros if not compiling mm source file */
#ifndef MM_SOURCEFILE
#define malloc(s)	 _MmMalloc(s,__FILE__,__LINE__)
#define calloc(n,s)	 _MmCalloc(n,s,__FILE__,__LINE__)
#define realloc(a,s) _MmRealloc(a,s,__FILE__,__LINE__)
#define free(a)		 _MmFree(a,__FILE__,__LINE__)
#define strdup(s)	 _MmStrdup(s,__FILE__,__LINE__)
#endif  // MM_SOURCEFILE


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}  // close 'extern "C"'

int  _MmSetLine  (const char* file, int line);
int  _MmSetMMMain(void);
void ListMemTable(void);

// define macros if not compiling mm source file
#ifndef MM_SOURCEFILE
#define new _MmSetLine(__FILE__,__LINE__)?0:new
#define delete _MmSetLine(__FILE__,__LINE__),delete
#else
#define new _MmSetMMMain()?0:new
#define delete _MmSetMMMain(),delete
#endif  // MM_SOURCEFILE

#endif  // __cplusplus

#endif  // _MM_

#endif  //__MEM_MGR__


