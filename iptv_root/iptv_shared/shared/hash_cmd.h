#ifndef _HASHCMD
#define _HASHCMD


#include "HashT.h"
#include "cmd_base.h"



class Hash_cmd : public HashT<UINT>
{
   public:

   Hash_cmd(Cmd_str *cmd_vector);
};


#endif //_HASHCMD
