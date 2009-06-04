#ifndef _MEM_X_STR
#define _MEM_X_STR


#ifdef __cplusplus
extern "C" {
#endif

void * fill_mem(void *ptr,ULONG size,int ch);
void mov_mem(const void *orig,void *dest,ULONG tam);
int  mem_chr(const void *ptr, ULONG tam, char ch);
char * mem_str(char *buf, ULONG bsize, char *str, ULONG ssize, ULONG *poffset);

#ifdef __cplusplus
}
#endif


#endif  // _MEM_X_STR

