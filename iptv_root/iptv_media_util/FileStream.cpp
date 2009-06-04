#include "FileStream.h"
#include "global_error.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FileStream::FileStream() : m_hFile(NULL)
{
}

FileStream::FileStream(LPCSTR sztFile, LPCSTR sztMode) : m_hFile(NULL), 
                                                         m_strHost(sztFile),
	                                                     m_strProtocol(sztMode)
{
}

FileStream::~FileStream()
{
    CloseUrl();
}

void FileStream::OpenUrl()
{
    m_FileSemaph.Wait();

    if (m_hFile)
    {
        return;
    }

    m_hFile = fopen(m_strHost.c_str(), m_strProtocol.c_str());

    m_FileSemaph.Signal();
}

void FileStream::CloseUrl()
{
    m_FileSemaph.Wait();
	
    if (m_hFile)
    {
        fclose(m_hFile);
        m_hFile = NULL;
    }

    m_FileSemaph.Signal();
}

unsigned FileStream::Read(void *pv, ULONG cb, ULONG *pcbRead)
{	
    if (!pv)
    {
        return RET_ERROR;
    }

    m_FileSemaph.Wait();

    unsigned ret = RET_OK;
	
    if (m_hFile)
    {
        size_t read = fread(pv, 1, cb, m_hFile);
        if (ferror(m_hFile))
	    {
            ret = RET_ERROR;
        }

        if (pcbRead)
        {
            *pcbRead = read;
        }    }
    else
    {
        ret = RET_ERROR;
    }
    
    m_FileSemaph.Signal();

    return ret;
}

unsigned FileStream::Write( const void *pv, ULONG cb, ULONG *pcbWritten)
{
    if (!pv)
    {
        return RET_ERROR;
    }
    
    m_FileSemaph.Wait();

    unsigned ret = RET_OK;

    if (m_hFile)
    {
        size_t written = fwrite(pv, 1, cb, m_hFile);

        if (ferror(m_hFile))
        {
            ret = RET_ERROR;
        }

        if (pcbWritten)
        {
            *pcbWritten = written;
        }
    }
    else
    {
        ret = RET_ERROR;
    }

    m_FileSemaph.Signal();

    return ret;
}

unsigned FileStream::Seek(
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition)
{
    m_FileSemaph.Wait();

    unsigned ret = RET_OK;

    if (m_hFile)
    {
        if (fseek(m_hFile, (long) dlibMove.QuadPart, (int) dwOrigin) != 0)
	    {
            ret = RET_ERROR;
	    }
        else
        if (plibNewPosition)
        {
            plibNewPosition->QuadPart = ftell(m_hFile);
        }
    }
    else
    {
        ret = RET_ERROR;
    }

    m_FileSemaph.Signal();

    return ret;
}

unsigned FileStream::Commit(DWORD grfCommitFlags)
{
    m_FileSemaph.Wait();

    unsigned ret = RET_OK;

    if (m_hFile)
    {
        if (fflush(m_hFile) != 0)
        {
            ret = RET_ERROR;
        }
    }
    else
    {
        ret = RET_ERROR;
    }

    m_FileSemaph.Signal();

    return ret;
}

