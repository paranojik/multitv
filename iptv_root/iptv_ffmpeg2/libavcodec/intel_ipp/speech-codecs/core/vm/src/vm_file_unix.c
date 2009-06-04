/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2003-2007 Intel Corporation. All Rights Reserved.
//
*/
/*
 * VM 64-bits buffered file operations library
 *       UNIX implementation
 */
/* codecws compilation fence */
#if defined(LINUX32) || defined(OSX)
#include <sys/types.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include "vm_file.h"


/* obtain file info. return 0 if file is not accessible,
   file_size or file_attr can be NULL if either is not interested */
Ipp32s vm_file_getinfo(const vm_char *filename, Ipp64u *file_size, Ipp32u *file_attr) {
   struct stat buf;
#if defined(OSX) || defined(LINUX64)
   if (stat(filename,&buf)) return 0;
#else
   if (stat64(filename,&buf)) return 0;
#endif
   if (file_size) *file_size=buf.st_size;
   if (file_attr) {
      *file_attr=0;
      if (buf.st_mode & S_IFREG) *file_attr|=VM_FILE_ATTR_FILE;
      if (buf.st_mode & S_IFDIR) *file_attr|=VM_FILE_ATTR_DIRECTORY;
      if (buf.st_mode & S_IFLNK) *file_attr|=VM_FILE_ATTR_LINK;
   }
   return 1;
  }



Ipp64u vm_file_fseek(vm_file *fd, Ipp64s position, VM_FILE_SEEK_MODE mode) {
#if defined(OSX) || defined(LINUX64)
  return fseeko(fd, (off_t)position, mode);
#else
  return fseeko64(fd, (__off64_t)position, mode);
#endif
  }

Ipp64u vm_file_ftell(vm_file *fd) {
#if defined(OSX) || defined(LINUX64)
  return (Ipp64u) ftello(fd);
#else
  return (Ipp64u)ftello64(fd);
#endif
  }

/*
 *   Directory manipulations
 */
Ipp32s vm_dir_remove(vm_char *path) {
   return !remove(path);
}

Ipp32s vm_dir_mkdir(vm_char *path) {
   return !mkdir(path,0777);
}

Ipp32s vm_dir_open(vm_dir **dd, vm_char *path) {
   dd[0]=opendir(path);
   return (dd) ? 1 : 0;
}

/*
 * directory traverse */
Ipp32s vm_dir_read(vm_dir *dd, vm_char *filename,int nchars) {
  Ipp32s rtv = 0;
   struct dirent *ent=readdir(dd);
   if (ent) {
     vm_string_strncpy(filename,ent->d_name,nchars);
     rtv = 1;
     }
   return rtv;
}

void vm_dir_close(vm_dir *dd) {
  closedir(dd);
}

Ipp64u vm_dir_get_free_disk_space( void ) {
  Ipp64u rtv = 0;
  struct statvfs fst;
  if (statvfs(".", &fst) >= 0) {
    rtv = fst.f_bsize*fst.f_bavail;
    }
  return rtv;
  }
#else
# pragma warning( disable: 4206 )
#endif
