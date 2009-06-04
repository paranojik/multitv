#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <string>
#include "shared/compat.h"
#include "shared/semaphore.h"
#include "defs.h"

#define MAX_PATH 260

class FileStream
{
private:
    std::string m_strHost,
	        m_strProtocol;

public:
	FileStream();
	FileStream(LPCSTR sztUrl, LPCSTR sztMode = "rb");

	virtual ~FileStream();

	void OpenUrl();
	void CloseUrl();
	bool IsOpened() { return m_hFile != NULL; }

	unsigned Read (void *pv, ULONG cb, ULONG *pcbRead);
	unsigned Write(const void *pv, ULONG cb, ULONG *pcbWritten);

	unsigned Seek (LARGE_INTEGER dlibMove,
                   DWORD dwOrigin,
                   ULARGE_INTEGER *plibNewPosition);

    unsigned Commit( DWORD grfCommitFlags);

private:
    
    FILE * m_hFile;
    _SEMAPHORE m_FileSemaph;
};


#endif

