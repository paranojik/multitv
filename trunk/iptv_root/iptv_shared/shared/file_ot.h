#ifndef _FILE_OT_STR
#define _FILE_OT_STR

#include <fcntl.h>


class FileRead
{
   LPSTR   pdata;

   long    OffRead,
           SizeRead,
           lRealSize,
           lNumCr;

   public:

   BOOL    BufOk;

   FileRead (LPCSTR path, UINT mode);
  ~FileRead (void);

   long GetFileSize  (void) { return SizeRead;  }
   long GetRealSize  (void);
   long CountCr      (void);
   UINT read_line  (LPSTR buf, UINT size);
   UINT read_line2 (LPSTR buf, UINT size);
   long tell(void) { return (OffRead); }
   long seek(long offset);
};


#endif
