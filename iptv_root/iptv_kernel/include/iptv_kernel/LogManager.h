#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <fstream>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4267 4996)
#endif

#include "compat.h"
#include "semaphore.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "VBLib/VBLib.h"

/** @brief This is a log class.
 *
 */
class LogManager
{
public:
	virtual ~LogManager();
	static LogManager* Instance();

	void AddLogEntry(br::com::sbVB::VBLib::VBString logEntry);
	void AddLogHeader(br::com::sbVB::VBLib::VBString logHeader);
	br::com::sbVB::VBLib::VBString GetLog();
	void WriteLog();
	void WriteLogBinary(char* data, int size);

protected:
	LogManager();

private:
	static LogManager* m_instance;
	br::com::sbVB::VBLib::VBString m_log;
	_SEMAPHORE m_logMutex;
};


#endif
