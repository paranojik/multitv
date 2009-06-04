#ifndef __COMMON__
#define __COMMON__


#include "compat.h"


#define MAX_CMD_LINE_ARGS  64

int match(LPCSTR mask, LPCSTR string);

BOOL SplitUserID   (LPCSTR UserID, LPSTR nick, LPSTR user, LPSTR host);
void RemoveUserHost(LPSTR _pTarget, LPCSTR _pSource);

int encode64 (const char *instr, int instrlen, char *outstr, int *outstrlen);
int decode64 (const char *instr, int instrlen, char *outstr, int *outstrlen);

ULONG adler32(ULONG adler, const unsigned char *buf, UINT len);

int getopt(int argc, char *argv[], char *optstring, int *optind, char *optarg);

int __cdecl CmdLineToArgcArgv(char *pSysCmdLine, char *ppszArgv[]);

#endif	// __COMMON__
