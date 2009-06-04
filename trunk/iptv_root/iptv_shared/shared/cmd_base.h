#ifndef __CMD_BASE__
#define __CMD_BASE__


#include "compat.h"


struct Cmd_str
{
   LPCSTR  cmd;
   UINT    ind;
};



int    search_cmd_seq     (LPCSTR key, Cmd_str *pcmdVect);
LPCSTR search_cmd_code_seq(UINT code, Cmd_str *pcmdVect);
int    get_cmd_nregs      (Cmd_str *pcmdVect);


#endif    // __CMD_BASE__


