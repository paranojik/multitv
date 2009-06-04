#include "iptv_kernel/LogManager.h"

using namespace br::com::sbVB::VBLib;
using namespace std;

/** Initializes the instance.
*
*/
LogManager* LogManager::m_instance = NULL;

/** Default constructor.
*
*/
LogManager::LogManager() : m_logMutex(SEMAPH_MUTEX)
{

}

/** Default destructor.
*
*/
LogManager::~LogManager()
{
	// WriteLog();

	//if(m_instance != NULL)
	//	delete m_instance;
}

/** Returns the unique instance of LogManager.
*	Creates a new instance if none exists.
*/
LogManager* LogManager::Instance()
{
	if(m_instance == NULL)
		m_instance = new LogManager;

	return m_instance; 
}

/** Adds a log entry.
*
*/
void LogManager::AddLogEntry(VBString logEntry)
{
	m_logMutex.Wait();

	m_log = logEntry;

	WriteLog();

	m_logMutex.Signal();
}

/** Adds a log header.
*
*/
void LogManager::AddLogHeader(VBString logHeader)
{
	m_logMutex.Wait();

	//m_log = "\n";

	//if(!m_log.IsEmpty())
	//	m_log += "\n";

	m_log = "****** ";
	m_log += logHeader;
	m_log += " ******";

	WriteLog();

	m_log = "";

	m_logMutex.Signal();
}

/** Returns the log.
*
*/
VBString LogManager::GetLog()
{
	m_logMutex.Wait();

	VBString returnValue = m_log;

	m_logMutex.Signal();

	return returnValue;
}

/** Writes the log into disk.
*
*/
void LogManager::WriteLog()
{
	m_logMutex.Wait();

	 ofstream logFile;
	 VBString fileName = "log.txt";

	logFile.open(fileName, ios::app);

	logFile << m_log << endl;

	m_logMutex.Signal();
}

/** Writes binary data into the log.
*
*/
void LogManager::WriteLogBinary(char* data, int size)
{
	m_logMutex.Wait();

	 ofstream logFile;
	 VBString fileName = "log.txt";

	logFile.open(fileName, ios::app | ios::out | ios::binary);
	logFile.write(data, size);

	m_logMutex.Signal();
}
