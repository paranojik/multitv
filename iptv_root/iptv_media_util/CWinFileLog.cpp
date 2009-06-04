#include "compat.h"
#include <sstream>
#include "CWinFileLog.h"
#include "global_error.h"


CWinFileLog g_FileLog(DBG_FILENAME);

ULONG CWinFileLog::SetLog(LPCSTR _szString)
{   
	ULONG ret;

	m_Semaph.Wait();

    if ( (ret = Open() ) == RET_OK)
    {
	    SYSTEMTIME sysTime;
        GetSystemTime(&sysTime);

	    std::ostringstream ostr;
        ostr << "["
            << sysTime.wYear   << "/" 
            << sysTime.wMonth  << "/" 
            << sysTime.wDay    << " "
            << sysTime.wHour   << ":"
            << sysTime.wMinute << ":" 
            << sysTime.wSecond << ":"  
            << sysTime.wMilliseconds <<"] " 
		    << _szString;


	    if (fprintf(m_pFile, "%s\n", ostr.str().c_str()) < 0)
            ret = RET_ERROR;
    }

    Close();

	m_Semaph.Signal();

	return ret;
}