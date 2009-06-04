#include "compat.h"
#include "IFileLog.h"
#include "global_error.h"

IFileLog::IFileLog(LPCSTR _szFilename) : 
    m_Semaph()
{	
    m_pFile = NULL;

	if (_szFilename)
	{
        m_Filename = _szFilename;
        m_pFile = fopen(_szFilename, "w");
        
        if (m_pFile)   
		    Close();
    }
}

void IFileLog::Close()
{
    if (m_pFile)
    {   
        fclose(m_pFile);
        m_pFile = NULL; 
    }
}

ULONG IFileLog::Open()
{
    ULONG ret = RET_OK;

    Close();
    
    m_pFile = fopen(m_Filename.c_str(), "a");    
        
    if (!m_pFile)
        ret = RET_ERROR;

    return ret;
}


IFileLog::~IFileLog()
{
    Close();
}
