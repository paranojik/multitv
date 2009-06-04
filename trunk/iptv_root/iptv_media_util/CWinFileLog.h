#ifndef CWIN_FILE_LOG_H
#define CWIN_FILE_LOG_H

#include "IFileLog.h"

class CWinFileLog : public IFileLog
{
public:
	CWinFileLog(LPCSTR _szString) : IFileLog(_szString) {}
    virtual ULONG SetLog(LPCSTR _szString);
};

#endif