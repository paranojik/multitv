#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#ifdef WIN32
	#include "CWinFileLog.h"
	extern CWinFileLog g_FileLog;
#endif

#ifdef _LINUX
	#include "CLinuxFileLog.h"
	extern CLinuxFileLog g_FileLog;
#endif

#endif // #ifdef GLOBAL_TYPES_H

