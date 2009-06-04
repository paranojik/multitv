#ifdef _LINUX

#include "compat.h"
#include <sstream>
#include "CLinuxFileLog.h"
#include "global_error.h"


CLinuxFileLog g_FileLog(DBG_FILENAME);

ULONG CLinuxFileLog::SetLog(LPCSTR _szString)
{   
	ULONG ret;

	m_Semaph.Wait();

    if ( (ret = Open() ) == RET_OK)
    {
	    time_t rawtime;
	    struct tm * timeinfo;

	    time ( &rawtime );
	    timeinfo = localtime ( &rawtime );

	    char * pszDate = asctime (timeinfo);
	    char * pszTemp = new char[strlen(pszDate)-1];

	    if (pszTemp)
	    {
		    strncpy(pszTemp, pszDate, strlen(pszDate) -1);
		    pszTemp[strlen(pszDate) -1] = '\0';
	
		    if (fprintf (m_pFile, "[%s] %s\n", pszTemp, _szString) < 0)
			    ret = RET_ERROR;

		    delete pszTemp;
        }
	    else
            ret = RET_LOW_MEMORY;
    }

    Close();

	m_Semaph.Signal();

	return ret;
}

#endif
