#ifndef CFILE_LOG_H
#define CFILE_LOG_H

#include <string>
#include "shared/semaphore.h"

#define DBG_FILENAME "debug.dbg"

class IFileLog
{
protected:

	FILE *m_pFile;

	std::string m_Filename;
	_SEMAPHORE m_Semaph;

public:
	IFileLog(LPCSTR _szFilename);
	virtual ~IFileLog();
	virtual ULONG SetLog(LPCSTR _szString) = 0;
    
    ULONG Open();
    void Close();
};


#endif
