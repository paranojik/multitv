/*
** $Id: lgc.h 349 2005-09-24 22:52:00Z dario $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#ifndef lgc_h
#define lgc_h


#include "lobject.h"


void luaC_collect (lua_State *L, int all);
void luaC_collectgarbage (lua_State *L);
void luaC_checkGC (lua_State *L);


#endif
