#ifndef __FILE_X
#define __FILE_X


#ifdef __cplusplus
extern "C" {
#endif

//BOOL DeleteFile     (LPCSTR path);
//BOOL _FileExist     (LPCSTR path);
BOOL FileExist      (LPCSTR path);
long GetFileLength  (LPCSTR path);
BOOL RenameFile     (LPCSTR lpOldName, LPCSTR lpNewName);
//BOOL GetDOSFileDate (LPCSTR path, UINT FAR *lpDate, UINT FAR *lpTime);
BOOL DirectoryExist (LPCSTR _dir);
time_t _GetFileTime    (LPCSTR _fname);
BOOL   _SetFileTime    (LPCSTR _fname, time_t _ctime);

int  GetLastFileError (void);

HFILE open_x  (LPCSTR path, OFSTRUCT FAR *ofstr, UINT flags, UINT mode=0);
int   close_x (HFILE hfile);
UINT  read_x  (HFILE hfile, void FAR *buf, UINT size); 
UINT  write_x (HFILE hfile, void FAR *buf, UINT size); 
LONG  seek_x  (HFILE hfile, LONG offset, int nOrigin);
LONG  tell_x  (HFILE hfile);
int   chdir_x (LPCSTR _dir);
int   rmdir_x (LPCSTR _dir);
int   mkdir_x (LPCSTR _dir, UINT _mode=0);

LPSTR getcwd_x(LPSTR _buf, UINT _size);


int fread_random(HFILE hfile, void FAR *buf, UINT treg, UINT nreg);


#if defined(_WIN32)

BOOL  lock_x  (HFILE hFile, DWORD dwFileOffsetLow, DWORD dwFileOffsetHigh,
               DWORD nNumberOfBytesToLockLow, DWORD nNumberOfBytesToLockHigh);

long  hread_x (HFILE hfile, void FAR *buf, long size);
long  hwrite_x(HFILE hfile, void FAR *buf, long size);

#endif


/*
int read_c(HFILE hfile);
int write_c(HFILE hfile, char c);
*/

BOOL fread_seq   (HFILE hfile, LPSTR buf, UINT tbuf);
BOOL fwrite_seq  (HFILE hfile, LPCSTR buf);
int  fread_n     (HFILE hfile, LPSTR buf, UINT n);
BOOL fwrite_n    (HFILE hfile, LPCSTR buf, UINT n);
int  fread_random(HFILE hfile, void FAR *buf, UINT treg, UINT nreg);


LPSTR sep_reduz       (LPCSTR ent, LPSTR reduzido, UINT size);
LPSTR sep_path        (LPCSTR ent, LPSTR path, LPSTR filename);
//LPSTR tira_reduz      (LPCSTR ent, LPSTR path);
LPSTR sep_extensao    (LPCSTR ent, LPSTR saida, LPSTR extensao);
LPSTR change_filename (LPCSTR source, LPSTR target, LPCSTR reduzido);
UINT  add_dirsufix    (LPCSTR _ent, LPSTR _saida);

//void  GetPath         (LPCSTR pathname, LPSTR saida);

BOOL  CriaDir   (LPCSTR pdir);
BOOL  ExisteDir (LPCSTR pdir);
BOOL  PodeCriarDir (LPCSTR pdiretorio);

#ifdef __cplusplus
}
#endif


#endif  // __FILE_X

