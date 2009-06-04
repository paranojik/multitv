#ifndef CLINUX_FILE_LOG_H
#define CLINUX_FILE_LOG_H


#include "IFileLog.h"

class CLinuxFileLog : public IFileLog
{
public:

	CLinuxFileLog(LPCSTR _szString) : IFileLog(_szString) {}
    virtual ULONG SetLog(LPCSTR _szString);
};

#endif

